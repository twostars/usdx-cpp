/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "stdafx.h"
#include "Config.h"
#include "PathUtils.h"
#include "Themes.h"
#include "Log.h"
#include "Skins.h"

#include "../lib/simpleini/SimpleIni.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

initialiseSingleton(Themes);

extern path ThemePath;

Themes::Themes()
{
	#define LOAD_THEME(name) name = new Theme ## name()

//	LOAD_THEME(Loading);
//	LOAD_THEME(Main);
//	LOAD_THEME(Name);
//	LOAD_THEME(Level);
//	LOAD_THEME(Song);
//	LOAD_THEME(Sing);
//	LOAD_THEME(Score);
//	LOAD_THEME(Top5);
//	LOAD_THEME(Options);
//	LOAD_THEME(OptionsGame);
//	LOAD_THEME(OptionsGraphics);
//	LOAD_THEME(OptionsSound);
//	LOAD_THEME(OptionsLyrics);
//	LOAD_THEME(OptionsThemes);
//	LOAD_THEME(OptionsRecord);
//	LOAD_THEME(OptionsAdvanced);
//	LOAD_THEME(Edit);
//	LOAD_THEME(ErrorPopup);
//	LOAD_THEME(CheckPopup);
//	LOAD_THEME(SongMenu);
//	LOAD_THEME(SongJumpto);
//	LOAD_THEME(PartyNewRound);
//	LOAD_THEME(PartyWin);
//	LOAD_THEME(PartyScore);
//	LOAD_THEME(PartyOptions);
//	LOAD_THEME(PartyPlayer);
//	LOAD_THEME(PartyRounds);
//	LOAD_THEME(StatMain);
//	LOAD_THEME(StatDetail);

	#undef LOAD_THEME

	LoadList();
}

/**
 * Frees existing theme objects, and recreates.
 */
void Themes::RecreateThemeObjects()
{
	#define LOAD_THEME(name) delete name; \
	name = new Theme ## name()

//	LOAD_THEME(Loading);
//	LOAD_THEME(Main);
//	LOAD_THEME(Name);
//	LOAD_THEME(Level);
//	LOAD_THEME(Song);
//	LOAD_THEME(Sing);
//	LOAD_THEME(Score);
//	LOAD_THEME(Top5);
//	LOAD_THEME(Options);
//	LOAD_THEME(OptionsGame);
//	LOAD_THEME(OptionsGraphics);
//	LOAD_THEME(OptionsSound);
//	LOAD_THEME(OptionsLyrics);
//	LOAD_THEME(OptionsThemes);
//	LOAD_THEME(OptionsRecord);
//	LOAD_THEME(OptionsAdvanced);
//	LOAD_THEME(Edit);
//	LOAD_THEME(ErrorPopup);
//	LOAD_THEME(CheckPopup);
//	LOAD_THEME(SongMenu);
//	LOAD_THEME(SongJumpto);
//	LOAD_THEME(PartyNewRound);
//	LOAD_THEME(PartyWin);
//	LOAD_THEME(PartyScore);
//	LOAD_THEME(PartyOptions);
//	LOAD_THEME(PartyPlayer);
//	LOAD_THEME(PartyRounds);
//	LOAD_THEME(StatMain);
//	LOAD_THEME(StatDetail);

	#undef LOAD_THEME
}

void Themes::LoadList()
{
	directory_iterator end;
	tstring message; // TODO: Clean this up

	try
	{
		message = _T("Searching for Theme: ");
		message += ThemePath.native();

		sLog.Status(_T("Themes::LoadList"), message.c_str());

		// Clear the existing themes list.
		_themes.clear();

		for (directory_iterator itr(ThemePath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (!is_directory(p)
				|| p.filename() == _T(".")
				|| p.filename() == _T(".."))
				continue;

			message = _T("Found theme ");
			message += p.filename().native();
			sLog.Status(_T("Themes::LoadList"), message.c_str());
			ParseDir(&p);
		}
	}
	catch (filesystem_error)
	{
		message = _T("Could not access ");
		message += ThemePath.native();
		sLog.Critical(message.c_str());
	}
}

void Themes::ParseDir(const path * dir)
{
	directory_iterator end;
	for (directory_iterator itr(*dir); itr != end; ++itr)
	{
		const path& p = itr->path();
		if (is_directory(p)
			|| !p.has_extension()
			|| p.extension() != _T(".ini"))
			continue;

		LoadHeader(&p);
	}
}

void Themes::LoadHeader(const path * iniFile)
{
	CSimpleIni ini(true);
	SI_Error result = ini.LoadFile(iniFile->native().c_str());
	if (result != SI_OK)
	{
		// TODO: Include filename of INI
		sLog.Warn(_T("Themes::LoadHeader"), _T("Failed to load INI"));
		return;
	}

	ThemeEntry theme;
	const TCHAR * themeVersion;
	const TCHAR * creator;

	theme.FileName = iniFile->filename().native();
	theme.Name = ini.GetValue(_T("Theme"), _T("Name"), _T(""));
	themeVersion = ini.GetValue(_T("Theme"), _T("US_Version"), _T("no version tag"));

	// Attempt to lookup theme's creator.
	// NOTE: Current USDX loads "Creator", but older themes use "Author".
	// Attempt to fallback to "Author" if "Creator" isn't found.
	creator				= ini.GetValue(_T("Theme"), _T("Creator"));
	if (creator == NULL)
		creator			= ini.GetValue(_T("Theme"), _T("Author"), _T(""));

	theme.Creator		= creator;

	if (themeVersion != USDX_THEME_VERSION)
	{
		// TODO: Clean this up
		tstring message = _T("Wrong version (");
		message += themeVersion;
		message += _T(") for theme ");
		message += theme.Name;

		sLog.Warn(_T("Theme::LoadHeader"), message.c_str());
		return;
	}

	theme.DefaultSkin = sSkins.LookupSkinForTheme(theme.Name);
	if (theme.DefaultSkin == NULL)
	{
		// TODO: Clean this up
		tstring message = _T("Skin does not exist for theme ");
		message += theme.Name;

		sLog.Warn(_T("Theme::LoadHeader"), message.c_str());
		return;
	}

	_themes.push_back(theme);
}

Themes::~Themes()
{
	#define FREE_THEME(name) delete name

//	FREE_THEME(Loading);
//	FREE_THEME(Main);
//	FREE_THEME(Name);
//	FREE_THEME(Level);
//	FREE_THEME(Song);
//	FREE_THEME(Sing);
//	FREE_THEME(Score);
//	FREE_THEME(Top5);
//	FREE_THEME(Options);
//	FREE_THEME(OptionsGame);
//	FREE_THEME(OptionsGraphics);
//	FREE_THEME(OptionsSound);
//	FREE_THEME(OptionsLyrics);
//	FREE_THEME(OptionsThemes);
//	FREE_THEME(OptionsRecord);
//	FREE_THEME(OptionsAdvanced);
//	FREE_THEME(Edit);
//	FREE_THEME(ErrorPopup);
//	FREE_THEME(CheckPopup);
//	FREE_THEME(SongMenu);
//	FREE_THEME(SongJumpto);
//	FREE_THEME(PartyNewRound);
//	FREE_THEME(PartyWin);
//	FREE_THEME(PartyScore);
//	FREE_THEME(PartyOptions);
//	FREE_THEME(PartyPlayer);
//	FREE_THEME(PartyRounds);
//	FREE_THEME(StatMain);
//	FREE_THEME(StatDetail);

	#undef FREE_THEME
}
