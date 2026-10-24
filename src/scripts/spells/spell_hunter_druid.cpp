#include "scriptPCH.h"
#include "Chat.h"
#include <unordered_map>

static std::unordered_map<uint32, std::pair<uint32, float>> g_petOriginalDisplay;
static std::unordered_map<uint32, uint32> g_hunterDruidTeam;

bool HasHunterDruidTeam(uint32 playerGuid)
{
	return g_hunterDruidTeam.find(playerGuid) != g_hunterDruidTeam.end();
}

void HunterDruidTransform(Player* player)
{
	if (!player)
		return;

	Pet* pet = player->GetPet();
	if (!pet || !pet->IsAlive())
		return;

	if (player->GetDistance(pet) > 20.0f)
		return;

	uint32 team = player->GetTeamId();
	auto it = g_hunterDruidTeam.find(player->GetGUIDLow());
	if (it != g_hunterDruidTeam.end())
	{
		team = it->second;
		g_hunterDruidTeam.erase(it);
	}

	static const uint32 ALLIANCE_FORMS[] = { 892, 2281 };
	static const uint32 HORDE_FORMS[] = { 8571, 2289 };

	const uint32* forms = (team == TEAM_ALLIANCE) ? ALLIANCE_FORMS : HORDE_FORMS;
	uint32 formCount = 2;

	uint32 currentDisplay = pet->GetDisplayId();
	bool isTransformed = false;
	for (uint32 i = 0; i < formCount; ++i)
	{
		if (currentDisplay == forms[i])
		{
			isTransformed = true;
			break;
		}
	}

	uint32 petGuidLow = pet->GetGUIDLow();

	if (isTransformed)
	{
		auto it2 = g_petOriginalDisplay.find(petGuidLow);
		if (it2 != g_petOriginalDisplay.end())
		{
			pet->SetDisplayId(it2->second.first);
			pet->SetObjectScale(it2->second.second);
			pet->ForceValuesUpdateAtIndex(UNIT_FIELD_DISPLAYID);
			g_petOriginalDisplay.erase(it2);
		}
	}
	else
	{
		g_petOriginalDisplay[petGuidLow] = { currentDisplay, pet->GetObjectScale() };
		uint32 newForm = forms[urand(0, formCount - 1)];
		pet->SetDisplayId(newForm);
		pet->SetObjectScale(1.0f);
		pet->ForceValuesUpdateAtIndex(UNIT_FIELD_DISPLAYID);
	}
}

struct HunterDruidScript : SpellScript
{
	SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
	{
		Player* player = spell->m_caster ? spell->m_caster->ToPlayer() : nullptr;
		if (!player)
			return SPELL_FAILED_ERROR;

		if (player->GetClass() != CLASS_HUNTER)
			return SPELL_FAILED_BAD_TARGETS;

		Pet* pet = player->GetPet();
		if (!pet || !pet->IsAlive())
		{
			ChatHandler(player->GetSession()).SendSysMessage(
				"\xE8\xAF\xB7\xE5\x8F\xAC\xE5\x94\xA4\xE5\x87\xBA\xE5\xAE\xA0\xE7\x89\xA9\xE5\x86\x8D\xE4\xBD\xBF\xE7\x94\xA8");
			return SPELL_FAILED_BAD_TARGETS;
		}

		ObjectGuid guid = spell->m_targets.getUnitTargetGuid();
		if (!guid)
			return SPELL_FAILED_BAD_TARGETS;

		if (guid == player->GetObjectGuid())
			return SPELL_CAST_OK;

		Player* targetPlayer = sObjectMgr.GetPlayer(guid);
		if (!targetPlayer)
			return SPELL_FAILED_BAD_TARGETS;

		if (targetPlayer->GetClass() != CLASS_DRUID)
			return SPELL_FAILED_BAD_TARGETS;

		return SPELL_CAST_OK;
	}

	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_0)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		Unit* target = spell->GetUnitTarget();
		if (!target)
			return true;

		if (target == player)
		{
			g_hunterDruidTeam[player->GetGUIDLow()] = player->GetTeamId();
			return true;
		}

		Player* targetPlayer = target->ToPlayer();
		if (targetPlayer && targetPlayer->GetClass() == CLASS_DRUID)
		{
			g_hunterDruidTeam[player->GetGUIDLow()] = targetPlayer->GetTeamId();
		}

		return true;
	}
};

SpellScript* GetScript_HunterDruid(SpellEntry const*)
{
	return new HunterDruidScript();
}

void AddSC_spell_hunter_druid()
{
	Script* newscript = new Script;
	newscript->Name = "spell_hunter_druid";
	newscript->GetSpellScript = &GetScript_HunterDruid;
	newscript->RegisterSelf();
}