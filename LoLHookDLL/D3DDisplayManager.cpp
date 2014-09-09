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

#include "stdafx.h"
#include "D3DDisplayManager.h"
#include "DataAccessor.h"
#include "structs.h"
#include "Spell.h"
#include <stdexcept>

// Global instantiation of this manager
D3DDisplayManager* gpDisplayManager = NULL;

// Vertex format struct for use drawing 2d stuff
struct D3DVERTEX {
	float x, y, z, rhw;
	D3DCOLOR color;
};

D3DDisplayManager::D3DDisplayManager()
{
	pFont = NULL;
	pTitleFont = NULL;
	pFontSprite = NULL;
	mpWindowRect = NULL;
	pLine = NULL;

	// Setup display parameters
	mDisplayParams = {
		50, // inset from right
		110, // inset from top
		25, // row spacing
		100, // column spacing
		160, // backdrop width
		180, // backdrop height
		50 // inset from left
	};

	mAbilityKey[0] = 'Q';
	mAbilityKey[1] = 'W';
	mAbilityKey[2] = 'E';
	mAbilityKey[3] = 'R';

	InitializeSummonerSpellMap();

	mHasCache = false;
	mCacheLastTime = 0.0f;

	CallRiotLog("D3DDisplayManager class created successfully.");
}

D3DDisplayManager::~D3DDisplayManager()
{
	DeviceReset();
	CleanupCache();
}

/// <summary>Cleans up all cache assets and marks the cache
/// as out of date.</summary>
void D3DDisplayManager::CleanupCache() {
	if (mHasCache) {
		for (size_t i = 0; i < mCachedCooldownText.size(); i++) {
			delete mCachedCooldownText[i];
		}
		for (size_t i = 0; i < mCachedHeroes.size(); i++) {
			delete mCachedHeroes[i];
		}
	}
	mHasCache = false;
}

/// <sumary>Initializes the map of behind-the-scenes summoner
/// spell names to point to more display friendly ones.</summary>
void D3DDisplayManager::InitializeSummonerSpellMap() {
	mSummonerSpellMap["SummonerTeleport"] = "Teleport";
	mSummonerSpellMap["teleportcancel"] = "TPing";
	mSummonerSpellMap["SummonerBarrier"] = "Barrier";
	mSummonerSpellMap["SummonerFlash"] = "Flash";
	mSummonerSpellMap["SummonerDot"] = "Ignite";
	mSummonerSpellMap["SummonerHeal"] = "Heal";
	mSummonerSpellMap["SummonerHaste"] = "Ghost";
	mSummonerSpellMap["SummonerMana"] = "Clarity";
	mSummonerSpellMap["SummonerSmite"] = "Smite";
	mSummonerSpellMap["SummonerBoost"] = "Cleanse";
	mSummonerSpellMap["SummonerRevive"] = "Revive";
	mSummonerSpellMap["SummonerClairvoyance"] = "CV";
	mSummonerSpellMap["SummonerExhaust"] = "Exhaust";
}

/// <summary>Manages the creation of all resources required by this
/// class to draw text.</summary>
/// <params name="pDevice">LPDIRECT3DDEVICE9 to create data with. Must be
/// the same device later used to render.</params>
/// <returns>true if successful, false if failed.</returns>
bool D3DDisplayManager::_CreateFont(LPDIRECT3DDEVICE9 pDevice){
	// TODO: scale font size with resolution?
	if (!pFont) {
		// Create a font if we haven't already
		HRESULT hr = D3DXCreateFont(pDevice, 18, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pFont);
		if (FAILED(hr))
			return false;
	}
	if (!pTitleFont) {
		HRESULT hr = D3DXCreateFont(pDevice, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pTitleFont);
		if (FAILED(hr))
			return false;
	}
	if (!pFontSprite) {
		HRESULT hr = D3DXCreateSprite(pDevice, &pFontSprite);
		if (FAILED(hr))
			return false;
	}
	if (!pLine) {
		HRESULT hr = D3DXCreateLine(pDevice, &pLine);
		if (FAILED(hr))
			return false;
		pLine->SetAntialias(false);
		pLine->SetGLLines(true);
	}
	return true;
}

