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

#ifndef _SKINS_H
#define _SKINS_H
#pragma once

struct SkinEntry
{
	tstring Path;
	tstring FileName;
	tstring Theme;
	tstring Name;
	tstring Creator;
	uint32  DefaultColor;
};

class Skins : public Singleton<Skins>
{
public:
	Skins();
	void LoadList();
	void ParseDir(const boost::filesystem::path * dir);
	void LoadHeader(const boost::filesystem::path * iniFile);
	SkinEntry* LookupSkinForTheme(const tstring& themeName);
	~Skins();

private:
	typedef std::map<tstring, SkinEntry, tstring_ci> SkinEntryMap;
	SkinEntryMap _skins;
};

#define sSkins (Skins::getSingleton())

#endif