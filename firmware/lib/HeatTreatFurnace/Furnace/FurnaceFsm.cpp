#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    FurnaceFsm::FurnaceFsm(Log::LogService& aLogger)
        : etl::fsm(FURNACE_FSM_ROUTER), Log::Loggable(aLogger), // Router ID 0
          myQueueManager(aLogger),
          myLogger(aLogger)
    {
    }

    void FurnaceFsm::Init()
    {
        set_states(myStatePack);
        start();
    }

    void FurnaceFsm::ProcessQueue()
    {
        myQueueManager.DrainQueue([this](etl::imessage const& aMsg)
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

    bool FurnaceFsm::SetHeaterTarget(float aTargetTemp)
    {
        return true;
    }

    bool FurnaceFsm::IsHeaterOn() const
    {
        return false;
    }

    bool FurnaceFsm::SetHeaterOff()
    {
        return false;
    }

    bool FurnaceFsm::SetHeaterOn()
    {
        return false;
    }

    bool FurnaceFsm::ClearProgram()
    {
        return false;
    }

    FurnaceFsm::ProfileUpdateResult FurnaceFsm::UpdateNextProfileTempTarget()
    {
        return FurnaceFsm::ProfileUpdateResult::ERROR;
    }

    bool FurnaceFsm::SetCurrentProfileCurrentSegment(uint16_t aSegment, std::chrono::seconds aSegmentTimePosition)
    {
        if (aSegment > myCurrentProfile.segments.size())
        {
            //todo LOG
            return false;
        }

        if (aSegmentTimePosition > (myCurrentProfile.segments[aSegment].rampTime + myCurrentProfile.segments[aSegment].dwellTime))
        {
            //todo LOG
            return false;
        }

        myCurrentProfile.currentSegment = aSegment;
        myCurrentProfile.currentSegmentTime = aSegmentTimePosition;
        return true;
    }
} // namespace HeatTreatFurnace::FSM

