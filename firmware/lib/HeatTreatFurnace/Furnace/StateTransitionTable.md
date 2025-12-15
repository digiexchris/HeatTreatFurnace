
# State Transition Table

This document defines the complete state transition table for the ETL-based queued FSM implementation.

## State Transition Matrix

| State                 | Event                       | Next State                               | Guards/Preconditions                              | Side effects / Notes                                     |
|-----------------------|-----------------------------|------------------------------------------|---------------------------------------------------|----------------------------------------------------------|
| IDLE                  | `EvtLoadProfile{ Profile }` | LOADED                                   | Profile validity check (non-empty, constraints)   | Log "Loaded" (profile ownership external)                |
| IDLE                  | `EvtSetManualTemp{ temp }`  | MANUAL_TEMP                              | `temp` within safety bounds                       | Enter manual control; does not modify `programIsRunning` |
| IDLE                  | `EvtError{...}`             | ERROR                                    | —                                                 | Log error details                                        |
| LOADED                | `EvtStart{}`                | RUNNING                                  | Safety OK (interlocks, sensors), controller ready | Start control loop; initialize timers/logging            |
| LOADED                | `EvtLoadProfile{ Profile }` | LOADED                                   | Profile validity                                  | Replace previously loaded profile; log replace           |
| LOADED                | `EvtError{...}`             | ERROR                                    | —                                                 | Log error details                                        |
| LOADED                | `EvtClearProgram{}`         | IDLE                                     | —                                                 | Log; profile clearing handled by owner                   |
| LOADED                | `EvtSetManualTemp{ temp }`  | MANUAL_TEMP                              | `temp` within safety bounds                       | Enter manual control; does not modify `programIsRunning` |
| RUNNING               | `EvtPause{}`                | PAUSED                                   | —                                                 | Suspend profile, maintain current temp                   |
| RUNNING               | `EvtComplete{}`             | COMPLETED                                | Profile complete condition                        | Stop control loop; safe shutdown                         |
| RUNNING               | `EvtCancel{}`               | CANCELLED                                | —                                                 | Abort program; safe shutdown                             |
| RUNNING               | `EvtError{...}`             | ERROR                                    | —                                                 | Log error; stop control loop                             |
| RUNNING               | `EvtSetManualTemp{ temp }`  | PROFILE_TEMP_OVERRIDE                    | `temp` within safety bounds                       | Hold temp at manual set point                            |
| PAUSED                | `EvtResume{}`               | RUNNING                                  | —                                                 | Resume profile                                           |
| PAUSED                | `EvtCancel{}`               | CANCELLED                                | —                                                 | Abort program; safe shutdown                             |
| PAUSED                | `EvtError{...}`             | ERROR                                    | —                                                 | Log error, safe shutdown                                 |
| COMPLETED             | `EvtClearProgram{}`         | IDLE                                     | —                                                 | Log; profile clearing handled by owner                   |
| COMPLETED             | `EvtLoadProfile{ Profile }` | LOADED                                   | Profile validity                                  | Loads new program                                        |
| COMPLETED             | `EvtError{...}`             | ERROR                                    | —                                                 | Log error                                                |
| COMPLETED             | `EvtSetManualTemp{ temp }`  | MANUAL_TEMP                              | `temp` within safety bounds                       | Hold temp at manual set point                            |
| CANCELLED             | `EvtLoadProfile{ Profile }` | LOADED                                   | Profile validity                                  | Allow load new profile after cancel                      |
| CANCELLED             | `EvtClearProgram{}`         | IDLE                                     | —                                                 | Log; profile clearing handled by owner                   |
| CANCELLED             | `EvtError{...}`             | ERROR                                    | —                                                 | Log error                                                |
| ERROR                 | `EvtReset{}`                | IDLE                                     | —                                                 | System recovery path; clear error flags, clear program   |
| ERROR                 | `EvtLoadProfile{ Profile }` | LOADED                                   | Profile validity                                  | Allow recovery directly to LOADED                        |
| MANUAL_TEMP           | `EvtResume{}`               | `LOADED` if profile present; else `IDLE` | —                                                 | Disable manual control                                   |
| MANUAL_TEMP           | `EvtSetManualTemp{ temp }`  | MANUAL_TEMP (internal)                   | `temp` within safety bounds                       | Update manual setpoint only                              |
| MANUAL_TEMP           | `EvtError{...}`             | ERROR                                    | —                                                 | Log and route to error                                   |
| PROFILE_TEMP_OVERRIDE | `EvtResume{}`               | RUNNING                                  | —                                                 | Exit override; program continues                         |
| PROFILE_TEMP_OVERRIDE | `EvtSetManualTemp{ temp }`  | PROFILE_TEMP_OVERRIDE (internal)         | `temp` within safety bounds                       | Update override setpoint only                            |
| PROFILE_TEMP_OVERRIDE | `EvtError{...}`             | ERROR                                    | —                                                 | Log and route to error                                   |

## Notes

- Invalid events in any state will be logged and will route to `ERROR`.
- Queue overflow routing: if a non-UI event overflows the queue, we will immediately route to `ERROR` (and log). UI overflow will only log.
