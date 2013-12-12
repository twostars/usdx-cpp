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
#include "LuaCore.h"

initialiseSingleton(LuaCore);

LuaCore::LuaCore()
{
}

void LuaCore::LoadPlugins()
{
	// TODO
}

void LuaCore::BrowseDir(TCHAR * directory)
{
	// TODO
}

void LuaCore::LoadPlugin(TCHAR * filename)
{
	// TODO
}

void LuaCore::DumpPlugins()
{
	// TODO
}

LuaPlugin * LuaCore::GetPluginById(int id)
{
	// TODO
	return NULL;
}

/* this function adds a module loader for your functions
    moduleName is the name the script needs to write in its require()
    functionList is an array of lua calling compatible functions.
	*/
void LuaCore::RegisterModule(const TCHAR * moduleName, luaL_Reg * functionList)
{
}

LuaCore::~LuaCore()
{
}
