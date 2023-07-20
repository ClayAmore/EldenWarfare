#include "EldenWarfare.h"

using namespace GameItems;
using namespace OF;

#define HOOK_RUNTIME_EXCEPTION throw std::runtime_error("Something is wrong. Couldn't hook one of the functions needed for the mod!");

/*
	All static variables from all the code that's included are Instantiate here.
	This is to keep my sanity because of C++ way of handling includes or my limitation in
	understanding it. 
*/
bool EldenWarfare::InMatch;
bool EldenWarfare::IsItemTeleporting;
Mode EldenWarfare::RequestedMode = Mode::None;
VFTHook* EldenWarfare::enemyInsHook;
uint32_t EldenWarfare::LastPlayerToHitMe = 0;
bool Map::Map::NPCsHidden = false;
uint32_t Map::Map::CurrentMapId = 0;
std::queue<std::string> Menu::PulseMessages;
std::unordered_map<std::string, bool> Menu::InQueue;
std::unordered_map<std::string, std::pair<TimePoint, double>> Timer::Timers;
Settings::ModPreferences Settings::modPreferences;

/* 
	Remove NoAttack speffect that gets applied when you enter
	areas like Roundtable Hold or Volcano Manor.
*/
SPeffect::AddSpeffectFunction* SPeffect::AddSpEffectFunctionOriginal = nullptr;
void EldenWarfare::AddSPEffectHook(uint8_t* playerIns, uint32_t speffectId, int param3) {
	if (speffectId == SPEFFECT_NOATTACK) return;
	SPeffect::AddSpEffectFunctionOriginal(playerIns, speffectId, param3);
}

/* 
	Not sure this works 100 % to be honest. It's supposed to stop items from 
	entering your inventory while a match is active so you don't have to reorgranize your 
	equipment. 
*/ 
Item::ItemGiveLuaFunction* Item::ItemGiveLuaFunctionOriginal = nullptr;
void EldenWarfare::ItemGiveLuaHook(uint64_t mapItemManager, Item::ItemGiveStruct* itemInfo, void* itemDetails) {
	// Any item recieved during a match is intercepted and nullified. 
	// This is to prevent people from having to redo their menu because of 
	// npc lots after killAllEnemies is called
	if (EldenWarfare::InMatch) {
		itemInfo->item_info->itemId = -1;
		itemInfo->item_info->itemQuantity = 0;
		itemInfo->item_info->itemAshesOfWar = 0;
		itemInfo->item_info->itemRelayValue = 0;
	}
	return Item::ItemGiveLuaFunctionOriginal(mapItemManager, itemInfo, itemDetails);
}

/* Updates the netChrSyncId/PlayerNetworkHandle in a static
   variable that can be accessed when a player dies to 
   determine who killed the player. 
   This can be sent as part of the event or handled locally or anything.
   Up to the rules.
*/
Damage::DamageFunction* Damage::DamageFunctionOriginal = nullptr;
void EldenWarfare::DamageFunctionHook(Damage::ChrDamageModule* damageModule, Damage::ChrIns* chrIns, Damage::DamageStruct* damageStruct, uint64_t param_4, uint8_t param_5) {
	Damage::DamageFunctionOriginal(damageModule, chrIns, damageStruct, param_4, param_5);
	
	// If not in a match return without checking anything.
	if (!EldenWarfare::InMatch) return;

	uint32_t netChrSyndId = Player::GetPlayerNetChrSyncID();
	Player::PlayerIns* hitterPlayerIns = (Player::PlayerIns*)chrIns;
	Player::PlayerIns* victimPlayerIns = (Player::PlayerIns*)damageModule->playerIns;

	// Validate that no pointer is a nullptr
	if (hitterPlayerIns == nullptr || victimPlayerIns == nullptr) return;

	// Validate that the playerHandle is of a player and not and npc or trap.
	bool isValidPlayerId = (hitterPlayerIns->net_chrset_sync_id % 0x2D000 == hitterPlayerIns->net_chrset_sync_id - 0x2D000);
	bool isMeGettingHit = (netChrSyndId == victimPlayerIns->net_chrset_sync_id);

	// Update last person to hit me
	if (isMeGettingHit && isValidPlayerId) {
		LastPlayerToHitMe = hitterPlayerIns->net_chrset_sync_id;
	}
}

