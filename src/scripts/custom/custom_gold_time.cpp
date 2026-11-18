#include "scriptPCH.h"
#include "Chat.h"

#include <cstdio>
#include <mutex>

namespace
{
    std::mutex g_goldTimeMutex;

    uint64 const COPPER_PER_GOLD = 10000ULL;
    uint32 const HEALER_BUFF_SPELL = 34007;

    char const* const TXT_PRICE_HEAD = "\xE5\x85\x91\xE6\x8D\xA2\xE8\xB4\xA6\xE6\x88\xB7\xE6\x97\xB6\xE9\x97\xB4\xEF\xBC\x88\xE5\xBD\x93\xE5\x89\x8D\xE4\xBB\xB7\xE6\xA0\xBC\x20";
    char const* const TXT_PRICE_TAIL = "\x20\xE9\x87\x91\xE5\xB8\x81\xEF\xBC\x8C\xE7\x82\xB9\xE5\x87\xBB\xE6\x9F\xA5\xE7\x9C\x8B\xEF\xBC\x89";
    char const* const TXT_CONFIRM_HEAD = "\xE7\xA1\xAE\xE8\xAE\xA4\xE5\x85\x91\xE6\x8D\xA2\xEF\xBC\x9A\xE6\xB6\x88\xE8\x80\x97\x20";
    char const* const TXT_CONFIRM_TAIL = "\x20\xE9\x87\x91\xE5\xB8\x81\xEF\xBC\x8C\xE8\x8E\xB7\xE5\xBE\x97\x20\x34\x30\x30\x30\x20\xE5\x88\x86\xE9\x92\x9F\xE8\xB4\xA6\xE6\x88\xB7\xE6\x97\xB6\xE9\x97\xB4";
    char const* const TXT_NO_MONEY_HEAD = "\x5B\xE5\x85\x91\xE6\x8D\xA2\xE5\xA4\xB1\xE8\xB4\xA5\x5D\x20\xE9\x87\x91\xE5\xB8\x81\xE4\xB8\x8D\xE8\xB6\xB3\xEF\xBC\x8C\xE5\xBD\x93\xE5\x89\x8D\xE9\x9C\x80\xE8\xA6\x81\x20";
    char const* const TXT_NO_MONEY_TAIL = "\x20\xE9\x87\x91\xE5\xB8\x81";
    char const* const TXT_CANCEL = "\xE5\x8F\x96\xE6\xB6\x88";
    char const* const TXT_BUY_OK = "\x5B\xE5\x85\x91\xE6\x8D\xA2\xE6\x88\x90\xE5\x8A\x9F\x5D\x20\xE5\xB7\xB2\xE4\xB8\xBA\xE8\xB4\xA6\xE5\x8F\xB7\xE5\xA2\x9E\xE5\x8A\xA0\x20\x34\x30\x30\x30\x20\xE5\x88\x86\xE9\x92\x9F\xE8\xB4\xA6\xE6\x88\xB7\xE6\x97\xB6\xE9\x97\xB4";

