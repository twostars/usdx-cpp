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
	path Path;
	path FileName;
	std::string Theme;
	std::string Name;
	std::string Creator;
	eColor  DefaultColor;
};

class Skins : public Singleton<Skins>
{
public:
	Skins();
	void LoadList();
	void ParseDir(const path& dir);
	void LoadHeader(const path& iniFile);
	void LoadSkin(SkinEntry * skin, eColor color);

	SkinEntry* LookupSkinForTheme(const std::string& themeName);
	SkinEntry* LookupSkinForTheme(const std::string& skinName, const std::string& themeName);
	const path* GetTextureFileName(const std::string& textureName);
	const path* GetTextureFileName(const char * fmt, ...);

	~Skins();

private:
	typedef std::multimap<std::string, SkinEntry, string_ci> SkinEntryMap;
	typedef std::map<std::string, std::string, string_ci> SkinThemeMap;

	SkinEntryMap _skins;
	SkinThemeMap _skinThemeMap;

public: 
	// Skin specific config
	path SkinPath;
	eColor SkinColor;

	typedef std::map<std::string, path, string_ci> SkinTextureMap;
	SkinTextureMap SkinTextures;
};

#define sSkins (Skins::getSingleton())

#endif
