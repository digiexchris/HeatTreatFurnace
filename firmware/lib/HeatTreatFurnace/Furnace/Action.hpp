#pragma once

namespace HeatTreatFurnace::Furnace
{
    enum class ActionId
    {
        LOAD_PROFILE,
        CLEAR_PROFILE,
        START_PROFILE,
        PAUSE_PROFILE,
        RESUME_PROFILE,
        STOP_PROFILE,
        COMPLETE_PROFILE,
        RESTART
    };

    const std::map<StateId, std::set<ActionId>> validStateActions = {
        {StateId::IDLE, {ActionId::LOAD_PROFILE, ActionId::RESTART, ActionId::STOP_PROFILE}},
        {StateId::LOADED, {ActionId::CLEAR_PROFILE, ActionId::LOAD_PROFILE, ActionId::RESTART, ActionId::START_PROFILE, ActionId::STOP_PROFILE}},
        {StateId::RUNNING,
         {ActionId::RESTART, ActionId::PAUSE_PROFILE, ActionId::COMPLETE_PROFILE, ActionId::STOP_PROFILE}},
        {StateId::PAUSED, {ActionId::RESTART, ActionId::RESUME_PROFILE, ActionId::STOP_PROFILE}},
        {StateId::COMPLETED, {ActionId::CLEAR_PROFILE, ActionId::LOAD_PROFILE, ActionId::RESTART, ActionId::START_PROFILE, ActionId::STOP_PROFILE}},
        {StateId::CANCELLED, {ActionId::CLEAR_PROFILE, ActionId::LOAD_PROFILE, ActionId::RESTART, ActionId::START_PROFILE, ActionId::STOP_PROFILE}},
        {StateId::ERROR, {ActionId::CLEAR_PROFILE, ActionId::LOAD_PROFILE, ActionId::START_PROFILE, ActionId::STOP_PROFILE}},
        {StateId::WAITING_FOR_TEMP,
         {ActionId::PAUSE_PROFILE, ActionId::STOP_PROFILE}}
    };

    class ActionBase
    {
    public:
        ActionBase();
    };
} //namespace HeatTreatFurnace