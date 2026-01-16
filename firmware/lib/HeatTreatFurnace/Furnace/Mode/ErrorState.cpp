#include "ErrorState.hpp"
#include "Furnace/StateId.hpp"
#include "Furnace/Events.hpp"
#include "Log/LogService.hpp"

#include <etl/fsm.h>

#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFSM;

    etl::fsm_state_id_t ErrorState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Entered ERROR state");
        return No_State_Change;
    }

    void ErrorState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting ERROR state");
    }

    etl::fsm_state_id_t ErrorState::on_event(EvtModeOff const& anEvent)
    {
        return STATE_OFF;
    }

    etl::fsm_state_id_t ErrorState::on_event(EvtModeManual const& anEvent)
    {
        return STATE_MANUAL;
    }

    // etl::fsm_state_id_t ErrorState::on_event(EvtManualSetTemp const& anEvent)
    // {
    //     return get_fsm_context().HandleEvent(anEvent);
    // }

    etl::fsm_state_id_t ErrorState::on_event(EvtModeProfile const& anEvent)
    {
        return STATE_PROFILE;
    }

    // etl::fsm_state_id_t ErrorState::on_event(EvtProfileLoad const& anEvent)
    // {
    //     auto &fsm = get_fsm_context();
    //     fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileLoad");
    //
    //     return fsm.HandleEvent(anEvent);
    // }
    //
    // etl::fsm_state_id_t ErrorState::on_event(EvtProfileClear const& anEvent)
    // {
    // }
    //
    // etl::fsm_state_id_t ErrorState::on_event(EvtProfileSetNextSegment const& anEvent)
    // {
    // }
    //
    // etl::fsm_state_id_t ErrorState::on_event(EvtProfileStart const& anEvent)
    // {
    // }

    etl::fsm_state_id_t ErrorState::on_event(EvtTick const& anEvent)
    {
        return No_State_Change;
    }

    etl::fsm_state_id_t ErrorState::on_event(EvtError const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received additional EvtError: {}", anEvent.msg);

        // TODO: Log additional error
        // TODO: May need to escalate error severity

        result = No_State_Change;

        return result;
    }

    etl::fsm_state_id_t ErrorState::on_event_unknown(etl::imessage const& aMsg)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        result = No_State_Change;

        return result;
    }

    StateName ErrorState::Name() const
    {
        return "ErrorState";
    }
} // namespace HeatTreatFurnace::FSM