/*
	Hook for the function that writes text when menu is visible.
	This is used to replace item name and description for certain items during runtime.
	Probably not the best solution. Definetly far from optimal.
*/
Menu::MenuHookFunction* Menu::MenuHookOriginal = nullptr;
uint8_t EldenWarfare::MenuHook(void* param1, void* param2, uint8_t param3, uint8_t param4) {

	char* param2Text = (char*)param2;
	std::string text = std::string(param2Text);
	
	if (text == BRAWL_ORIGINAL_ITEM_NAME) {
		Text::ReplaceMenuText(param2Text, BRAWL_NEW_ITEM_NAME);
	}
	else if (text == BRAWL_ORIGINAL_ITEM_DESCRIPTION) {
		Text::ReplaceMenuText(param2Text, BRAWL_NEW_ITEM_DESCRIPTION);
	}
	else if (text == TEAMFIGHT_ORIGINAL_ITEM_NAME) {
		Text::ReplaceMenuText(param2Text, TEAMFIGHT_NEW_ITEM_NAME);
	}
	else if (text == TEAMFIGHT_ORIGINAL_ITEM_DESCRIPTION) {
		Text::ReplaceMenuText(param2Text, TEAMFIGHT_NEW_ITEM_DESCRIPTION);
	}
	else if (text == SOAP_ORIGINAL_ITEM_NAME) {
		Text::ReplaceMenuText(param2Text, SOAP_NEW_ITEM_NAME);
	}
	else if (text == SOAP_ORIGINAL_ITEM_DESCRIPTION) {
		Text::ReplaceMenuText(param2Text, SOAP_NEW_ITEM_DESCRIPTION);
	}
	else if (text == TARNISHED_WIZENED_FINGER_ORIGINAL_ITEM_NAME) {
		Text::ReplaceMenuText(param2Text, TARNISHED_WIZENED_FINGER_NEW_ITEM_NAME);
	}
	else if (text == TARNISHED_WIZENED_FINGER_ORIGINAL_ITEM_DESCRIPTION) {
		Text::ReplaceMenuText(param2Text, TARNISHED_WIZENED_FINGER_NEW_ITEM_DESCRIPTION);
	}
	else if (text == YOU_DIED_ORIGINAL) {
		Text::ReplaceMenuText(param2Text, YOU_DIED);
	}
	else if (text == BRAWL_ORIGINAL_DIALOG_TEXT) {
		Text::ReplaceMenuText(param2Text, BRAWL_NEW_DIALOG_TEXT);
	}
	else if (text == TEAMFIGHT_ORIGINAL_DIALOG_TEXT) {
		Text::ReplaceMenuText(param2Text, TEAMFIGHT_NEW_DIALOG_TEXT);
	}

	return Menu::MenuHookOriginal(param1, param2, param3, param4);
}

/*
	Inspired by how Seamless handles item use. Checks the refId
	on the item used. Usually that number is -1, but for mod items 
	it's been changed to identify the item.
*/
Item::ItemUseFunction* Item::ItemUseFunctionOriginal = nullptr;
void EldenWarfare::ItemUseHook(Item::ItemUseStruct itemUseStruct) {
	if (itemUseStruct.refId == -10) {
		RequestedMode = Mode::TeamFight;
	}
	else if (itemUseStruct.refId == -11) {
		RequestedMode = Mode::Brawl;
	}
	else if (itemUseStruct.itemId == 2120) {
		Seamless::RemoveRot();
	}
	else if (itemUseStruct.refId == -12) {
		uint32_t myNetChrSyncId = Player::GetPlayerNetChrSyncID();
		uint32_t itemNetChrSyncId = itemUseStruct.ChrIns->net_chrset_sync_id;
		bool isMe = myNetChrSyncId == itemNetChrSyncId;
		if(isMe) IsItemTeleporting = true;
	}
	Item::ItemUseFunctionOriginal(itemUseStruct);
}

