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
#include "structs.h"
#include <exception>

class NullReferenceException :
	public std::exception
{
	virtual const char* what() const throw()
	{
		return "Null address passed to ObjectAccessor.";
	}
};

extern NullReferenceException NULL_REF_EX;

/// <summary>Class intended to be extended to allow access to
/// various fields of structs and clases, given a base address.</summary>
class ObjectAccessor
{
protected:
	DWORD mBaseMemoryAddress; // the base address of the class in memory

	static DWORD CallVtableFunction(DWORD classAddr, DWORD offset);
public:
	/// <summary>Takes a memory address as a base to begin accessing
	/// data from.</summary>
	/// <param name="baseMemoryAddress">The memory address to use as
	/// a base when retrieving data from offsets. Must not be NULL.</param>
	ObjectAccessor(DWORD baseMemoryAddress) {
		if (baseMemoryAddress == 0)
			throw NULL_REF_EX;
		mBaseMemoryAddress = baseMemoryAddress;
	}
	~ObjectAccessor() { }

	/// <returns>Gets the base memory address of this object
	/// accessor.</returns>
	DWORD GetAddress() { return mBaseMemoryAddress; }
};