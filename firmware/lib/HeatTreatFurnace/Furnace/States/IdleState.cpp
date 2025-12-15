#include "IdleState.hpp"

#include "FSM/FurnaceFsm.hpp"

namespace HeatTreatFurnace::FSM
{
etl::fsm_state_id_t IdleState::on_enter_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered IDLE state");
    return No_State_Change;
}

void IdleState::on_exit_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Exiting IDLE state");
}

etl::fsm_state_id_t IdleState::on_event(EvtLoadProfile const& anEvent)
{
    // TODO: Validate profile
    // TODO: Store profile reference in FurnaceState
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile loaded, transitioning to LOADED");
    return STATE_LOADED;
}

etl::fsm_state_id_t IdleState::on_event(EvtSetManualTemp const& anEvent)
{
    // TODO: Validate temperature bounds
    // TODO: Set manual temperature control
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Manual temperature requested, transitioning to MANUAL_TEMP");
    return STATE_MANUAL_TEMP;
}

etl::fsm_state_id_t IdleState::on_event(EvtError const& anEvent)
{
    get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Error event received: {}", anEvent.msg.c_str());
    return STATE_ERROR;
}

etl::fsm_state_id_t IdleState::on_event_unknown(etl::imessage const& aMsg)
{
    get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Invalid event {} received in IDLE state, routing to ERROR",
        static_cast<int>(aMsg.get_message_id()));
    return STATE_ERROR;
}

StateName IdleState::Name() const
{
    return "IdleState";
}
} // namespace HeatTreatFurnace::FSM

