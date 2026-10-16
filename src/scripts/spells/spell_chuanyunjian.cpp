#include "scriptPCH.h"
#include "Chat.h"
#include "Language.h"
#include "ArrowRecord.h"
#include <ctime>
#include <string>

ArrowRecord g_arrowRecords[2];

// 白名单区域
static const uint32 ALLOWED_ZONES[] = {
	28, 139, 36, 267, 47, 45, 11, 51, 3, 46, 44, 10, 41, 8, 33, 4,
	361, 618, 331, 406, 405, 15, 357, 400, 1377, 490, 440,
};

// 34001 - 穿云箭
struct ChuanyunjianScript : SpellScript
{
	SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
	{
		Player* player = spell->m_caster ? spell->m_caster->ToPlayer() : nullptr;
		if (!player)
			return SPELL_FAILED_ERROR;

		// 地图限制
		uint32 zoneId = player->GetZoneId();
		bool allowed = false;
		for (uint32 z : ALLOWED_ZONES)
		{
			if (z == zoneId) { allowed = true; break; }
		}
		if (!allowed)
		{
			ChatHandler(player->GetSession()).SendSysMessage("\xE6\x9C\xAC\xE5\x9C\xB0\xE5\x9B\xBE\xE7\xA6\x81\xE6\xAD\xA2\xE4\xBD\xBF\xE7\x94\xA8");
			return SPELL_FAILED_NOT_HERE;
		}

		// 阵营判定
		uint32 team = player->GetTeamId();
		if (team > 1)
			return SPELL_FAILED_ERROR;

		// 同阵营已有记录
		if (g_arrowRecords[team].active && time(nullptr) < g_arrowRecords[team].expireTime)
		{
			ChatHandler(player->GetSession()).SendSysMessage("\xE5\xBD\x93\xE5\x89\x8D\xE5\x8F\xB7\xE5\x8F\xAC\xE8\xBF\x98\xE6\x9C\xAA\xE7\xBB\x93\xE6\x9D\x9F\xEF\xBC\x8C\xE8\xAF\xB7\xE7\xA8\x8D\xE5\x90\x8E\xE5\x86\x8D\xE8\xAF\x95");
			return SPELL_FAILED_NOT_READY;
		}

		return SPELL_CAST_OK;
	}

	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_1)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		uint32 team = player->GetTeamId();
		if (team > 1)
			return true;

		g_arrowRecords[team].active = true;
		g_arrowRecords[team].team = team;
		g_arrowRecords[team].mapId = player->GetMapId();
		g_arrowRecords[team].x = player->GetPositionX();
		g_arrowRecords[team].y = player->GetPositionY();
		g_arrowRecords[team].z = player->GetPositionZ();
		g_arrowRecords[team].o = player->GetOrientation();
		g_arrowRecords[team].casterName = player->GetName();
		g_arrowRecords[team].expireTime = time(nullptr) + 120;

		std::string mapName;
		uint32 areaId = player->GetAreaId();
		sObjectMgr.GetAreaLocaleString(areaId, player->GetSession()->GetSessionDbLocaleIndex(), &mapName);
		if (mapName.empty())
		{
			areaId = player->GetZoneId();
			sObjectMgr.GetAreaLocaleString(areaId, player->GetSession()->GetSessionDbLocaleIndex(), &mapName);
		}
		if (mapName.empty())
			mapName = "未知地图";

		g_arrowRecords[team].mapName = mapName;

		std::string factionName = (team == 0)
			? "\xE8\x81\x94\xE7\x9B\x9F"
			: "\xE9\x83\xA8\xE8\x90\xBD";
		std::string msg = std::string("|cffffd700")
			+ "\xE4\xB8\x80\xE6\x94\xAF\xE7\xA9\xBF\xE4\xBA\x91\xE7\xAE\xAD"
			+ "\xEF\xBC\x8C"
			+ "\xE5\x8D\x83\xE5\x86\x9B\xE4\xB8\x87\xE9\xA9\xAC\xE6\x9D\xA5\xE7\x9B\xB8\xE8\xA7\x81"
			+ "\xEF\xBC\x81"
			+ "\xE2\x80\x94\xE2\x80\x94"
			+ factionName
			+ "\xE7\x8E\xA9\xE5\xAE\xB6"
			+ player->GetName()
			+ "\xE5\x9C\xA8"
			+ mapName
			+ "\xE4\xBD\xBF\xE7\x94\xA8\xE4\xBA\x86\xE7\xA9\xBF\xE4\xBA\x91\xE7\xAE\xAD"
			+ "\xEF\xBC\x8C"
			+ "\xE8\xAF\xB7\xE9\x99\x84\xE8\xBF\x91\xE7\x9A\x84\xE7\x8E\xA9\xE5\xAE\xB6"
			+ "\xE6\x94\xAF\xE6\x8F\xB4"
			+ "\xEF\xBC\x8C"
			+ "\xE8\xBE\x93\xE5\x85\xA5"
			+ "\"." + "\xE5\x8F\x82\xE6\x88\x98" + "\""
			+ "\xE7\xAB\x8B\xE5\x8D\xB3\xE5\x93\x8D\xE5\xBA\x94\xE5\x8F\xB7\xE5\x8F\xAC"
			+ "\xEF\xBC\x8C"
			+ "\xE5\x8F\xB7\xE5\x8F\xAC\xE5\x80\x92\xE8\xAE\xA1\xE6\x97\xB6"
			+ "2"
			+ "\xE5\x88\x86\xE9\x92\x9F"
			+ "|r";

		sWorld.SendWorldText(LANG_SYSTEMMESSAGE, msg.c_str());

		return true;
	}
};

SpellScript* GetScript_Chuanyunjian(SpellEntry const*)
{
	return new ChuanyunjianScript();
}

void AddSC_chuanyunjian()
{
	Script* newscript = new Script;
	newscript->Name = "spell_chuanyunjian";
	newscript->GetSpellScript = &GetScript_Chuanyunjian;
	newscript->RegisterSelf();
}