/*
	This is where the applications starts!
	1. Checks for seamless, if not then it shows an error and crashes.
	2. Read the EldenWarfare.ini settings and stores them in static variables.
	3. Sets up the overlay.
	4. Instantiate P2P objects. This where all the networking code starts.
	5. Hooks into Elden Ring game functions so you can whenever they fire we 
		can do our own thing like in Speffects we stop the NoAttack from applying
		to the player.
	6. Using Dasaavs RTTI scanner with VFTHook to easier fadeout all enemies in 
		world once a match starts.
*/
void EldenWarfare::Setup()
{
	// Seamless Check ============
	if (!Seamless::IsRunningSeamless()) {
		MessageBox(NULL, "Cannot run the Vanilla game with Eldenwarfare mod enabled.", "Error: Mod Conflict!", MB_ICONERROR | MB_OK);
		throw std::runtime_error("Eldenwarfare attached to Vanilla game! The process will be stopped.");
	}

	// Settings ==================
	INIReader reader(dllPath + "\\EldenWarfare.ini");
	Settings::ParseIniFile(reader);
	
	// Overlay ===================
	InitFramework(device, spriteBatch, window, dllPath);
	windowWidth = ofWindowWidth;
	windowHeight = ofWindowHeight;

	overlay = CreateBox(424, 340, OF::Right, OF::Bottom, { 0, 0, 100, 140 });
	timer = CreateBox(overlay, 424, 40, OF::Left, OF::Top, { 0, 40, 0, 0 });
	timerBar = CreateBox(timer, 350, 32, OF::Left, OF::Top, { 10, 5, 0, 0 });
	timerBorder = CreateBox(timer, 444, 60, OF::Left, OF::Top, { -6, -8, 0, 0 });
	greatestCombatnat = CreateBox(600, 300, OF::HCenter, OF::VCenter, { 0, 0, 0, 0 });
	messageBox = CreateBox(2500, 300, OF::HCenter, OF::Bottom, { 0, 0, 0, 600 });
	font = LoadFont(dllPath + "\\EldenWarfare\\hook_fonts\\font.spritefont");
	brawlTexture = LoadTexture(dllPath + "\\EldenWarfare\\hook_textures\\OverlayBrawl.png");
	teamFightTexture = LoadTexture(dllPath + "\\EldenWarfare\\hook_textures\\OverlayTeamFight.png");
	messageBoxTexture = LoadTexture(dllPath + "\\EldenWarfare\\hook_textures\\MessageBG.png");
	timerTexture = LoadTexture(dllPath + "\\EldenWarfare\\hook_textures\\timer.png");
	timerBarTexture = LoadTexture(dllPath + "\\EldenWarfare\\hook_textures\\timerBar.png");
	SetFont(font);

	// P2P =======================
	SteamInterfaceProvider* steamInterfaceProvider = new SteamInterfaceProvider();
	steamInterfaceProvider->CreateInterfacesWhenReady();
	auto _steamUser = steamInterfaceProvider->GetUser();
	auto _steamMatchmaking = steamInterfaceProvider->GetSteamMatchMaking();
	_p2pLobby = new P2PLobby(_steamUser, _steamMatchmaking);

	// Hooks ======================
	if (!HookSetup::Init()) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::MENU.ptr(), (void*)MenuHook, (void**)&Menu::MenuHookOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::ITEM_USE.ptr(), (void*)ItemUseHook, (void**)&Item::ItemUseFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::DAMAGE_FUNCTION.ptr(), (void*)DamageFunctionHook, (void**)&Damage::DamageFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::CreateHook((void*)Global::ADD_SPEFFECT_FUNCTION.ptr(), (void*)AddSPEffectHook, (void**)&SPeffect::AddSpEffectFunctionOriginal)) HOOK_RUNTIME_EXCEPTION;
	if (!HookSetup::EnableHooks()) HOOK_RUNTIME_EXCEPTION;

	// VFTHook =======================
	RTTIScanner* scanner = new RTTIScanner();
	if (scanner->scan()) {
		enemyInsHook = new VFTHook("CS::EnemyIns", 20, Map::Map::FadeOutAllNPCs);
	}
}