/// <summary>Gets the current resolution of the game.</summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 to get information from.</param>
/// <returns>A pointer to a RECT struct containing the resolution data</returns>
RECT* D3DDisplayManager::_GetWindowRect(LPDIRECT3DDEVICE9 pDevice) {
	D3DDEVICE_CREATION_PARAMETERS cparams;
	if (!mpWindowRect) {
		pDevice->GetCreationParameters(&cparams);
		mpWindowRect = new RECT();
		GetClientRect(cparams.hFocusWindow, mpWindowRect);
		CallRiotLog("rect.bot = %d, rect.right = %d", mpWindowRect->bottom, mpWindowRect->right);
	}
	return mpWindowRect;
}

/// <summary>Caches the current cooldown text for all champions
/// into an array of strings. Also updates the cached copy of the
/// global champions array.</summary>
/// <returns>true on success, false on fail.</summary>
bool D3DDisplayManager::CacheChampionCooldownData() {
	// get the current time
	float simulationTime = DataAccessor::GetInstance()->GetSimulationTime();

	// ensure we clear our previously cached list of heroes, cleaning up memory
	for (size_t i = 0; i < mCachedHeroes.size(); i++) {
		delete mCachedHeroes[i];
	}
	mCachedHeroes.clear();

	// get the most recent list of heroes
	DWORD numHeroes = DataAccessor::GetInstance()->GetNumScoreboardHeroes();
	ObjAIAccessor** pHeroes = new ObjAIAccessor*[numHeroes];
	DataAccessor::GetInstance()->GetScoreboardHeroes(pHeroes);

	// we are assuming here that a spell name will not be absolutely massive
	char outputBuf[40];
	for (DWORD i = 0; i < numHeroes; i++) {
		ObjAIAccessor* pHero = pHeroes[i];
		
		// ensure a string has been created for this index
		while (mCachedCooldownText.size() <= i)
			mCachedCooldownText.push_back(new std::string());
		std::string* cooldownString = mCachedCooldownText.at(i);

		// we reuse old strings for speed, and just clear them
		cooldownString->clear();

		// Add the heroes regular abilities to the string
		SpellbookAccessor spellbook = pHero->GetSpellbook();
		for (DWORD i = 0; i < 4; i++) {
			SpellDataInstAccessor spellDataInst = spellbook.GetSpellDataInstForSlot(i);
			float cooldownSecs = spellDataInst.GetCooldownExpires() - simulationTime;

			if (cooldownSecs <= 0.0f)
				_snprintf_s(outputBuf, sizeof(outputBuf), "%c [%d]: UP\n",
					mAbilityKey[i], spellDataInst.GetSpellLevel());
			else
				_snprintf_s(outputBuf, sizeof(outputBuf), "%c [%d]: %3.1f sec\n",
					mAbilityKey[i], spellDataInst.GetSpellLevel(), cooldownSecs);
			cooldownString->append(outputBuf);
		}

		// Add the summoner abilities to the string
		spellbook = pHero->GetSummonerSpellbookByVtable();
		for (DWORD i = 0; i < 2; i++) {
			SpellDataInstAccessor spellDataInst = spellbook.GetSpellDataInstForSlot(i);

			float cooldownSecs = spellDataInst.GetCooldownExpires() - simulationTime;

			std::string summonerSpellNameStr = 
				StringHolder_GetCString(spellDataInst.GetSpellData()
				.GetSpellName());

			// the hopefully resolved name from our lookup table
			std::string summonerSpellNameStrResolved;

			try {
				summonerSpellNameStrResolved = 
					mSummonerSpellMap.at(summonerSpellNameStr);
			}
			catch (const std::out_of_range) {
				// The spell name wasn't in our predefined lookup table
				// Print the ugly internal name instead
				summonerSpellNameStrResolved = summonerSpellNameStr;
			}

			const char* summonerSpellName = summonerSpellNameStrResolved.c_str();

			if (cooldownSecs <= 0.0f)
				_snprintf_s(outputBuf, sizeof(outputBuf), "%s: UP\n", summonerSpellName);
			else
				_snprintf_s(outputBuf, sizeof(outputBuf), "%s: %3.1f sec\n",
					summonerSpellName, cooldownSecs);
			cooldownString->append(outputBuf);
		}

		mCachedHeroes.push_back(pHero); // add the hero to the hero array
	}

	// clean up the array pointer we used
	delete[] pHeroes;
	mHasCache = true;
	mCacheLastTime = simulationTime;
	return true;
}

