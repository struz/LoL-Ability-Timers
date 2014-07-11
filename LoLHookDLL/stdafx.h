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