    char const* const TXT_HEALER_ENTRY = "\xE6\xB2\xBB\xE7\x96\x97\xE5\xA4\xA9\xE8\xB5\x8B\xE5\xA2\x9E\xE5\xBC\xBA\xEF\xBC\x88\xE4\xBC\xA4\xE5\xAE\xB3\x20\x2B\x35\x30\x25\xEF\xBC\x8C\xE6\xB2\xBB\xE7\x96\x97\x20\x2D\x35\x30\x25\xEF\xBC\x89";
    char const* const TXT_HEALER_RULES = "\x5B\xE8\xAF\xB4\xE6\x98\x8E\x5D\x20\xE4\xBB\x85\xE5\x9C\xA8\xE5\x8D\x87\xE7\xBA\xA7\xE9\x98\xB6\xE6\xAE\xB5\xE5\x8F\xAF\xE7\x94\xA8\xEF\xBC\x9B\xE8\xBF\x9B\xE5\x85\xA5\xE5\x89\xAF\xE6\x9C\xAC\xE3\x80\x81\xE5\x9B\xA2\xE9\x98\x9F\xE5\x89\xAF\xE6\x9C\xAC\xE3\x80\x81\xE6\x88\x98\xE5\x9C\xBA\xE6\x88\x96\xE8\xBE\xBE\xE5\x88\xB0\x20\x36\x30\x20\xE7\xBA\xA7\xE5\x90\x8E\xE8\x87\xAA\xE5\x8A\xA8\xE5\x8F\x96\xE6\xB6\x88\xE3\x80\x82\xE5\xBC\x80\xE5\x90\xAF\xE6\x9C\x9F\xE9\x97\xB4\xE6\xB2\xBB\xE7\x96\x97\xE6\x95\x88\xE6\x9E\x9C\xE9\x99\x8D\xE4\xBD\x8E\x20\x35\x30\x25\xEF\xBC\x8C\xE9\x80\xA0\xE6\x88\x90\xE7\x9A\x84\xE4\xBC\xA4\xE5\xAE\xB3\xE6\x8F\x90\xE9\xAB\x98\x20\x35\x30\x25\xE3\x80\x82";
    char const* const TXT_HEALER_ENABLE = "\xE5\xBC\x80\xE5\x90\xAF\xE6\xB2\xBB\xE7\x96\x97\xE5\xA4\xA9\xE8\xB5\x8B\xE5\xA2\x9E\xE5\xBC\xBA";
    char const* const TXT_HEALER_BACK = "\xE8\xBF\x94\xE5\x9B\x9E";
    char const* const TXT_HEALER_OK = "\xE6\xB2\xBB\xE7\x96\x97\xE5\xA2\x9E\xE5\xBC\xBA\xE5\xBC\x80\xE5\x90\xAF\xE6\x88\x90\xE5\x8A\x9F";
    char const* const TXT_HEALER_ALREADY = "\xE6\xB2\xBB\xE7\x96\x97\xE5\xA2\x9E\xE5\xBC\xBA\xE5\xB7\xB2\xE5\xBC\x80\xE5\x90\xAF";
    char const* const TXT_HEALER_NOT_QUALIFIED = "\xE5\xBD\x93\xE5\x89\x8D\xE5\xA4\xA9\xE8\xB5\x8B\xE4\xB8\x8D\xE6\xBB\xA1\xE8\xB6\xB3\xEF\xBC\x8C\xE9\x9C\x80\xE6\xB2\xBB\xE7\x96\x97\xE7\xB3\xBB\xE6\xAF\x94\xE5\x8F\xA6\xE4\xB8\xA4\xE7\xB3\xBB\xE5\x90\x84\xE9\xAB\x98\x20\x35\x20\xE7\x82\xB9\xE4\xBB\xA5\xE4\xB8\x8A\xE3\x80\x82";
    char const* const TXT_HEALER_MAXLEVEL = "\x36\x30\x20\xE7\xBA\xA7\xE5\x90\x8E\xE6\x97\xA0\xE6\xB3\x95\xE5\xBC\x80\xE5\x90\xAF\xE8\xAF\xA5\xE5\xA2\x9E\xE5\xBC\xBA\xE3\x80\x82";

    uint32 GetGoldTimePrice()
    {
        uint32 price = 400;
        auto result = LoginDatabase.Query("SELECT current_price FROM gold_time_price WHERE id = 1");
        if (result)
        {
            Field* fields = result->Fetch();
            if (fields)
                price = fields[0].GetUInt32();
        }
        return price;
    }

    void SendMenu(Player* player, Creature* creature)
    {
        player->PrepareGossipMenu(creature, creature->GetDefaultGossipMenuId());

        uint32 price = GetGoldTimePrice();
        char text[256];
        snprintf(text, sizeof(text), "%s%u%s", TXT_PRICE_HEAD, price, TXT_PRICE_TAIL);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, text, GOSSIP_SENDER_MAIN, 100);

