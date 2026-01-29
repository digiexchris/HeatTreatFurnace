//
// Created by chris on 1/16/26.
//

#ifndef TEST_APP_MANUALON_HPP
#define TEST_APP_MANUALON_HPP


#include "Furnace/Events.hpp"
#include "Furnace/StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    class ManualOnState : public BaseState, public etl::fsm_state
                          <FurnaceFsm, ManualOnState, STATE_MANUAL_ON,
                           EvtModeOff, EvtModeProfile, EvtManualSetTemp, EvtManualSetOff, EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;
        etl::fsm_state_id_t on_event(EvtModeProfile const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtManualSetOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtModeOff const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const override;
    };
}


#endif //TEST_APP_MANUALON_HPP
