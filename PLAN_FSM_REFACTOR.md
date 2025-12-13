### Refactor Plan: Move State/StateMachine to ETL Queued FSM

This document outlines the step-by-step plan to refactor the existing State/StateMachine implementation to use ETL’s queued FSM, using the ETL version already vendored in `firmware/lib/etl-20.44.1`.

No implementation will occur until each step is reviewed and approved.

---

### Guiding Principles
1) Do not allocate any dynamic memory.
2) Conform to MISRA2023.
3) Follow guidelines set in claude.md.
4) Prefer C++23 features and project naming/style conventions.

### Decisions captured from latest review (applied to the plan)
- Events: use structured `EvtError{ Error enum, Domain enum (UI/Furnace/StateMachine), msg }` and keep the rest of the proposed event set.
- TRANSITIONING: drop as a concrete state; enforce exit→enter ordering; on enter failure post `EvtError` and go to `ERROR`.
- Queuing: implement priority handling across three classes of events (Critical, Furnace, UI). Use a single ETL-based priority queue when feasible; otherwise fall back to three separate queues drained in order Critical → Furnace → UI each cycle.
- Queue depth: 16 per priority class (Critical/Furnace/UI). With a single priority queue design, total capacity will be 48 entries.
- Queue overflow: drop-newest, log error, increment a counter.
- Posting context & synchronization: Option A selected — use `std::mutex` + `std::lock_guard` in task/context code. For ISR posts, use a tiny ISR-safe staging buffer (lock-free SPSC per queue) that is drained into the main queues in task context to remain MISRA-compliant without locking in ISRs.
- API compatibility: retain `StateMachine::GetState()`. Remove `TransitionTo(...)` in favour of ETL’s event-driven transitions. Keep `CanTransition(StateId)` temporarily (best-effort), then refactor to state-level capability checks.
- Invalid event policy: route invalid events to `ERROR` (after logging) from any state.
- Manual temperature control: use two discrete states — `PROGRAM_TEMP_OVERRIDE` (when a program is running) and `MANUAL_TEMP` (when no program is running). Selection on `EvtSetManualTemp` is based on `FurnaceState::programIsRunning` and whether a profile is loaded. Resume behavior:
  - From `PROGRAM_TEMP_OVERRIDE` on `EvtResume` → `RUNNING`.
  - From `MANUAL_TEMP` on `EvtResume` → `LOADED` if a profile is loaded, otherwise `IDLE`.
  - `FurnaceState::programIsRunning` is ONLY updated when transitioning into `RUNNING` (never by manual temp states).

#### 1) Clarify requirements and constraints (Approval needed before proceeding)
- Events and triggers:
  - Proposed events: `EvtLoadProfile{ Profile }`, `EvtStart{}`, `EvtPause{}`, `EvtResume{}`, `EvtCancel{}`, `EvtComplete{}`, `EvtTempReached{}`, `EvtError{ Error enum, Domain enum (eg. UI, Furnace, StateMachine), msg }`, `EvtReset{}`; optional `EvtTick{}`.
- TRANSITIONING handling:
  - Proposal: Drop as a concrete state; use ETL transition ordering (exit then enter). On enter failure, post `EvtError` to route to `ERROR`.
- Queue and concurrency:
  - Implement as a priority queue with 3 levels: Critical, Furnace, UI.
    - If ETL cannot provide a priority queue, use 3 separate queues; on each processing iteration, always drain Critical first, then Furnace, then UI.
  - Depth per queue: 16 (confirmed).
  - Overflow policy: drop-newest with error log and counter. If Critical or Furnace priority overflows, transition to ERROR state.
  - Any event may be posted from an ISR. Synchronization mechanism: pending selection (see options below) while ensuring MISRA conformance.
- Backward compatibility & API:
  - Keep `StateMachine::GetState()`.
  - Remove `TransitionTo(...)` in favour of ETL’s event-driven transitions.
  - `CanTransition(StateId)`: keep best-effort for now; plan refactor to per-state checks.
- Error policy for invalid events:
  - Log and route to `ERROR` from any state (confirmed).
- Logging:
  - Retain `Log::LogService` and existing style. Confirm any structured fields required.

Deliverable: A short decision list resolving the bullets above (all Step 1 decisions resolved).

##### Synchronization choice (final)
- Selected: Option A — `std::mutex` + `std::lock_guard` for task/context code.
- ISR posting path: lock-free SPSC staging buffers per queue; drained into main queues in task context. This avoids locking in ISR and maintains MISRA-friendly bounded behavior.

---

