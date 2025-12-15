#ifndef HEATTREATFURNACE_FSM_STATES_PAUSEDSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_PAUSEDSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "FSM/Events.hpp"
#include "StateId.hpp"

namespace HeatTreatFurnace::FSM
{
class FurnaceFsm;

/**
 * @brief FSM state representing paused profile execution
 *
 * Handles resume, cancel, and error events while maintaining
 * current temperature control. Profile time progression is halted.
 */
class PausedState : public etl::fsm_state<FurnaceFsm, PausedState, STATE_PAUSED,
                                           EvtResume, EvtCancel, EvtError>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtResume const& anEvent);
    etl::fsm_state_id_t on_event(EvtCancel const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

    [[nodiscard]] StateName Name() const;

private:
    static constexpr etl::string_view myDomain = "FSM::Paused";
};

} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_PAUSEDSTATE_HPP

