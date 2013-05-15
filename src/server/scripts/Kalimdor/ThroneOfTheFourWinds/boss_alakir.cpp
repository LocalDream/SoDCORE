/*
 * Copyright (C) 2005 - 2011 MaNGOS <http://www.getmangos.org/>
 * Copyright (C) 2008 - 2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2011 - 2012 ArkCORE <http://www.arkania.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
 SDName: boss_al'akir.cpp
 SD%Complete: 80
 SDComment: by dev/Kir
 SDCategory: throne_of_the_four_winds
 EndScriptData */

// HELP me, correct the timers to cast for boss.
// HELP me, find mobs.

#include "ObjectAccessor.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "throne_of_the_four_winds.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Vehicle.h"
#include "GridNotifiers.h"

enum Texts
{
    SAY_AGGRO                           = 1, // Your challenge is accepted, mortals! Know that you face Al'Akir, Elemental Lord of Air! You have no hope of defeating me!
    SAY_WIND_BURST                      = 2, // Winds! Obey my command!
    SAY_PHASE_2                         = 3, // Your futile persistence angers me.
	SAY_SQUALL_LINE                     = 4, // Storms! I summon you to my side!
	SAY_PHASE_3                         = 5, // ENOUGH! I WILL NO LONGER BE CONTAINED!
    SAY_KILL                            = 6, // Like swatting insects...
    SAY_KILL_2                          = 7,   // This little one will vex me no longer.
    SAY_BERSERK                         = 8, // Enough! I tire of these games!
    SAY_DEFEAT                          = 9, // After every storm... comes the calm...
};

enum Spells
{
		// Al'Akir
		SPELL_WIND_BURST_N_10           = 87770, //PHASE ONE
        SPELL_WIND_BURST_H_10           = 93261,
        SPELL_WIND_BURST_N_25           = 93262,
        SPELL_WIND_BURST_H_25           = 93263,
		SPELL_LIGHTNING_STRIKE_N_10		= 88214,
		SPELL_LIGHTNING_STRIKE_H_10		= 93255,
		SPELL_LIGHTNING_STRIKE_N_25		= 93256,
		SPELL_LIGHTNING_STRIKE_H_25		= 93257,
		SPELL_ICE_STORM_SUMMON			= 88239,

        SPELL_ACID_RAIN_N_10            = 88301, // PHASE TWO
        SPELL_ACID_RAIN_H_10            = 93279,
		SPELL_ACID_RAIN_N_25            = 93280,
        SPELL_ACID_RAIN_H_25            = 93281,

		SPELL_RENTLESS_STORM			= 88866, // PHASE THREE
        SPELL_EYE_OFTHE_STORM          = 82724,		
        SPELL_LIGHTING_ROD_N_10         = 89667,
		SPELL_LIGHTING_ROD_H_10         = 93293,
		SPELL_LIGHTING_ROD_N_25         = 93294,
		SPELL_LIGHTING_ROD_H_25         = 93295,
		SPELL_WIND_BURST2_N_10          = 88858, 
        SPELL_WIND_BURST2_H_10          = 93286,
        SPELL_WIND_BURST2_N_25          = 93287,
        SPELL_WIND_BURST2_H_25          = 93288,
		SPELL_LIGHTNING_N_10			= 89641,
		SPELL_LIGHTNING_H_10			= 93290,
		SPELL_LIGHTNING_N_25			= 93291,
		SPELL_LIGHTNING_H_25			= 93292,
		
        SPELL_ELECTROCUE                = 88427, // PHASE ONE & TWO
		SPELL_STATIC_SHOCK				= 87873,

        SPELL_BLAST_OF_AIR              = 55912, // COMBI WITH WIND BURST
		SPELL_RELENTLESS_STORM			= 88875, // AURA ON PHASE THREE
		SPELL_BERSERK 					= 95211, // BERSERK I HOPE ITS THE RIGHT BERSERK
		
		// Icestorm npc
		SPELL_ICE_STORM_N_10			= 91020,
		SPELL_ICE_STORM_H_10			= 93258,
		SPELL_ICE_STORM_N_25			= 93259,
		SPELL_ICE_STORM_H_25			= 93260,
		SPELL_ICE_STORM_AURA_SPAWN		= 87472,
		SPELL_ICE_STORM_AURA2			= 87469,
		
		// Squall line
		SPELL_SQUALL_LINE_AURA			= 87621,
		SPELL_SQUALL_LINE_DISMOUNT		= 95757,
		SPELL_SQUALL_LINE_DAMAGE_N_10	= 87856,
		SPELL_SQUALL_LINE_DAMAGE_H_10	= 93283,
		SPELL_SQUALL_LINE_DAMAGE_N_25	= 93284,
		SPELL_SQUALL_LINE_DAMAGE_H_25	= 87855,
		