/// <summary>Does the formatting and drawing of the champion cooldown
/// interface for a single champion, based on what is in the cache
/// at the time of calling.</summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 to render to.</param>
/// <param name="cacheIndex">Index within the cache storage of the hero
/// data to be rendered.</param>
/// <param name="numHeroOnTeam">The number specifying this champion's
/// order within its team. For example, if we've drawn 2 other champions
/// already for this hero's team, then this number should be 3 for proper
/// display formatting.</param>
/// <returns>0 for success, 1 for fail.</returns>
HRESULT D3DDisplayManager::DoDrawChampionCooldownsFromCache(LPDIRECT3DDEVICE9 pDevice,
		size_t cacheIndex, BYTE numHeroOnTeam) {
	RECT* pWindowRect = _GetWindowRect(pDevice);
	ObjAIAccessor* pHero = mCachedHeroes[cacheIndex];

	DWORD team = pHero->GetTeam();
	if (team != RIOT_TEAM_ORDER && team != RIOT_TEAM_CHAOS)
		return 1; // invalid team for display purposes

	// Order on the left, chaos on the right - might change this to
	// always being the player team on a particular side sometime
	LONG x, y;
	if (team == RIOT_TEAM_ORDER) {
		x = mDisplayParams.insetFromLeft;
	}
	else {
		// go far enough in from the right to have space for 2 columns plus the inset
		x = (pWindowRect->right - mDisplayParams.insetFromRight -
			(2 * mDisplayParams.backdropWidth) - mDisplayParams.columnSpacing);
	}
	y = ((numHeroOnTeam / 2) * (mDisplayParams.backdropHeight +
		mDisplayParams.rowSpacing)) + mDisplayParams.insetFromTop;

	// two columns of display per team
	if (numHeroOnTeam % 2 == 1)
		x += mDisplayParams.columnSpacing + mDisplayParams.backdropWidth;

	// Different colour for the different teams
	BYTE colorR = (team == RIOT_TEAM_ORDER) ? 0 : 220;
	BYTE colorG = (team == RIOT_TEAM_ORDER) ? 220 : 0;
	BYTE colorB = (team == RIOT_TEAM_ORDER) ? 0 : 0;

	// Draw the backdrop
	gpDisplayManager->DrawRectToScreen(pDevice, (float)x, (float)y,
		(float)mDisplayParams.backdropWidth, (float)mDisplayParams.backdropHeight,
		180, 0, 0, 0);
	y += mDisplayParams.backdropHeight / 10;

	// Draw the champ name
	DrawCooldownTextToScreen(pDevice, x, y, 220, colorR, colorG, colorB,
		pHero->GetSkinName(), FontType::TITLE);
	y += 35;

	// Draw the ability text
	DrawCooldownTextToScreen(pDevice, x, y, 220, colorR, colorG, colorB,
		mCachedCooldownText[cacheIndex]->c_str());
	return 0;
}