        if (player->GetLevel() < 60)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TXT_HEALER_ENTRY, GOSSIP_SENDER_MAIN, 200);

        player->SendPreparedGossip(creature);
    }

    void ShowBuyConfirm(Player* player, Creature* creature)
    {
        player->PrepareGossipMenu(creature, creature->GetDefaultGossipMenuId());

        uint32 price = GetGoldTimePrice();
        char text[256];
        snprintf(text, sizeof(text), "%s%u%s", TXT_CONFIRM_HEAD, price, TXT_CONFIRM_TAIL);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, text, GOSSIP_SENDER_MAIN, 101);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TXT_CANCEL, GOSSIP_SENDER_MAIN, 102);

        player->SendPreparedGossip(creature);
    }

    void DoBuy(Player* player, Creature* creature)
    {
        std::lock_guard<std::mutex> lock(g_goldTimeMutex);

        uint32 price = GetGoldTimePrice();
        uint64 cost = uint64(price) * COPPER_PER_GOLD;

        if (cost > 2000000000ULL || uint64(player->GetMoney()) < cost)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "%s%u%s", TXT_NO_MONEY_HEAD, price, TXT_NO_MONEY_TAIL);
            ChatHandler(player->GetSession()).SendSysMessage(msg);
            player->PlayerTalkClass->CloseGossip();
            return;
        }

        player->ModifyMoney(-(int32)cost);

        LoginDatabase.PExecute("UPDATE account SET playtime_remaining = playtime_remaining + 4000 WHERE UPPER(username) = '%s'",
            player->GetSession()->GetUsername().c_str());
        LoginDatabase.Execute("UPDATE gold_time_price SET current_price = current_price + 1, last_purchase_time = UNIX_TIMESTAMP() WHERE id = 1");
        LoginDatabase.PExecute("INSERT INTO gold_time_log (account_id, player_name, change_type, price_before, price_after, change_value) VALUES (%u, '%s', 'BUY', %u, %u, 1)",
            player->GetSession()->GetAccountId(), player->GetName(), price, price + 1);

        ChatHandler(player->GetSession()).SendSysMessage(TXT_BUY_OK);
        player->PlayerTalkClass->CloseGossip();
    }

    void ShowHealerMenu(Player* player, Creature* creature)
    {
        player->PrepareGossipMenu(creature, creature->GetDefaultGossipMenuId());
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, TXT_HEALER_ENABLE, GOSSIP_SENDER_MAIN, 201);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TXT_HEALER_BACK, GOSSIP_SENDER_MAIN, 202);

        ChatHandler(player->GetSession()).SendSysMessage(TXT_HEALER_RULES);

        player->SendPreparedGossip(creature);
    }

    void DoEnableHealerBuff(Player* player)
    {
        char const* msg = nullptr;

        if (player->GetLevel() >= 60)
            msg = TXT_HEALER_MAXLEVEL;
        else if (player->HasAura(HEALER_BUFF_SPELL))
            msg = TXT_HEALER_ALREADY;
        else if (!player->HasHealerTalent())
            msg = TXT_HEALER_NOT_QUALIFIED;
        else
        {
            player->CastSpell(player, HEALER_BUFF_SPELL, true);
            msg = TXT_HEALER_OK;
        }

        ChatHandler(player->GetSession()).SendSysMessage(msg);
        player->PlayerTalkClass->CloseGossip();
    }
}

bool GossipHello_GoldTime(Player* player, Creature* creature)
{
    SendMenu(player, creature);
    return true;
}

bool GossipSelect_GoldTime(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    switch (action)
    {
        case 100:
            ShowBuyConfirm(player, creature);
            return true;
        case 101:
            DoBuy(player, creature);
            return true;
        case 102:
            player->PlayerTalkClass->CloseGossip();
            return true;
        case 200:
            ShowHealerMenu(player, creature);
            return true;
        case 201:
            DoEnableHealerBuff(player);
            return true;
        case 202:
            SendMenu(player, creature);
            return true;
        default:
            break;
    }
    return false;
}

void AddSC_custom_gold_time()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "custom_gold_time";
    pNewScript->pGossipHello = &GossipHello_GoldTime;
    pNewScript->pGossipSelect = &GossipSelect_GoldTime;
    pNewScript->RegisterSelf();
}