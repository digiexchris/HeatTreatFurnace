
# State Transition Table

This document defines the complete state transition table for the ETL-based queued FSM implementation.

## State Transition Matrix

| State | Event | Next State | Guards/Preconditions | Side effects / Notes |
|---|---|---|---|---|
| IDLE | `EvtLoadProfile{ Profile }` | LOADED | Profile validity check (non-empty, constraints) | Store profile as loaded; log "Loaded" |
| IDLE | `EvtSetManualTemp{ temp }` | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning` |
| IDLE | `EvtError{...}` | ERROR | — | Log error details |
| IDLE | `EvtReset{}` | IDLE (no-op) | — | Clear any transient data if present |
| LOADED | `EvtStart{}` | RUNNING | Safety OK (interlocks, sensors), controller ready | Start control loop; initialize timers/logging |
| LOADED | `EvtLoadProfile{ Profile }` | LOADED | Profile validity | Replace previously loaded profile; log replace |
| LOADED | `EvtError{...}` | ERROR | — | Log error details |
| LOADED | `EvtClearProgram{}` | IDLE | — | Clear loaded profile |
| LOADED | `EvtSetManualTemp{ temp }` | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning` |
| RUNNING | `EvtPause{}` | PAUSED | — | Suspend control loop; maintain outputs safe |
| RUNNING | `EvtComplete{}` | COMPLETED | Program complete condition | Stop control loop; safe shutdown |
| RUNNING | `EvtCancel{}` | CANCELLED | — | Abort program; safe shutdown |
| RUNNING | `EvtError{...}` | ERROR | — | Log error; stop control loop |
| RUNNING | `EvtTick{}` / `EvtTempReached{}` | RUNNING (internal) | — | Process step progression without state change (keeps legacy transitions intact) |
| RUNNING | `EvtSetManualTemp{ temp }` | PROGRAM_TEMP_OVERRIDE | `temp` within safety bounds | Enable override; does not modify `programIsRunning` |
| PAUSED | `EvtResume{}` | RUNNING | — | Resume control loop |
| PAUSED | `EvtCancel{}` | CANCELLED | — | Abort program; safe shutdown |
| PAUSED | `EvtError{...}` | ERROR | — | Log error, safe shutdown |
| COMPLETED | `EvtClearProgram{}` | IDLE | — | Clear loaded profile |
| COMPLETED | `EvtLoadProfile{ Profile }` | LOADED | Profile validity | Allow immediate reload after completion |
| COMPLETED | `EvtError{...}` | ERROR | — | Log error |
| COMPLETED | `EvtSetManualTemp{ temp }` | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning` |
| CANCELLED | `EvtLoadProfile{ Profile }` | LOADED | Profile validity | Allow reload after cancel |
| CANCELLED | `EvtError{...}` | ERROR | — | Log error |
| CANCELLED | `EvtSetManualTemp{ temp }` | MANUAL_TEMP | `temp` within safety bounds | Enter manual control; does not modify `programIsRunning` |
| ERROR | `EvtReset{}` | IDLE | — | System recovery path; clear error flags, clear program |
| ERROR | `EvtLoadProfile{ Profile }` | LOADED | Profile validity | Allow recovery directly to LOADED |
| MANUAL_TEMP | `EvtResume{}` | `LOADED` if profile present; else `IDLE` | — | Disable manual control |
| MANUAL_TEMP | `EvtSetManualTemp{ temp }` | MANUAL_TEMP (internal) | `temp` within safety bounds | Update manual setpoint only |
| MANUAL_TEMP | `EvtError{...}` | ERROR | — | Log and route to error |
| PROGRAM_TEMP_OVERRIDE | `EvtResume{}` | RUNNING | — | Exit override; program continues |
| PROGRAM_TEMP_OVERRIDE | `EvtSetManualTemp{ temp }` | PROGRAM_TEMP_OVERRIDE (internal) | `temp` within safety bounds | Update override setpoint only |
| PROGRAM_TEMP_OVERRIDE | `EvtError{...}` | ERROR | — | Log and route to error |

## Notes

- The mapping above preserves the legacy `myValidTransitions` set in `StateMachine.hpp` (no new outward transitions were introduced).
- `RUNNING → WAITING_FOR_TEMP` is intentionally not defined, as it is not listed in the existing valid transitions.
- Temperature gating is kept as an internal `RUNNING` behavior via `EvtTick`/`EvtTempReached` without a state change.
- Invalid events in any state will be logged and will route to `ERROR` per Step 1 decision.
- Queue overflow routing: if a non-UI event overflows the queue, we will immediately route to `ERROR` (and log). UI overflow will only log and increment the counter.
