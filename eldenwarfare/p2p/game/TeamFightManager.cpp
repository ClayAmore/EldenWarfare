#include "TeamFightManager.h"

#define TEAMFIGHT_TIMER "TEAMFIGHT_TIMER"

using namespace GameItems;

namespace P2P {
	bool TeamFightManager::CheckIfTeamPicked() {
		Seamless::SeamlessInfo seamlessInfo = Seamless::GetSeamlessInfo();
		return seamlessInfo.team != Seamless::FREE_FOR_ALL;
	}

	void TeamFightManager::PickingTeam() {
		Player::SetNoDamage(ON);
	}

	void TeamFightManager::DisplayPickTeamMessage() {
		Menu::EnqueueUnique(PULSING_MESSAGE_PICK_TEAM);
	}

	void TeamFightManager::PrepareMap() {
		Map::Map::ToggleFadeOutAllNPCs(ON);
	}

	bool TeamFightManager::StartMatch(uint32_t duration) {
		
		if (!Timer::HasTimer(TEAMFIGHT_TIMER)) {
			Player::ForceAnimationPlayback(ANIMATION_FADEOUT);
			Timer::StartTimer(TEAMFIGHT_TIMER, 5.0);
			Item::ToggleBannedItems(OFF);
			Player::RandomTeleport(50.0f);
			Player::SetNoDamage(OFF);
			Death::ToggleDeathEventTriggerListener(OFF);
			Death::ToggleDeathFallCameraListener(OFF);
			Death::ToggleDeathMapHitListener(OFF);
			Menu::EnqueueUnique(PULSING_MESSAGE_STARTING_SOON);
		}

		if (Timer::RemainingTime(TEAMFIGHT_TIMER) > 0.0) return false;

		Player::SetFadeOut(OFF);
		Player::HealToFull();
		SPeffect::LawOfRegression();
		SPeffect::AddSpeffect(SPEFFECT_SOAP);
		SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
		SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
		Player::ResetPoise();
		Player::ForceAnimationPlayback(ANIMATION_SPAWN);
		Menu::DisplayBanner(Menu::Commence);
		Timer::EraseTimer(TEAMFIGHT_TIMER);
		Timer::StartTimer(MATCH_TIMER_KEY, duration);
		return true;
	}

	void TeamFightManager::OnKillUpdate() {
		SPeffect::AddSpeffect(SPEFFECT_MID_HEAL);
	}

	void TeamFightManager::OnKillUpdate(Seamless::Team selfTeam, Seamless::Team victimTeam) {
		bool isSelfBlueTeam = selfTeam == Seamless::BLUE;
		bool isSelfRedTeam = selfTeam == Seamless::RED;
		bool isVictimBlueTeam = victimTeam == Seamless::BLUE;
		bool isVictimRedTeam = victimTeam == Seamless::RED;

		if (isSelfBlueTeam && isVictimRedTeam) SPeffect::AddSpeffect(SPEFFECT_SMALL_HEAL);
		else if (isSelfRedTeam && isVictimBlueTeam) SPeffect::AddSpeffect(SPEFFECT_SMALL_HEAL);
	}

	bool TeamFightManager::EndMatch(Result result) {

		static enum MatchState {
			CeaseCombat,
			DisplayResult,
			TurnOffRemainingFlags
		} currentState = CeaseCombat;

		switch (currentState) {
		case CeaseCombat:
			if (!Timer::HasTimer(TEAMFIGHT_TIMER)) {
				Timer::StartTimer(TEAMFIGHT_TIMER, 10.0);
				Player::SetNoDead(ON);
				Player::SetNoDamage(ON);
				Player::SetNoGravity(ON);
				Item::ToggleBannedItems(ON);
				Player::CurrentAnimation(ANIMATION_IDLE);
				SPeffect::RemoveSpEffect(SPEFFECT_NOATTACK);
				Menu::PulseMessages.push(PULSING_MESSAGE_CEASE_COMBAT);
				currentState = DisplayResult;
			}
			break;

		case DisplayResult:
			if (Timer::RemainingTime(TEAMFIGHT_TIMER) > 7.0)
				return false;

			Item::ToggleBannedItems(ON);
			Map::Map::ToggleFadeOutAllNPCs(OFF);
			Death::ToggleDeathEventTriggerListener(ON);
			Death::ToggleDeathFallCameraListener(ON);
			Death::ToggleDeathMapHitListener(ON);
			Timer::StartTimer(GREATEST_COMBATANT_TIMER_KEY, 7.0);

			switch (result) {
			case VICTORY:
				Menu::DisplayBanner(Menu::Victory);
				Menu::PulseMessages.push(PULSING_MESSAGE_VICTORY);
				break;
			case DEFEAT:
				Menu::DisplayBanner(Menu::Defeat);
				Menu::PulseMessages.push(PULSING_MESSAGE_DEFEAT);
				break;
			default:
				Menu::DisplayBanner(Menu::Stalemate);
				Menu::PulseMessages.push(PULSING_MESSAGE_STALEMATE);
				break;
			}
			Menu::PulseMessages.push(PULSING_MESSAGE_SEPERATING_WORLDS);
			currentState = TurnOffRemainingFlags;
			break;

		case TurnOffRemainingFlags:
			if (Timer::RemainingTime(TEAMFIGHT_TIMER) > 0.0)
				return false;

			Timer::EraseTimer(GREATEST_COMBATANT_TIMER_KEY);
			Player::SetNoDead(OFF);
			Player::SetNoDamage(OFF);
			// Teleport player is player has been falling with no gravity
			if (Player::FallTimer() > 0.0f) Player::RandomTeleport(50.0f);
			Player::SetNoGravity(OFF);
			Timer::EraseTimer(TEAMFIGHT_TIMER);
			currentState = CeaseCombat;
			return true;
		}

		return false;
	}

	void TeamFightManager::CancelMatch() {
		Player::SetNoDamage(OFF);
		Item::ToggleBannedItems(ON);
		Map::Map::ToggleFadeOutAllNPCs(OFF);
		Death::ToggleDeathEventTriggerListener(ON);
		Death::ToggleDeathFallCameraListener(ON);
		Death::ToggleDeathMapHitListener(ON);
	}
}