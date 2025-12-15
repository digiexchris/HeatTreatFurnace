/**
 * @file RunningState.hpp
 * @brief FSM state for active furnace operation
 *
 * The RUNNING state represents active profile execution. The furnace
 * is actively controlling temperature according to the loaded profile.
 * This state handles pause, cancel, completion, and error events.
 */

#ifndef HEATTREATFURNACE_FSM_STATES_RUNNINGSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_RUNNINGSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "Furnace/Events.hpp"
#include "StateId.hpp"

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
    class RunningState : public etl::fsm_state<FurnaceFsm, RunningState, STATE_RUNNING,
                                               EvtPause, EvtCancel, EvtComplete,
                                               EvtSetManualTemp, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtPause const& anEvent);
        etl::fsm_state_id_t on_event(EvtCancel const& anEvent);
        etl::fsm_state_id_t on_event(EvtComplete const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::Running";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_RUNNINGSTATE_HPP

