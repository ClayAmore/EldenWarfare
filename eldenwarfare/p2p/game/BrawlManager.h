#pragma once
#include "Player.h"
#include "Menu.h"
#include "Death.h"
#include "Timer.h"
#include "SpEffect.h"
#include "Seamless.h"
#include "Item.h"
#include "Settings.h"

namespace P2P {
    class BrawlManager {
    public: 
        enum Result {
            VICTORY,
            DEFEAT,
            STALEMATE
        };
        void PrepareMap();
        bool StartMatch(uint32_t duration);
        void OnKillUpdate();
        bool EndMatch(Result);
        void CancelMatch();
    };
}   