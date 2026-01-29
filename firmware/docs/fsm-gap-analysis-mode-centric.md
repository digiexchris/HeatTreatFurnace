# FSM Protocol Gap Analysis (Mode-Centric Approach)

Analysis of changes needed to align the Frontend/Simulator protocol with the C++ backend's mode-centric FSM design.

## Approach

Align the protocol to match the C++ backend's hierarchical mode-based design:
- **Modes:** OFF, ERROR, PROFILE, MANUAL
- **No separate Pause state:** Stop/Start handles pause semantics (stop holds position, start resumes)

---

## Protocol Changes Required

### Replace ProgramStatus Enum

**Current protocol:**
```flatbuffers
enum ProgramStatus : byte {
  None = 0,
  Ready = 1,
  Running = 2,
  Paused = 3,        // REMOVE
  Stopped = 4,
  Error = 5,
  WaitingThreshold = 6,  // REMOVE (not implemented)
  Finished = 7
}
```

**New mode-centric protocol:**
```flatbuffers
enum FurnaceMode : byte {
  Off = 0,
  Error = 1,
  Profile = 2,
  Manual = 3
}

enum ProfileSubState : byte {
  None = 0,        // In Profile mode, no profile loaded
  Loaded = 1,      // Profile loaded, ready to start
  Running = 2,     // Profile executing
  Stopped = 3,     // Profile stopped (start resumes from current position)
  Completed = 4    // Profile finished
}

enum ManualSubState : byte {
  Off = 0,         // Heater off
  On = 1           // Heater on, holding target
}
```

### Update State Message

**Current:**
```flatbuffers
table State {
  program_status: ProgramStatus;
  // ... other fields
}
```

**New:**
```flatbuffers
table State {
  mode: FurnaceMode;
  profile_state: ProfileSubState;  // Valid when mode == Profile
  manual_state: ManualSubState;    // Valid when mode == Manual
  // ... other fields unchanged
}
```

### Update Commands

**Remove:**
- `PauseCommand` - no longer needed
- `ResumeCommand` - no longer needed

**Keep as-is:**
- `StartCommand` - starts or resumes profile
- `StopCommand` - stops profile (holds current position for resume)
- `LoadCommand` - loads profile
- `UnloadCommand` - clears profile
- `SetTempCommand` - sets manual temperature (**only works in Manual mode**)

**Add:**
- `SetModeCommand { mode: FurnaceMode }` - explicit mode switching (also clears errors)

### Update MarkerType Enum

**Current:**
```flatbuffers
enum MarkerType : byte {
  Start = 0,
  Stop = 1,
  Finish = 2,
  Pause = 3,     // REMOVE
  Resume = 4,    // REMOVE
  Target = 5,
  Step = 6
}
```

**New:**
```flatbuffers
enum MarkerType : byte {
  Start = 0,
  Stop = 1,
  Finish = 2,
  Target = 3,
  Step = 4
}
```

---

## State Mapping (Now 1:1)

| C++ State | Protocol Mode | Protocol SubState |
|-----------|---------------|-------------------|
| `OFF` | `Off` | n/a |
| `ERROR` | `Error` | n/a |
| `PROFILE` | `Profile` | `None` |
| `PROFILE_LOADED` | `Profile` | `Loaded` |
| `PROFILE_RUNNING` | `Profile` | `Running` |
| `PROFILE_STOPPED` | `Profile` | `Stopped` |
| `PROFILE_COMPLETED` | `Profile` | `Completed` |
| `MANUAL` | `Manual` | `Off` (default) |
| `MANUAL_OFF` | `Manual` | `Off` |
| `MANUAL_ON` | `Manual` | `On` |

---

## Command-to-Event Mapping (Now Direct)

| Protocol Command | C++ Event | Notes |
|------------------|-----------|-------|
| `SetModeCommand(Off)` | `EvtModeOff` | Also clears errors |
| `SetModeCommand(Profile)` | `EvtModeProfile` | Also clears errors |
| `SetModeCommand(Manual)` | `EvtModeManual` | Also clears errors |
| `LoadCommand` | `EvtProfileLoad` | Parse JSON → Profile struct |
| `UnloadCommand` | `EvtProfileClear` | Direct mapping |
| `StartCommand` | `EvtProfileStart` + optional `EvtProfileSetNextSegment` | segment/minute params |
| `StopCommand` | `EvtProfileStop` | Direct mapping |
| `SetTempCommand` | `EvtManualSetTemp` | **Only works in Manual mode** |

