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