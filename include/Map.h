#pragma once
#include "vector3/Vector3.hpp"
#include "Global.h"
#include "Pointer.h"
#include <fstream>
#include <nlohmann.json/json.hpp>

#define MAX_MAP_CACHE_SIZE 5
#define MAX_RECURSIVE_CALLS 40

using namespace EW;

namespace Map {

	class Map {
	public:
		struct CachedMap {
			uint32_t mapId;
			std::vector<Vector3> spawnPoints;
		};

		static void ToggleFadeOutAllNPCs(bool on);
		static void FadeOutAllNPCs(void* enemyIns);
		typedef void WarpFunction(void* CSLuaEventProxy, void* CSLuaEventScriptImitation, uint32_t warpLocation);
		static WarpFunction* WarpFunctionOriginal;

		static uint32_t CurrentMapId;
		static std::vector<Vector3> SpawnPoints;
		static std::vector<CachedMap> CachedMaps;

		static bool TryRefreshMapSpawnPointsFromCache();
		static void RefreshMapSpawnPoints(std::string dllPath);
		static std::vector<Vector3> GetNearbySpwanPoints(Vector3 currentPos, float maxDistance = 50.0f, int numRecursiveCalls = MAX_RECURSIVE_CALLS);

	private:
		static bool NPCsHidden;
	};

	inline void Map::ToggleFadeOutAllNPCs(bool on) { Map::NPCsHidden = on; }
	inline void Map::FadeOutAllNPCs(void* enemyIns) {
		uint8_t* enemyData = static_cast<uint8_t*>(enemyIns);
		uint8_t* fadeout = enemyData + 0x20;
		*fadeout = Map::NPCsHidden;
	}

	inline std::vector<Vector3> Map::SpawnPoints;
	inline void Map::RefreshMapSpawnPoints(std::string dllPath) {

		// In case a player is walking in and out of a dungeoun.
		// The last 5 maps are cached so there's no need to look up
		// the json file multiple times.
		if (TryRefreshMapSpawnPointsFromCache()) return;

		using json = nlohmann::json;
		std::ifstream f(dllPath + "\\EldenWarfare\\SpawnPoints.json");
		json jsonArray = json::parse(f);

		for (const auto& obj : jsonArray) {
			std::string mapId = obj["mapId"];
			uint32_t mapIdUint = std::stoul(mapId, nullptr, 16);
			if (CurrentMapId == mapIdUint) {
				printf("This mapId matched: %s\n", mapId.c_str()); 
				const auto& spawnPoints = obj["invasionPoints"];
				size_t numSpawnPoints = spawnPoints.size();
				printf("Number of spawn points for this mapId: %zu\n", numSpawnPoints);
				SpawnPoints.clear();
				for (const auto& invasionPoint : spawnPoints) {
					float X = invasionPoint["X"];
					float Y = invasionPoint["Y"];
					float Z = invasionPoint["Z"];
					SpawnPoints.push_back(Vector3(X, Y, Z));
				}

				CachedMap cachedMap;
				cachedMap.mapId = CurrentMapId;
				cachedMap.spawnPoints = SpawnPoints;
				CachedMaps.push_back(cachedMap);

				if (CachedMaps.size() > MAX_MAP_CACHE_SIZE) CachedMaps.erase(CachedMaps.begin());
			}
		}
	}
	inline std::vector<Map::CachedMap> Map::CachedMaps;
	inline bool Map::TryRefreshMapSpawnPointsFromCache() {
		for (const auto& cachedMap : CachedMaps) {
			if (cachedMap.mapId == CurrentMapId) {
				printf("Map: %X\n", cachedMap.mapId);
				printf("Was already cached!\n");
				printf("Number of spawn points for this mapId: %zu\n", cachedMap.spawnPoints.size());
				SpawnPoints = cachedMap.spawnPoints;
				return true;
			}
		}
		return false;
	}

	inline std::vector<Vector3> Map::GetNearbySpwanPoints(Vector3 currentPos, float maxDistance, int numRecursiveCalls) {
		std::vector<Vector3> nearbyPoints = {};

		if (numRecursiveCalls > MAX_RECURSIVE_CALLS) {
			printf("Maximum recursive calls exceeded. Random teleport failed.\n");
			return { currentPos };
		}

		std::vector<Vector3> spawnPoints = Map::Map::SpawnPoints;

		for (int i = 0; i < spawnPoints.size(); i++) {
			if (currentPos.x == spawnPoints[i].x &&
				currentPos.y == spawnPoints[i].y &&
				currentPos.z == spawnPoints[i].z) {
				continue;
			}

			auto distance = std::sqrt(
				std::pow((currentPos.x - spawnPoints[i].x), 2) +
				std::pow((currentPos.y - spawnPoints[i].y), 2) +
				std::pow((currentPos.z - spawnPoints[i].z), 2)
			);

			if (distance < maxDistance) {
				nearbyPoints.push_back(spawnPoints[i]);
			}
		}

		if (nearbyPoints.size() < 2) {
			return Map::GetNearbySpwanPoints(currentPos, maxDistance + 10.0f, numRecursiveCalls + 1);
		}

		return nearbyPoints;
	}
}