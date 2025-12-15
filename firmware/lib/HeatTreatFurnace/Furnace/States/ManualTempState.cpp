#include "ManualTempState.hpp"
#include "StateId.hpp"
#include "FSM/FurnaceFsm.hpp"

namespace HeatTreatFurnace::FSM
{

etl::fsm_state_id_t ManualTempState::on_enter_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered MANUAL_TEMP state");
    return No_State_Change;
}

void ManualTempState::on_exit_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting MANUAL_TEMP state");
}

etl::fsm_state_id_t ManualTempState::on_event(EvtResume const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtResume");

    // TODO: Stop manual temperature control
    // TODO: Check if profile is present
    // TODO: Return to LOADED if profile present, else IDLE

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Disabling manual control, returning to IDLE");
    result = static_cast<etl::fsm_state_id_t>(StateId::IDLE);

    return result;
}

etl::fsm_state_id_t ManualTempState::on_event(EvtSetManualTemp const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtSetManualTemp, new target: {} C", anEvent.targetTemp);

    // TODO: Update manual temperature setpoint
    // TODO: Apply new temperature control setpoint

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Manual temperature updated");
    result = No_State_Change;

    return result;
}

etl::fsm_state_id_t ManualTempState::on_event(EvtError const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

    result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

    return result;
}

etl::fsm_state_id_t ManualTempState::on_event_unknown(etl::imessage const& aMsg)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
    result = No_State_Change;

    return result;
}

StateName ManualTempState::Name() const
{
    return "ManualTempState";
}

} // namespace HeatTreatFurnace::FSM

