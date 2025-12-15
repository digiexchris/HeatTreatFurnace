#ifndef HEATTREATFURNACE_FSM_STATES_RUNNINGSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_RUNNINGSTATE_HPP

#include <etl/fsm.h>
#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    /**
     * @brief FSM state representing active furnace operation
     *
     * Handles events during active profile execution including pause,
     * cancel, profile completion, and error conditions. Processes
     * periodic ticks for temperature control and profile progression.
     */
    class ProfileRunningState : public BaseState, public etl::fsm_state
                                <FurnaceFsm, ProfileRunningState, STATE_PROFILE_RUNNING,
                                 EvtModeOff, EvtModeManual, EvtManualSetTemp, EvtProfileStop,
                                 EvtProfileClear, EvtProfileSetNextSegment, EvtTick, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtModeOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtModeManual const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileStop const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileClear const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileSetNextSegment const& anEvent);
        etl::fsm_state_id_t on_event(EvtTick const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_RUNNINGSTATE_HPP

