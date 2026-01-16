#ifndef HEATTREATFURNACE_FSM_STATES_MANUALTEMPSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_MANUALTEMPSTATE_HPP

#include <etl/fsm.h>
#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    //doesn't accept EvtManualOn or EvtManualSetTemp, because on_enter auto-transitions to ManualOffState
    class ManualState : public BaseState, public etl::fsm_state
                        <FurnaceFsm, ManualState, STATE_MANUAL,
                         EvtModeOff, EvtModeProfile, EvtManualSetOff, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtModeProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtModeOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_MANUALTEMPSTATE_HPP

