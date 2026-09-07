#include "scriptPCH.h"

// 34004 - 一瓶可乐
// 使用后随机获得瓶盖：26001(60%)、26004(20%)、26005(20%)
struct ColaDrinkScript : SpellScript
{
	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_0)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		uint32 roll = urand(1, 100);
		uint32 bottleCapEntry;

		if (roll <= 15)
			bottleCapEntry = 26004;
		else if (roll <= 85)
			bottleCapEntry = 26001;
		else
			bottleCapEntry = 26005;

		// 直接添加到背包
		player->StoreNewItemInBestSlots(bottleCapEntry, 1);

		return true;
	}
};

SpellScript* GetScript_ColaDrink(SpellEntry const*)
{
	return new ColaDrinkScript();
}

void AddSC_spell_cola_drink()
{
	Script* newscript = new Script;
	newscript->Name = "spell_cola_drink";
	newscript->GetSpellScript = &GetScript_ColaDrink;
	newscript->RegisterSelf();
}