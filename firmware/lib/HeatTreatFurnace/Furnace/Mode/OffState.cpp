#include "OffState.hpp"

#include "OffState.hpp"
#include "../StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t OffState::on_enter_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterOff();
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered OFF state");
        return No_State_Change;
    }

    void OffState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting OFF state");
    }

    // //ie resume stopped or loaded fresh profile
    // etl::fsm_state_id_t OffState::on_event(EvtProfileStart const& anEvent)
    // {
    //     etl::fsm_state_id_t result = No_State_Change;
    //
    //     get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileStart");
    //
    //     //todo:
    //     // if had a loaded program: transition to running
    //     // else no_state_change and log it.
    //     return STATE_PROFILE_RUNNING;
    // }
    //
    // etl::fsm_state_id_t OffState::on_event(EvtProfileClear const& anEvent)
    // {
    //     etl::fsm_state_id_t result = No_State_Change;
    //
    //     get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileStart");
    //
    //     //todo:
    //     // if had a loaded program: clear it
    //     return No_State_Change;
    // }
    //
    // etl::fsm_state_id_t OffState::on_event(EvtProfileLoad const& anEvent)
    // {
    //     etl::fsm_state_id_t result = No_State_Change;
    //
    //     get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileStart");
    //
    //     //todo:
    //     // load the profile
    //     return STATE_PROFILE_LOADED;
    // }

    etl::fsm_state_id_t OffState::on_event(EvtModeManual const& anEvent)
    {
        return STATE_MANUAL;
    }

    etl::fsm_state_id_t OffState::on_event(EvtModeProfile const& anEvent)
    {
        return STATE_PROFILE;
    }

    etl::fsm_state_id_t OffState::on_event(EvtError const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        return STATE_ERROR;
    }

    etl::fsm_state_id_t OffState::on_event_unknown(etl::imessage const& aMsg)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        return No_State_Change;
    }

    StateName OffState::Name() const
    {
        return "OffState";
    }
} // namespace HeatTreatFurnace::FSM