/// <summary>Updates the cache if it needs updating and then draws the cooldowns
/// for each cached champion to the screen in a two column layout.</summary>
/// <param "pDevice">LPDIRECT3DDEVICE9 to use for rendering.</param>
/// <returns>0 if successful, 1 otherwise.</returns>
HRESULT D3DDisplayManager::DrawChampionCooldownsFromCache(LPDIRECT3DDEVICE9 pDevice) {
	// check if we need to update the cache
	if (!mHasCache || DataAccessor::GetInstance()->GetSimulationTime()
		> (mCacheLastTime + mCacheDelta)) {
		CacheChampionCooldownData();
	}

	BYTE numTeamOrder = 0, numTeamChaos = 0;
	DWORD team;
	for (size_t i = 0; i < mCachedHeroes.size(); i++) {
		team = mCachedHeroes[i]->GetTeam();
		if (team == RIOT_TEAM_ORDER) {
			try {
				DoDrawChampionCooldownsFromCache(pDevice, i, numTeamOrder);
			}
			catch (InvalidSpellException ex) {
				// ignore, just means a spell hasn't loaded yet
			}
			numTeamOrder++;
		}
		else if (team == RIOT_TEAM_CHAOS) {
			try {
				DoDrawChampionCooldownsFromCache(pDevice, i, numTeamChaos);
			}
			catch (InvalidSpellException ex) {
				// ignore, just means a spell hasn't loaded yet
			}
			numTeamChaos++;
		} // can have other values sometimes
	}
	return 0;
}

/// <summary>Draws a rectangle to the screen at the specified coordinates
/// with the given width, height and color.</summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 to render to.</param>
/// <param name="x">Screen X coordinate to draw to.</param>
/// <param name="y">Screen Y coordinate to draw to.</param>
/// <param name="width">Width of the rectangle.</param>
/// <param name="height">Height of the rectangle.</param>
/// <param name="alpha">Alpha level for the rectangle (transparency).</param>
/// <param name="r">Red color component for the colour of the rectangle.</param>
/// <param name="g">Green color component for the colour of the rectangle.</param>
/// <param name="b">Blue color component for the colour of the rectangle.</param>
bool D3DDisplayManager::DrawRectToScreen(LPDIRECT3DDEVICE9 pDevice, float x,
		float y, float width, float height, int alpha, int r, int g, int b) {

	_CreateFont(pDevice);

	D3DCOLOR rectColor = D3DCOLOR_ARGB(alpha, r, g, b);

	pLine->SetWidth(width);

	// Draw a line with the thickness of width / 2 to get the desired width
	D3DXVECTOR2 vLine[2];
	vLine[0].x = x + width / 2;
	vLine[0].y = y;
	vLine[1].x = x + width / 2;
	vLine[1].y = y + height;

	pLine->Begin();
	pLine->Draw(vLine, 2, rectColor);
	pLine->End();

	return true;
}

/// <summary>Overload for optional fontType argument - 
/// defaults fontType to FontType::REGULAR</summary>
bool D3DDisplayManager::DrawCooldownTextToScreen(LPDIRECT3DDEVICE9 pDevice, unsigned int x,
		unsigned int y, int alpha, int r, int g, int b, LPCSTR message) {

	return DrawCooldownTextToScreen(pDevice, x, y, alpha, r, g, b, message, FontType::REGULAR);
}

