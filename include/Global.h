#pragma once
#include "SigScan.h"

using namespace EW;

namespace Global {
    // WORKING
    const SigScannedPtr<uint8_t> WORLD_CHR_MAN(Signature("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 0F 48 39 88", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> GAME_DATA_MAN(Signature("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 05 48 8B 40 58 C3 C3", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> SESSION_MAN(Signature("4C 8B 05 ?? ?? ?? ?? 48 8B D9 33 C9 0F 29 74 24 ?? 0F 29 7C 24 ?? B2 01", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> SOLO_PARAM_REPOSITORY(Signature("48 8B 0D ?? ?? ?? ?? 48 85 C9 0F 84 ?? ?? ?? ?? 45 33 C0 BA 90", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> GAME_MAN(Signature("48 8B 15 ?? ?? ?? ?? 8B C6 0F B7 8A ?? ?? ?? ?? 66 41 03 C8", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> CS_MENU_MAN_IMP(Signature("48 8B 0D ?? ?? ?? ?? 83 79 ?? 00 0F 85 ?? ?? ?? ?? 49 8B 87 ?? ?? ?? ?? 48 8B 88 ?? ?? ?? ?? E8", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> ITEM_USE(Signature("40 53 48 83 EC 40 8B 51 18 48 8B D9 48 8D 4C 24 20 48 C7 44 24 20 00 00", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> MENU(Signature("44 88 4C 24 20 55 53 56", 0, Signature::Type::Address)); 
    const SigScannedPtr<uint8_t> DISPLAY_BANNER_FUNCTION(Signature("40 53 48 83 EC 30 48 8D 4C 24 20 C7 44 24 20 FF FF FF FF 8B DA 48 C7 44 24 28 00", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> DAMAGE_FUNCTION(Signature("4C 8B DC 55 53 56 57 41 56 41 57 49 8D 6B 88 48 81 EC 48 01 00 00", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> ITEM_GIVE_HOOK_LUA(Signature("E8 ?? ?? ?? ?? 83 7D 98 00 74", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> LUA_WARP(Signature("C3 ?? ?? ?? ?? ?? ?? 57 48 83 EC ?? 48 8B FA 44", 0x2, Signature::Type::Address));
    const SigScannedPtr<uint8_t> ADD_SPEFFECT_FUNCTION(Signature("48 8B C4 48 89 58 08 48 89 70 10 57 48 81 EC ???????? 0F2805 ???????? 48 8B F1 0F280D ???????? 48 8D 48 88", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> REMOVE_SPEFFECT_FUNCTION(Signature("48 83 EC 28 8B C2 48 8B 51 08 48 85 D2 ???? 90", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> FD4PAD_MAN(Signature("48 8B 0D ?? ?? ?? ?? 33 D2 E8 ?? ?? ?? ?? 48 89 45 17", 0, Signature::Type::InInstruction));
    const SigScannedPtr<uint8_t> ADD_ITEM_FUNCTION(Signature("40 56 57 41 56 48 83 ec 50 48 c7 44 24 30 fe ff ff ff 48 89 5c 24 70 48 89 6c 24 78 41 8b f8 8b f2 48 8b 05 80 55 6f 03 48 8b 58 08", 0, Signature::Type::Address));

    // Differnet autorevive 
    const SigScannedPtr<uint8_t> DEATH_TRIGGER_EVENT_SETTER(Signature("01 48 8B 0D DC 2A 8E 03", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> FALL_CAMERA_SETTER(Signature("01 C0 E2 02 08 91 C8 01 00 00 C3 49 89 06 48 8D", 0, Signature::Type::Address));
    const SigScannedPtr<uint8_t> DEATH_DISABLE_MAP_HIT_SETTER(Signature("01 C3 83 A1 F0 00 00 00", 0, Signature::Type::Address));
}
