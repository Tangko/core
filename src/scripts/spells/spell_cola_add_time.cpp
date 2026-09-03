#include "scriptPCH.h"

// 34003 - 可乐瓶盖-谢谢惠顾！增加账户时间100分钟
struct ColaAddTimeScript : SpellScript
{
	SpellCastResult OnCheckCast(Spell* spell, bool strict) const override
	{
		Player* player = spell->m_caster ? spell->m_caster->ToPlayer() : nullptr;
		if (!player)
			return SPELL_FAILED_ERROR;

		if (player->IsInCombat())
			return SPELL_FAILED_AFFECTING_COMBAT;

		return SPELL_CAST_OK;
	}

	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_0)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		uint32 accountId = player->GetSession()->GetAccountId();
		LoginDatabase.PExecute(
			"UPDATE account SET playtime_remaining = playtime_remaining + 100 WHERE id = %u",
			accountId
		);

		return true;
	}
};

SpellScript* GetScript_ColaAddTime(SpellEntry const*)
{
	return new ColaAddTimeScript();
}

void AddSC_spell_cola_add_time()
{
	Script* newscript = new Script;
	newscript->Name = "spell_cola_add_time";
	newscript->GetSpellScript = &GetScript_ColaAddTime;
	newscript->RegisterSelf();
}