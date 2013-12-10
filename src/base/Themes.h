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

struct ThemePlaylist
{
	tstring CatText;
};

struct ThemeEntry
{
	tstring Path;
	tstring FileName;
	tstring Theme;
	tstring Name;
	tstring Creator;
	struct SkinEntry * DefaultSkin;
};

class Themes : public Singleton<Themes>
{
public:
	Themes();
	void RecreateThemeObjects();
	void LoadList();
	void ParseDir(const boost::filesystem::path * dir);
	void LoadHeader(const boost::filesystem::path * iniFile);
	~Themes();

public:
	#define DECL_THEME(name) class Theme ## name * name

	DECL_THEME(Loading);
	DECL_THEME(Main);
	DECL_THEME(Name);
	DECL_THEME(Level);
	DECL_THEME(Song);
	DECL_THEME(Sing);
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

	#undef DECL_THEME

private:
	typedef std::vector<ThemeEntry> ThemeEntrySet;
	ThemeEntrySet _themes;
};

#define sThemes (Themes::getSingleton())

#endif
