#pragma once
#include "Clock.h"
#include "MemoryManager.h"
#include "ObjAIAccessor.h"

// LuaScriptHelper__errorreporting in IDA
typedef void(__cdecl *riot_log)
	(int unk1, int unk2, int unk3, const char* format, ...);

class DataAccessor {
private:
	MODULEENTRY32 mModuleInfo; // module info for the module to access
	DWORD mModuleBaseAddr; // the base address of the module to access
	BOOL mModuleSet = FALSE; // module not set at startup
	TimeClockClientAccessor* mDisplayClock; // accessor for display time
	RiotGameClockAccessor* mSimulationClock; // accessor for simulation time
	riot_log pRiotLog;
	// === Offsets for global object access ===
	// TODO: centralize these offsets
	// and the patterns to find them into a patchable external file
	DWORD mDisplayClockOffset = 0x2A1A770; // pGameClockForDisplay in IDA
	DWORD mSimulationClockOffset = 0x2A1B510; // pSpellCooldownGameClockInstance in IDA
	DWORD mArrayOfGlobalObjects = 0x29F0ED0; // pArrayOfGlobalObjects in IDA
	DWORD mArrayOfGlobalObjects_Size = 0x29F0ED4; // pArrayOfGlobalObjects_Size in IDA
	DWORD mArrayOfChampions = 0x1DA8264; // pScoreboardChampArray in IDA
	DWORD mArrayOfChampionsEnd = 0x1DA8268; // pScoreboardChampArray_End in IDA
	DWORD mPlayerChampion = 0x2A1A76C; // pPlayerData in IDA, must dereference +0 for an obj_AI_Hero type object
	DWORD mRiotLogOffset = 0x1F59D0; // LuaScriptHelper__errorreporting in IDA
	DWORD mpMenu_GUI = 0x2A1A714; // pMenu_GUI in IDA
	// flag for compile time changing of offset locating code
	bool mFindOffsetsDynamically = true;
	bool GetOffsetsDynamically();
	// ========================================

	static DataAccessor* gpDataAccessor; // global instance
public:
	/// <summary>Creates a DataAccessor to access data within the
	/// module described by moduleInfo.</summary>
	/// <param name="moduleInfo">A MODULEENTRY32 describing the module
	/// to set up data accessing for.</param>
	DataAccessor(MODULEENTRY32 moduleInfo) {
		mModuleInfo = moduleInfo;

		if (mFindOffsetsDynamically) {
			GetOffsetsDynamically();
		}
		else {
			pRiotLog = (riot_log)((DWORD)moduleInfo.modBaseAddr + mRiotLogOffset);
		}

		mDisplayClock = new TimeClockClientAccessor(
			*reinterpret_cast<DWORD*>((DWORD)moduleInfo.modBaseAddr + mDisplayClockOffset)
		);
		mSimulationClock = new RiotGameClockAccessor(
			*reinterpret_cast<DWORD*>((DWORD)moduleInfo.modBaseAddr + mSimulationClockOffset)
		);

		pRiotLog(3, 1, 0, "Successfully created DataAccessor instance.");
	}
	~DataAccessor() {
		delete mDisplayClock;
		delete mSimulationClock;
	}
	/// <returns>TRUE if the module is set, FALSE otherwise.</returns>
	BOOL IsModuleSet() { return mModuleSet; }
	/// <returns>Gets the function pointer to the riot logging function.</returns>
	riot_log GetRiotLog() { return pRiotLog; }
	/// <returns>Return the time in seconds as displayed in the game timer
	/// of league of legends.</returns>
	float GetDisplayTime() {
		return mDisplayClock->GetGameTime();
	}
	/// <returns>Return the time in seconds as used for simulation data
	/// such as cooldowns.</returns>
	float GetSimulationTime() {
		return mSimulationClock->GetSimulationTime();
	}
	/// <summary>Return a pointer to an object from the global objects
	/// array.</summary>
	/// <param name="index">The index to query for a value</param>
	/// <returns>NULL if the index is invalid or there is no object
	/// at that index, a pointer to an object otherwise.</returns>
	DWORD GetGameObjectByIndex(UINT index) {
		if (index >= mArrayOfGlobalObjects_Size)
			return NULL;
		return *reinterpret_cast<DWORD*>(mArrayOfGlobalObjects + (index * sizeof(DWORD)));
	}
	//ObjAIAccessor GetPlayerHero() {
	//	return ObjAIAccessor(*reinterpret_cast<DWORD*>((DWORD)mModuleInfo.modBaseAddr + mPlayerChampion));
	//}
	/// <returns>Gets the number of heroes on the scoreboard.</returns>
	DWORD GetNumScoreboardHeroes() {
		DWORD baseAddr = (DWORD)mModuleInfo.modBaseAddr;
		DWORD numBytesInArray = *reinterpret_cast<DWORD*>(baseAddr + mArrayOfChampionsEnd) -
			*reinterpret_cast<DWORD*>(baseAddr + mArrayOfChampions);
		return numBytesInArray / sizeof(DWORD);
	}
	BOOL GetScoreboardHeroes(ObjAIAccessor** pObjAIArray);
	bool IsScoreboardShown();

	/// <summary>Singleton accessor function.</summary>
	/// <returns>The single instance of DataAccessor that
	/// accesses the "League of Legends.exe" module.</returns>
	static DataAccessor* GetInstance() {
		if (!gpDataAccessor)
			gpDataAccessor = new DataAccessor(
				MemoryManager::GetModuleInfo(LOL_PROGRAM_NAME)
			);
		return gpDataAccessor;
	}
};
