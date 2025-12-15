/**
 * @file CompletedState.hpp
 * @brief FSM state for successfully completed furnace operation
 *
 * The COMPLETED state represents successful profile execution completion.
 * The furnace has finished the programmed heat treatment cycle.
 * User can acknowledge and return to idle, or load a new profile.
 */

#ifndef HEATTREATFURNACE_FSM_STATES_COMPLETEDSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_COMPLETEDSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "Furnace/Events.hpp"
#include "StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    /**
     * @brief FSM state representing successfully completed profile execution
     *
     * Handles clearing program to return to idle state, loading a new profile,
     * or entering manual temperature mode. Monitors for error conditions.
     */
    class CompletedState : public etl::fsm_state<FurnaceFsm, CompletedState, STATE_COMPLETED,
                                                 EvtClearProgram, EvtLoadProfile, EvtSetManualTemp,
                                                 EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
        etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::Completed";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_COMPLETEDSTATE_HPP

