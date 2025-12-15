#ifndef HEATTREATFURNACE_FSM_STATES_LOADEDSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_LOADEDSTATE_HPP

#include <etl/fsm.h>
#include "Furnace/Events.hpp"
#include "StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    class LoadedState : public etl::fsm_state<FurnaceFsm, LoadedState, STATE_LOADED,
                                              EvtStart, EvtLoadProfile, EvtClearProgram,
                                              EvtSetManualTemp, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtStart const& anEvent);
        etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::Loaded";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_LOADEDSTATE_HPP

