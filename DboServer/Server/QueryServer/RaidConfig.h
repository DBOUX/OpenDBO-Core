#pragma once
#include "stdafx.h"

struct sRaidWeeklyWeapon
{
    uint64_t id;
    int itemId;
    time_t startDate;
    time_t endDate;
    std::string note;
};

class CRaidConfig : public CNtlSingleton<CRaidConfig>
{
public:
    CRaidConfig();
    ~CRaidConfig();

    void Init();
    void Destroy();

    // Load active weekly weapon (based on server time). Returns nullptr if none.
    sRaidWeeklyWeapon* GetActiveWeeklyWeapon();

    // Load per-player progression from DB
    float LoadPlayerProgression(CHARACTERID charId);
    void SavePlayerProgression(CHARACTERID charId, float fProgress);

    // Read generic config by key (used for custom enchanting item IDs)
    std::string GetConfigValue(const std::string& key);

    // Reload config/weekly weapon from DB at runtime
    void Reload();

private:
    sRaidWeeklyWeapon* m_pActiveWeapon; // cached
};