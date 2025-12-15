#include "CancelledState.hpp"
#include "StateId.hpp"
#include "FSM/FurnaceFsm.hpp"
#include <etl/fsm.h>

namespace HeatTreatFurnace::FSM
{
etl::fsm_state_id_t CancelledState::on_enter_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this,"Entered CANCELLED state");
    return No_State_Change;
}

void CancelledState::on_exit_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting CANCELLED state");
}

etl::fsm_state_id_t CancelledState::on_event(EvtLoadProfile const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtLoadProfile");

    // TODO: Load new profile
    // Profile is owned by FurnaceState, not FSM

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile loaded, transitioning to LOADED");
    result = static_cast<etl::fsm_state_id_t>(StateId::LOADED);

    return result;
}

etl::fsm_state_id_t CancelledState::on_event(EvtClearProgram const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtClearProgram");

    // TODO: Clear program and cancellation status
    // TODO: Perform any cleanup

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Program cleared, returning to IDLE");
    result = static_cast<etl::fsm_state_id_t>(StateId::IDLE);

    return result;
}

etl::fsm_state_id_t CancelledState::on_event(EvtError const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

    result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

    return result;
}

etl::fsm_state_id_t CancelledState::on_event_unknown(etl::imessage const& aMsg)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
    result = No_State_Change;

    return result;
}

StateName CancelledState::Name() const
{
    return "CancelledState";
}

} // namespace HeatTreatFurnace::FSM

