#include "stdafx.h"
#include "RaidConfig.h"
#include "QueryServer.h" // use the same DB helpers as Dojo

CRaidConfig::CRaidConfig()
{
    Init();
}

CRaidConfig::~CRaidConfig()
{
    Destroy();
}

void CRaidConfig::Init()
{
    m_pActiveWeapon = nullptr;
    Reload();
}

void CRaidConfig::Destroy()
{
    if (m_pActiveWeapon) { delete m_pActiveWeapon; m_pActiveWeapon = nullptr; }
}

void CRaidConfig::Reload()
{
    // TODO: query raid_weekly_weapon where start_date <= NOW() AND end_date >= NOW()
    // and populate m_pActiveWeapon (only one active expected)
    if (m_pActiveWeapon)
    {
        delete m_pActiveWeapon;
        m_pActiveWeapon = nullptr;
    }

    // Example (pseudocode):
    // smart_ptr<QueryResult> result = GetCharDB.Query("SELECT id, item_id, UNIX_TIMESTAMP(start_date), UNIX_TIMESTAMP(end_date), note FROM raid_weekly_weapon WHERE start_date <= NOW() AND end_date >= NOW() LIMIT 1");
    // if (result) { populate m_pActiveWeapon }
}

sRaidWeeklyWeapon* CRaidConfig::GetActiveWeeklyWeapon()
{
    return m_pActiveWeapon;
}

float CRaidConfig::LoadPlayerProgression(CHARACTERID charId)
{
    // TODO: SELECT progression FROM raid_progression WHERE char_id = ?
    // Return 0.0f if not found
    return 0.0f;
}

void CRaidConfig::SavePlayerProgression(CHARACTERID charId, float fProgress)
{
    // TODO: INSERT ... ON DUPLICATE KEY UPDATE progression = fProgress, last_updated = NOW()
}

std::string CRaidConfig::GetConfigValue(const std::string& key)
{
    // TODO: SELECT cfg_value FROM raid_config WHERE cfg_key = ?
    return std::string();
}