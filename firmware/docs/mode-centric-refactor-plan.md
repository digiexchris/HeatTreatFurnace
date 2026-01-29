# Mode-Centric Refactor Plan

Refactor the frontend and simulator to align with the C++ backend's mode-centric FSM design.

## Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| State model | Mode-centric (OFF, ERROR, PROFILE, MANUAL) | Match C++ backend |
| Pause state | **Remove** - use Stop/Start | C++ has no separate pause state |
| ClearErrorCommand | **Remove** - use mode switch | User selects a mode to clear error |
| SetTempCommand | **Option B** - only works in Manual mode | Matches C++ backend behavior |

---

## Protocol Changes

### 1. Replace ProgramStatus Enum

**Remove:**
```flatbuffers
enum ProgramStatus : byte {
  None = 0,
  Ready = 1,
  Running = 2,
  Paused = 3,           // REMOVE
  Stopped = 4,
  Error = 5,
  WaitingThreshold = 6, // REMOVE
  Finished = 7
}
```

**Add:**
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
  Stopped = 3,     // Profile stopped (start resumes)
  Completed = 4    // Profile finished
}

enum ManualSubState : byte {
  Off = 0,         // Heater off
  On = 1           // Heater on, holding target
}
```

### 2. Update State Message

**Before:**
```flatbuffers
table State {
  program_status: ProgramStatus;
  ...
}
```

**After:**
```flatbuffers
table State {
  mode: FurnaceMode;
  profile_state: ProfileSubState;  // Valid when mode == Profile
  manual_state: ManualSubState;    // Valid when mode == Manual
  ...
}
```

### 3. Update Commands

**Remove:**
- `PauseCommand`
- `ResumeCommand`
- `ClearErrorCommand`

**Add:**
```flatbuffers
table SetModeCommand {
  mode: FurnaceMode;
}
```

**Keep unchanged:**
- `StartCommand` - starts or resumes profile
- `StopCommand` - stops profile
- `LoadCommand` - loads profile
- `UnloadCommand` - clears profile
- `SetTempCommand` - sets manual temperature (only works in Manual mode)

### 4. Update MarkerType Enum

**Remove:**
- `Pause = 3`
- `Resume = 4`

**Result:**
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

## State Mapping (1:1 with C++ Backend)

| C++ State | Protocol Mode | Protocol SubState |
|-----------|---------------|-------------------|
| `OFF` | `Off` | n/a |
| `ERROR` | `Error` | n/a |
| `PROFILE` | `Profile` | `None` |
| `PROFILE_LOADED` | `Profile` | `Loaded` |
| `PROFILE_RUNNING` | `Profile` | `Running` |
| `PROFILE_STOPPED` | `Profile` | `Stopped` |
| `PROFILE_COMPLETED` | `Profile` | `Completed` |
| `MANUAL` | `Manual` | `Off` |
| `MANUAL_OFF` | `Manual` | `Off` |
| `MANUAL_ON` | `Manual` | `On` |

---

## Command-to-Event Mapping

| Protocol Command | C++ Event | Notes |
|------------------|-----------|-------|
| `SetModeCommand(Off)` | `EvtModeOff` | Also clears errors |
| `SetModeCommand(Profile)` | `EvtModeProfile` | Also clears errors |
| `SetModeCommand(Manual)` | `EvtModeManual` | Also clears errors |
| `LoadCommand` | `EvtProfileLoad` | Parse JSON → Profile |
| `UnloadCommand` | `EvtProfileClear` | Direct |
| `StartCommand` | `EvtProfileStart` | + optional `EvtProfileSetNextSegment` |
| `StopCommand` | `EvtProfileStop` | Direct |
| `SetTempCommand` | `EvtManualSetTemp` | **Only works in Manual mode** |

---

## Backend Test Required

**Test:** Confirm `EvtManualSetTemp` is ignored/rejected when not in Manual mode.

**Location:** `firmware/test-app/main/FSM/test_ManualTempState.cpp`

**Test cases needed:**
- [ ] `EvtManualSetTemp` from OFF state → stays in OFF (ignored)
- [ ] `EvtManualSetTemp` from PROFILE_LOADED → stays in PROFILE_LOADED (ignored)
- [ ] `EvtManualSetTemp` from PROFILE_RUNNING → stays in PROFILE_RUNNING (ignored)
- [ ] `EvtManualSetTemp` from ERROR → stays in ERROR (ignored)

---

## Frontend Changes

### 1. State Store Updates

**File:** `frontend/src/stores/state.ts` (or equivalent)

- Replace `programStatus` with `mode` + `profileState` + `manualState`
- Update all status checks throughout the codebase

### 2. UI Component Updates

**Status Badge:**
- Show mode as primary indicator (OFF, ERROR, PROFILE, MANUAL)
- Show sub-state as secondary (e.g., "PROFILE: Running")

**Buttons:**
- Remove Pause button
- Remove Resume button (Start handles resume)
- Add mode selector (Off / Profile / Manual)
- Disable SetTemp input unless in Manual mode

**Error Handling:**
- Remove "Clear Error" button
- Show mode selector to exit error state

### 3. Command Updates

**Remove:**
- `sendPauseCommand()`
- `sendResumeCommand()`
- `sendClearErrorCommand()`

**Add:**
- `sendSetModeCommand(mode: FurnaceMode)`

**Update:**
- `sendSetTempCommand()` - show error/warning if not in Manual mode

### 4. BDD Test Updates

**Files to update:**
- `bdd/features/program-control.feature` - remove pause/resume scenarios
- `bdd/features/dashboard.feature` - update error overlay behavior
- `bdd/features/connection.feature` - update state restoration

---

## Simulator Changes

### 1. State Machine Updates

**File:** `simulator/src/state.ts` (or equivalent)

- Replace `ProgramStatus` enum with mode-centric enums
- Update state transitions to match C++ FSM
- Remove pause/resume handling

### 2. Protocol Handling

**File:** `simulator/src/websocket.ts` (or equivalent)

- Update message parsing for new State structure
- Remove `PauseCommand`/`ResumeCommand` handlers
- Add `SetModeCommand` handler
- Update state broadcasting format

### 3. SetTempCommand Handling

- Only process if in Manual mode
- Return error/nak if not in Manual mode

---

## Migration Checklist

### Phase 1: Protocol
- [ ] Update `proto/furnace.fbs` with new enums and messages
- [ ] Regenerate TypeScript bindings
- [ ] Regenerate C bindings (if used)

### Phase 2: Backend Test
- [ ] Add test cases for `EvtManualSetTemp` rejection in non-Manual modes

### Phase 3: Simulator
- [ ] Update state machine to mode-centric model
- [ ] Update protocol handlers
- [ ] Test all state transitions

### Phase 4: Frontend
- [ ] Update state store
- [ ] Update UI components
- [ ] Update commands
- [ ] Update BDD tests

### Phase 5: Integration Testing
- [ ] Test frontend with simulator
- [ ] Verify all state transitions
- [ ] Verify command handling

---

## References

- [Gap Analysis (Mode-Centric)](./fsm-gap-analysis-mode-centric.md)
- [Gap Analysis (Original)](./fsm-gap-analysis.md)
- [C++ FSM Events](../lib/HeatTreatFurnace/Furnace/Events.hpp)
- [C++ FSM States](../lib/HeatTreatFurnace/Furnace/StateId.hpp)
- [Protocol Definition](../../proto/furnace.fbs)
