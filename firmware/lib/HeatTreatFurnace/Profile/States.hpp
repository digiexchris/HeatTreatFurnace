//
// Created by chris on 12/14/25.
//

#ifndef TEST_APP_STATES_HPP
#define TEST_APP_STATES_HPP
#include "etl/fsm.h"
#include "etl/string.h"
#include "Events.hpp"

namespace HeatTreatFurnace::Furnace
{
    struct EvtClearProgram;
}

namespace HeatTreatFurnace::Profile
{
    class ProfileFsm;


    constexpr size_t MAX_STATE_ID_NAME_LENGTH = 16;

    using StateName = etl::string<MAX_STATE_ID_NAME_LENGTH>;

    enum class StateId : uint8_t
    {
        IDLE, //not running
        RUNNING, //profile loaded, advancing the program each tick
        NUM_STATES
    };

    constexpr etl::fsm_state_id_t STATE_IDLE = static_cast<etl::fsm_state_id_t>(StateId::IDLE);
    constexpr etl::fsm_state_id_t STATE_RUNNING = static_cast<etl::fsm_state_id_t>(StateId::RUNNING);

    class IdleState : public etl::fsm_state<ProfileFsm, IdleState, STATE_IDLE,
                                            EvtLoadProfile, EvtStart, EvtClearProgram, EvtSetNextSegment, EvtTick>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtStart const& anEvent);
        etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetNextSegment const& anEvent);
        etl::fsm_state_id_t on_event(EvtTick const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::Idle";
    };

    class RunningState : public etl::fsm_state<ProfileFsm, RunningState, STATE_RUNNING,
                                               EvtStop, EvtSetNextSegment, EvtTick>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtStop const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetNextSegment const& anEvent);
        etl::fsm_state_id_t on_event(EvtTick const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;
    };
}

#endif //TEST_APP_STATES_HPP
