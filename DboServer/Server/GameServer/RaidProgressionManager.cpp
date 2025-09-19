#include "stdafx.h"
#include "RaidProgressionManager.h"
#include "RaidConfig.h" // for DB persistence and weekly weapon
#include "PlayerManager.h"
#include <algorithm>

CRaidProgressionManager::CRaidProgressionManager()
{
    Init();
}

CRaidProgressionManager::~CRaidProgressionManager()
{
    Destroy();
}

void CRaidProgressionManager::Init()
{
    m_mapProgress.clear();
    ReloadWeeklyWeaponConfig();
}

void CRaidProgressionManager::Destroy()
{
    for (auto &p : m_mapProgress)
    {
        SaveProgressionToDB(p.first);
    }
    m_mapProgress.clear();
}

void CRaidProgressionManager::LoadAllProgression()
{
    // Optional: batch load all rows from raid_progression and fill m_mapProgress
}

float CRaidProgressionManager::GetPlayerProgression(CHARACTERID charId)
{
    auto it = m_mapProgress.find(charId);
    if (it != m_mapProgress.end()) return it->second.fProgress;
    float val = (float)CRaidConfig::GetInstance()->LoadPlayerProgression(charId);
    sPlayerProgress sp; sp.charId = charId; sp.fProgress = val; sp.lastUpdated = time(NULL);
    m_mapProgress.insert(std::make_pair(charId, sp));
    return val;
}

void CRaidProgressionManager::SetPlayerProgression(CHARACTERID charId, float fPercent, bool bSaveToDB)
{
    if (fPercent < 0.0f) fPercent = 0.0f;
    if (fPercent > 100.0f) fPercent = 100.0f;

    sPlayerProgress sp;
    sp.charId = charId;
    sp.fProgress = fPercent;
    sp.lastUpdated = time(NULL);
    m_mapProgress[charId] = sp;

    if (bSaveToDB) SaveProgressionToDB(charId);

    // TODO: notify client with packet about progression update
}

void CRaidProgressionManager::SaveProgressionToDB(CHARACTERID charId)
{
    auto it = m_mapProgress.find(charId);
    if (it == m_mapProgress.end()) return;
    CRaidConfig::GetInstance()->SavePlayerProgression(charId, it->second.fProgress);
}

float CRaidProgressionManager::CalcProgressIncrement(float currentProgress)
{
    if (currentProgress < 50.0f)
        return 5.0f;    // +5% per stage until 50%
    else
        return 1.5f;    // after 50% each stage adds +1.5%
}

void CRaidProgressionManager::OnStageComplete(uint64_t runId, const std::vector<CHARACTERID>& presentPlayers)
{
    for (CHARACTERID cid : presentPlayers)
    {
        float current = GetPlayerProgression(cid);
        float inc = CalcProgressIncrement(current);
        float next = std::min(100.0f, current + inc);
        SetPlayerProgression(cid, next, true);
    }

    TryDistributeWeeklyWeaponIfAllFull(presentPlayers, runId);
}

bool CRaidProgressionManager::CanPartyEnterRaid(const std::vector<CHARACTERID>& partyMembers, int minLevel, int requiredPartySize, std::string* outFailReason)
{
    if ((int)partyMembers.size() != requiredPartySize)
    {
        if (outFailReason) *outFailReason = "Party must be exactly 6 members.";
        return false;
    }

    for (CHARACTERID cid : partyMembers)
    {
        CPlayer* p = g_pPlayerManager->FindPlayerWithCharID(cid);
        if (!p)
        {
            if (outFailReason) *outFailReason = "All party members must be online.";
            return false;
        }
        if (p->GetLevel() < minLevel)
        {
            if (outFailReason) *outFailReason = "All party members must be level 75 or higher.";
            return false;
        }
    }

    return true;
}

void CRaidProgressionManager::TryDistributeWeeklyWeaponIfAllFull(const std::vector<CHARACTERID>& partyMembers, uint64_t runId)
{
    if (partyMembers.empty()) return;

    bool allFull = true;
    for (CHARACTERID cid : partyMembers)
    {
        float prog = GetPlayerProgression(cid);
        if (prog < 100.0f)
        {
            allFull = false;
            break;
        }
    }

    if (!allFull) return;

    sRaidWeeklyWeapon* w = CRaidConfig::GetInstance()->GetActiveWeeklyWeapon();
    if (!w) return;

    for (CHARACTERID cid : partyMembers)
    {
        CPlayer* p = g_pPlayerManager->FindPlayerWithCharID(cid);
        if (p && p->IsInitialized())
        {
            // TODO: call the server inventory API to add item w->itemId to player p
            p->SendSystemMessage(L"You have received the Raid Weapon of the Week!");
        }
    }

    for (CHARACTERID cid : partyMembers)
    {
        SetPlayerProgression(cid, 0.0f, true); // reset after awarding
    }
}

void CRaidProgressionManager::ReloadWeeklyWeaponConfig()
{
    CRaidConfig::GetInstance()->Reload();
}