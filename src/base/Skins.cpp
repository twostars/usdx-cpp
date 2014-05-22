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
#include "Skins.h"
#include "Log.h"
#include "Ini.h"

using namespace boost::filesystem;

initialiseSingleton(Skins);

extern path ThemePath;

Skins::Skins()
	: SkinColor(Color::Blue)
{
	LoadList();
}

void Skins::LoadList()
{
	directory_iterator end;

	try
	{
		// Clear the existing skins list.
		_skins.clear();
		_skinThemeMap.clear();

		for (directory_iterator itr(ThemePath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (!is_directory(p)
				|| p.filename() == _T(".")
				|| p.filename() == _T(".."))
				continue;

			ParseDir(p);
		}
	}
	catch (filesystem_error)
	{
		sLog.Critical(_T("Skins::LoadList"), _T("Could not access themes directory %s."), 
			ThemePath.native().c_str());
	}
}

void Skins::ParseDir(const path& dir)
{
	directory_iterator end;
	for (directory_iterator itr(dir); itr != end; ++itr)
	{
		const path& p = itr->path();
		if (is_directory(p)
			|| !p.has_extension()
			|| p.extension() != _T(".ini"))
			continue;

		LoadHeader(p);
	}
}

void Skins::LoadHeader(const path& iniFile)
{
	CSimpleIni ini(true);
	SI_Error result = ini.LoadFile(iniFile.native().c_str());
	if (result != SI_OK)
	{
		return sLog.Warn(_T("Skins::LoadHeader"), _T("Failed to load INI (%s)."), 
			iniFile.native().c_str());
	}

	SkinEntry skin;
	const TCHAR * creator;

	skin.Path			= iniFile.branch_path();
	skin.FileName		= iniFile;
	skin.Theme			= ini.GetValue(_T("Skin"), _T("Theme"), _T(""));
	skin.Name			= ini.GetValue(_T("Skin"), _T("Name"), _T(""));

	// Attempt to lookup skin's creator.
	// NOTE: Current USDX loads "Creator", but older skins use "Author".
	// Attempt to fallback to "Author" if "Creator" isn't found.
	creator				= ini.GetValue(_T("Skin"), _T("Creator"));
	if (creator == NULL)
		creator			= ini.GetValue(_T("Skin"), _T("Author"), _T(""));

	skin.Creator		= creator;

	// Parse color names (stored as, for example, "Blue")
	skin.DefaultColor	= LOOKUP_ENUM_VALUE(Color, _T("Skin"), _T("Color"), Color::Blue);

	_skins.insert(std::make_pair(skin.Name, skin));
	if (!skin.Theme.empty())
		_skinThemeMap.insert(std::make_pair(skin.Theme, skin.Name));
}

void Skins::LoadSkin(SkinEntry * skin, eColor color)
{
	CSimpleIni ini(true);

	SI_Error result = ini.LoadFile(skin->FileName.native().c_str());
	if (result != SI_OK)
	{
		return sLog.Warn(_T("Skins::LoadSkin"), _T("Failed to load INI (%s)."), 
			skin->FileName.c_str());
	}

	const TCHAR * section = _T("Textures");
	
	SkinColor = color;
	SkinPath = skin->Path;

	const CSimpleIni::TKeyVal * sectionKeys = ini.GetSection(section);
	if (sectionKeys == NULL)
		return;

	for (CSimpleIni::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
		SkinTextures.insert(std::make_pair(itr->first.pItem, SkinPath / path(itr->second)));
}

SkinEntry* Skins::LookupSkinForTheme(const tstring& themeName)
{
	SkinThemeMap::const_iterator itr = _skinThemeMap.find(themeName);
	if (itr == _skinThemeMap.end())
		return NULL;

	return LookupSkinForTheme(itr->second, themeName);
}

SkinEntry* Skins::LookupSkinForTheme(const tstring& skinName, const tstring& themeName)
{
	std::pair<SkinEntryMap::iterator, SkinEntryMap::iterator> range
		= _skins.equal_range(skinName);

	for (SkinEntryMap::const_iterator itr = range.first; itr != range.second; ++itr)
	{
		if (itr->second.Theme == themeName)
			return (SkinEntry *)&itr->second;
	}

	return NULL;
}

const path* Skins::GetTextureFileName(const tstring& textureName)
{
	SkinTextureMap::const_iterator itr = SkinTextures.find(textureName);
	return (itr == SkinTextures.end() ? NULL : &itr->second);
}

const path* Skins::GetTextureFileName(const TCHAR * fmt, ...)
{
	TCHAR buffer[1024];
	tstring filename;
	va_list args;
	va_start(args, fmt);
	_vsntprintf(buffer, 1024, fmt, args);
	va_end(args);

	filename = buffer;
	return GetTextureFileName(filename);
}

Skins::~Skins()
{
}
