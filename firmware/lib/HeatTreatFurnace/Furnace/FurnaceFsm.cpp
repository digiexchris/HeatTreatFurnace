#include "../FurnaceFsm.hpp"

#include "States/States.hpp"

namespace HeatTreatFurnace::FSM
{
    FurnaceFsm::FurnaceFsm(Furnace::FurnaceState& aFurnaceState, Log::LogService& aLogger)
    : etl::fsm(FURNACE_FSM_ROUTER), Log::Loggable(aLogger),  // Router ID 0
      myQueueManager(aLogger),
      myFurnaceState(aFurnaceState),
      myLogger(aLogger)
{
}

void FurnaceFsm::Init()
{
    set_states(myStatePack);
    start();
}

void FurnaceFsm::receive(etl::imessage const& aMsg)
{
    // Override receive to queue messages instead of processing immediately
    myQueueManager.Post(aMsg, EventPriority::Furnace);
}

bool FurnaceFsm::Post(etl::imessage const& aMsg, EventPriority aPriority)
{
    return myQueueManager.Post(aMsg, aPriority);
}

void FurnaceFsm::ProcessQueue()
{
    myQueueManager.DrainQueue([this](etl::imessage& aMsg)
    {
        // Call base class receive to route to state handlers
        etl::fsm::receive(aMsg);
    });
}

StateId FurnaceFsm::GetCurrentState() const noexcept
{
    etl::fsm_state_id_t stateId = get_state_id();
    return static_cast<StateId>(stateId);
}

uint32_t FurnaceFsm::GetOverflowCount() const noexcept
{
    return myQueueManager.GetOverflowCount();
}

} // namespace HeatTreatFurnace::FSM

