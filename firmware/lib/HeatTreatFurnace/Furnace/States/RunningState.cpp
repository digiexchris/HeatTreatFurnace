#include "RunningState.hpp"
#include "StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t RunningState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered RUNNING state");
        return No_State_Change;
    }

    void RunningState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting RUNNING state");
    }

    etl::fsm_state_id_t RunningState::on_event(EvtPause const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtPause");

        // TODO: Pause profile execution
        // TODO: Maintain current temperature setpoint

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Pausing program execution");
        result = static_cast<etl::fsm_state_id_t>(StateId::PAUSED);

        return result;
    }

    etl::fsm_state_id_t RunningState::on_event(EvtCancel const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtCancel");

        // TODO: Stop profile execution
        // TODO: Begin safe shutdown sequence

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Cancelling program execution");
        result = static_cast<etl::fsm_state_id_t>(StateId::CANCELLED);

        return result;
    }

    etl::fsm_state_id_t RunningState::on_event(EvtComplete const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtComplete");

        // TODO: Profile has completed successfully
        // TODO: Perform any post-completion actions

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile execution completed");
        result = static_cast<etl::fsm_state_id_t>(StateId::COMPLETED);

        return result;
    }

    etl::fsm_state_id_t RunningState::on_event(EvtSetManualTemp const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtSetManualTemp, target: {} C", anEvent.targetTemp);

        // TODO: Store override temperature
        // TODO: Switch to override control mode

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Transitioning to PROFILE_TEMP_OVERRIDE mode");
        result = static_cast<etl::fsm_state_id_t>(StateId::PROFILE_TEMP_OVERRIDE);

        return result;
    }

    etl::fsm_state_id_t RunningState::on_event(EvtError const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

        return result;
    }

    etl::fsm_state_id_t RunningState::on_event_unknown(etl::imessage const& aMsg)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        result = No_State_Change;

        return result;
    }

    StateName RunningState::Name() const
    {
        return "RunningState";
    }
} // namespace HeatTreatFurnace::FSM

