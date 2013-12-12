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

#ifndef _THEMES_H
#define _THEMES_H
#pragma once

// TODO: Clean up access to these
#include "ThemeDefines.h"

struct ThemeEntry
{
	boost::filesystem::path Path;
	boost::filesystem::path FileName;
	tstring Theme;
	tstring Name;
	tstring Creator;
	struct SkinEntry * DefaultSkin;
};

#define DEFAULT_THEME _T("DELUXE")

class Themes : public Singleton<Themes>
{
public:
	Themes();

	INLINE size_t GetThemeCount() { return _themes.size(); }

	void CreateThemeObjects();
	void LoadList();
	void LoadHeader(const boost::filesystem::path& iniFile);
	void LoadTheme(ThemeEntry * theme, eColor color);
	void LoadColors();
	void LoadColor(RGB& rgb, tstring color);

	void LoadThemeBasic(ThemeBasic * theme, const tstring& name);
	void LoadThemeBackground(ThemeBackground& themeBackground, const tstring& name);
	void LoadThemeText(ThemeText& themeText, const tstring& name);
	void LoadThemeTexts(AThemeText& themeTextCollection, const tstring& name);
	void LoadThemeStatic(ThemeStatic& themeStatic, const tstring& name);
	void LoadThemeStatics(AThemeStatic& themeStaticCollection, const tstring& name);
	void LoadThemeButton(ThemeButton& themeButton, const tstring& name, AThemeButtonCollection* themeButtonCollection = NULL);
	void LoadThemeButtonCollection(ThemeButtonCollection& themeButtonCollection, const tstring& name);
	void LoadThemeButtonCollections(AThemeButtonCollection& themeButtonCollection, const tstring& name);
	void LoadThemeSelectSlide(ThemeSelectSlide& themeSelectSlide, const tstring& name);
	void LoadThemeEqualizer(ThemeEqualizer& themeEqualizer, const tstring& name);

	ThemeEntry * LookupTheme(tstring themeName);
	ThemeEntry * LookupThemeDefault(tstring themeName, tstring defaultTheme);

	~Themes();

public:
	#define DECL_THEME(name) class Theme ## name * name

	DECL_THEME(Loading);
	DECL_THEME(Main);
	DECL_THEME(Name);
	DECL_THEME(Level);
	DECL_THEME(Song);
	DECL_THEME(Sing);
	DECL_THEME(LyricBar);
	DECL_THEME(Score);
	DECL_THEME(Top5);
	DECL_THEME(Options);
	DECL_THEME(OptionsGame);
	DECL_THEME(OptionsGraphics);
	DECL_THEME(OptionsSound);
	DECL_THEME(OptionsLyrics);
	DECL_THEME(OptionsThemes);
	DECL_THEME(OptionsRecord);
	DECL_THEME(OptionsAdvanced);
	DECL_THEME(Edit);
	DECL_THEME(ErrorPopup);
	DECL_THEME(CheckPopup);
	DECL_THEME(SongMenu);
	DECL_THEME(SongJumpTo);
	DECL_THEME(PartyNewRound);
	DECL_THEME(PartyWin);
	DECL_THEME(PartyScore);
	DECL_THEME(PartyOptions);
	DECL_THEME(PartyPlayer);
	DECL_THEME(PartyRounds);
	DECL_THEME(StatMain);
	DECL_THEME(StatDetail);
	DECL_THEME(Playlist);

	#undef DECL_THEME

private:
	typedef std::map<tstring, ThemeEntry, tstring_ci> ThemeEntryMap;
	typedef std::map<tstring, RGB, tstring_ci> ColorMap;

	ThemeEntryMap _themes;
	ColorMap _colors;
	CSimpleIni ini;

	ThemeBasic * _lastThemeBasic;
};

#define sThemes (Themes::getSingleton())

#endif