/*
	Render loop. This is a function that is running a loop 
	rendering the overlay to the screen and also handling mod logic that requires
	continous checks.
*/
void EldenWarfare::Render()
{
	// Game pointer.
	uint8_t* playerIns = Player::GetPlayerIns<uint8_t>(0);
	uint8_t* gameMan = Global::GAME_MAN.ptr();
	uint8_t* soloParamRepository = Global::SOLO_PARAM_REPOSITORY.ptr();
	uint32_t* mapId = AccessDeepPtr<uint32_t>(gameMan, 0xBDC);
	uint32_t* sessionState = AccessDeepPtr<uint32_t>(Global::SESSION_MAN.ptr(), 0xC);

	/* 
		If any of the pointer a nullptr then we stop the loop until they are ready.
		Example: You can't access a players HP when they are mid warp. The code will continue 
		stopping here until the player has finished warping.
	*/
	if (playerIns == nullptr || gameMan == nullptr || soloParamRepository == nullptr || sessionState == nullptr) {
		return;
	}

	/*
		Used for things that should run once when the game is ready.
		It's currently used to change the items for the mod.
	*/
	if (!ranOnce) {
		Item::SetupItems();
		ranOnce = true;
	}

	/*
		Check for wether the user in online or offline.
		It's done by checking if the user is marked as `Host` or `None`.
		In Seamless all users are Host but if you're not in a session then 
		this value is None = 0.
	*/
	if (*sessionState != 0) {

		/*
			Continusly checking on the state of session and wether 
			the player has started a match or is in an ongoing match that
			is ending.
		*/ 
		if (_p2pLobby->IsHost()) {
			checkForMatchStart();
			checkForMatchEnd();
		}

		/*
			All communication is handled through LobbyData except player death.
			When a player dies they the player sends a lobby chat message to 
			all lobby members informing them of the death and including data 
			such as the playerhandle of the killer if theres any. That message
			is put in a queue and that queue gets handled here.
		*/
		_p2pLobby->HandleMessageQueue();
		
		/*
			Allows us to be able to easliy check on wether a match is active 
			or not. It's used to determine among other things wether the overlay code should 
			be executed or not. 
		*/
		InMatch = _p2pLobby->LocalLobbyData.mode != Mode::None;

	}
	/*
		If the player is not in a session then we:
		1. Check if a player was just in a match. If yes cancel.
		2. Set the ReqeustedMode which is used to determine which gamemode to start to None.
		3. Update the InMatch status.
	*/
	else if (RequestedMode != Mode::None) {
		if(InMatch) _p2pLobby->Cancel();
		RequestedMode = Mode::None;
		InMatch = false;
	}

	/*
		Keeps track of the current MapId. Everytime it changes the mod
		updates the spawn locations for the new map.
	*/
	if (mapId != nullptr && Map::Map::CurrentMapId != *mapId) {
		Map::Map::CurrentMapId = *mapId;
		Map::Map::RefreshMapSpawnPoints(dllPath);
	}

	/*
		Check if the player has used the Teleport Item if yes then handle 
		it.
	*/
	if (IsItemTeleporting) handleItemTeleporting(playerIns);

	/*
		Pulsingmessage queue. My own implemenation for a notification/messaging system beacuse
		I couldn't figure out how to use the games own. 
		If there's a message in the queue display it on screen for 6 seconds.
	*/
	if (!Menu::PulseMessages.empty()) {
		if (!Timer::HasTimer(PULSE_MESSAGE_TIMER_KEY)) Timer::StartTimer(PULSE_MESSAGE_TIMER_KEY, 6.0);
		displayPulseMessage(Menu::PulseMessages.front());
		if (Timer::RemainingTime(PULSE_MESSAGE_TIMER_KEY) < 0) {
			Menu::PopMessageQueue();
			Timer::EraseTimer(PULSE_MESSAGE_TIMER_KEY);
		}
	}

	/*
		Checks if the games window has changed size and properly scales the 
		overlay to the new size.
	*/
	checkForWindowResize();

	/*
		Stops the code unless you are in the a match.
	*/
	if (!InMatch) return;

	/*
		Handles deaths in an active match.
	*/
	handleDeath(playerIns);

	/*
		Update players position to other lobby members can
		use that position for closer respawn.
	*/
	updatePosition();

	/*
		Renders the game overlays.
	*/
	displayScoreOverlay();
	displayTimerOverlay();
	displayGreatestCombatantOverlay();
}

