/*  LoL Ability Timers. Injects into League of Legends to show ability
timers for all champions.
Copyright (C) 2014  Matthew Whittington

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#pragma once
#include "ObjectAccessor.h"
#include <exception>
#define MAX_SPELLBOOK_SLOTS 64

class InvalidSpellException :
	public std::exception
{
	virtual const char* what() const throw()
	{
		return "Invalid spell index accessed.";
	}
};

extern InvalidSpellException INVALID_SPELL_EX;

class SpellDataAccessor :
	public ObjectAccessor {
public:
	SpellDataAccessor(DWORD baseMemoryAddress) :
		ObjectAccessor(baseMemoryAddress) { }
	~SpellDataAccessor() { }
	/// <returns>The game-internal name of the spell, i.e. "SummonerTeleport"
	/// rather than "Teleport".</returns>
	STRING_HOLDER* GetSpellName() {
		return reinterpret_cast<STRING_HOLDER*>(mBaseMemoryAddress + 0x18);
	}
	/// <returns>Returns the name of the animation for the spell as a STRING_HOLDER</returns>
	STRING_HOLDER GetSpellAnimationName() {
		return *reinterpret_cast<STRING_HOLDER*>(mBaseMemoryAddress + 0x1D4);
	}
	// There's a lot more stuff of interest here, such as exact damage values, cooldown
	// times, mana costs, scaling, etc, but they weren't needed for this project
	// and they weren't immediately obvious from the code, so it wasn't worth the time
	// documenting them.
};

class SpellDataInstAccessor :
	public ObjectAccessor {
public:
	SpellDataInstAccessor(DWORD baseMemoryAddress) :
		ObjectAccessor(baseMemoryAddress) { }
	~SpellDataInstAccessor() { }
	/// <returns>0 for unleveled spell, 1-X for number of points in spell.</returns>
	DWORD GetSpellLevel() { 
		return *reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0x10);
	}
	/// <returns>The simulation time at which the cooldown will expire -
	/// simulation time is NOT equal to the display time (i.e. top right corner).</returns>
	float GetCooldownExpires() { 
		return *reinterpret_cast<float*>(mBaseMemoryAddress + 0x14);
	}
	/// <returns>The amount of ammo in stock for the spell.</returns>
	DWORD GetSpellAmmo() { return *reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0x18); }
	/// <returns>The simulation time at which ammo will start to recharge?</returns>
	float GetAmmoRechargeStart() { return *reinterpret_cast<float*>(mBaseMemoryAddress + 0x24); }
	// to do with Nid/Jayce/Elise?
	DWORD GetToggleState() { return *reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0x30); }
	/// <returns>The total cooldown of the spell - includes modification from CDR.</returns>
	float GetTotalCooldown() { 
		return *reinterpret_cast<float*>(mBaseMemoryAddress + 0x34);
	}
	/// <returns>A SpellDataAccessor to access the asssociated SpellData struct.</returns>
	SpellDataAccessor GetSpellData() {
		return SpellDataAccessor(*reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0xD8));
	}
};

class SpellbookAccessor :
	public ObjectAccessor {
public:
	SpellbookAccessor(DWORD baseMemoryAddress) :
		ObjectAccessor(baseMemoryAddress) { }
	~SpellbookAccessor() { }
	SpellDataInstAccessor GetSpellDataInstForSlot(UINT index) {
		if (index >= MAX_SPELLBOOK_SLOTS)
			return NULL;
		return SpellDataInstAccessor(
			*reinterpret_cast<DWORD*>(mBaseMemoryAddress + 0x4D0 + (index * sizeof(DWORD)))
		);
	}
	BYTE IsCastingSpell() {
		return *reinterpret_cast<BYTE*>(mBaseMemoryAddress + 0x428);
	}
	BYTE IsSummonerSpellbook() {
		return *reinterpret_cast<BYTE*>(mBaseMemoryAddress + 0x8);
	}
};