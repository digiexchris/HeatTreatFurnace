#ifndef HEATTREATFURNACE_FSM_STATES_LOADEDSTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_LOADEDSTATE_HPP

#include <etl/fsm.h>
#include "Furnace/Events.hpp"
#include "../StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    class ProfileLoadedState : public BaseState, public etl::fsm_state
                               <FurnaceFsm, ProfileLoadedState, STATE_PROFILE_LOADED,
                                EvtProfileStart, EvtProfileLoad, EvtProfileClear,
                                EvtManualSetTemp, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtProfileStart const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileLoad const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileClear const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;

    private:
        static constexpr etl::string_view myDomain = "FSM::Loaded";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_LOADEDSTATE_HPP