		// Clouds
		SPELL_LIGHTING_CLOUD_N_10       = 89588,
		SPELL_LIGHTING_CLOUD_H_10       = 93297,
		SPELL_LIGHTING_CLOUD_N_25       = 93298,
		SPELL_LIGHTING_CLOUD_H_25       = 93299,

		// Stormling
		SPELL_FEEDBACK					= 87904,
		SPELL_STORMLING_H				= 87908,	//only in herioc
		
};

Position const StormlingListPositions[5] =
{{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f}};

Position const StormlineswListPositions[7] =
{
{-25.299749f,875.881104f,189.983994f,0.0f},
{0.0f,0.0f,189.983994f,0.0f},
{0.0f,0.0f,189.983994f,0.0f},
{0.0f,0.0f,189.983994f,0.0f},
{0.0f,0.0f,189.983994f,0.0f},
{0.0f,0.0f,189.983994f,0.0f},
{-38.400715f,844.311096f,189.983994f,0.0f}
};

enum Events
{
	EVENT_PHASE_THREE		= 0,
	EVENT_BERSERK			= 1,
	EVENT_ICE_STORM			= 2,
	EVENT_STATIC_SHOCK		= 3,
	EVENT_WIND_BURST		= 4,
	EVENT_LIGHTNING_STRIKE	= 5,
	EVENT_ELECTROCUE		= 6,
	EVENT_SQUALL_LINE		= 7,
	EVENT_ACID_RAIN			= 8,
	EVENT_STATIC_SHOCK2		= 9,
	EVENT_ELECTROCUE2		= 10,
	EVENT_LIGHTING_ROD		= 11,
	EVENT_SUMMON_STORMLING	= 12,
	EVENT_EYE_OF_THE_STORM	= 13,
	EVENT_RENTLESS_STORM	= 14,
	EVENT_WIND_BURST2		= 15,
	EVENT_LIGHTNING_CLOUD	= 16,
	EVENT_SQUALL_LINE2		= 17,
	EVENT_LIGHTING			= 18,
};
enum AlakirEvents
{
    DATA_PHASE,
};
enum Phases
{
    PHASE_ONE                   = 1,
    PHASE_TWO                   = 2,
    PHASE_THREE                 = 3,
};
enum MiscData
{
	NORMAL		= 2768,
	DARK		= 2810,
};
//ACHI
enum Achievements
{
        ACHIEVEMENT_ALAKIR_DEAD         = 5123,
        ACHIEVEMENT_FOUR_PLAY           = 5305,
        ACHIEVEMENT_KILLED_ALAKIR       = 5463,
};

class HeightDifferenceCheck
{
    public:
        HeightDifferenceCheck(GameObject* go, float diff, bool reverse)
            : _baseObject(go), _difference(diff), _reverse(reverse)
        {
        }

        bool operator()(Unit* unit) const
        {
            return (unit->GetPositionZ() - _baseObject->GetPositionZ() > _difference) != _reverse;
        }

    private:
        GameObject* _baseObject;
        float _difference;
        bool _reverse;
};

class boss_alakir : public CreatureScript
{
        public:
                boss_alakir() : CreatureScript("boss_alakir") { }

                struct boss_alakirAI : public BossAI
                {
                        boss_alakirAI(Creature* pCreature) : BossAI(pCreature, DATA_ALAKIR)
                        {
								instance = pCreature->GetInstanceScript();
								me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
								me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                        }


						uint8 _phase;
						uint32 phase;
                        void Reset()
                        {
						_Reset();
						SetPhase(PHASE_ONE, true);
						if (GameObject *Go = GetClosestGameObjectWithEntry(me, GO_FLOOR_ALAKIR, 1000.0f))
						Go->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
															Map *pMap = me->GetMap();
									Map::PlayerList const &PlayerList = pMap->GetPlayers();

									if (PlayerList.isEmpty())
									return;
									for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)							
									{
                                        Player* plr = i->getSource();
                                        if (plr->isAlive())
                                        {
											plr->RemoveAurasDueToSpell(SPELL_EYE_OFTHE_STORM);
										}
									}
						SendWeather(WEATHER_STATE_FINE);
                        }
						
						void KilledUnit(Unit* /*who*/)
						{
						Talk (RAND(SAY_KILL, SAY_KILL_2));
						}
						uint32 GetData(uint32 data)
						{
							if (data == DATA_PHASE)
								return _phase;

							return 0;
						}
						