---

## Remaining Gaps (Resolved)

### Gap 1: ClearErrorCommand ~~Mapping~~ REMOVED

**Decision:** Remove `ClearErrorCommand` from the protocol.

Users will select a mode (Off, Profile, or Manual) to clear the error state. This matches the C++ backend where `EvtModeOff`, `EvtModeProfile`, and `EvtModeManual` all transition out of ERROR state.

### Gap 2: SetTempCommand Mode Behavior

**Decision:** Option B - Only works if already in Manual mode.

The C++ backend only handles `EvtManualSetTemp` in `ManualOffState` and `ManualOnState`. The event is not accepted in OFF, PROFILE, or ERROR states.

Frontend must:
1. Switch to Manual mode first (via `SetModeCommand`)
2. Then send `SetTempCommand`

**Backend test needed:** Confirm `EvtManualSetTemp` is ignored when not in Manual mode.

### Gap 3: History Markers (Implementation Detail)

The FSM needs to emit markers at state transitions:
- `Start` - when entering `PROFILE_RUNNING`
- `Stop` - when entering `PROFILE_STOPPED`
- `Finish` - when entering `PROFILE_COMPLETED`
- `Target` - when target temperature changes
- `Step` - when segment changes

This is handled in the WebSocket/State broadcasting layer, not the FSM itself.

---

## Frontend Changes Required

### State Handling
- Replace `programStatus` checks with `mode` + `subState` checks
- Update status badge logic for mode-centric display
- Update button enable/disable logic

### Commands
- Remove Pause button (or repurpose as Stop)
- Remove Resume button (Start handles resume)
- Update command sending to match new protocol

### UI Labels
- "PAUSED" badge → remove or use "STOPPED"
- Consider showing mode in status bar (e.g., "MANUAL: ON")

---

## Simulator Changes Required

### State Machine
- Replace `ProgramStatus` enum with mode-centric enums
- Update state transitions to match C++ FSM
- Remove pause/resume handling

### Protocol Handling
- Update message parsing for new State structure
- Remove PauseCommand/ResumeCommand handlers
- Update state broadcasting

---

## Summary

With the mode-centric approach and finalized design decisions:

| Original Gap | New Status |
|--------------|------------|
| Gap 1: Pause vs Stop | **ELIMINATED** - using stop/start |
| Gap 2: ClearErrorCommand | **ELIMINATED** - removed, use SetModeCommand |
| Gap 3: WaitingThreshold | **ELIMINATED** - removed from protocol |
| Gap 4: Manual mode status | **ELIMINATED** - Manual is a first-class mode |
| Gap 5: State mapping | **ELIMINATED** - 1:1 mapping |
| Gap 6: Command mapping | **SIMPLIFIED** - direct mappings |
| Gap 7: History markers | **UNCHANGED** - implementation detail |

**Design Decisions:**
- `ClearErrorCommand` removed - user selects a mode to clear errors
- `SetTempCommand` only works in Manual mode (matches C++ backend)
- Added `SetModeCommand` for explicit mode switching

The C++ FSM is now the source of truth. Only change needed: add backend test to confirm `EvtManualSetTemp` is ignored in non-Manual modes.

See [Mode-Centric Refactor Plan](./mode-centric-refactor-plan.md) for implementation details.

---

## References

- Protocol definition: [`proto/furnace.fbs`](../../proto/furnace.fbs)
- C++ FSM events: [`Furnace/Events.hpp`](../lib/HeatTreatFurnace/Furnace/Events.hpp)
- C++ FSM states: [`Furnace/StateId.hpp`](../lib/HeatTreatFurnace/Furnace/StateId.hpp)
- State transition table: [`Furnace/StateTransitionTable.md`](../lib/HeatTreatFurnace/Furnace/StateTransitionTable.md)
