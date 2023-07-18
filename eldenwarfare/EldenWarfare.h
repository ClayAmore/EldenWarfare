#pragma once
#include "IRenderCallback.h"
#include "OverlayFramework.h"
#include <queue>
#include "Global.h"
#include "Pointer.h"
#include "P2PLobby.h"
#include "ParamEditor.h"
#include "Settings.h"
#include "EquipParamGoods.h"
#include "FeTextEffectParam.h"
#include "Item.h"
#include "HookSetup.h"
#include "Text.h"
#include "Player.h"
#include "Menu.h"
#include "Damage.h"
#include "Death.h"
#include "RTTIScanner.h"
#include "VFTHook.h"

// For error dialog if game is running without Seamless
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISMODULE ((HINSTANCE)&__ImageBase)

#define MAX_MESSAGE_LENGTH 258

using namespace GameItems;
using namespace P2P;

class EldenWarfare : public IRenderCallback
{
public:
	void Setup();
	void Render();
	EldenWarfare(char path[2048]) {
		std::string s(path);
		dllPath = s.substr(0, s.find_last_of("\\/"));
	}


	static VFTHook* enemyInsHook;
	static VFTHook* testHook;
	static uint32_t LastPlayerToHitMe;
	static Mode RequestedMode;
	static bool IsItemTeleporting;
	static bool InMatch;

	static uint8_t MenuHook(void* param1, void* param2, uint8_t param3, uint8_t param4);
	static uint8_t* DisplayBlinkingMessageHook(uint16_t param1, uint8_t param2, const WCHAR** param3);
	static void AddSPEffectHook(uint8_t* playerIns, uint32_t speffectId, int param3);
	static void ItemGiveLuaHook(uint64_t mapItemManager, Item::ItemGiveStruct* itemInfo, void* itemDetails);
	static void DamageFunctionHook(Damage::ChrDamageModule* damageModule, Damage::ChrIns* chrIns, Damage::DamageStruct* damageStruct, uint64_t param_4, uint8_t param_5);
	static void EventMessageHook(uint64_t param1, uint32_t param2, uint8_t* csEmkEventIns);
	static void ItemUseHook(Item::ItemUseStruct itemUseStruct);
	static void WarpHook(void* CSLuaEventProxy, void* CSLuaEventScriptImitation, uint32_t warpLocation);

private:
	std::string dllPath;

	RECT rect;
	OF::Box* overlay = nullptr;
	OF::Box* timer = nullptr;
	OF::Box* timerBar = nullptr;
	OF::Box* timerBorder = nullptr;
	OF::Box* greatestCombatnat = nullptr;
	OF::Box* messageBox = nullptr;
	float scale = 0.0f;
	int windowWidth = 0;
	int windowHeight = 0;
	int brawlTexture = 0;
	int teamFightTexture = 0;
	int messageBoxTexture = 0;
	int timerTexture = 0;
	int timerBarTexture = 0;
	int font = 0;
	int textoffsetX = 0;
	int textoffsetY = 0;
	float pulse = 1.0;
	float time = 1.0;


	P2PLobby* _p2pLobby = nullptr;

	bool ranOnce;
	void checkForMatchStart();
	void checkForMatchEnd();
	void checkForWindowResize();
	void displayPulseMessage(std::string);
	void handleDeath(uint8_t* playerIns);
	void handleItemTeleporting(uint8_t* playerIns);
	void sendScoreUpdateRequest();
	void displayScoreOverlay();
	void displayTimerOverlay();
	void displayGreatestCombatantOverlay();
	float Pulse(float amplitude, float period, float phaseShift, float verticalShift);
};