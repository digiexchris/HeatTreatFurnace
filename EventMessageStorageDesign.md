
# Event and Message Storage Design

## Overview
This document captures the current design for event/message types and their storage mechanism for the ETL-based queued FSM implementation. It defines event structs, the priority queue storage model, priority handling, thread-safety for posting/draining in task context, and overflow policy.

## Design

### 1. Event Structs
Event structs derive from `etl::imessage`. They represent the different types of events that can be posted to the FSM. No dynamic allocation is used; fixed-capacity ETL strings are preferred over STL strings.
```c++
// Example event structs
struct EvtLoadProfile : public etl::imessage {
  Profile profile; // Provided by owner; FSM does not own/allocate profile
};

struct EvtStart   : public etl::imessage { };
struct EvtPause   : public etl::imessage { };
struct EvtResume  : public etl::imessage { };
struct EvtCancel  : public etl::imessage { };
struct EvtComplete: public etl::imessage { };
struct EvtClearProgram : public etl::imessage { };

struct EvtError : public etl::imessage {
  Error error;
  Domain domain; // e.g., UI, Furnace, StateMachine
  etl::string<96> msg; // Fixed-capacity
};

struct EvtReset : public etl::imessage { };

struct EvtSetManualTemp : public etl::imessage {
  Temperature temp;
};

struct EvtTick : public etl::imessage { };
```

### 2. Storage Model
Use a single bounded priority queue that stores heterogeneous messages via `etl::message_packet<MaxMessageSize>`, without dynamic allocation.

```c++
// Define maximum message size (adjust as needed to fit all events)
constexpr size_t MaxMessageSize = 64U;

// Priority levels
enum class EventPriority : uint8_t { Critical = 0U, Furnace = 1U, UI = 2U };

// Message wrapper with priority and sequence to ensure stability (FIFO within same priority)
struct QueuedMsg {
  EventPriority priority;
  uint32_t      seq;   // monotonic sequence (wraps naturally)
  etl::message_packet<MaxMessageSize> packet;
};

// Comparator: lower enum value = higher priority; earlier seq = higher priority
struct MsgCompare {
  bool operator()(const QueuedMsg& a, const QueuedMsg& b) const noexcept {
    if (a.priority != b.priority) {
      return static_cast<uint8_t>(a.priority) > static_cast<uint8_t>(b.priority);
    }
    return a.seq > b.seq;
  }
};

// Container and priority queue alias (48 total: effectively 16 per priority class)
using PQContainer = etl::vector<QueuedMsg, 48U>;
using EventPriorityQueue = etl::priority_queue<QueuedMsg, PQContainer, MsgCompare>;
```

### 3. Thread-Safety Wrappers (Task Context)
Posting and draining occur in task/context code using a mutex. ISR posting is out of scope for this refactor.

```c++
class EventPoster {
 public:
  void Post(const etl::imessage& msg, EventPriority priority);
  void DrainQueue();

 private:
  EventPriorityQueue queue_;
  std::mutex mutex_;
};
```

### 4. Priority Handling
Priority handling is implemented via `EventPriority`, `QueuedMsg`, and `MsgCompare` in a single `EventPriorityQueue`.

```c++
// See section 2 for the concrete types
```

### 5. Overflow Policy
Define how overflow is handled in the queue.

```c++
// Overflow policy: drop-newest, log error, increment counter
// If Critical or Furnace priority overflows, immediately route to ERROR state
// UI overflow only logs
```

## Notes
- Ensure all event structs are properly aligned and sized to fit within `MaxMessageSize`.
- Verify that the chosen storage model supports all required events without dynamic allocation.
- The design should remain MISRA-compliant.