						void SetPhase(uint8 phase, bool setEvents = false)
						{
							events.Reset();

							events.SetPhase(phase);
							_phase = phase;
							if (setEvents)
							SetPhaseEvents();
						}
						
						
				        void SetPhaseEvents()
						{
							switch (_phase)
							{
								case PHASE_ONE:
									events.ScheduleEvent(EVENT_STATIC_SHOCK, 12000, 0, _phase);
									events.ScheduleEvent(EVENT_ELECTROCUE, 20000, 0, _phase);
									events.ScheduleEvent(EVENT_WIND_BURST, 3000, 0, _phase);
									events.ScheduleEvent(EVENT_LIGHTNING_STRIKE, 18000, 0, _phase);
									break;
								case PHASE_TWO:
									events.ScheduleEvent(EVENT_STATIC_SHOCK2, 8000, 0, _phase);
									events.ScheduleEvent(EVENT_ACID_RAIN, 2000, 0, _phase);
									events.ScheduleEvent(EVENT_ELECTROCUE2, 5000, 0, _phase);
									break;
								case PHASE_THREE:
									events.ScheduleEvent(EVENT_WIND_BURST2, 25000, 0, _phase);
									events.ScheduleEvent(EVENT_LIGHTING_ROD, 20000, 0, _phase);
									events.ScheduleEvent(EVENT_LIGHTING, 25000, 0, _phase);
									break;
								default:
									break;
							}
						}
						
				        void StartPhaseTwo()
						{
							SetPhase(PHASE_TWO, true);
							SendLightOverride(2768, 5000);
							SendWeather(WEATHER_STATE_MEDIUM_RAIN);
						}

