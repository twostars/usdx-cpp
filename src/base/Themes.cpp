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
#include "PathUtils.h"
#include "Themes.h"
#include "Log.h"
#include "Skins.h"
#include "ThemeDefines.h"

using namespace boost::filesystem;

initialiseSingleton(Themes);

extern path ThemePath;

Themes::Themes()
{
	#define LOAD_THEME(name) name = new Theme ## name()

	LOAD_THEME(Loading);
	LOAD_THEME(Main);
	LOAD_THEME(Name);
	LOAD_THEME(Level);
	LOAD_THEME(Song);
	LOAD_THEME(Sing);
	LOAD_THEME(Score);
	LOAD_THEME(Top5);
	LOAD_THEME(Options);
	LOAD_THEME(OptionsGame);
	LOAD_THEME(OptionsGraphics);
	LOAD_THEME(OptionsSound);
	LOAD_THEME(OptionsLyrics);
	LOAD_THEME(OptionsThemes);
	LOAD_THEME(OptionsRecord);
	LOAD_THEME(OptionsAdvanced);
	LOAD_THEME(Edit);
	LOAD_THEME(ErrorPopup);
	LOAD_THEME(CheckPopup);
	LOAD_THEME(SongMenu);
	LOAD_THEME(SongJumpTo);
	LOAD_THEME(PartyNewRound);
	LOAD_THEME(PartyWin);
	LOAD_THEME(PartyScore);
	LOAD_THEME(PartyOptions);
	LOAD_THEME(PartyPlayer);
	LOAD_THEME(PartyRounds);
	LOAD_THEME(StatMain);
	LOAD_THEME(StatDetail);
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

	LOAD_THEME(Loading);
	LOAD_THEME(Main);
	LOAD_THEME(Name);
	LOAD_THEME(Level);
	LOAD_THEME(Song);
	LOAD_THEME(Sing);
	LOAD_THEME(Score);
	LOAD_THEME(Top5);
	LOAD_THEME(Options);
	LOAD_THEME(OptionsGame);
	LOAD_THEME(OptionsGraphics);
	LOAD_THEME(OptionsSound);
	LOAD_THEME(OptionsLyrics);
	LOAD_THEME(OptionsThemes);
	LOAD_THEME(OptionsRecord);
	LOAD_THEME(OptionsAdvanced);
	LOAD_THEME(Edit);
	LOAD_THEME(ErrorPopup);
	LOAD_THEME(CheckPopup);
	LOAD_THEME(SongMenu);
	LOAD_THEME(SongJumpTo);
	LOAD_THEME(PartyNewRound);
	LOAD_THEME(PartyWin);
	LOAD_THEME(PartyScore);
	LOAD_THEME(PartyOptions);
	LOAD_THEME(PartyPlayer);
	LOAD_THEME(PartyRounds);
	LOAD_THEME(StatMain);
	LOAD_THEME(StatDetail);

	#undef LOAD_THEME
}

void Themes::LoadList()
{
	directory_iterator end;

	try
	{
		sLog.Status(_T("Themes::LoadList"), _T("Searching for themes in directory: %s"),
			ThemePath.native().c_str());

		// Clear the existing themes list.
		_themes.clear();

		for (directory_iterator itr(ThemePath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (is_directory(p)
				|| !p.has_extension()
				|| p.extension() != _T(".ini"))
				continue;

			sLog.Status(_T("Themes::LoadList"), _T("Found theme %s"),
				p.stem().native().c_str());

			LoadHeader(p);
		}
	}
	catch (filesystem_error)
	{
		sLog.Critical(_T("Themes::LoadList"), _T("Could not access themes directory (%s)"), 
			ThemePath.native().c_str());
	}
}

void Themes::LoadHeader(const path& iniFile)
{
	CSimpleIni ini(true);
	SI_Error result = ini.LoadFile(iniFile.native().c_str());
	if (result != SI_OK)
	{
		return sLog.Warn(_T("Themes::LoadHeader"), _T("Failed to load INI (%s)"),
			iniFile.native().c_str());
	}

	const TCHAR * section = _T("Theme");

	ThemeEntry theme;
	tstring themeVersion;
	const TCHAR * creator;

	theme.FileName = iniFile.filename().native();
	theme.Name = ini.GetValue(section, _T("Name"), _T("(no name)"));
	themeVersion = ini.GetValue(section, _T("US_Version"), _T("no version"));

	// Attempt to lookup theme's creator.
	// NOTE: Current USDX loads "Creator", but older themes use "Author".
	// Attempt to fallback to "Author" if "Creator" isn't found.
	creator				= ini.GetValue(section, _T("Creator"));
	if (creator == NULL)
		creator			= ini.GetValue(section, _T("Author"), _T(""));

	theme.Creator		= creator;

	if (themeVersion != USDX_THEME_VERSION)
	{
		return sLog.Warn(_T("Theme::LoadHeader"), _T("Wrong version (%s) for theme %s."),
			themeVersion.c_str(), theme.Name.c_str());
	}

	theme.DefaultSkin = sSkins.LookupSkinForTheme(theme.Name);
	if (theme.DefaultSkin == NULL)
	{
		return sLog.Warn(_T("Theme::LoadHeader"), _T("Skin does not exist for theme %s."),
			theme.Name.c_str());
	}

	_themes.insert(std::make_pair(theme.Name, theme));
}

/**
 * Looks up a given theme by its name.
 *
 * @param	themeName	Name of the theme to lookup.
 *
 * @return	NULL if it fails, else a pointer to the associated ThemeEntry.
 */

ThemeEntry * Themes::LookupTheme(tstring themeName)
{
	ThemeEntryMap::const_iterator itr = _themes.find(themeName);
	return (itr == _themes.end() ? NULL : (ThemeEntry *)&itr->second);
}

/**
 * Looks up a given theme by its name.
 * If the given theme was not found, attempt to fallback to the
 * specified default them.
 *
 * @param	themeName   	Name of the theme to lookup.
 * @param	defaultTheme	The theme to default to, in the event
 * 							the initial theme wasn't found.
 *
 * @return	NULL if it fails, else a pointer to the associated ThemeEntry.
 */

ThemeEntry * Themes::LookupThemeDefault(tstring themeName, tstring defaultTheme)
{
	ThemeEntry * result = LookupTheme(themeName);
	if (result != NULL)
		return result;

	return LookupTheme(defaultTheme);
}

Themes::~Themes()
{
	#define FREE_THEME(name) delete name

	FREE_THEME(Loading);
	FREE_THEME(Main);
	FREE_THEME(Name);
	FREE_THEME(Level);
	FREE_THEME(Song);
	FREE_THEME(Sing);
	FREE_THEME(Score);
	FREE_THEME(Top5);
	FREE_THEME(Options);
	FREE_THEME(OptionsGame);
	FREE_THEME(OptionsGraphics);
	FREE_THEME(OptionsSound);
	FREE_THEME(OptionsLyrics);
	FREE_THEME(OptionsThemes);
	FREE_THEME(OptionsRecord);
	FREE_THEME(OptionsAdvanced);
	FREE_THEME(Edit);
	FREE_THEME(ErrorPopup);
	FREE_THEME(CheckPopup);
	FREE_THEME(SongMenu);
	FREE_THEME(SongJumpTo);
	FREE_THEME(PartyNewRound);
	FREE_THEME(PartyWin);
	FREE_THEME(PartyScore);
	FREE_THEME(PartyOptions);
	FREE_THEME(PartyPlayer);
	FREE_THEME(PartyRounds);
	FREE_THEME(StatMain);
	FREE_THEME(StatDetail);

	#undef FREE_THEME
}
