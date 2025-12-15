/**
 * @file CancelledState.hpp
 * @brief FSM state for cancelled furnace operation
 *
 * The CANCELLED state represents user-cancelled profile execution.
 * The furnace has stopped the programmed heat treatment cycle
 * and is performing safe shutdown. User can acknowledge and return
 * to idle, or load a new profile.
 */

#ifndef HEATTREATFURNACE_FSM_STATES_CANCELLEDSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_CANCELLEDSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "FSM/Events.hpp"
#include "StateId.hpp"
#include "Log/LogService.hpp"

namespace HeatTreatFurnace::FSM
{
class FurnaceFsm;

/**
 * @brief FSM state representing cancelled profile execution
 *
 * Handles clearing program to return to idle state, or loading a new
 * profile. Monitors for error conditions during post-cancellation.
 */
class CancelledState : public etl::fsm_state<FurnaceFsm, CancelledState, STATE_CANCELLED,
                                              EvtLoadProfile, EvtClearProgram, EvtError>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
    etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

    [[nodiscard]] StateName Name() const;

private:
    static constexpr etl::string_view myDomain = "FSM::Cancelled";
};

} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_CANCELLEDSTATE_HPP

