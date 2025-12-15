#include "PausedState.hpp"
#include "StateId.hpp"
#include "FSM/FurnaceFsm.hpp"

namespace HeatTreatFurnace::FSM
{

etl::fsm_state_id_t PausedState::on_enter_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered PAUSED state");
    return No_State_Change;
}

void PausedState::on_exit_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting PAUSED state");
}

etl::fsm_state_id_t PausedState::on_event(EvtResume const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtResume");

    // TODO: Resume profile execution from paused position
    // TODO: Validate conditions are still safe to resume

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Resuming program execution");
    result = static_cast<etl::fsm_state_id_t>(StateId::RUNNING);

    return result;
}

etl::fsm_state_id_t PausedState::on_event(EvtCancel const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtCancel");

    // TODO: Stop profile execution
    // TODO: Begin safe shutdown sequence

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Cancelling program execution");
    result = static_cast<etl::fsm_state_id_t>(StateId::CANCELLED);

    return result;
}

etl::fsm_state_id_t PausedState::on_event(EvtError const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

    result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

    return result;
}

etl::fsm_state_id_t PausedState::on_event_unknown(etl::imessage const& aMsg)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
    result = No_State_Change;

    return result;
}

StateName PausedState::Name() const
{
    return "PausedState";
}

} // namespace HeatTreatFurnace::FSM