void EldenWarfare::checkForMatchStart() {
	// Check if a mode is requested and the local lobby is not already in a mode
	if (RequestedMode != Mode::None && _p2pLobby->LocalLobbyData.mode == Mode::None) {
		// Check if the requested mode is TeamFight
		if (RequestedMode == Mode::TeamFight) {
			// Check if the Seamless mode is set to TEAM
			if (Seamless::GetSeamlessInfo().mode == Seamless::TEAM) {
				// Set the local lobby mode to TeamFight
				_p2pLobby->LocalLobbyData.mode = Mode::TeamFight;
				_p2pLobby->LocalLobbyData.state = State::TeamFightInitiate;
				_p2pLobby->UpdateLobbyData();
			}
			else if (Menu::PulseMessages.empty() || Menu::PulseMessages.front().compare(PULSING_MESSAGE_RULE_XX) != 0) {
				// Add the pulsing message PULSING_MESSAGE_RULE_XX if not already present
				Seamless::ForceUseSeamlessItem(Seamless::JUDICATORS_RULEBOOK_REF);
				Menu::PulseMessages.push(PULSING_MESSAGE_RULE_XX);
			}
		}
		// Check if the requested mode is Brawl
		else if (RequestedMode == Mode::Brawl) {
			// Check if the Seamless mode is set to ENEMIES
			if (Seamless::GetSeamlessInfo().mode == Seamless::ENEMIES) {
				// Set the local lobby mode to Brawl
				_p2pLobby->LocalLobbyData.mode = Mode::Brawl;
				_p2pLobby->LocalLobbyData.state = State::BrawlInitiate;
				_p2pLobby->UpdateLobbyData();
			}
			else if (Menu::PulseMessages.empty() || Menu::PulseMessages.front().compare(PULSING_MESSAGE_RULE_IV) != 0) {
				// Add the pulsing message PULSING_MESSAGE_RULE_IV if not already present
				Seamless::ForceUseSeamlessItem(Seamless::JUDICATORS_RULEBOOK_REF);
				Menu::PulseMessages.push(PULSING_MESSAGE_RULE_IV);
			}
		}
	}
}

void EldenWarfare::checkForMatchEnd() {
	// Check if brawl or team fight is ongoing
	bool isBrawlOngoing = _p2pLobby->MyLobbyMemberData.state == State::BrawlOngoing;
	bool isTeamFightOngoing = _p2pLobby->MyLobbyMemberData.state == State::TeamFightOngoing;

	// Check if brawl or team fight is ending
	bool isBrawlEnding = _p2pLobby->LocalLobbyData.state == State::BrawlEndMatch;
	bool isTeamFightEnding = _p2pLobby->LocalLobbyData.state == State::TeamFightEndMatch;

	// If neither brawl nor team fight is ongoing or it's already end match, return
	if ((!isBrawlOngoing && !isTeamFightOngoing) || isBrawlEnding || isTeamFightEnding)
		return;

	// If match timer doesn't exist or still has remaining time, return
	if (Timer::RemainingTime(MATCH_TIMER_KEY) > 0.0)
		return;

	// End the match
	if (_p2pLobby->LocalLobbyData.mode == Mode::TeamFight) {
		_p2pLobby->LocalLobbyData.state = State::TeamFightEndMatch;
	}
	else if (_p2pLobby->LocalLobbyData.mode == Mode::Brawl) {
		_p2pLobby->LocalLobbyData.state = State::BrawlEndMatch;
	}

	// Erase the timer and update the lobby data
	Timer::EraseTimer(MATCH_TIMER_KEY);
	_p2pLobby->UpdateLobbyData();
}

