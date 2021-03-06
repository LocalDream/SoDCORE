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

#include "ScriptPCH.h"
#include "the_vortex_pinnacle.h"

#define ENCOUNTERS 3
#define STREAMS 12

/* Boss Encounters
------------------
   Grand Vizier Ertan
   Altairus
   Asaad
*/

enum Enums
{
	//Slipstream

	// Spells
	SPELL_SLIPSTREAM = 95911,
};

		
const uint64 streamGuids[12]  = {15860522, 15860520, 15860516, 15860524, 15860539, 15860552, 15860548, 15860544, 15860550, 15860546,15860554, 15861015};

class instance_the_vortex_pinnacle : public InstanceMapScript
{
public:
    instance_the_vortex_pinnacle() : InstanceMapScript("instance_the_vortex_pinnacle", 657) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_the_vortex_pinnacle_InstanceMapScript(map);
    }

    struct instance_the_vortex_pinnacle_InstanceMapScript: public InstanceScript
    {
        instance_the_vortex_pinnacle_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

        uint32 uiEncounter[ENCOUNTERS];

        uint64 uiGrandVizierErtan;
        uint64 uiAltairus;
        uint64 uiAsaad;

        void Initialize()
        {
            uiGrandVizierErtan = 0;
            uiAltairus = 0;
            uiAsaad = 0;

        for(uint8 i=0; i < ENCOUNTERS; ++i)
                uiEncounter[i] = NOT_STARTED;
        }

        bool IsEncounterInProgress() const
        {
            for(uint8 i=0; i < ENCOUNTERS; ++i)
            {
                if (uiEncounter[i] == IN_PROGRESS)
                    return true;
            }

            return false;
        }

        void OnCreatureCreate(Creature* pCreature, bool)
        {
            switch(pCreature->GetEntry())
            {
                case BOSS_GRAND_VIZIER_ERTAN:
                    uiGrandVizierErtan = pCreature->GetGUID();
                    break;
                case BOSS_ALTAIRUS:
                    uiAltairus = pCreature->GetGUID();
                    break;
                case BOSS_ASAAD:
                    uiAsaad = pCreature->GetGUID();
                    break;
            }
        }

        uint64 GetData64(uint32 identifier)
        {
            switch(identifier)
            {
                case DATA_GRAND_VIZIER_ERTAN:
                    return uiGrandVizierErtan;
                case DATA_ALTAIRUS:
                    return uiAltairus;
                case DATA_ASAAD:
                    return uiAsaad;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case DATA_GRAND_VIZIER_ERTAN:
                    uiEncounter[0] = data;
                    break;
                case DATA_ALTAIRUS:
                    uiEncounter[1] = data;
                    break;
                case DATA_ASAAD:
                    uiEncounter[2] = data;
                    break;
            }

         if (data == DONE)
             SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch(type)
            {
                case DATA_GRAND_VIZIER_ERTAN:
                    return uiEncounter[0];
                case DATA_ALTAIRUS:
                    return uiEncounter[1];
                case DATA_ASAAD:
                    return uiEncounter[2];
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::string str_data;
            std::ostringstream saveStream;
            saveStream << "V P" << uiEncounter[0] << " " << uiEncounter[1]  << " " << uiEncounter[2];
            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1, data2;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2;

            if (dataHead1 == 'V' && dataHead2 == 'P')
            {
                uiEncounter[0] = data0;
                uiEncounter[1] = data1;
                uiEncounter[2] = data2;

                for(uint8 i=0; i < ENCOUNTERS; ++i)
                    if (uiEncounter[i] == IN_PROGRESS)
                        uiEncounter[i] = NOT_STARTED;
            }
            else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };
};

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


void AddSC_instance_the_vortex_pinnacle()
{
    new instance_the_vortex_pinnacle();
	//new npc_slipstream();
}