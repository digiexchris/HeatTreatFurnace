/**
 * @file ErrorState.hpp
 * @brief FSM state for error condition handling
 *
 * The ERROR state represents a fault condition that requires user
 * intervention. The furnace has stopped operation and requires
 * acknowledgement before returning to normal operation.
 * Depending on error severity, may transition to idle or require reset.
 */

#ifndef HEATTREATFURNACE_FSM_STATES_ERRORSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_ERRORSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "Furnace/Events.hpp"
#include "StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    /**
     * @brief FSM state representing error condition
     *
     * Handles error reset and recovery, or loading a profile directly.
     * Monitors for additional errors and performs safe shutdown procedures.
     */
    class ErrorState : public etl::fsm_state<FurnaceFsm, ErrorState, STATE_ERROR,
                                             EvtReset, EvtLoadProfile, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtReset const& anEvent);
        etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::Error";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_ERRORSTATE_HPP

