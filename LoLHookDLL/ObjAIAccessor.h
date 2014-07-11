#pragma once
#include "ObjectAccessor.h"
#include "Spell.h"
/// <summary>An interface to access data of an obj_AI_Base from
/// League of Legends.</summary>
class ObjAIAccessor :
	public ObjectAccessor
{
public:
	ObjAIAccessor(DWORD baseMemoryAddress) : ObjectAccessor(baseMemoryAddress) { }
	~ObjAIAccessor() { }

	/// <returns>1 if unit is alive, 0 otherwise.</returns>
	BYTE IsAlive() { return *reinterpret_cast<BYTE*>(mBaseMemoryAddress + 0x12); }

	/// <returns>The team number for the hero, compatible with the RIOT_TEAM_ORDER,
	/// RIOT_TEAM_CHAOS and RIOT_TEAM_UNKNOWN defines.</returns>
	DWORD GetTeam() { 
		return *reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0x1C);
	}

	/// <summary>Gets the summoner name of the object, if it is an obj_AI_Hero,
	/// otherwise this is the name of the object in question.</summary>
	/// <returns>A STRING_HOLDER containing the data.</returns>
	STRING_HOLDER* GetObjectName() {
		return reinterpret_cast<STRING_HOLDER*>(mBaseMemoryAddress + 0x28);
	}

	///<summary>Gets the current X, Y, Z coordinates of the object.</summary>
	///<returns>RIOT_COORD with the coordinates of the object.</returns>
	RIOT_COORD* GetCurrentPos() {
		return reinterpret_cast<RIOT_COORD*>(mBaseMemoryAddress + 0x64);
	}

	/// <summary>Gets the skin name of the obj_AI_Base.</summary>
	/// <returns>C string representation of the skin name.</returns>
	char* GetSkinName() {
		return reinterpret_cast<char*>(mBaseMemoryAddress + 0x4EC);
	}

	/// <returns>1 if the obj_AI_Hero is player controlled, 0 otherwise.</returns>
	DWORD IsPlayerControlled() { 
		return *reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0x570);
	}

	/// <returns>The gold value of the given object. For champions this is the gold
	/// in their bank. In non-spectator mode this will only work on teammates, enemies
	/// will always show as having 0 gold.</returns>
	float GetObjectGold() {
		return *reinterpret_cast<float*>(mBaseMemoryAddress + 0xC48);
	}

	/// <summary>Gets the spellbook of the obj_AI_Hero for regular spells.</summary>
	/// <returns>A SpellbookAccessor that can be used to access the spells.</returns>
	SpellbookAccessor GetSpellbook() {
		return SpellbookAccessor(mBaseMemoryAddress + 0x10F0);
	}

	/// <summary>Gets the spellbook of the obj_AI_Hero for summoner spells.</summary>
	/// <returns>A SpellbookAccessor that can be used to access the spells.</returns>
	SpellbookAccessor GetSummonerSpellbook() {
		return SpellbookAccessor(mBaseMemoryAddress + 0x5870);
	}

	/// <summary>Gets the champion name of the obj_AI_Base.</summary>
	/// <returns>A STRING_HOLDER containing the champion name.</returns>
	STRING_HOLDER* GetChampName() {
		return *reinterpret_cast<STRING_HOLDER**>(mBaseMemoryAddress + 0x55C4);
	}
};

