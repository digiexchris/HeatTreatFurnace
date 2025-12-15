#ifndef HEATTREATFURNACE_FSM_STATES_IDLESTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_IDLESTATE_HPP

#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"
#include <etl/fsm.h>

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    class OffState : public BaseState, public etl::fsm_state
                     <FurnaceFsm, OffState, STATE_OFF, EvtModeManual, EvtModeProfile, EvtProfileStart,
                      EvtProfileClear, EvtProfileLoad, EvtManualSetTemp,
                      EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtModeManual const& anEvent);
        etl::fsm_state_id_t on_event(EvtModeProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileStart const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileClear const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileLoad const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;

    private:
        static constexpr etl::string_view myDomain = "FSM::Idle";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_IDLESTATE_HPP

