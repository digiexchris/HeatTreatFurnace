#ifndef HEAT_TREAT_FURNACE_STATE_HPP
#define HEAT_TREAT_FURNACE_STATE_HPP

#include "etl/fsm.h"
#include "etl/string.h"

namespace HeatTreatFurnace::Furnace
{
    constexpr size_t MAX_STATE_ID_NAME_LENGTH = 24;

    using StateName = etl::string<MAX_STATE_ID_NAME_LENGTH>;

    enum class StateId : uint8_t
    {
        IDLE,
        LOADED,
        RUNNING,
        PAUSED,
        COMPLETED,
        CANCELLED,
        ERROR,
        MANUAL_TEMP,
        PROFILE_TEMP_OVERRIDE,
        NUM_STATES
    };

    constexpr etl::fsm_state_id_t STATE_RUNNING = static_cast<etl::fsm_state_id_t>(StateId::RUNNING);
    constexpr etl::fsm_state_id_t STATE_PROFILE_TEMP_OVERRIDE = static_cast<etl::fsm_state_id_t>(StateId::PROFILE_TEMP_OVERRIDE);
    constexpr etl::fsm_state_id_t STATE_PAUSED = static_cast<etl::fsm_state_id_t>(StateId::PAUSED);
    constexpr etl::fsm_state_id_t STATE_MANUAL_TEMP = static_cast<etl::fsm_state_id_t>(StateId::MANUAL_TEMP);
    constexpr etl::fsm_state_id_t STATE_LOADED = static_cast<etl::fsm_state_id_t>(StateId::LOADED);
    constexpr etl::fsm_state_id_t STATE_IDLE = static_cast<etl::fsm_state_id_t>(StateId::IDLE);
    constexpr etl::fsm_state_id_t STATE_ERROR = static_cast<etl::fsm_state_id_t>(StateId::ERROR);
    constexpr etl::fsm_state_id_t STATE_COMPLETED = static_cast<etl::fsm_state_id_t>(StateId::COMPLETED);
    constexpr etl::fsm_state_id_t STATE_CANCELLED = static_cast<etl::fsm_state_id_t>(StateId::CANCELLED);
} //namespace fsm

#endif //HEAT_TREAT_FURNACE_STATE_HPP