float EldenWarfare::Pulse(float amplitude, float period, float phaseShift, float verticalShift) {
	float pi = 3.14;

	return amplitude * sin((2 * pi / period) * (Timer::ElapsedTime(PULSE_MESSAGE_TIMER_KEY) + phaseShift)) + verticalShift;
}

void EldenWarfare::displayPulseMessage(std::string pulseMessage) {
	int messageLength = pulseMessage.size();

	// If empty string return
	if (messageLength == 0) return;

	int singleLineLength = MAX_MESSAGE_LENGTH / 3;
	messageBox->height = messageLength <= singleLineLength ? 160 : (messageLength <= (singleLineLength * 2) ? 260 : 300);

	// First line
	std::string output = pulseMessage.substr(0, singleLineLength) + "\n";

	// Second line
	if (messageLength > 86) output += pulseMessage.substr(singleLineLength, singleLineLength) + "\n";

	// Third line
	if (messageLength > 172) output += pulseMessage.substr(singleLineLength * 2, singleLineLength);

	// Update pulse value
	auto pulseFactor = Pulse(0.1f, 10.0f, 0.0f, 1.0f);
	DrawBox(messageBox, messageBoxTexture, scale * pulseFactor);
	DrawText(messageBox, output, scale * pulseFactor, 221, 221, 221, 255, 0.0f, OF::HCenter, OF::VCenter, { 0, 0, 0, 0 });
}

void EldenWarfare::handleItemTeleporting(uint8_t* playerIns) {
	static enum PlayerState {
		Teleporting,
		ApplyingPenalty,
		Done,
	} currentState = Teleporting;


	uint8_t* FD4PadMan = AccessDeepPtr<uint8_t>(Global::FD4PAD_MAN.ptr());
	uint32_t* teleportButton = AccessDeepPtr<uint32_t>(FD4PadMan, 0x18, 0x8, 0x7C8, 0x20);
	uint32_t* spawnButton = AccessDeepPtr<uint32_t>(FD4PadMan, 0x18, 0x8, 0x7C8, 0x10);
	Menu::EnqueueUnique(PHANTOM_FINGER_RULE);

	if (FD4PadMan::IsButtonPressed(FD4PadMan::A) || FD4PadMan::IsButtonPressed(FD4PadMan::Space)) {
		if (InMatch) {
			Vector3 coords = _p2pLobby->GetRandomLobbyMemberCoords();
			Player::RandomTeleport(coords, 25.0f, playerIns);
		}
		else {
			Player::Teleport(100.0f, playerIns);
		}
	}

	if (FD4PadMan::IsButtonPressed(FD4PadMan::B) || FD4PadMan::IsButtonPressed(FD4PadMan::E)) currentState = Done;

	// This can be moved to the main render function if button press
	// is used in multiple functions. For now this is good here.
	FD4PadMan::RegisterDownButtons();

	switch (currentState) {
	case Teleporting: {
		if (!Timer::HasTimer(TELEPORT_TIMER)) {
			Player::ForceAnimationPlayback(ANIMATION_DEATH_FADE);
			Timer::StartTimer(TELEPORT_TIMER, 3.0);
		}
		else if (Timer::RemainingTime(TELEPORT_TIMER) <= 0.0) {
			currentState = ApplyingPenalty;
			Timer::EraseTimer(TELEPORT_TIMER);
		}
		break;
	}
	case ApplyingPenalty: {
		if (!Timer::HasTimer(TELEPORT_TIMER)) {
			double health = Player::GetHealthPercentage(playerIns);
			Timer::StartTimer(TELEPORT_TIMER, health / 10.0);
		}
		double remainingTime = Timer::RemainingTime(TELEPORT_TIMER);
		double remainingHealth = remainingTime * 10;
		Player::SetHealth(remainingHealth, playerIns);

		if (remainingTime < 0.0) currentState = Done;
		break;
	}
	case Done: {
		Player::ForceAnimationPlayback(ANIMATION_SPAWN);
		IsItemTeleporting = false;
		Timer::EraseTimer(TELEPORT_TIMER);
		currentState = Teleporting;
		break;
	}
	}
}

