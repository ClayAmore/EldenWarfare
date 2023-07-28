#pragma once
#include <cstdint>
#include "ParamEditor.h"
#include "EquipParamGoods.h"
#include "Player.h"

// Items changed
#define ITEM_SCRIPT_STONE 3320
#define ITEM_MEMORY_OF_GRACE 115
#define ITEM_SOAP 2120
#define ITEM_BEAST_EYE 8979

// Banlist
#define CrimsonFlask         1000
#define CrimsonFlaskEmpty    1001
#define CrimsonFlask1        1002
#define CrimsonFlaskEmpty1   1003
#define CrimsonFlask2        1004
#define CrimsonFlaskEmpty2   1005
#define CrimsonFlask3        1006
#define CrimsonFlaskEmpty3   1007
#define CrimsonFlask4        1008
#define CrimsonFlaskEmpty4   1009
#define CrimsonFlask5        1010
#define CrimsonFlaskEmpty5   1011
#define CrimsonFlask6        1012
#define CrimsonFlaskEmpty6   1013
#define CrimsonFlask7        1014
#define CrimsonFlaskEmpty7   1015
#define CrimsonFlask8        1016
#define CrimsonFlaskEmpty8   1017
#define CrimsonFlask9        1018
#define CrimsonFlaskEmpty9   1019
#define CrimsonFlask10       1020
#define CrimsonFlaskEmpty10  1021
#define CrimsonFlask11       1022
#define CrimsonFlaskEmpty11  1023
#define CrimsonFlask12       1024
#define CrimsonFlaskEmpty12  1025
#define FrenzyflameStone     3311
#define RawMeatDumplings     1235
#define SpectralSteedWhistle 0130
#define WarmingStone         3310

namespace GameItems {
    class Item {
    public:
        struct ItemUseStruct {
            Player::PlayerIns* ChrIns;
            int unk01;
            int unk02;
            uint32_t itemId;
            int8_t unk04;
            int8_t unk05;
            int8_t unk06;
            int8_t unk07;
            int refId;
            float unk08;
        };

        struct ItemInfo {
            ItemInfo() {
                itemId = -1;
                itemQuantity = 0;
                itemRelayValue = 0;
                itemAshesOfWar = 0;
            };
            uint32_t itemId;
            uint32_t itemQuantity;
            uint32_t itemRelayValue;
            uint32_t itemAshesOfWar;
        };

        struct ItemGiveStruct {
            uint32_t item_struct_count;
            ItemInfo item_info[10];
        };

        enum ItemType {
            WEAPON = 0,
            ARMOR = 0x10000000,
            TALISMAN = 0x20000000,
            GOODS = 0x40000000,
            ASH_OF_WAR = 0x80000000
        };

        typedef void ItemUseFunction(ItemUseStruct itemUseStruct);
        typedef void ItemGiveFunction(ItemType itemType, int itemId, int quantity);
        typedef void ItemGiveLuaFunction(uint64_t mapItemManager, ItemGiveStruct* itemInfo, void* itemDetails);
		typedef void RestoreFlasksFunction(void* playerIns);
        static ItemUseFunction* ItemUseFunctionOriginal;
        static ItemGiveLuaFunction* ItemGiveLuaFunctionOriginal;

		
        static void ToggleBannedItems(bool on);
        static void DirectlyGivePlayerItem(ItemType itemType, int id, int quantity);
        static void RemoveItemFromPlayer(ItemType itemType, int id, uint32_t quantity);
		static void RestoreFlasks();
		static void SetupItems();
    };

    const uint32_t itemBanList[] = {
        CrimsonFlask,        CrimsonFlaskEmpty,     CrimsonFlask1,        CrimsonFlaskEmpty1,
        CrimsonFlask2,       CrimsonFlaskEmpty2,    CrimsonFlask3,        CrimsonFlaskEmpty3,
        CrimsonFlask4,       CrimsonFlaskEmpty4,    CrimsonFlask5,        CrimsonFlaskEmpty5,
        CrimsonFlask6,       CrimsonFlaskEmpty6,    CrimsonFlask7,        CrimsonFlaskEmpty7,
        CrimsonFlask8,       CrimsonFlaskEmpty8,    CrimsonFlask9,        CrimsonFlaskEmpty9,
        CrimsonFlask10,      CrimsonFlaskEmpty10,   CrimsonFlask11,       CrimsonFlaskEmpty11,
        CrimsonFlask12,      CrimsonFlaskEmpty12,   WarmingStone,          FrenzyflameStone,
        RawMeatDumplings,    SpectralSteedWhistle
    };

    inline void Item::ToggleBannedItems(bool on) {
        ParamEditor<EquipParamGoods> pEditor(*(uint64_t*)Global::SOLO_PARAM_REPOSITORY.ptr());
        ParamHeader* header = pEditor.GetParamHeader();
        EquipParamGoods* pParam = pEditor.GetParamDataPtr();
        std::vector<uint32_t> rowIds = pEditor.GetRowIds();

        for (int i = 0; i < header->RowCount; i++) {
            uint32_t rowId = rowIds[i];
            bool inArray = std::find(std::begin(itemBanList), std::end(itemBanList), rowId) != std::end(itemBanList);

            if (inArray) pParam[i].enable_live = on;
        }
    }

