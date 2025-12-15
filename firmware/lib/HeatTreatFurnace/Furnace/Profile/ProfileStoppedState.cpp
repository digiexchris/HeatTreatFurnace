#include "ProfileStoppedState.hpp"
#include "../StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"
#include <etl/fsm.h>

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ProfileStoppedState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered CANCELLED state");
        return No_State_Change;
    }

    void ProfileStoppedState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting CANCELLED state");
    }

    etl::fsm_state_id_t ProfileStoppedState::on_event(EvtProfileLoad const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileLoad");

        // TODO: Load new profile
        // Profile is owned by FurnaceState, not FSM

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile loaded, transitioning to PROFILE");
        result = static_cast<etl::fsm_state_id_t>(StateId::PROFILE);

        return result;
    }

    etl::fsm_state_id_t ProfileStoppedState::on_event(EvtProfileClear const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileClear");

        // TODO: Clear program and cancellation status
        // TODO: Perform any cleanup

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Program cleared, returning to OFF");
        result = static_cast<etl::fsm_state_id_t>(StateId::OFF);

        return result;
    }

    etl::fsm_state_id_t ProfileStoppedState::on_event(EvtError const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

        return result;
    }

    etl::fsm_state_id_t ProfileStoppedState::on_event_unknown(etl::imessage const& aMsg)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        result = No_State_Change;

        return result;
    }

    StateName ProfileStoppedState::Name() const
    {
        return "ProfileStoppedState";
    }
} // namespace HeatTreatFurnace::FSM

