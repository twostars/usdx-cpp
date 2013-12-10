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

#include <SDL.h>

#include "Main.h"
#include "Platform.h"
#include "Config.h"
#include "Common.h"

#include "../lua/LuaCore.h"
#include "Time.h"
#include "RelativeTimer.h"
#include "CommandLine.h"
#include "Log.h"
#include "Language.h"
#include "Skins.h"
#include "Themes.h"

/* globals */
// TODO: Clean these up

Time USTime;
RelativeTimer VideoBGTimer;
CMDParams Params;

int usdxMain(int argc, TCHAR ** argv)
{
	try
	{
		const TCHAR * windowTitle = USDXVersionStr();

		if (Platform::TerminateIfAlreadyRunning(windowTitle))
			return 1;

		// fix the locale for string-to-float parsing in C-libs
		Common::SetDefaultNumericLocale();

		// initialize SDL
		// without SDL_INIT_TIMER SDL_GetTicks() might return strange values
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

		// create LuaCore first so other classes can register their events
		new LuaCore();

		// load the command-line arguments
		Params.Load(argc, argv);

		// Setup the logger/benchmarker
		new Log();
		sLog.BenchmarkStart(0);

		// Language
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Initialize Paths"), _T("Initialization"));
		InitializePaths();
		sLog.Status(_T("Load Language"), _T("Initialization"));
		new Language();

		// Add const values
		sLanguage.AddConst(_T("US_VERSION"), USDXVersionStr());
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Language"));

		// Skins
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Loading Skin List"), _T("Initialization"));
		new Skins();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Skin List"));

		// Themes
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Loading Theme List"), _T("Initialization"));
		new Themes();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Theme List"));
	}
	catch (CriticalException& e)
	{
		_tprintf(_T("Critical exception occurred: %s\n"), e.twhat());
	}
	catch (...)
	{
		_tprintf(_T("Unhandled exception occurred\n"));
	}

	delete Skins::getSingletonPtr();
	delete Language::getSingletonPtr();
	delete Log::getSingletonPtr();
	delete LuaCore::getSingletonPtr();

	SDL_Quit();

	return 0;
}
