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
#include "Language.h"
#include "PathUtils.h"
#include "Log.h"

#include "../lib/simpleini/SimpleIni.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

initialiseSingleton(Language);

extern path LanguagesPath;

Language::Language()
{
	LoadList();
}

void Language::LoadList()
{
	directory_iterator end;
	tstring message; // TODO: Clean this up

	try
	{
		for (directory_iterator itr(LanguagesPath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (is_directory(p)
				|| !p.has_extension()
				|| p.extension() != _T(".ini"))
				continue;

			tstring languageName = p.filename().stem().c_str();

			// insert the new language name to the set
			_langSet.insert(languageName);
		}
	}
	catch (filesystem_error)
	{
		message = _T("Could not access ");
		message += LanguagesPath.native();
		sLog.Critical(message.c_str());
	}

	if (_langSet.empty())
		sLog.Critical(_T("Could not load any language file."));

	LanguageSet::const_iterator itr = _langSet.find(DEFAULT_LANGUAGE);
	if (itr == _langSet.end())
		sLog.Critical(_T("Default language (") DEFAULT_LANGUAGE _T(") does not exist."));

	ChangeLanguage(DEFAULT_LANGUAGE);
}

void Language::ChangeLanguage(const tstring& language)
{
	const TCHAR * sectionName = _T("Text");

	CSimpleIni ini(true);
	path iniPath(LanguagesPath);
	iniPath /= language;
	iniPath.replace_extension(_T(".ini"));

	SI_Error result = ini.LoadFile(iniPath.native().c_str());

	// TODO: Provide the language name.
	if (result != SI_OK)
		sLog.Critical(_T("Change language"), _T("Failed to change language."));

	CSimpleIni::TNamesDepend keys;
	ini.GetAllKeys(sectionName, keys);
	for (CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(); itr != keys.end(); ++itr)
		_langEntryMap[itr->pItem] = ini.GetValue(sectionName, itr->pItem);
}

void Language::AddConst(const tstring& id, const tstring& text)
{
	_langConstEntryMap[id] = text;
}

Language::~Language()
{
}
