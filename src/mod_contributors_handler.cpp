/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>,
 * released under GNU AGPL v3 license:
 * https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "GameTime.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Contributors.h"

ContributorsMgr* ContributorsMgr::instance()
{
    static ContributorsMgr instance;
    return &instance;
}

void ContributorsMgr::LoadAccountData()
{
    LOG_INFO("sql.sql", "Loading Contributor account data...");

    if (QueryResult result = CharacterDatabase.Query("SELECT AccountId, GrantedDate, EndDate, Level, Permanent, Expired FROM mod_contributors_accounts"))
    {
        do
        {
            Field* fields = result->Fetch();
            ContributorAccountData data;
            data.AccountId = fields[0].Get<uint32>();
            data.GrantedDate = fields[1].Get<uint32>();
            data.EndDate = fields[2].Get<uint32>();
            data.Level = fields[3].Get<uint8>();
            data.Permanent = fields[4].Get<bool>();
            data.Expired = fields[5].Get<bool>();
            Accounts[data.AccountId] = data;
        } while (result->NextRow());
    }
}

void ContributorsMgr::InsertAccountData(ContributorAccountData data)
{
    Accounts[data.AccountId] = data;
    CharacterDatabase.Execute(
        "REPLACE INTO mod_contributors_accounts (AccountId, GrantedDate, EndDate, Level, Permanent, Expired) VALUES ({}, {}, {}, {}, {}, {})",
        data.AccountId, data.GrantedDate, data.EndDate, data.Level, data.Permanent ? 1 : 0, data.Expired ? 1 : 0);
}

void ContributorsMgr::SetExpired(uint32 accountId)
{
    CharacterDatabase.DirectExecute("UPDATE mod_contributors_accounts SET Expired = 1 WHERE AccountId = {}", accountId);

    if (auto itr = Accounts.find(accountId); itr != Accounts.end())
        itr->second.Expired = true;
}

void ContributorsMgr::GrantContributor(uint32 accountId, int32 days, uint8 level)
{
    uint32 now = GameTime::GetGameTime().count();
    bool permanent = (days < 0);
    uint32 expiration = permanent ? 0 : now + (static_cast<uint32>(days) * DAY);
    InsertAccountData(ContributorAccountData(accountId, now, expiration, level, permanent));
}

void ContributorsMgr::SetContributorLevel(uint32 accountId, uint8 level)
{
    if (auto itr = Accounts.find(accountId); itr != Accounts.end())
    {
        itr->second.Level = level;
        CharacterDatabase.DirectExecute("UPDATE mod_contributors_accounts SET Level = {} WHERE AccountId = {}", level, accountId);
    }
}

void ContributorsMgr::RevokeContributor(uint32 accountId)
{
    CharacterDatabase.DirectExecute("DELETE FROM mod_contributors_accounts WHERE AccountId = {}", accountId);
    Accounts.erase(accountId);
}