void EldenWarfare::checkForWindowResize() {
	if (GetClientRect(this->window, &rect))
	{
		bool should_reisze = false;
		LONG tempWidth = rect.right - rect.left;
		LONG tempHeight = rect.bottom - rect.top;

		if (windowWidth != tempWidth) {
			windowWidth = tempWidth;
			should_reisze = true;
		}

		if (windowHeight != tempHeight) {
			windowHeight = tempHeight;
			should_reisze = true;
		}

		if (scale == 0.0f) {
			should_reisze = true;
		}

		if (should_reisze) {
			scale = ((float)windowWidth / 4000.0f);
		}
	}
}

void EldenWarfare::handleDeath(uint8_t* playerIns) {
	// Define player states
	static enum PlayerState {
		Alive,
		Dying,
		BannerShowing,
		Respawning
	} currentState = Alive;

	// State-based logic
	switch (currentState) {
	case Alive:
		// Check if the player is dying
		if (Player::IsDying(playerIns)) {
			sendScoreUpdateRequest();
			currentState = Dying;
			Timer::StartTimer(DEATH_TIMER, 10.0);
		}
		break;

	case Dying:
		// Check remaining time
		if (Timer::RemainingTime(DEATH_TIMER) > 5.0) return;
		// Display slain banner and switch to banner showing state
		Menu::DisplayBanner(Menu::Slain);
		currentState = BannerShowing;
		break;

	case BannerShowing:
		// Calculate elapsed time since banner showing started
		if (Timer::RemainingTime(DEATH_TIMER) > 0.0) return;
		// Switch to respawning state
		currentState = Respawning;
		break;

	case Respawning:
		// Perform actions for respawning
		Item::RestoreFlasks();
		Player::HealToFull();
		Player::ResetPoise();
		Player::RandomTeleport(50.0f, playerIns);
		Player::ResetGhostFadeOut();
		Player::ForceAnimationPlayback(ANIMATION_FADEOUT);
		SPeffect::LawOfRegression();
		SPeffect::AddSpeffect(SPEFFECT_SOAP);
		SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
		SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
		Player::ForceAnimationPlayback(ANIMATION_SPAWN, playerIns);
		Player::SetNoGravity(OFF); // Todo:: Check if this is still neccessary
		Player::SetMapHit(OFF); // Todo:: Check if this is still neccessary
		Timer::EraseTimer(DEATH_TIMER);
		currentState = Alive;
		break;
	}
}

void EldenWarfare::updatePosition() {
	if (!Timer::HasTimer(PLAYER_COORDS_UPDATE_TIMER_KEY)) 
		Timer::StartTimer(PLAYER_COORDS_UPDATE_TIMER_KEY, 5.0);

	if (Timer::RemainingTime(PLAYER_COORDS_UPDATE_TIMER_KEY) > 0.0) return;

	Vector3 currentPos = Player::GetCurrentPos();
	_p2pLobby->MyLobbyMemberData.coords = currentPos;
	_p2pLobby->UpdateLobbyMemberData();

	std::cout << "Updated current position." << std::endl;

	Timer::EraseTimer(PLAYER_COORDS_UPDATE_TIMER_KEY);
}

void EldenWarfare::displayScoreOverlay() {
	std::stringstream score;
	int16_t blueTeamScore = _p2pLobby->LocalLobbyData.blueTeamScore;
	int16_t redTeamScore = _p2pLobby->LocalLobbyData.redTeamScore;
	int16_t mostKills = _p2pLobby->LocalLobbyData.mostKills;
	int16_t ownKills = _p2pLobby->MyLobbyMemberData.kills;

	if (_p2pLobby->LocalLobbyData.mode == Mode::TeamFight &&
		_p2pLobby->MyLobbyMemberData.state == State::TeamFightOngoing) {
		score << std::to_string(blueTeamScore) << std::setfill(' ') << std::setw(19) << std::to_string(redTeamScore);
		DrawBox(overlay, teamFightTexture, scale);
		DrawText(overlay, score.str(), scale, 221, 221, 221, 255, 0.0f, OF::Right, OF::Bottom, {0, 0, 10, 10});
	}
	else if (_p2pLobby->MyLobbyMemberData.state == State::BrawlOngoing) {
		score << std::to_string(mostKills) << std::setfill(' ') << std::setw(19) << std::to_string(ownKills);
		DrawBox(overlay, brawlTexture, scale);
		DrawText(overlay, score.str(), scale, 221, 221, 221, 255, 0.0f, OF::Right, OF::Bottom, {0, 0, 10, 10});
	}
}