##### ETL priority queue feasibility and design
It is feasible to build a single bounded priority queue using ETL constructs:
- Underlying container: `etl::vector`
- Priority queue: `etl::priority_queue`
- Message storage: `etl::message_packet<MaxMessageSize>` to hold heterogeneous `etl::imessage`-derived events without dynamic allocation
- Stability: add a monotonic sequence number to preserve FIFO order within the same priority

Sketch:
```
enum class EventPriority : uint8_t { Critical = 0U, Furnace = 1U, UI = 2U };

struct QueuedMsg {
  EventPriority priority;
  uint32_t      seq;   // monotonic, wraps naturally
  etl::message_packet<MaxMessageSize> packet;
};

struct MsgCompare {
  bool operator()(const QueuedMsg& a, const QueuedMsg& b) const noexcept {
    if (a.priority != b.priority) {
      return static_cast<uint8_t>(a.priority) > static_cast<uint8_t>(b.priority); // lower enum = higher priority
    }
    return a.seq > b.seq; // earlier seq has higher priority (min-heap via comparator)
  }
};

using PQContainer = etl::vector<QueuedMsg, 48U>; // 16 per class → total 48
using EventPriorityQueue = etl::priority_queue<QueuedMsg, PQContainer, MsgCompare>;
```

Overflow policy with single PQ: on push failure (full), log; if `priority != UI`, immediately route to `ERROR` state; otherwise continue (counter incremented). This preserves the previously approved behavior.

If we encounter limitations integrating `etl::message_packet` inside the `QueuedMsg` (e.g., size traits), we will stop and ask for guidance..

#### 2) Map current states and transitions to ETL structure (Approval needed)
- States to implement as ETL states: `IDLE, LOADED, RUNNING, PAUSED, COMPLETED, CANCELLED, ERROR, PROGRAM_TEMP_OVERRIDE, MANUAL_TEMP`.
  - Note: `WAITING_FOR_TEMP` is intentionally deferred and not implemented now.
- Provide a table mapping current allowed transitions to per-state handlers (message → next state).
- Define guards or preconditions per transition where applicable.

Deliverable: A markdown table with state vs. accepted events and resulting transitions; notes on guards and side effects.

Proposed mapping table (for review):

| State | Event                            | Next State | Guards/Preconditions | Side effects / Notes                                                            |
|---|----------------------------------|---|---|---------------------------------------------------------------------------------|
| IDLE | `EvtLoadProfile{ Profile }`      | LOADED | Profile validity check (non-empty, constraints) | Store profile as loaded; log "Loaded"                                           |
| IDLE | `EvtSetManualTemp{ temp }`       | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning`                         |
| IDLE | `EvtError{...}`                  | ERROR | — | Log error details                                                               |
| IDLE | `EvtReset{}`                     | IDLE (no-op) | — | Clear any transient data if present                                             |
| LOADED | `EvtStart{}`                     | RUNNING | Safety OK (interlocks, sensors), controller ready | Start control loop; initialize timers/logging                                   |
| LOADED | `EvtLoadProfile{ Profile }`      | LOADED | Profile validity | Replace previously loaded profile; log replace                                  |
| LOADED | `EvtError{...}`                  | ERROR | — | Log error details                                                               |
| LOADED | `EvtClearProgram{}`              | IDLE | — | Clear loaded profile                                                            |
| LOADED | `EvtSetManualTemp{ temp }`       | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning`                         |
| RUNNING | `EvtPause{}`                     | PAUSED | — | Suspend control loop; maintain outputs safe                                     |
| RUNNING | `EvtComplete{}`                  | COMPLETED | Program complete condition | Stop control loop; safe shutdown                                                |
| RUNNING | `EvtCancel{}`                    | CANCELLED | — | Abort program; safe shutdown                                                    |
| RUNNING | `EvtError{...}`                  | ERROR | — | Log error; stop control loop                                                    |
| RUNNING | `EvtTick{}` / `EvtTempReached{}` | RUNNING (internal) | — | Process step progression without state change (keeps legacy transitions intact) |
| RUNNING | `EvtSetManualTemp{ temp }`       | PROGRAM_TEMP_OVERRIDE | `temp` within safety bounds | Enable override; does not modify `programIsRunning`                              |
| PAUSED | `EvtResume{}`                    | RUNNING | — | Resume control loop                                                             |
| PAUSED | `EvtCancel{}`                    | CANCELLED | — | Abort program; safe shutdown                                                    |
| PAUSED | `EvtError{...}`                  | ERROR | — | Log error, safe shutdown                                                        |
| COMPLETED | `EvtClearProgram{}`                     | IDLE | — | Clear loaded profile                                                            |
| COMPLETED | `EvtLoadProfile{ Profile }`      | LOADED | Profile validity | Allow immediate reload after completion                                         |
| COMPLETED | `EvtError{...}`                  | ERROR | — | Log error                                                                       |
| COMPLETED | `EvtSetManualTemp{ temp }`       | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning`                         |
| CANCELLED | `EvtLoadProfile{ Profile }`      | LOADED | Profile validity | Allow reload after cancel                                                       |
| CANCELLED | `EvtError{...}`                  | ERROR | — | Log error                                                                       |
| CANCELLED | `EvtSetManualTemp{ temp }`       | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning`                         |
| ERROR | `EvtReset{}`                     | IDLE | — | System recovery path; clear error flags, clear program                          |
| ERROR | `EvtLoadProfile{ Profile }`      | LOADED | Profile validity | Allow recovery directly to LOADED                                               |
| WAITING_FOR_TEMP | `EvtTempReached{}`               | RUNNING | — | Continue program once soak temperature reached                                  |
| WAITING_FOR_TEMP | `EvtPause{}`                     | PAUSED | — | Pause while waiting                                                             |
| WAITING_FOR_TEMP | `EvtError{...}`                  | ERROR | — | Log error                                                                       |

