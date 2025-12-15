#include "ErrorState.hpp"
#include "StateId.hpp"
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

    etl::fsm_state_id_t ErrorState::on_event(EvtReset const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtReset");

        // TODO: Clear error condition and error flags
        // TODO: Clear program
        // TODO: Perform any error recovery actions

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "System reset, returning to IDLE");
        result = static_cast<etl::fsm_state_id_t>(StateId::IDLE);

        return result;
    }

    etl::fsm_state_id_t ErrorState::on_event(EvtLoadProfile const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtLoadProfile");

        // TODO: Clear error condition
        // TODO: Load new profile
        // Profile is owned by FurnaceState, not FSM

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile loaded, transitioning to LOADED");
        result = static_cast<etl::fsm_state_id_t>(StateId::LOADED);

        return result;
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

