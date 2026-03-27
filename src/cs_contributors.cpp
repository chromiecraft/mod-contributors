/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>,
 * released under GNU AGPL v3 license:
 * https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "AccountMgr.h"
#include "Chat.h"
#include "GameTime.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Contributors.h"

using namespace Acore::ChatCommands;

class contributors_commandscript : public CommandScript
{
public:
    contributors_commandscript() : CommandScript("contributors_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable contributorTable =
        {
            { "disable",   HandleContributorDisableCommand,   SEC_PLAYER,        Console::Yes },
            { "info",      HandleContributorInfoCommand,      SEC_PLAYER,        Console::Yes },
            { "grant",     HandleContributorGrantCommand,     SEC_ADMINISTRATOR, Console::Yes },
            { "revoke",    HandleContributorRevokeCommand,    SEC_ADMINISTRATOR, Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "contributor", contributorTable },
        };

        return commandTable;
    }

    static bool HandleContributorDisableCommand(ChatHandler* handler, Optional<PlayerIdentifier> player, bool disable)
    {
        if (handler->GetSession() && AccountMgr::IsPlayerAccount(handler->GetSession()->GetSecurity()))
            player = PlayerIdentifier::FromSelf(handler);

        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        Player* targetPlayer = player ? player->GetConnectedPlayer() : nullptr;

        if (!targetPlayer)
        {
            handler->SendErrorMessage("Player not found or not online.");
            return false;
        }

        targetPlayer->UpdatePlayerSetting(ModContributorString, SETTING_CONTRIBUTOR_DISABLE, disable);

        if (!disable)
            targetPlayer->SendSystemMessage("Contributor rested XP bonus enabled.");
        else
            targetPlayer->SendSystemMessage("Contributor rested XP bonus disabled.");

        if (!handler->GetSession() || handler->GetSession()->GetPlayer() != targetPlayer)
            handler->PSendSysMessage("Contributor rested bonus {} for player {} ({}).", disable ? "disabled" : "enabled", targetPlayer->GetName(), targetPlayer->GetGUID().GetCounter());

        return true;
    }

    static bool HandleContributorInfoCommand(ChatHandler* handler, Optional<AccountIdentifier> account)
    {
        uint32 accountId = 0;

        if (handler->GetSession() && AccountMgr::IsPlayerAccount(handler->GetSession()->GetSecurity()))
            accountId = handler->GetSession()->GetAccountId();
        else if (account)
            accountId = account->GetID();
        else if (handler->GetSession())
            accountId = handler->GetSession()->GetAccountId();

        std::string accountName;
        AccountMgr::GetName(accountId, accountName);

        if (!sContributors->IsContributor(accountId))
        {
            handler->PSendSysMessage("Account {} (ID: {}) is not a contributor.", accountName, accountId);
            return true;
        }

        ContributorAccountData const& data = sContributors->GetAccountData(accountId);

        tm grantedTime = Acore::Time::TimeBreakdown(data.GrantedDate);
        handler->PSendSysMessage("Account {} (ID: {}) granted contributor status on: {:%Y-%m-%d %H:%M}.", accountName, accountId, grantedTime);

        if (data.Permanent)
        {
            handler->PSendSysMessage("Contributor bonus: PERMANENT.");
            return true;
        }

        tm endTime = Acore::Time::TimeBreakdown(data.EndDate);

        if (data.Expired || data.EndDate <= GameTime::GetGameTime().count())
        {
            handler->PSendSysMessage("Contributor bonus expired on: {:%Y-%m-%d %H:%M}.", endTime);
            return true;
        }

        handler->PSendSysMessage("Contributor bonus expires on: {:%Y-%m-%d %H:%M}.", endTime);
        return true;
    }

    // .contributor grant <account> [days]
    // Omit days to use config default. Use -1 for permanent.
    static bool HandleContributorGrantCommand(ChatHandler* handler, AccountIdentifier account, Optional<int32> days)
    {
        uint32 accountId = account.GetID();
        std::string accountName;
        AccountMgr::GetName(accountId, accountName);

        int32 duration = days.value_or(static_cast<int32>(sContributors->Duration));
        sContributors->GrantContributor(accountId, duration);

        if (duration < 0)
            handler->PSendSysMessage("Granted PERMANENT contributor status to account {} (ID: {}).", accountName, accountId);
        else
            handler->PSendSysMessage("Granted contributor status to account {} (ID: {}) for {} days.", accountName, accountId, duration);

        return true;
    }

    static bool HandleContributorRevokeCommand(ChatHandler* handler, AccountIdentifier account)
    {
        uint32 accountId = account.GetID();
        std::string accountName;
        AccountMgr::GetName(accountId, accountName);

        sContributors->RevokeContributor(accountId);

        handler->PSendSysMessage("Revoked contributor status from account {} (ID: {}).", accountName, accountId);
        return true;
    }
};

void AddSC_contributors_commandscript()
{
    new contributors_commandscript();
}
