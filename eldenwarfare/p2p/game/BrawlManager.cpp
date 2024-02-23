#include "BrawlManager.h"

#define BRAWL_TIMER "BRAWL_IMER"

using namespace GameItems;

namespace P2P {
	void BrawlManager::PrepareMap(){
	}

	bool BrawlManager::StartMatch(uint32_t duration) {
		if (!Timer::HasTimer(BRAWL_TIMER)) {
			Player::ForceAnimationPlayback(ANIMATION_FADEOUT);
			Timer::StartTimer(BRAWL_TIMER, 3.0);
			Player::RandomTeleport(50.0f);
			Menu::PulseMessages.push(PULSING_MESSAGE_STARTING_SOON);
		}

		if (Timer::RemainingTime(BRAWL_TIMER) > 0.0) return false;

		Player::HealToFull();
		Player::ResetPoise();
		Player::SetFadeOut(OFF);
		Item::ToggleBannedItems(OFF);
		Map::Map::ToggleFadeOutAllNPCs(ON);
		Death::ToggleDeathEventTriggerListener(OFF);
		Death::ToggleDeathFallCameraListener(OFF);
		Death::ToggleDeathMapHitListener(OFF);
		SPeffect::LawOfRegression();
		SPeffect::AddSpeffect(SPEFFECT_SOAP);
		SPeffect::AddSpeffect(SPEFFECT_GRACE_HEAL);
		SPeffect::RemoveSpEffect(SPEFFECT_DEATH_BLIGHT);
		Menu::DisplayBanner(Menu::Commence);
		SPeffect::RemoveSpEffect(SPEFFECT_NOATTACK);
		Player::ForceAnimationPlayback(ANIMATION_SPAWN);
		Timer::EraseTimer(BRAWL_TIMER);
		Timer::StartTimer(MATCH_TIMER_KEY, duration);
		return true;
	}

	void BrawlManager::OnKillUpdate() {
		SPeffect::AddSpeffect(SPEFFECT_MID_HEAL);
	}

	bool BrawlManager::EndMatch(Result result) {
		static enum MatchState {
			CeaseCombat,
			DisplayResult,
			TurnOffRemainingFlags
		} currentState = CeaseCombat;

		switch (currentState) {
		case CeaseCombat:
			if (!Timer::HasTimer(BRAWL_TIMER)) {
				Timer::StartTimer(BRAWL_TIMER, 10.0);
				Player::SetNoDead(ON);
				Player::SetNoDamage(ON);
				Player::SetNoGravity(ON);
				Player::CurrentAnimation(ANIMATION_IDLE);
				Menu::PulseMessages.push(PULSING_MESSAGE_CEASE_COMBAT);
				currentState = DisplayResult;
			}
			break;

		case DisplayResult:
			if (Timer::RemainingTime(BRAWL_TIMER) > 7.0)
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
			if (Timer::RemainingTime(BRAWL_TIMER) > 0.0)
				return false;

			Timer::EraseTimer(GREATEST_COMBATANT_TIMER_KEY);
			Player::SetNoDead(OFF);
			Player::SetNoDamage(OFF);
			// Teleport player if player has been falling with no gravity
			if (Player::FallTimer() > 0.0f) Player::RandomTeleport(100.0f);
			Player::SetNoGravity(OFF);
			Timer::EraseTimer(BRAWL_TIMER);
			currentState = CeaseCombat;
			return true;
		}

		return false;
	}

	void BrawlManager::CancelMatch() {
		Item::ToggleBannedItems(ON);
		Map::Map::ToggleFadeOutAllNPCs(OFF);
		Death::ToggleDeathEventTriggerListener(ON);
		Death::ToggleDeathFallCameraListener(ON);
		Death::ToggleDeathMapHitListener(ON);
	}
}