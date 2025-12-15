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
        OFF,
        ERROR, //sub-state of OFF mode. Stores the error that sent us here.
        PROFILE,
        PROFILE_LOADED,
        PROFILE_RUNNING,
        PROFILE_COMPLETED,
        PROFILE_STOPPED,
        MANUAL,
        NUM_STATES
    };

    constexpr etl::fsm_state_id_t STATE_OFF = static_cast<etl::fsm_state_id_t>(StateId::OFF);
    constexpr etl::fsm_state_id_t STATE_ERROR = static_cast<etl::fsm_state_id_t>(StateId::ERROR);
    constexpr etl::fsm_state_id_t STATE_PROFILE = static_cast<etl::fsm_state_id_t>(StateId::PROFILE);
    constexpr etl::fsm_state_id_t STATE_PROFILE_LOADED = static_cast<etl::fsm_state_id_t>(StateId::PROFILE_LOADED);
    constexpr etl::fsm_state_id_t STATE_PROFILE_RUNNING = static_cast<etl::fsm_state_id_t>(StateId::PROFILE_RUNNING);
    constexpr etl::fsm_state_id_t STATE_PROFILE_COMPLETED = static_cast<etl::fsm_state_id_t>(StateId::PROFILE_COMPLETED);
    constexpr etl::fsm_state_id_t STATE_PROFILE_STOPPED = static_cast<etl::fsm_state_id_t>(StateId::PROFILE_STOPPED);
    constexpr etl::fsm_state_id_t STATE_MANUAL = static_cast<etl::fsm_state_id_t>(StateId::MANUAL);

    class BaseState
    {
    public:
        virtual ~BaseState() = default;
        [[nodiscard]] virtual StateName Name() const = 0;
    };
} //namespace HeatTreatFurnace::Furnace

#endif //HEAT_TREAT_FURNACE_STATE_HPP
