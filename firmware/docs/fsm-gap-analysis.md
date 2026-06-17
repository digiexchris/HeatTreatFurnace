# FSM Protocol Gap Analysis

Analysis of capability gaps between the C++ FSM/Event system and what the Frontend/Simulator protocol expects.

## Summary

The C++ backend FSM has a **mode-based hierarchical design** (OFF/PROFILE/MANUAL) while the protocol expects a **program-centric linear state machine** (None/Ready/Running/Paused/Stopped/Finished). This creates mapping challenges and some missing capabilities.

---

## Gap 1: Missing PAUSED State (Critical)

**Protocol expects:**
- `PauseCommand` → status becomes `Paused (3)`
- `ResumeCommand` → status becomes `Running (2)`
- `StopCommand` → status becomes `Stopped (4)`

**C++ FSM has:**
- `EvtProfileStop` → state becomes `PROFILE_STOPPED` (conflates pause and stop)
- `EvtProfileStart` → resumes from stopped state

**Impact:** Cannot distinguish between pause and stop. Frontend shows both states distinctly with different UI behaviors.

**Fix needed:** Add `PROFILE_PAUSED` state and `EvtProfilePause`/`EvtProfileResume` events, or track pause vs stop semantically within `PROFILE_STOPPED`.

---

## Gap 2: Missing ClearErrorCommand Event (Medium)

**Protocol expects:**
- `ClearErrorCommand` clears error state

**C++ FSM has:**
- No dedicated `EvtClearError` event
- Error is cleared via mode transitions (`EvtModeOff`, `EvtModeProfile`, `EvtModeManual`)

**Impact:** Frontend sends `ClearErrorCommand` which has no direct mapping.

**Fix needed:** Add `EvtClearError` event that transitions from ERROR → OFF, or document that `EvtModeOff` serves this purpose.

---

## Gap 3: WaitingThreshold State Not Implemented (Low)

**Protocol defines:**
- `WaitingThreshold = 6` status

**C++ FSM has:**
- No corresponding state

**Note:** Simulator also marks this as "not actively simulated." This may be for future use (e.g., waiting for kiln to reach threshold before advancing).

**Fix needed:** Clarify if this feature is needed. If so, add `PROFILE_WAITING_THRESHOLD` state.

---

## Gap 4: Manual Mode Has No Protocol Status (Design Decision)

**C++ FSM has:**
- `MANUAL`, `MANUAL_OFF`, `MANUAL_ON` states

**Protocol has:**
- No manual mode status - only program-centric statuses

**Impact:** When in manual mode, what `ProgramStatus` should be reported?

**Options:**
1. Report `None` when in manual mode (frontend treats it as idle)
2. Add a `Manual` status to the protocol (breaking change)
3. Manual mode sets target temp while in a program state

**Current frontend behavior:** `SetTempCommand` appears to work independently of program state.

---

## Gap 5: State-to-ProgramStatus Mapping

The C++ backend needs a translation layer:

| C++ State | Protocol Status |
|-----------|-----------------|
| `OFF` | `None` (0) |
| `ERROR` | `Error` (5) |
| `PROFILE` | `None` (0) |
| `PROFILE_LOADED` | `Ready` (1) |
| `PROFILE_RUNNING` | `Running` (2) |
| `PROFILE_STOPPED` | `Stopped` (4) or `Paused` (3)? |
| `PROFILE_COMPLETED` | `Finished` (7) |
| `MANUAL` / `MANUAL_*` | `None` (0)? Needs decision |

---

## Gap 6: Command-to-Event Mapping

| Protocol Command | C++ Event | Notes |
|------------------|-----------|-------|
| `LoadCommand` | `EvtProfileLoad` | Needs profile parsing first |
| `UnloadCommand` | `EvtProfileClear` | OK |
| `StartCommand` | `EvtProfileStart` + optional `EvtProfileSetNextSegment` | segment/minute params need handling |
| `PauseCommand` | `EvtProfileStop`? | **GAP: No dedicated pause** |
| `ResumeCommand` | `EvtProfileStart` | OK (resumes from stopped) |
| `StopCommand` | `EvtProfileStop` | **GAP: Same as pause** |
| `SetTempCommand` | `EvtManualSetTemp` | May need mode switch |
| `ClearErrorCommand` | `EvtModeOff`? | **GAP: No dedicated event** |

---

## Gap 7: History Markers (Implementation Detail)

Protocol defines markers that should be emitted:
- `Start`, `Stop`, `Finish`, `Pause`, `Resume`, `Target`, `Step`

The FSM needs to emit these markers at state transitions for history logging. This is an implementation detail for the WebSocket/State broadcasting layer.

---

## Recommendations

### Minimal Changes (Keep current architecture)
1. Add semantic flag to `PROFILE_STOPPED` to track pause vs stop
2. Map `ClearErrorCommand` → `EvtModeOff`
3. Report `None` for manual mode states
4. Document the mapping in a translation layer

### Better Approach (Protocol alignment)
1. Add `PROFILE_PAUSED` state and events
2. Add `EvtClearError` event
3. Decide on manual mode status reporting
4. Implement state-to-status translation in WebSocket handler

---

## What's Already Compatible

The following commands map cleanly:
- `LoadCommand` → `EvtProfileLoad`
- `UnloadCommand` → `EvtProfileClear`
- `StartCommand` → `EvtProfileStart` (with `EvtProfileSetNextSegment` for resume position)
- `ResumeCommand` → `EvtProfileStart` (from stopped state)
- `SetTempCommand` → `EvtManualSetTemp`

The FSM structure (hierarchical states, event queue, error handling) is solid and appropriate for the embedded context.

---

## References

- Protocol definition: [`proto/furnace.fbs`](../../proto/furnace.fbs)
- C++ FSM events: [`Furnace/Events.hpp`](../lib/HeatTreatFurnace/Furnace/Events.hpp)
- C++ FSM states: [`Furnace/StateId.hpp`](../lib/HeatTreatFurnace/Furnace/StateId.hpp)
- State transition table: [`Furnace/StateTransitionTable.md`](../lib/HeatTreatFurnace/Furnace/StateTransitionTable.md)
