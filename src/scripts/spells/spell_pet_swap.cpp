#include "scriptPCH.h"
#include "Creature.h"
#include "MotionMaster.h"
#include "TemporarySummon.h"
#include <unordered_map>

static std::unordered_map<uint32, uint32> g_swapPlayerDisplay;
static std::unordered_map<uint32, ObjectGuid> g_illusionGuid;

// 34001 - 恶魔附身
struct WarlockPetSwapScript : SpellScript
{
	bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const final
	{
		if (effIdx != EFFECT_INDEX_0)
			return true;

		Player* player = spell->m_caster->ToPlayer();
		if (!player)
			return true;

		if (player->GetClass() != CLASS_WARLOCK)
			return true;

		if (player->InBattleGround())
			return true;

		Pet* pet = player->GetPet();
		if (!pet || !pet->IsAlive())
			return true;

		uint32 playerGuidLow = player->GetGUIDLow();
		bool isActive = g_swapPlayerDisplay.find(playerGuidLow) != g_swapPlayerDisplay.end();

		if (isActive)
		{
			// 取消附身
			uint32 originalDisplay = g_swapPlayerDisplay[playerGuidLow];
			player->SetDisplayId(originalDisplay);
			player->SetObjectScale(1.0f);
			g_swapPlayerDisplay.erase(playerGuidLow);

			// 删除幻象
			auto it = g_illusionGuid.find(playerGuidLow);
			if (it != g_illusionGuid.end())
			{
				if (Creature* illusion = player->GetMap()->GetCreature(it->second))
					illusion->ForcedDespawn(0);
				g_illusionGuid.erase(it);
			}
		}
		else
		{
			// 保存原模型
			uint32 playerDisplay = player->GetDisplayId();
			g_swapPlayerDisplay[playerGuidLow] = playerDisplay;

			// 变成宠物模型
			uint32 petDisplay = pet->GetDisplayId();
			player->SetDisplayId(petDisplay);
			player->SetObjectScale(1.0f);

			// 召唤幻象（非战斗小宠物）
			uint32 petEntry = pet->GetEntry();
			CreatureInfo const* petInfo = sObjectMgr.GetCreatureTemplate(petEntry);
			if (petInfo)
			{
				Creature* illusion = player->SummonCreature(petEntry,
					player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(),
					player->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
				if (illusion)
				{
					illusion->SetObjectScale(pet->GetObjectScale());
					illusion->SetFactionTemplateId(player->GetFactionTemplateId());
					illusion->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE_2);
					illusion->SetOwnerGuid(player->GetObjectGuid());
					illusion->SetCharmerGuid(player->GetObjectGuid());
					illusion->SetDefaultMovementType(FOLLOW_MOTION_TYPE);
					illusion->AIM_Initialize();
					illusion->GetMotionMaster()->MoveFollow(player, 1.0f, M_PI);
					g_illusionGuid[playerGuidLow] = illusion->GetObjectGuid();
				}
			}
		}

		return true;
	}
};

SpellScript* GetScript_WarlockPetSwap(SpellEntry const*)
{
	return new WarlockPetSwapScript();
}

void AddSC_pet_swap()
{
	Script* newscript = new Script;
	newscript->Name = "spell_warlock_pet_swap";
	newscript->GetSpellScript = &GetScript_WarlockPetSwap;
	newscript->RegisterSelf();
}