void EldenWarfare::displayTimerOverlay() {
	bool isBrawlEnding = _p2pLobby->LocalLobbyData.state == State::BrawlEndMatch;
	bool isTeamFightEnding = _p2pLobby->LocalLobbyData.state == State::TeamFightEndMatch;
	
	if (isBrawlEnding || isTeamFightEnding) return;

	double remainingTime = Timer::RemainingTime(MATCH_TIMER_KEY);
	if (remainingTime < 0.0f && ((int)remainingTime % 2 == 0)) {
		DrawBox(timer, 0, 0, 0, 255, scale);
		DrawBox(timerBorder, timerTexture, scale);
		return;
	}

	if (remainingTime < 0.0f) return;

	float remainingTimeNormalized = remainingTime / _p2pLobby->LocalLobbyData.matchDuration;
	timerBar->width = timer->width * remainingTimeNormalized;
	DrawBox(timer, 0, 0, 0, 255, scale);
	DrawBox(timerBar, timerBarTexture, scale);
	DrawBox(timerBorder, timerTexture, scale);
}

void EldenWarfare::displayGreatestCombatantOverlay() {
	// Check if brawl or team fight is ending
	bool isBrawlEnding = _p2pLobby->LocalLobbyData.state == State::BrawlEndMatch;
	bool isTeamFightEnding = _p2pLobby->LocalLobbyData.state == State::TeamFightEndMatch;
	bool hasTimer = Timer::HasTimer(GREATEST_COMBATANT_TIMER_KEY);
	std::string greatestCombatantsString = _p2pLobby->LocalLobbyData.greatestCombatant;

	// Return early if it's not the ending of a brawl or team fight
	if (!hasTimer && (!isBrawlEnding || !isTeamFightEnding || greatestCombatantsString.size() == 0)) return;

	// Draw the box and text overlay
	DrawBox(greatestCombatnat, 0, 0, 0, 0, scale);
	DrawText(greatestCombatnat, "Greatest combatant: " + greatestCombatantsString, scale * 1.2, 221, 221, 221, 255, 0.0f, OF::HCenter, OF::Bottom, {0, 20, 0, 0});

	// Erase the timer if it has expired
	if (Timer::RemainingTime(GREATEST_COMBATANT_TIMER_KEY) < 0.0) {
		Timer::EraseTimer(GREATEST_COMBATANT_TIMER_KEY);
	}
}

void EldenWarfare::sendScoreUpdateRequest() {
	// If nobody killed me, then no need to broadcast a message.
	if (LastPlayerToHitMe == 0) {
		if(_p2pLobby->LocalLobbyData.mode == Mode::Brawl)
		_p2pLobby->MyLobbyMemberData.kills--;

		_p2pLobby->MyLobbyMemberData.deaths++;
		_p2pLobby->UpdateLobbyMemberData();
		return;
	}

	// Broadcast to lobby 
	P2PLobbyUpdateKillsMsg msg{};
	P2PLobbyMsgHeader header{};
	header.type = UpdateKills;
	msg.header = header;
	msg.team = Seamless::GetSeamlessInfo().team;
	msg.killerId = LastPlayerToHitMe;
	char message[sizeof(P2PLobbyUpdateKillsMsg)];
	memcpy(message, &msg, sizeof(P2PLobbyUpdateKillsMsg));
	_p2pLobby->BroadcastMessageToLobbyMembers(message, sizeof(message));

	// Reset the lastPersonToHitMeReference
	LastPlayerToHitMe = 0;

	// Increment my deaths and update Lobby
	_p2pLobby->MyLobbyMemberData.deaths++;
	_p2pLobby->UpdateLobbyMemberData();
}