#pragma once
#include "stdafx.h"
#include <map>

struct sPlayerProgress
{
    CHARACTERID charId;
    float fProgress; // 0.0 - 100.0
    time_t lastUpdated;
};

class CRaidProgressionManager : public CNtlSingleton<CRaidProgressionManager>
{
public:
    CRaidProgressionManager();
    ~CRaidProgressionManager();

    void Init();
    void Destroy();

    // Called on server start to preload progression
    void LoadAllProgression(); // optional

    // Called when a raid stage completes. presentPlayers is the set of characters present/alive at completion
    void OnStageComplete(uint64_t runId, const std::vector<CHARACTERID>& presentPlayers);

    // Access per-player progress
    float GetPlayerProgression(CHARACTERID charId);
    void SetPlayerProgression(CHARACTERID charId, float fPercent, bool bSaveToDB = true);

    // Called by entry logic to verify level & party size (6 required)
    bool CanPartyEnterRaid(const std::vector<CHARACTERID>& partyMembers, int minLevel = 75, int requiredPartySize = 6, std::string* outFailReason = nullptr);

    // Attempt reward distribution if party all at 100%
    void TryDistributeWeeklyWeaponIfAllFull(const std::vector<CHARACTERID>& partyMembers, uint64_t runId);

    // Reload weekly weapon config
    void ReloadWeeklyWeaponConfig();

private:
    std::map<CHARACTERID, sPlayerProgress> m_mapProgress;
    void SaveProgressionToDB(CHARACTERID charId);

    // progression increment calculation
    float CalcProgressIncrement(float currentProgress);
};