/// <summary>Draws some text to the screen at the specified coordinates
/// with the given color, drawing from the display parameters initialized
/// in this display manager's constructor.</summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 to render to.</param>
/// <param name="x">Screen X coordinate to draw to.</param>
/// <param name="y">Screen Y coordinate to draw to.</param>
/// <param name="alpha">Alpha level for the rectangle (transparency).</param>
/// <param name="r">Red color component for the colour of the rectangle.</param>
/// <param name="g">Green color component for the colour of the rectangle.</param>
/// <param name="b">Blue color component for the colour of the rectangle.</param>
/// <param name="message">String to draw.</param>
/// <param name="fontType">D3DDisplayManager::FontType value to describe the
/// type of font which should be used.</param>
bool D3DDisplayManager::DrawCooldownTextToScreen(LPDIRECT3DDEVICE9 pDevice,
		unsigned int x,	unsigned int y, int alpha, int r, int g, int b,
		LPCSTR message,	FontType fontType) {

	_CreateFont(pDevice);

	// Choose our font based on input
	LPD3DXFONT font;
	switch (fontType) {
		case FontType::TITLE:
			font = pTitleFont;
			break;
		default:
			font = pFont;
	}

	D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, r, g, b);
	RECT rct;
	rct.left = x;
	rct.right = x + ((mDisplayParams.backdropWidth / 5) * 4);
	rct.top = y;
	rct.bottom = rct.top + mDisplayParams.backdropHeight;

	// TODO: add better support for different resolutions:
	// Use a lookup table to adjust font size dynamically?

	// Draw the text
	pFontSprite->Begin(D3DXSPRITE_SORT_TEXTURE);
	font->DrawTextA(pFontSprite, message, -1, &rct, DT_RIGHT, fontColor);
	pFontSprite->End();
	return true;
}

void D3DDisplayManager::DeviceLost() {

}

/// <summary>Frees all resources used by the display manager.
/// Intended to be called when the IDirect3DDevice9 is reset.</summary>
void D3DDisplayManager::DeviceReset() {
	if (pFont) {
		pFont->Release();
		pFont = NULL;
	}
	if (pTitleFont) {
		pTitleFont->Release();
		pTitleFont = NULL;
	}
	if (pFontSprite) {
		pFontSprite->Release();
		pFontSprite = NULL;
	}
	if (mpWindowRect) {
		delete mpWindowRect;
		mpWindowRect = NULL;
	}
	if (pLine) {
		pLine->Release();
		pLine = NULL;
	}
}

// === BEGIN DEPRECATED FUNCTIONS ===