Notes and alignment with legacy transitions:
- The mapping above preserves the legacy `myValidTransitions` set in `StateMachine.hpp` (no new outward transitions were introduced). In particular, `RUNNING → WAITING_FOR_TEMP` is intentionally not defined, as it is not listed in the existing valid transitions. Temperature gating is kept as an internal `RUNNING` behavior via `EvtTick`/`EvtTempReached` without a state change.
- We are introducing one new state (`MANUAL_TEMP`) per requirements; this extends, but does not break, legacy transitions.
- Invalid events in any state will be logged and will route to `ERROR` per Step 1 decision.
- Queue overflow routing: if a non-UI event overflows the queue, we will immediately route to `ERROR` (and log). UI overflow will only log and increment the counter.

Additional event/states per latest request (to be added):
- New event: `EvtSetManualTemp{ int temp }`.
- New states: `PROGRAM_TEMP_OVERRIDE`, `MANUAL_TEMP`.

New transitions (proposed for review):
- From `RUNNING` on `EvtSetManualTemp{ temp }` → `PROGRAM_TEMP_OVERRIDE`.
  - Guards: `temp` within allowed safety bounds.
- From `PROGRAM_TEMP_OVERRIDE` on `EvtResume{}` → `RUNNING`.
- From `IDLE | LOADED | COMPLETED | CANCELLED` on `EvtSetManualTemp{ temp }` → `MANUAL_TEMP`.
  - Guards: `temp` within safety bounds.
- From `MANUAL_TEMP` on `EvtResume{}` → `LOADED` if a profile is loaded; otherwise `IDLE`.

Ordering & invalid events:
- All invalid events continue to route to `ERROR` after logging.
- Overflow routing rule applies unchanged.

Breadcrumb/origin handling options (ETL capabilities vs design alternatives):
- ETL `fsm`/`hfsm` review: No built-in state history or breadcrumb trail preservation is exposed in `etl/fsm.h` or `etl/hfsm.h` in the vendored version. `hfsm` provides hierarchical composition but not automatic previous-state recall for arbitrary transitions.
- Current plan with `FurnaceState` flags: With `programIsRunning` and a non-null loaded profile pointer available on `FurnaceState`, breadcrumbs are not required for `MANUAL_TEMP` resume logic.
- Option 1: Maintain a tiny bounded state stack (depth 1–2) in the FSM context for last stable state (not required now, kept as fallback).
- Option 2: Maintain a tiny bounded state stack (depth 1–2) in the FSM context for last stable state:
  - Push origin before transitioning into manual states; on `EvtResume{}`, pop and transition back.
  - Bounded, no dynamic allocation; implemented with fixed-size array and index.
- Option 3: Split `MANUAL_TEMP` into two explicit states to avoid conditionals:
  - `MANUAL_TEMP_FROM_LOADED`, `MANUAL_TEMP_FROM_IDLELIKE` (where IdleLike ∈ {IDLE, CANCELLED, COMPLETED}).
  - `EvtResume{}` from each goes deterministically to `LOADED` or `IDLE` respectively.
  - Pros: explicit transitions; Cons: more states; not required with `FurnaceState` flags.
- Option 4: Guarded transition using a separate context flag `wasLoadedBeforeManual` (not required with `FurnaceState` flags).

Plan choice proposal:
- Use two discrete states as above (`PROGRAM_TEMP_OVERRIDE`, `MANUAL_TEMP`), selecting which to enter based on `programIsRunning` and profile presence. No breadcrumbs required.
-

Table additions for manual-control states (for clarity):