						void StartPhaseThree()
						{
							SetPhase(PHASE_THREE, true);
                GameObject* floor = me->FindNearestGameObject(GO_FLOOR_ALAKIR, 200);
                if (floor)
                    floor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);														
							me->AddAura(SPELL_RELENTLESS_STORM, me);					
							SendWeather(WEATHER_STATE_FINE);
							SendLightOverride(2810, 5000);
							Map *pMap = me->GetMap();
							Map::PlayerList const &PlayerList = pMap->GetPlayers();
							if (PlayerList.isEmpty())
								return;
								for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)							
								{
                                    Player* plr = i->getSource();
                                    if (plr->isAlive())
                                    {
										plr->CastSpell(plr, SPELL_EYE_OFTHE_STORM, false);
                                    }
								}
						}
						
						void EnterCombat(Unit* /*who*/)
						{
							Talk(SAY_AGGRO);
							_EnterCombat();
							//instance->SendEncounterUnit(ENCOUNTER_FRAME_ADD, me, 1);
							me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
							events.SetPhase(PHASE_ONE);
							events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
						}
												
                        void JustDied(Unit* killer)
                        {
								_JustDied();
								me->SummonGameObject(GO_HEART_OF_THE_WIND, 25.359699f, 777.276733f, 200.264008f, 0, 0, 0, 0, 0, 100000);

                        }


                        void UpdateAI(uint32 const diff)
                        {
						    if (!UpdateVictim())
							return;
							events.Update(diff);
						if (!instance)
                           return;
						
						if (me->GetHealthPct() <= 80.0f && _phase == PHASE_ONE)
							StartPhaseTwo();
						if (me->GetHealthPct() <= 25.0f && _phase == PHASE_TWO)
							StartPhaseThree();
							
						if (me->HasUnitState(UNIT_STATE_CASTING))
							return;
										
							while (uint32 eventId = events.ExecuteEvent())
							{
								switch (eventId)
								{
								case EVENT_BERSERK:
									DoCast(me, SPELL_BERSERK);
									break;
								case EVENT_WIND_BURST:
									Talk(SAY_WIND_BURST);
									if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM))
									{
									DoCast(pTarget, SPELL_BLAST_OF_AIR);
									DoCast(pTarget,RAID_MODE(SPELL_WIND_BURST_N_10, SPELL_WIND_BURST_H_10,
									SPELL_WIND_BURST_N_25, SPELL_WIND_BURST_H_25));
									}
									events.ScheduleEvent(EVENT_WIND_BURST, 25000, 0, PHASE_ONE);
									break;
								case EVENT_LIGHTNING_STRIKE:
									DoCastVictim(RAID_MODE(SPELL_LIGHTNING_STRIKE_N_10, SPELL_LIGHTNING_STRIKE_H_10,
									SPELL_LIGHTNING_STRIKE_N_25, SPELL_LIGHTNING_STRIKE_H_25));
									events.ScheduleEvent(EVENT_LIGHTNING_STRIKE, 20000, 0, PHASE_ONE);
									break;
								case EVENT_ELECTROCUE:
									if (Unit* pTarget = me->getVictim())
									if (me->IsWithinDistInMap(pTarget, 5.0f))
									DoCast(me->getVictim(), SPELL_ELECTROCUE);
									events.ScheduleEvent(EVENT_ELECTROCUE, 17000, 0, PHASE_ONE);
									break;
								case EVENT_STATIC_SHOCK:
									DoCast(me->getVictim(), SPELL_STATIC_SHOCK);
									events.ScheduleEvent(EVENT_STATIC_SHOCK, 17000, 0, PHASE_ONE);
									break;
								case EVENT_STATIC_SHOCK2:
									DoCast(me->getVictim(), SPELL_STATIC_SHOCK);
									events.ScheduleEvent(EVENT_STATIC_SHOCK2, 17000, 0, PHASE_TWO);
									break;
								case EVENT_ELECTROCUE2:
									if (Unit* pTarget = me->getVictim())
									if (me->IsWithinDistInMap(pTarget, 5.0f))
									DoCast(me->getVictim(), SPELL_ELECTROCUE);
									events.ScheduleEvent(EVENT_ELECTROCUE2, 17000, 0, PHASE_TWO);
									break;
								case EVENT_ACID_RAIN:
									DoCastAOE(RAID_MODE(SPELL_ACID_RAIN_N_10, SPELL_ACID_RAIN_H_10,
									SPELL_ACID_RAIN_N_25, SPELL_ACID_RAIN_H_25));
									events.ScheduleEvent(EVENT_ACID_RAIN, 15000, 0, PHASE_TWO);
									break;
								case EVENT_WIND_BURST2:
									Talk(SAY_WIND_BURST);
									if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM))
									{
									DoCast(pTarget,RAID_MODE(SPELL_WIND_BURST2_N_10, SPELL_WIND_BURST2_H_10,
									SPELL_WIND_BURST2_N_25, SPELL_WIND_BURST2_H_25));
									}
									events.ScheduleEvent(EVENT_WIND_BURST2, 5000, 0, PHASE_THREE);
									break;
								case EVENT_LIGHTING_ROD:
									if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM))
									{
									DoCast(pTarget,RAID_MODE(SPELL_LIGHTING_ROD_N_10, SPELL_LIGHTING_ROD_H_10,
									SPELL_LIGHTING_ROD_N_25, SPELL_LIGHTING_ROD_H_25));
									}
									events.ScheduleEvent(EVENT_LIGHTING_ROD, 15000, 0, PHASE_THREE);
									break;
								case EVENT_LIGHTING:
									if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM))
									{
									DoCast(pTarget,RAID_MODE(SPELL_LIGHTNING_N_10, SPELL_LIGHTNING_H_10,
									SPELL_LIGHTNING_N_25, SPELL_LIGHTNING_H_25));
									}
									events.ScheduleEvent(EVENT_LIGHTING, 20000, 0, PHASE_THREE);
									break;
									default:
									break;
								}
							}
							DoMeleeAttackIfReady();
						}
				private:
						void SendLightOverride(uint32 overrideId, uint32 fadeInTime) const
						{
							WorldPacket data(SMSG_OVERRIDE_LIGHT, 12);
							data << uint32(2768);       // Light.dbc entry (map default)
							data << uint32(overrideId); // Light.dbc entry (override)
							data << uint32(fadeInTime);
							SendPacketToPlayers(&data);
						}
						void SendWeather(WeatherState weather) const
						{
							WorldPacket data(SMSG_WEATHER, 9);
							data << uint32(weather);
							data << float(0.5f);
							data << uint8(0);
							SendPacketToPlayers(&data);
						}
						
			            void SendPacketToPlayers(WorldPacket const* data) const
						{
							Map::PlayerList const& players = me->GetMap()->GetPlayers();
							if (!players.isEmpty())
								for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
								if (Player* player = itr->getSource())
								if (player->GetAreaId() == AREA_TO4W)
                                player->GetSession()->SendPacket(data);
						}
						InstanceScript* instance;
                };

        CreatureAI* GetAI(Creature* pCreature) const
        {
                return new boss_alakirAI(pCreature);
        }

};
class npc_stormling : public CreatureScript
{
public:
    npc_stormling() : CreatureScript("npc_stormling") { }

    struct npc_stormlingAI : public ScriptedAI
    {
        npc_stormlingAI(Creature * pCreature) : ScriptedAI(pCreature) {}

        uint32 GravityTimer;
	
        void Reset()
        {
			GravityTimer = 10000;
        }
        
		void JustDied(Unit* killer)
        {
			DoCast(SPELL_FEEDBACK);
        }

        void UpdateAI(const uint32 diff)
		{	
			if (IsHeroic())
            if (GravityTimer <= diff)
            {
                DoCast(SPELL_STORMLING_H);
                GravityTimer = 10000;
            }
			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormlingAI(creature);
    }
};

void AddSC_boss_alakir()
{
        new boss_alakir();
		new npc_stormling();
}