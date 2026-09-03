#include "scriptPCH.h"

// 34002 - 可乐瓶盖-再来一瓶！
struct ColaLevelUpScript : SpellScript
{
	SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
	{
		Player* player = spell->m_caster ? spell->m_caster->ToPlayer() : nullptr;
		if (!player)
			return SPELL_FAILED_ERROR;

		// 必须脱战
		if (player->IsInCombat())
			return SPELL_FAILED_AFFECTING_COMBAT;

		// 最高升到 59 级
		if (player->GetLevel() >= 59)
			return SPELL_FAILED_LEVEL_REQUIREMENT;

		return SPELL_CAST_OK;
	}

	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_0)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		// 提升等级
		player->GiveLevel(player->GetLevel() + 1);
		player->InitTalentForLevel();
		player->SetUInt32Value(PLAYER_XP, 0);

		return true;
	}
};

SpellScript* GetScript_ColaLevelUp(SpellEntry const*)
{
	return new ColaLevelUpScript();
}

void AddSC_spell_cola_level_up()
{
	Script* newscript = new Script;
	newscript->Name = "spell_cola_level_up";
	newscript->GetSpellScript = &GetScript_ColaLevelUp;
	newscript->RegisterSelf();
}