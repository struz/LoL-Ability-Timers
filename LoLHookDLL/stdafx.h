#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <string>
#include <vector>
#include <map>

#include <d3d9.h>
#include "d3dx9.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define LOL_PROGRAM_NAME L"League of Legends.exe"
#define D3D_DLL_NAME L"d3d9.dll"

#define RIOT_TEAM_ORDER 100
#define RIOT_TEAM_CHAOS 200
#define RIOT_TEAM_UNKNOWN 300

#define LOG_MESSAGES
// convenience macro for mass disabling of logging
#ifdef LOG_MESSAGES
	#define CallRiotLog(x, ...) DataAccessor::GetInstance()->GetRiotLog()(3, 1, 0, x, ##__VA_ARGS__)
#else
	#define CallRiotLog(x)
#endif