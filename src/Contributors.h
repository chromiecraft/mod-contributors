#ifndef DEF_CONTRIBUTORS_H
#define DEF_CONTRIBUTORS_H

#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

enum ContributorLevel : uint8
{
    CONTRIBUTOR_LEVEL_BRONZE   = 1,
    CONTRIBUTOR_LEVEL_SILVER   = 2,
    CONTRIBUTOR_LEVEL_GOLD     = 3,
    CONTRIBUTOR_LEVEL_PLATINUM = 4,
    CONTRIBUTOR_LEVEL_MAX      = CONTRIBUTOR_LEVEL_PLATINUM
};

struct ContributorAccountData
{
    uint32 AccountId;
    uint32 GrantedDate;
    uint32 EndDate;
    uint8 Level;
    bool Permanent;
    bool Expired;

    ContributorAccountData() : AccountId(0), GrantedDate(0), EndDate(0), Level(CONTRIBUTOR_LEVEL_BRONZE), Permanent(false), Expired(false) { }

    ContributorAccountData(uint32 accountId, uint32 grantedDate, uint32 endDate, uint8 level, bool permanent, bool expired = false)
        : AccountId(accountId), GrantedDate(grantedDate), EndDate(endDate), Level(level), Permanent(permanent), Expired(expired) { }
};

enum ContributorSettings
{
    SETTING_CONTRIBUTOR_DISABLE = 0
};

const std::string ModContributorString = "mod_contributors";

class ContributorsMgr
{
private:
    std::unordered_map<uint32, ContributorAccountData> Accounts;
    uint8 MaxAffectedLevel{ 69 };

public:
    static ContributorsMgr* instance();

    bool IsEnabled{ false };
    uint32 Duration{ 30 };

    [[nodiscard]] bool IsContributor(uint32 accountId) const { return Accounts.find(accountId) != Accounts.end(); }

    void InsertAccountData(ContributorAccountData data);

    [[nodiscard]] ContributorAccountData GetAccountData(uint32 accountId) const
    {
        auto itr = Accounts.find(accountId);
        if (itr != Accounts.end())
            return itr->second;

        return {};
    }

    void SetExpired(uint32 accountId);
    void LoadAccountData();
    void GrantContributor(uint32 accountId, int32 days, uint8 level = CONTRIBUTOR_LEVEL_BRONZE);
    void SetContributorLevel(uint32 accountId, uint8 level);
    void RevokeContributor(uint32 accountId);

    static char const* GetLevelName(uint8 level)
    {
        switch (level)
        {
            case CONTRIBUTOR_LEVEL_BRONZE:   return "Bronze";
            case CONTRIBUTOR_LEVEL_SILVER:   return "Silver";
            case CONTRIBUTOR_LEVEL_GOLD:     return "Gold";
            case CONTRIBUTOR_LEVEL_PLATINUM: return "Platinum";
            default:                         return "Unknown";
        }
    }

    [[nodiscard]] uint8 GetMaxAffectedLevel() const { return MaxAffectedLevel; }
    void SetMaxAffectedLevel(uint8 level) { MaxAffectedLevel = level; }
};

#define sContributors ContributorsMgr::instance()

#endif
