#include "ProfileLoadedState.hpp"
#include "Furnace/FurnaceFsm.hpp"
#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"
#include "Log/LogService.hpp"


namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ProfileLoadedState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered LOADED state");
        return No_State_Change;
    }

    void ProfileLoadedState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting LOADED state");
    }

    etl::fsm_state_id_t ProfileLoadedState::on_event(EvtProfileStart const& anEvent)
    {
        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileStart");

        return STATE_PROFILE_RUNNING;
    }

    etl::fsm_state_id_t ProfileLoadedState::on_event(EvtProfileLoad const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileLoad, replacing current profile");

        // TODO: Replace the loaded profile with new profile
        // Profile is owned by FurnaceState, not FSM

        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile reloaded");
        result = No_State_Change;

        return result;
    }

    etl::fsm_state_id_t ProfileLoadedState::on_event(EvtProfileClear const& anEvent)
    {
        auto& fsm = get_fsm_context();

        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileClear");

        // TODO: Clear the loaded profile

        fsm.SendLog(Log::LogLevel::Info, *this, "Program cleared, returning to PROFILE");
        return STATE_PROFILE;
    }

    etl::fsm_state_id_t ProfileLoadedState::on_event(EvtManualSetTemp const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtManualSetTemp, target: {} C", anEvent.targetTemp);

        return fsm.HandleEvent(anEvent);
    }

    etl::fsm_state_id_t ProfileLoadedState::on_event(EvtError const& anEvent)
    {
        get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        return STATE_ERROR;
    }

    etl::fsm_state_id_t ProfileLoadedState::on_event_unknown(etl::imessage const& aMsg)
    {
        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));

        return No_State_Change;
    }

    StateName ProfileLoadedState::Name() const
    {
        return "ProfileLoadedState";
    }
} // namespace HeatTreatFurnace::FSM

