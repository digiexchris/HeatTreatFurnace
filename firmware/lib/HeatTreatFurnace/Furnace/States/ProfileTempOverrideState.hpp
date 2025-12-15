#ifndef HEATTREATFURNACE_FSM_STATES_PROFILETEMPOVERRIDESTATE_HPP
#define HEATTREATFURNACE_FSM_STATES_PROFILETEMPOVERRIDESTATE_HPP

#include <etl/fsm.h>
#include <etl/string_view.h>
#include "Furnace/Events.hpp"
#include "StateId.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;

    /**
     * @brief FSM state representing profile temperature override
     *
     * Handles override temperature changes, clearing override to resume
     * profile control, profile cancellation, and error conditions.
     * Profile time continues to progress.
     */
    class ProfileTempOverrideState : public etl::fsm_state<FurnaceFsm, ProfileTempOverrideState, STATE_PROFILE_TEMP_OVERRIDE,
                                                           EvtResume, EvtSetManualTemp, EvtCancel,
                                                           EvtError>
    {
    public:
        etl::fsm_state_id_t on_enter_state() override;
        void on_exit_state() override;

        etl::fsm_state_id_t on_event(EvtResume const& anEvent);
        etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
        etl::fsm_state_id_t on_event(EvtCancel const& anEvent);
        etl::fsm_state_id_t on_event(EvtError const& anEvent);
        etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);

        [[nodiscard]] StateName Name() const;

    private:
        static constexpr etl::string_view myDomain = "FSM::ProfileTempOverride";
    };
} // namespace HeatTreatFurnace::FSM

#endif // HEATTREATFURNACE_FSM_STATES_PROFILETEMPOVERRIDESTATE_HPP