/// <summary>Draws a UI to the screen that shows all of the cooldowns for
/// the supplied champion.</summary>
/// <param name="pDevice">The D3D device to do render with.</param>
/// <param name="pHero">The obj_AI_Hero accessor object.</param>
/// <param name="numHeroOnTeam">The number specifying this champion's
/// order within its team. For example, if we've drawn 2 other champions
/// already for this hero's team, then this number should be 3 for proper
/// display formatting.</param>
/// <returns>1 if failed, 0 otherwise.</summary>
HRESULT D3DDisplayManager::DoDrawChampionCooldowns(LPDIRECT3DDEVICE9 pDevice,
	ObjAIAccessor* pHero, BYTE numHeroOnTeam) {
	DWORD team = pHero->GetTeam();
	if (team != RIOT_TEAM_ORDER && team != RIOT_TEAM_CHAOS)
		return 1; // invalid team for display purposes

	RECT* pWindowRect = _GetWindowRect(pDevice);
	float simulationTime = DataAccessor::GetInstance()->GetSimulationTime();

	// we are assuming here that a spell name will not be absolutely massive
	char outputBuf[100];

	// Order on the left, chaos on the right - might change this to
	// always being the player team on the left some time
	LONG x, y;
	if (team == RIOT_TEAM_ORDER) {
		x = mDisplayParams.insetFromLeft;
	}
	else {
		// go far enough in from the right to have space for 2 columns plus the inset
		x = (pWindowRect->right - mDisplayParams.insetFromRight -
			(2 * mDisplayParams.backdropWidth) - mDisplayParams.columnSpacing);
	}
	y = ((numHeroOnTeam / 2) * (mDisplayParams.backdropHeight +
		mDisplayParams.rowSpacing)) + mDisplayParams.insetFromTop;

	// two columns of display per team
	if (numHeroOnTeam % 2 == 1)
		x += mDisplayParams.columnSpacing + mDisplayParams.backdropWidth;

	// Different colour for the different teams
	BYTE colorR = (team == RIOT_TEAM_ORDER) ? 0 : 220;
	BYTE colorG = (team == RIOT_TEAM_ORDER) ? 220 : 0;
	BYTE colorB = (team == RIOT_TEAM_ORDER) ? 0 : 0;

	// Draw the backdrop
	gpDisplayManager->DrawRectToScreen(pDevice, (float)x, (float)y,
		(float)mDisplayParams.backdropWidth, (float)mDisplayParams.backdropHeight,
		180, 0, 0, 0);
	y += mDisplayParams.backdropHeight / 10;

	// Draw the champ name
	DrawCooldownTextToScreen(pDevice, x, y, 220, colorR, colorG, colorB,
		pHero->GetSkinName(), FontType::TITLE);
	y += 35;

	// String to hold our data before drawing
	std::string abilitiesString;

	// Draw their regular abilities
	SpellbookAccessor spellbook = pHero->GetSpellbook();
	for (DWORD i = 0; i < 4; i++) {
		SpellDataInstAccessor spellDataInst = spellbook.GetSpellDataInstForSlot(i);
		float cooldownSecs = spellDataInst.GetCooldownExpires() - simulationTime;

		if (cooldownSecs <= 0)
			sprintf_s(outputBuf, "%c: UP\n", mAbilityKey[i]);
		else
			sprintf_s(outputBuf, "%c: %3.1f sec\n", mAbilityKey[i], cooldownSecs);
		abilitiesString.append(outputBuf);
	}

	// Draw their summoners
	spellbook = pHero->GetSummonerSpellbookByVtable();
	for (DWORD i = 0; i < 2; i++) {
		SpellDataInstAccessor spellDataInst = spellbook.GetSpellDataInstForSlot(i);
		float cooldownSecs = spellDataInst.GetCooldownExpires() - simulationTime;
		const char* summonerSpellName =
			mSummonerSpellMap[
				StringHolder_GetCString(
					spellDataInst.GetSpellData().GetSpellName())
			].c_str();

		if (cooldownSecs <= 0)
			sprintf_s(outputBuf, "%s: UP\n", summonerSpellName);
		else
			sprintf_s(outputBuf, "%s: %3.1f sec\n", summonerSpellName, cooldownSecs);
		abilitiesString.append(outputBuf);
	}

	// Draw the text
	DrawCooldownTextToScreen(pDevice, x, y, 220, colorR, colorG, colorB, abilitiesString.c_str());
	return 0;
}

/// <summary>Gets the cooldown data for each champion and draws it to the
/// screen in a formatted way.</summary>
/// <param "pDevice">LPDIRECT3DDEVICE9 to use for rendering.</param>
/// <returns>0 if successful, 1 otherwise.</returns>
HRESULT D3DDisplayManager::DrawChampionCooldowns(LPDIRECT3DDEVICE9 pDevice) {
	// get the list of heroes
	DWORD numHeroes = DataAccessor::GetInstance()->GetNumScoreboardHeroes();
	ObjAIAccessor** pHeroes = new ObjAIAccessor*[numHeroes];
	DataAccessor::GetInstance()->GetScoreboardHeroes(pHeroes);

	BYTE numTeamOrder = 0, numTeamChaos = 0;
	DWORD team;
	for (DWORD i = 0; i < numHeroes; i++) {
		team = pHeroes[i]->GetTeam();
		if (team == RIOT_TEAM_ORDER) {
			try {
				DoDrawChampionCooldowns(pDevice, pHeroes[i], numTeamOrder);
			}
			catch (...) {

			}
			numTeamOrder++;
		}
		else if (team == RIOT_TEAM_CHAOS) {
			try {
				DoDrawChampionCooldowns(pDevice, pHeroes[i], numTeamChaos);
			}
			catch (...) {

			}
			numTeamChaos++;
		} // can have other values sometimes
		// clean up the pointer to the hero we're done with
		delete pHeroes[i];
	}
	delete[] pHeroes;
	return 0;
}

// === END DEPRECATED FUNCTIONS ===