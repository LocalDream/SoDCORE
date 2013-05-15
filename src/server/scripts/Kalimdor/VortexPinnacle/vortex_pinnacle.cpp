/*
 * Copyright (C) 2011 True Blood <http://www.trueblood-servers.com/>
 * By Asardial
 *
 * Copyright (C) 2011 - 2013 ArkCORE <http://www.arkania.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "the_vortex_pinnacle.h"
#include "InstanceScript.h"
#include "ScriptedGossip.h"
#include "ScriptPCH.h"
#define STREAMS 12
enum Enums
{
	//Slipstream

	// Spells
	SPELL_SLIPSTREAM = 95911,
};

		
const uint64 streamGuids[12]  = {15860522,
								 15860520,
								 15860516,
								 15860524,
								 15860539,
								 15860552,
								 15860548,
								 15860544,
								 15860550,
								 15860546,
								 15860554,
								 15861015};

class npc_slipstream : public CreatureScript
{
public:
	npc_slipstream() : CreatureScript("npc_slipstream"){}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_slipstreamAI(creature);
	}

	struct npc_slipstreamAI : public ScriptedAI
	{
		npc_slipstreamAI(Creature *c) : ScriptedAI(c)
		{
			instance = me->GetInstanceScript();
			isSlipstreamNo = 0;

			for (int i=0; i <= STREAMS; i++)
			{
				if (streamGuids[i] == me->GetGUID())
				{
					isSlipstreamNo = i;
					break;
				}
			}

			Reset();

		}

		InstanceScript* instance;
		uint8 isSlipstreamNo;

		void Reset()
		{
			//me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
		}

		void UpdateAI(const uint32 diff)
		{
			
		}

	void MoveInLineOfSight(Unit* who)
{

if (who->GetDistance(me) >= 5.0f)
return;
me->MonsterSay("Slipstream: " + isSlipstreamNo,0,0);
me->CastSpell(who,SPELL_SLIPSTREAM,true);

if (Creature* toChargeStream = ObjectAccessor::GetCreature(*me,streamGuids[isSlipstreamNo+1]))
{
me->MonsterSay("Charge",0,0);	
who->GetMotionMaster()->MoveCharge(toChargeStream->GetPositionX(),toChargeStream->GetPositionY(),toChargeStream->GetPositionZ());
}
}

	};
};

void AddSC_vortex_pinnacle()
{
    new npc_slipstream();
}