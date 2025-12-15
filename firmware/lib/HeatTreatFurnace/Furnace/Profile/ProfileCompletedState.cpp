#include "ProfileCompletedState.hpp"
#include "../StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ProfileCompletedState::on_enter_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Info, *this, "Entered COMPLETED state");
        return No_State_Change;
    }

    void ProfileCompletedState::on_exit_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Info, *this, "Exiting COMPLETED state");
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtModeOff const& anEvent)
    {
        return STATE_OFF;
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtModeManual const& anEvent)
    {
        return STATE_MANUAL;
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtProfileStart const& anEvent)
    {
        return STATE_PROFILE_RUNNING;
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtProfileClear const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileClear");

        // TODO: Clear program

        fsm.SendLog(Log::LogLevel::Info, *this, "Program cleared, returning to PROFILE");
        return STATE_PROFILE;
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtManualSetTemp const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtManualSetTemp, target: {} C", anEvent.targetTemp);
        return fsm.HandleEvent(anEvent);
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtProfileLoad const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileLoad");
        return fsm.HandleEvent(anEvent);
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtError const& anEvent)
    {
        return STATE_ERROR;
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event_unknown(etl::imessage const& aMsg)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        result = No_State_Change;

        return result;
    }

    etl::fsm_state_id_t ProfileCompletedState::on_event(EvtProfileSetNextSegment const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileSetNextSegment");
        fsm.SetCurrentProfileCurrentSegment(anEvent.segmentIndex, anEvent.segmentTime);
        return STATE_PROFILE_LOADED;
    }

    StateName ProfileCompletedState::Name() const
    {
        return "ProfileCompletedState";
    }
} // namespace HeatTreatFurnace::FSM

