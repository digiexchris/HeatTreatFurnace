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
- Events: use structured `EvtError{ Error enum, Domain enum (UI/Furnace/StateMachine), msg }` and the approved event set (see Step 1).
- TRANSITIONING: drop as a concrete state; enforce exit→enter ordering; on enter failure post `EvtError` and go to `ERROR`.
- Queuing: implement priority handling across three classes of events (Critical, Furnace, UI). Use a single ETL-based priority queue when feasible; otherwise fall back to three separate queues drained in order Critical → Furnace → UI each cycle.
- Queue depth: 16 per priority class (Critical/Furnace/UI). With a single priority queue design, total capacity will be 48 entries.
- Queue overflow: drop-newest, log error, increment a counter.
- Posting context & synchronization: Option A selected — use `std::mutex` + `std::lock_guard` in task/context code.
- API compatibility: retain `StateMachine::GetState()`. Remove `TransitionTo(...)` in favour of ETL’s event-driven transitions. Keep `CanTransition(StateId)` temporarily (best-effort), then refactor to state-level capability checks.
- Invalid event policy: route invalid events to `ERROR` (after logging) from any state.
- Manual temperature control: use two discrete states — `PROFILE_TEMP_OVERRIDE` (when a program is running) and `MANUAL_TEMP` (when no program is running). Selection on `EvtSetManualTemp` is based on `FurnaceState::programIsRunning` and whether a profile is loaded. Resume behavior:
  - From `PROFILE_TEMP_OVERRIDE` on `EvtResume` → `RUNNING`.
  - From `MANUAL_TEMP` on `EvtResume` → `LOADED` if a profile is loaded, otherwise `IDLE`.
  - `FurnaceState::programIsRunning` is ONLY updated when transitioning into `RUNNING` (never by manual temp states).

#### 1) Clarify requirements and constraints (Approval needed before proceeding)
- Events and triggers:
  - Proposed events: `EvtLoadProfile{ Profile }`, `EvtStart{}`, `EvtPause{}`, `EvtResume{}`, `EvtCancel{}`, `EvtComplete{}`, `EvtClearProgram{}`, `EvtError{ Error enum, Domain enum (eg. UI, Furnace, StateMachine), msg }`, `EvtReset{}`; optional `EvtTick{}`.
- TRANSITIONING handling:
  - Drop as a concrete state; use ETL transition ordering (exit then enter). On enter failure, post `EvtError` to route to `ERROR`.
- Queue and concurrency:
  - Implement as a priority queue with 3 levels: Critical, Furnace, UI.
  - Depth per queue: 16.
  - Overflow policy: drop-newest with error log and counter. If Critical or Furnace priority overflows, transition to ERROR state.
- Backward compatibility & API:
  - Keep `StateMachine::GetState()`.
  - Remove `TransitionTo(...)` in favour of ETL’s event-driven transitions.
  - `CanTransition(StateId)`: refactor to per-state checks.
- Error policy for invalid events:
  - Log and route to `ERROR` from any state (confirmed).
- Logging:
  - Retain `Log::LogService` and existing style. Confirm any structured fields required.

Deliverable: A short decision list resolving the bullets above (all Step 1 decisions resolved).

##### Synchronization choice (final)
- `std::mutex` + `std::lock_guard` for task/context code.

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
- States to implement as ETL states: `IDLE, LOADED, RUNNING, PAUSED, COMPLETED, CANCELLED, ERROR, PROFILE_TEMP_OVERRIDE, MANUAL_TEMP`.
- Provide a table mapping current allowed transitions to per-state handlers (message → next state).
- Define guards or preconditions per transition where applicable.

See the table in StateTransitionTable.md for the final states, events, and transitions.

Ownership note: The FSM will not allocate or free the program. It only reads `FurnaceState::myLoadedProgram` presence (managed elsewhere) and `programIsRunning` to decide which manual state to enter and where to resume. Manual-temp transitions never modify `programIsRunning`.

---

#### 3) Design event/message types and storage (Approval needed)
- Finalize event structs (deriving from `etl::imessage`).
- Choose storage model:
  - Use a single bounded priority queue (`etl::priority_queue<QueuedMsg, PQContainer, MsgCompare>`) carrying `etl::message_packet<MaxMessageSize>` for heterogeneous messages without dynamic allocation.
  - If three queues are needed as a fallback: distinct typedefs for Critical/Furnace/UI with the same packet type, drained in priority order.
- Define maximum message size and alignment; ensure no dynamic allocation and MISRA-compliant layout.
- Define thread-safety wrappers (mutex/critical sections) for post/drain.

See EventMessageStorageDesign.md for final design.

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
- Implement `StPaused`, `StCompleted`, `StCancelled` with handlers.
- Add tests for pause/resume, cancel, and complete.

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