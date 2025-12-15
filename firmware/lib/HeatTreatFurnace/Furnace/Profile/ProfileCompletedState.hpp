#ifndef HEATTREATFURNACE_FSM_STATES_COMPLETEDSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_COMPLETEDSTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    /**
     * @brief FSM state representing successfully completed profile execution
     *
     * Handles clearing program to return to idle state, loading a new profile,
     * or entering manual temperature mode. Monitors for error conditions.
     */
    class ProfileCompletedState : public BaseState, public etl::fsm_state
                                  <FurnaceFsm, ProfileCompletedState, STATE_PROFILE_COMPLETED,
                                   EvtModeOff, EvtModeManual, EvtProfileLoad, EvtProfileStart,
                                   EvtProfileSetNextSegment, EvtProfileClear, EvtManualSetTemp,
                                   EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtModeOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtModeManual const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileLoad const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileStart const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileSetNextSegment const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileClear const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;

    private:
        static constexpr etl::string_view myDomain = "FSM::Completed";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_COMPLETEDSTATE_HPP

