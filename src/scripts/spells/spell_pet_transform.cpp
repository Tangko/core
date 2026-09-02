#include "scriptPCH.h"
#include <unordered_map>
static std::unordered_map<uint32, uint32> g_petOriginalDisplay;

// 26003 - 抓个德鲁伊宝宝
struct PetTransformScript : SpellScript
{
	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_0)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		// 猎人专属
		if (player->GetClass() != CLASS_HUNTER)
			return true;

		// 战场禁止
		if (player->InBattleGround())
			return true;

		Pet* pet = player->GetPet();
		if (!pet || !pet->IsAlive())
			return true;

		// 检查宠物是否在施法者附近（20码）
		if (player->GetDistance(pet) > 20.0f)
			return true;

		static const uint32 FORMS[] = { 892, 8571, 2281, 2289 };

		uint32 currentDisplay = pet->GetDisplayId();
		bool isTransformed = false;
		for (uint32 form : FORMS)
		{
			if (currentDisplay == form)
			{
				isTransformed = true;
				break;
			}
		}

		uint32 petGuidLow = pet->GetGUIDLow();

		if (isTransformed)
		{
			auto it = g_petOriginalDisplay.find(petGuidLow);
			if (it != g_petOriginalDisplay.end())
			{
				pet->SetDisplayId(it->second);
				pet->SetObjectScale(0.6f);
				pet->ForceValuesUpdateAtIndex(UNIT_FIELD_DISPLAYID);
				g_petOriginalDisplay.erase(it);
			}
		}
		else
		{
			g_petOriginalDisplay[petGuidLow] = currentDisplay;
			uint32 newForm = FORMS[urand(0, 3)];
			pet->SetDisplayId(newForm);
			pet->SetObjectScale(1.0f);
			pet->ForceValuesUpdateAtIndex(UNIT_FIELD_DISPLAYID);
		}
		return true;
	}
};

SpellScript* GetScript_PetTransform(SpellEntry const*)
{
	return new PetTransformScript();
}

void AddSC_pet_transform()
{
	Script* newscript = new Script;
	newscript->Name = "spell_druid_pet_transform";
	newscript->GetSpellScript = &GetScript_PetTransform;
	newscript->RegisterSelf();
}