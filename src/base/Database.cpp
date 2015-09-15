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
#include "Database.h"
#include "Log.h"
#include <sqlite/sqlite3.h>

initialiseSingleton(Database);

Database::Database()
{
}

bool Database::Init(const path& scorePath)
{
	try
	{
		sLog.Status(_T("Database::Init"), _T("Initializing database: '%s'"), scorePath.generic_string().c_str());

		int r = _database.Open(scorePath);
		if (r != SQLITE_OK)
		{
			sLog.Critical(_T("Database"),
				_T("Unable to open sqlite3 database: %s (%d - ") _T(ANSI_FORMAT) _T(")"),
				scorePath.c_str(), r, _database.ErrMsg());
			return false;
		}

		_database.Initialize();
		return true;
	}
	catch (const Sqlite3NoRowsException& e)
	{
		_tprintf(_T("Sqlite3NoRowsException occurred: ") _T(ANSI_FORMAT) _T("\n"), e.what());
	}
	catch (const Sqlite3Exception& e)
	{
		_tprintf(_T("Sqlite3Exception occurred: ") _T(ANSI_FORMAT) _T("\n"), e.what());
	}

	return false;
}

Database::~Database()
{
}
