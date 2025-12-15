#include "ProfileTempOverrideState.hpp"
#include "StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ProfileTempOverrideState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered PROFILE_TEMP_OVERRIDE state");
        return No_State_Change;
    }

    void ProfileTempOverrideState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting PROFILE_TEMP_OVERRIDE state");
    }

    etl::fsm_state_id_t ProfileTempOverrideState::on_event(EvtResume const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtResume");

        // TODO: Clear override temperature
        // TODO: Resume profile-based temperature control

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Override cleared, resuming profile control");
        result = static_cast<etl::fsm_state_id_t>(StateId::RUNNING);

        return result;
    }

    etl::fsm_state_id_t ProfileTempOverrideState::on_event(EvtSetManualTemp const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtSetManualTemp, new target: {} C", anEvent.targetTemp);

        // TODO: Update override temperature setpoint
        // TODO: Apply new temperature control setpoint

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Override temperature updated");
        result = No_State_Change;

        return result;
    }

    etl::fsm_state_id_t ProfileTempOverrideState::on_event(EvtCancel const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtCancel");

        // TODO: Stop profile execution
        // TODO: Begin safe shutdown sequence

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Cancelling program execution");
        result = static_cast<etl::fsm_state_id_t>(StateId::CANCELLED);

        return result;
    }

    etl::fsm_state_id_t ProfileTempOverrideState::on_event(EvtError const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

        return result;
    }

    etl::fsm_state_id_t ProfileTempOverrideState::on_event_unknown(etl::imessage const& aMsg)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        result = No_State_Change;

        return result;
    }

    StateName ProfileTempOverrideState::Name() const
    {
        return "ProfileTempOverrideState";
    }
} // namespace HeatTreatFurnace::FSM

