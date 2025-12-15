//
// Created by chris on 12/14/25.
//

#ifndef TEST_APP_STATES_HPP
#define TEST_APP_STATES_HPP
#include "etl/fsm.h"
#include "etl/string.h"
#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    //Parent state of all profile states. No profile loaded. If there is one, the on_enter will transition it to loaded..
    class ProfileState : public BaseState, public etl::fsm_state<
                             FurnaceFsm, ProfileState, STATE_PROFILE,
                             EvtModeOff, EvtModeManual,
                             EvtProfileLoad, EvtManualSetTemp, EvtTick, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;
        etl::fsm_state_id_t on_event(EvtModeOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtModeManual const& anEvent);
        etl::fsm_state_id_t on_event(EvtProfileLoad const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtTick const& anEvent);
        etl::fsm_state_id_t onEvent(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;
    };
}

#endif //TEST_APP_STATES_HPP
