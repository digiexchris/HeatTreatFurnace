#ifndef HEATTREATFURNACE_FSM_STATES_IDLESTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_IDLESTATE_HPP

#include "Furnace/Events.hpp"
#include "StateId.hpp"
#include <etl/fsm.h>

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    class IdleState : public etl::fsm_state<FurnaceFsm, IdleState, STATE_IDLE,
                                            EvtLoadProfile, EvtSetManualTemp, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::Idle";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_IDLESTATE_HPP