    inline void Item::DirectlyGivePlayerItem(Item::ItemType itemType, int itemId, int quantity) {
        ItemGiveFunction* addItemFunction = (ItemGiveFunction*)(Global::ADD_ITEM_FUNCTION.ptr());
        if (addItemFunction == nullptr) {
            printf("Failed getting addItemFunction!\n");
            return;
        }
        addItemFunction(itemType, itemId, quantity);
    }

    inline void Item::RemoveItemFromPlayer(ItemType itemType, int itemId, uint32_t quantity) {
        if (quantity > 1) quantity = quantity * -1;
        DirectlyGivePlayerItem(itemType, itemId, quantity);
    }

	inline void Item::RestoreFlasks() {
		uintptr_t* playerIns = Player::GetPlayerIns<uintptr_t>(0);
		RestoreFlasksFunction* restoreFlasksF = (RestoreFlasksFunction*)(Global::RESTORE_FLASKS.ptr());
		if (restoreFlasksF == nullptr) {
			printf("Failed getting restoreFlasksF!\n");
			return;
		}
		restoreFlasksF((void*)*playerIns);
	}

	inline void Item::SetupItems() {

		ParamEditor<EquipParamGoods> pEditor(*(uint64_t*)Global::SOLO_PARAM_REPOSITORY.ptr());
		ParamHeader* header = pEditor.GetParamHeader();
		EquipParamGoods* pParam = pEditor.GetParamDataPtr();
		std::vector<uint32_t> rowIds = pEditor.GetRowIds();

		for (int i = 0; i < header->RowCount; i++) {
			pParam[i].enable_live = true;
			if (rowIds[i] == ITEM_SCRIPT_STONE) {
				pParam[i].isOnlyOne = true;
				pParam[i].refId_default = -12;
				pParam[i].iconId = 634;
				pParam[i].sortId = 570;
				pParam[i].sortGroupId = 10;
				pParam[i].isConsume = false;
				pParam[i].goodsUseAnim = 66;
				pParam[i].fireSfxId = 301031;
			}
			else if (rowIds[i] == ITEM_MEMORY_OF_GRACE) {
				pParam[i].isOnlyOne = true;
				pParam[i].refId_default = -10;
				pParam[i].iconId = 3087;
				pParam[i].sortId = 550;
				pParam[i].sortGroupId = 10;
				pParam[i].sellValue = 0;
				pParam[i].consumeMP = -1;
				pParam[i].castSfxId = 304573;
				pParam[i].fireSfxId = 304570;
				pParam[i].refCategory = 2;
				pParam[i].goodsUseAnim = 16;
				pParam[i].goodsType = 0;
				pParam[i].useLimitSpEffectType = 3;
				pParam[i].isEquip = 1;
				pParam[i].isDrop = 1;
				pParam[i].isDiscard = 1;
				pParam[i].isDeposit = 1;
				pParam[i].isUseMultiPlayPreparation = 1;
				pParam[i].enable_ActiveBigRune = 1;
				pParam[i].isUseNoAttackRegion = 1;
				pParam[i].enableRiding = 1;
				pParam[i].enable_live = 1;
				pParam[i].enable_gray = 1;
				pParam[i].enable_white = 1;
				pParam[i].enable_black = 1;
				pParam[i].enable_multi = 1;
			}
			else if (rowIds[i] == ITEM_SOAP) {
				pParam[i].isOnlyOne = true;
				pParam[i].sortId = 580;
				pParam[i].sortGroupId = 10;
				pParam[i].isConsume = 0;
			}
			else if (rowIds[i] == ITEM_BEAST_EYE) {
				pParam[i].isOnlyOne = true;
				pParam[i].opmeMenuType = 1;
				pParam[i].refId_default = -11;
				pParam[i].iconId = 3088;
				pParam[i].yesNoDialogMessageId = 20000070;
				pParam[i].sortId = 560;
				pParam[i].sortGroupId = 10;
				pParam[i].sellValue = 0;
				pParam[i].consumeMP = -1;
				pParam[i].refCategory = 2;
				pParam[i].goodsUseAnim = 16;
				pParam[i].goodsType = 0;
				pParam[i].castSfxId = 305040;
				pParam[i].fireSfxId = 305041;
				pParam[i].useLimitSpEffectType = 3;
				pParam[i].isEquip = 1;
				pParam[i].isDrop = 1;
				pParam[i].isDiscard = 1;
				pParam[i].isDeposit = 1;
				pParam[i].isUseMultiPlayPreparation = 1;
				pParam[i].enable_ActiveBigRune = 1;
				pParam[i].isUseNoAttackRegion = 1;
				pParam[i].enableRiding = 1;
				pParam[i].enable_live = 1;
				pParam[i].enable_gray = 1;
				pParam[i].enable_white = 1;
				pParam[i].enable_black = 1;
				pParam[i].enable_multi = 1;
			}

			Item::DirectlyGivePlayerItem(Item::GOODS, ITEM_SCRIPT_STONE, 1);
			Item::DirectlyGivePlayerItem(Item::GOODS, ITEM_SOAP, 1);
			Item::DirectlyGivePlayerItem(Item::GOODS, ITEM_BEAST_EYE, 1);
		}
	}

}