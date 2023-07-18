#pragma once
#include <cstdint>
#include <queue>

#define PULSE_MESSAGE_TIMER_KEY "PULSE_MESSAGE_TIMER"

#define PHANTOM_FINGER_RULE "Controller: X/A and L2.\nKeyboard: Space and C"

#define PULSING_MESSAGE_RULE_IV "You need to use the judicators book and pick Rule IV: All wanderers are enemies!"
#define PULSING_MESSAGE_RULE_XX "You need to use the judicators book and pick Rule XX: Some wanderers are enemies!"

#define PULSING_MESSAGE_PICK_TEAM "Please use Judicators book to pick a team!"
#define PULSING_MESSAGE_STARTING_SOON "Starting match..."
#define PULSING_MESSAGE_CEASE_COMBAT "Cease  combat..."
#define PULSING_MESSAGE_VICTORY "Combat ends in your victory!"
#define PULSING_MESSAGE_STALEMATE "Combat ends in a stalemate!"
#define PULSING_MESSAGE_DEFEAT "Combat ends in your loss!"
#define PULSING_MESSAGE_SEPERATING_WORLDS "Seperating worlds..."

class Menu {
public: 
	enum BannerType {
        YouDied = 5,
        HostVanquished = 7,
        BloodyFingerVanquished = 8,
        LostGraceDiscovered = 0xb,
        Unknown14 = 0xc,
        LegendFelled = 2,
        DemigodFelled = 1,
        GreatEnemyFelled = 3,
        EnemyFelled = 4,
        DutyFulfilled = 9,
        MapFound = 0x11,
        GreatRuneRestored = 0x15,
        GodSlain = 0x16,
        DuelistVanquished = 0x17,
        RecusantVanquished = 0x18,
        InvaderVanquished = 0x19,
        Commence = 0xd,
        Stalemate = 0xf,
        Victory = 0xe,
        Defeat = 0x10,
        Uknown6 = 6,
        Uknown10 = 10,
        Uknown0x12 = 0x12,
        Uknown0x13 = 0x13,
        Uknown0x14 = 0x14,
        Uknown0x1e = 0x1e,
        Uknown0x1f = 0x1f,
        DefeatFadeToBlack = 0x20,
        MatchStart = 0x21,
        MatchEnd = 0x22,
        Slain = 0x23,
        Victory2 = 0x24,
        Draw = 0x25,
        Defeat2 = 0x26,
        Uknown0x27 = 0x27
	};

    typedef void DisplayBannerFunction(void* csMenuMapImp, BannerType bannerType);
    typedef uint8_t MenuHookFunction(void* param1, void* param2, uint8_t param3, uint8_t param4);
    typedef uint8_t* DisplayBlinkingMessageFunction(uint16_t param1, uint8_t param2, const WCHAR** param3);
    typedef void EventMessageFunctions(uint64_t param1, uint32_t param2, uint8_t* csEmkEventIns);
    
    static std::queue<std::string> PulseMessages;
	static MenuHookFunction* MenuHookOriginal;
    static EventMessageFunctions* EventMessageHookOriginal;
    static DisplayBlinkingMessageFunction* DisplayBlinkingMessageOriginal;

    static void EnqueueUnique(const std::string& message);
    static void RemoveIfFirst(const std::string& message);

    static void DisplayBanner(BannerType bannerType);
};

inline void Menu::EnqueueUnique(const std::string& message) {
    if (PulseMessages.empty() || PulseMessages.front() != message) {
        PulseMessages.push(message);
    }
}

inline void Menu::RemoveIfFirst(const std::string& message) {
    if (PulseMessages.front() == message) PulseMessages.pop();
}

inline void Menu::DisplayBanner(Menu::BannerType bannerType) {
    DisplayBannerFunction* displayBannerF = (DisplayBannerFunction*)Global::DISPLAY_BANNER_FUNCTION.ptr();
    if (displayBannerF == nullptr) {
        printf("displayBannerF is null\n");
        return;
    }
    void* CSMenuManImp = Global::CS_MENU_MAN_IMP.ptr();
    if (CSMenuManImp == nullptr) {
        printf("CSMenuManImp is null\n");
        return;
    }
    displayBannerF(CSMenuManImp, bannerType);
}