| State | Event | Next State | Guards/Preconditions | Side effects / Notes |
|---|---|---|---|---|
| MANUAL_TEMP | `EvtResume{}` | `LOADED` if profile present; else `IDLE` | — | Disable manual control |
| MANUAL_TEMP | `EvtSetManualTemp{ temp }` | MANUAL_TEMP (internal) | `temp` within safety bounds | Update manual setpoint only |
| MANUAL_TEMP | `EvtError{...}` | ERROR | — | Log and route to error |
| PROGRAM_TEMP_OVERRIDE | `EvtResume{}` | RUNNING | — | Exit override; program continues |
| PROGRAM_TEMP_OVERRIDE | `EvtSetManualTemp{ temp }` | PROGRAM_TEMP_OVERRIDE (internal) | `temp` within safety bounds | Update override setpoint only |
| PROGRAM_TEMP_OVERRIDE | `EvtError{...}` | ERROR | — | Log and route to error |

Ownership note: The FSM will not allocate or free the program. It only reads `FurnaceState::myLoadedProgram` presence (managed elsewhere) and `programIsRunning` to decide which manual state to enter and where to resume. Manual-temp transitions never modify `programIsRunning`.

---

#### 3) Design event/message types and storage (Approval needed)
- Finalize event structs (deriving from `etl::imessage`).
- Choose storage model:
  - Preferred: `etl::message_packet<T>` for heterogeneous messages in a single queue, no dynamic allocation.
  - Queue types:
    - If one queue: `etl::queue<etl::message_packet<MaxMessageSize>, QueueDepth>`.
    - If three queues: distinct typedefs for Critical/Furnace/UI with same packet type.
- Define maximum message size and alignment; ensure no dynamic allocation and MISRA-compliant layout.
- Define thread-safety wrappers (mutex/critical sections) for post/drain.

Deliverable: Header sketch showing event structs and queue typedefs.

---

#### 4) FSM class skeleton and minimal states (Implementation after approval)
- Add `FurnaceFsm` class deriving from `etl::fsm`:
  - Owns queue(s), state instances, references to `FurnaceState` and `LogService`.
  - Methods: `post(T, Domain priority)`, `process_queue()`, `current_state_id()`.
- Implement minimal states `StIdle` and `StError` with logging and basic event handling (`EvtReset`, `EvtError`).
- Keep existing `StateMachine` intact.

Deliverable: PR adding `FurnaceFsm` skeleton and two states, no behavior change to callers.

---

#### 5) Facade bridging (Implementation after approval)
- Modify `StateMachine` to contain a `FurnaceFsm` member and forward-facing methods:
  - `GetState()` → `fsm.current_state_id()`.
  - Remove `TransitionTo(StateId)` from the public API (or mark deprecated immediately with replacement notes).
  - `CanTransition(StateId)` → temporary best-effort; add TODO to migrate to per-state query.
- Provide `Step()` or integrate `process_queue()` into existing polling loop.

Deliverable: PR that wires the facade while keeping tests green.

---

#### 6) Port core paths (Implementation after approval)
- Implement states and handlers for `Idle → Loaded → Running` and error routing.
- Translate any existing `OnEnter/OnExit` logic into `on_enter_state/on_exit_state`.
- Add unit tests covering the happy path and basic invalid-event behavior.

Deliverable: PR with states `StLoaded`, `StRunning`, unit tests, and documentation notes.

---

#### 7) Port remaining states (Implementation after approval)
- Implement `StPaused`, `StCompleted`, `StCancelled`, `StWaitingForTemp` with handlers.
- Add tests for pause/resume, cancel, complete, and temperature gating.

Deliverable: PR completing all states and tests.

---

#### 8) Remove legacy internals and finalize (Implementation after approval)
- Remove `myValidTransitions` and unused legacy state classes (retain `StateId`).
- Update documentation (`API.md`, `ARCHITECTURE.md`) to reflect queued FSM design.
- Ensure no dynamic allocations; confirm queue overflow and ISR behavior per decisions.

Deliverable: PR cleaning up legacy code, updated docs, final tests.

---

#### 9) Verification on targets (Implementation after approval)
- Run unit tests on Linux target.
- Verify on ESP32 target if applicable (build and basic runtime check), observing logging and event flow.

Deliverable: Notes from verification and any tuning (queue depth, timing).

---

### Open Questions (to be answered in Step 1)
1) Event list completeness and any additional domain-specific signals?
2) Exact queue depth per priority class (e.g., 16/16/16 or different per class)?
3) Invalid event policy: confirm final choice (log-and-ignore vs route to `ERROR`).
4) Mutex/critical section preference on ESP32 (std::mutex vs IDF critical sections) to meet MISRA.
5) Any additional structured fields for logging beyond Error/Domain/msg?
