#ifndef HEATTREATFURNACE_FSM_STATES_MANUALTEMPSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_MANUALTEMPSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "FSM/Events.hpp"
#include "StateId.hpp"

namespace HeatTreatFurnace::FSM
{
class FurnaceFsm;

/**
 * @brief FSM state representing manual temperature control
 *
 * Handles manual temperature changes, resuming to previous state,
 * and transition to profile-based control. Monitors for error conditions.
 */
class ManualTempState : public etl::fsm_state<FurnaceFsm, ManualTempState, STATE_MANUAL_TEMP,
                                               EvtResume, EvtSetManualTemp, EvtError>
{
public:

    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtResume const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

    [[nodiscard]] StateName Name() const;

private:
    static constexpr etl::string_view myDomain = "FSM::ManualTemp";
};

} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_MANUALTEMPSTATE_HPP

