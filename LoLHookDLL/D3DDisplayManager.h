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
#include "ObjAIAccessor.h"
#include <string>

struct DisplayParams {
	LONG insetFromRight;
	LONG insetFromTop;
	LONG rowSpacing;
	LONG columnSpacing;
	LONG backdropWidth;
	LONG backdropHeight;
	LONG insetFromLeft;
};

class D3DDisplayManager
{
protected:
	// Our fonts
	LPD3DXFONT pFont;
	LPD3DXFONT pTitleFont;
	LPD3DXSPRITE pFontSprite;

	// Background for drawing on
	LPD3DXLINE pLine;

	bool _CreateFont(LPDIRECT3DDEVICE9 pDevice);
	// Current window size / resolution
	RECT* mpWindowRect;
	RECT* _GetWindowRect(LPDIRECT3DDEVICE9 pDevice);

	// helper table convert ability num to key
	char mAbilityKey[4];
	// helper table convert summoner spell name to readable name
	std::map<std::string, std::string> mSummonerSpellMap;
	void InitializeSummonerSpellMap();

	// Display related stuff
	DisplayParams mDisplayParams;
	HRESULT DoDrawChampionCooldownsFromCache(LPDIRECT3DDEVICE9 pDevice,
		size_t cacheIndex, BYTE numHeroOnTeam);

	// For performance reasons we won't update EVERY frame
	std::vector<std::string*> mCachedCooldownText; // last cached cooldown text
	std::vector<ObjAIAccessor*> mCachedHeroes; // last cached list of heroes
	bool mHasCache; // does the cache exist?
	float mCacheLastTime; // the time we last cached data
	const float mCacheDelta = 0.4f; // num seconds between cache updates
	bool D3DDisplayManager::CacheChampionCooldownData();

	// DEPRECATED FUNCTIONS
	HRESULT DoDrawChampionCooldowns(LPDIRECT3DDEVICE9 pDevice,
		ObjAIAccessor* pHero, BYTE numHeroOnTeam);
public:
	// enum for controlling font types
	enum FontType {
		REGULAR,
		TITLE
	};

	D3DDisplayManager();
	~D3DDisplayManager();
	bool DrawCooldownTextToScreen(LPDIRECT3DDEVICE9 pDevice, unsigned int x,
		unsigned int y, int alpha, int r, int g, int b, LPCSTR message);
	bool DrawCooldownTextToScreen(LPDIRECT3DDEVICE9 pDevice, unsigned int x,
		unsigned int y, int alpha, int r, int g, int b, LPCSTR message,
		FontType fontType);
	bool DrawRectToScreen(LPDIRECT3DDEVICE9 pDevice, float x, float y,
		float width, float height, int alpha, int r, int g, int b);
	HRESULT DrawChampionCooldownsFromCache(LPDIRECT3DDEVICE9 pDevice);
	void DeviceLost();
	void DeviceReset();
	void CleanupCache();

	// DEPRECATED FUNCTIONS
	HRESULT DrawChampionCooldowns(LPDIRECT3DDEVICE9 pDevice);
};

extern D3DDisplayManager* gpDisplayManager;