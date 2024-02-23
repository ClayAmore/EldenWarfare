#pragma once
#include "Player.h"
#include "Menu.h"
#include "Seamless.h"
#include "Death.h"
#include "Timer.h"
#include "SpEffect.h"
#include "Item.h"
#include "Settings.h"

namespace P2P {
    class TeamFightManager {
    public:
        enum Result {
            VICTORY,
            DEFEAT,
            STALEMATE
        };
        bool CheckIfTeamPicked();
        void PickingTeam();
        void DisplayPickTeamMessage();
        void PrepareMap();
        bool StartMatch(uint32_t duration);
        void OnKillUpdate();
        void OnKillUpdate(Seamless::Team selfTeam, Seamless::Team victimTeam);
        bool EndMatch(Result result);
        void CancelMatch();
    };
}