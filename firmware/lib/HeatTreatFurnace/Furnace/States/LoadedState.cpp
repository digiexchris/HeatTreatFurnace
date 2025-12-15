#include "LoadedState.hpp"
#include "Furnace/FurnaceFsm.hpp"
#include "Furnace/Events.hpp"
#include "StateId.hpp"
#include "Log/LogService.hpp"


namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t LoadedState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered LOADED state");
        return No_State_Change;
    }

    void LoadedState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting LOADED state");
    }

    etl::fsm_state_id_t LoadedState::on_event(EvtStart const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtStart");

        // TODO: Validate safety conditions before starting
        // TODO: Check profile is valid
        // TODO: Perform any pre-start initialization

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Starting program execution");
        result = static_cast<etl::fsm_state_id_t>(StateId::RUNNING);

        return result;
    }

    etl::fsm_state_id_t LoadedState::on_event(EvtLoadProfile const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtLoadProfile, replacing current profile");

        // TODO: Replace the loaded profile with new profile
        // Profile is owned by FurnaceState, not FSM

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile reloaded");
        result = No_State_Change;

        return result;
    }

    etl::fsm_state_id_t LoadedState::on_event(EvtClearProgram const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtClearProgram");

        // TODO: Clear the loaded profile

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Program cleared, returning to IDLE");
        result = static_cast<etl::fsm_state_id_t>(StateId::IDLE);

        return result;
    }

    etl::fsm_state_id_t LoadedState::on_event(EvtSetManualTemp const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtSetManualTemp, target: {} C", anEvent.targetTemp);

        // TODO: Store manual temperature setpoint
        // TODO: Transition to manual temperature control mode

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Transitioning to MANUAL_TEMP mode");
        result = static_cast<etl::fsm_state_id_t>(StateId::MANUAL_TEMP);

        return result;
    }

    etl::fsm_state_id_t LoadedState::on_event(EvtError const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

        return result;
    }

    etl::fsm_state_id_t LoadedState::on_event_unknown(etl::imessage const& aMsg)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        result = No_State_Change;

        return result;
    }

    StateName LoadedState::Name() const
    {
        return "LoadedState";
    }
} // namespace HeatTreatFurnace::FSM

