/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>,
 * released under GNU AGPL v3 license:
 * https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Contributors.h"
#include "Chat.h"
#include "Config.h"
#include "GameTime.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"

class mod_contributors_playerscript : public PlayerScript
{
public:
    mod_contributors_playerscript() : PlayerScript("mod_contributors_playerscript", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_LEVEL_CHANGED
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (!sContributors->IsEnabled)
            return;

        if (player->GetLevel() == 1)
            return;

        if (player->GetLevel() >= sContributors->GetMaxAffectedLevel())
            return;

        ProcessBonusChecks(player);
    }

    void OnPlayerLevelChanged(Player* player, uint8 oldlevel) override
    {
        if (!sContributors->IsEnabled)
            return;

        if (oldlevel >= sContributors->GetMaxAffectedLevel())
            return;

        if (player->GetLevel() >= sContributors->GetMaxAffectedLevel())
            return;

        ProcessBonusChecks(player);
    }

    bool ProcessBonusChecks(Player* player) const
    {
        uint32 accountId = player->GetSession()->GetAccountId();

        if (!sContributors->IsContributor(accountId))
            return false;

        ContributorAccountData const& data = sContributors->GetAccountData(accountId);

        if (data.Expired)
            return false;

        // Check if player has opted out
        if (player->GetPlayerSetting(ModContributorString, SETTING_CONTRIBUTOR_DISABLE).IsEnabled())
        {
            player->SendSystemMessage("|cffff0000You are eligible for the Contributor rested XP bonus, but you have disabled it. Use .contributor disable off to re-enable.|r");
            return false;
        }

        // Permanent contributors always get the bonus
        if (data.Permanent)
        {
            player->SetRestBonus(sObjectMgr->GetXPForLevel(player->GetLevel()));
            player->SendSystemMessage("|cff00ccffThank you for contributing! You have been granted rested experience. (Permanent)|r");
            return true;
        }

        // Timed contributors - check expiry
        uint32 now = GameTime::GetGameTime().count();
        if (data.EndDate && data.EndDate > now)
        {
            player->SetRestBonus(sObjectMgr->GetXPForLevel(player->GetLevel()));
            tm endTime = Acore::Time::TimeBreakdown(data.EndDate);
            ChatHandler(player->GetSession()).PSendSysMessage("|cff00ccffThank you for contributing! You have been granted rested experience until {:%Y-%m-%d %H:%M}.|r", endTime);
            return true;
        }

        // Bonus expired
        sContributors->SetExpired(accountId);
        return false;
    }
};

class mod_contributors_worldscript : public WorldScript
{
public:
    mod_contributors_worldscript() : WorldScript("mod_contributors_worldscript", {
        WORLDHOOK_ON_AFTER_CONFIG_LOAD
    }) { }

    void OnAfterConfigLoad(bool reload) override
    {
        sContributors->IsEnabled = sConfigMgr->GetOption<bool>("ModContributors.Enable", false);
        sContributors->Duration = sConfigMgr->GetOption<uint32>("ModContributors.Duration", 30);
        sContributors->SetMaxAffectedLevel(sConfigMgr->GetOption<uint8>("ModContributors.MaxAffectedLevel", 70));

        if (!reload)
            sContributors->LoadAccountData();
    }
};

void AddModContributorsScripts()
{
    new mod_contributors_playerscript();
    new mod_contributors_worldscript();
}
