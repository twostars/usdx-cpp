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

#include "Main.h"
#include "Platform.h"

#include "../lua/LuaCore.h"
#include "Time.h"
#include "RelativeTimer.h"
#include "CommandLine.h"
#include "Log.h"
#include "Language.h"
#include "Skins.h"
#include "Themes.h"
#include "Ini.h"
#include "Music.h"
#include "Graphic.h"

extern boost::filesystem::path ScoreFile;

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

		// INI file
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Loading INI"), _T("Initialization"));
		new Ini();
		sIni.Load();

		// Set the language.
		// NOTE: Command-line takes precedence.
		if (!Params.LanguageName.empty())
			sLanguage.ChangeLanguage(Params.LanguageName);
		else
			sLanguage.ChangeLanguage(sIni.LanguageName);
		
		// It is possible that this is the first run, so create an .ini file if necessary.
		sLog.Status(_T("Writing INI"), _T("Initialization"));
		sIni.Save();

		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading INI"));

		// Sound
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Initialize Sound"), _T("Initialization"));
		InitializeSound(); // TODO
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Initializing Sound"));

		// Lyrics engine with media reference timer
		// new LyricsState();
		
		// Theme
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Load Theme"), _T("Initialization"));
		sThemes.LoadTheme(sIni.Theme, sIni.ThemeColor);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Theme"));

		// Covers cache
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Creating Covers cache"), _T("Initialization"));
		// new Covers();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Covers cache"));

		// Category covers
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Creating Category Covers array"), _T("Initialization"));
		// new CatCovers();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Category Covers array"));

		// Songs
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Creating song array"), _T("Initialization"));
		// new Songs();
		sLog.Status(_T("Creating 2nd song array"), _T("Initialization"));
		// new CatSongs();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading songs"));

		// Graphics
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Initialize 3D"), _T("Initialization"));
		Initialize3D(windowTitle);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Initializing 3D"));

		// Score saving
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Loading database"), _T("Initialization"));
		// new Database();
		if (ScoreFile.empty())
		{
			Platform::GetGameUserPath(&ScoreFile);
			ScoreFile /= _T("Ultrastar.db");
		}
		// sDatabase.Init(ScoreFile);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading database"));

		// Playlist manager
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Playlist manager"), _T("Initialization"));
		// new PlaylistManager();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading playlist manager"));

		// Effect manager (for the twinkling golden stars and such)
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Effect manager"), _T("Initialization"));
		// new EffectManager();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading effect manager"));

		// Joypad
		if (sIni.Joypad || Params.Joypad)
		{
			sLog.BenchmarkStart(1);
			sLog.Status(_T("Joystick"), _T("Initialization"));
			// new Joystick();
			sLog.BenchmarkEnd(1);
			sLog.Benchmark(1, _T("Loading joystick"));
		}

		// Party manager
		sLog.BenchmarkStart(1);
		sLog.Status(_T("Party manager"), _T("Initialization"));
		// new PartyGame();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading party manager"));

		// Lua
		sLog.BenchmarkStart(1);
		// sLuaCore.RegisterModule(_T("Log"),        LuaLog_Lib_f);
		// sLuaCore.RegisterModule(_T("Gl"),         LuaGl_Lib_f);
		// sLuaCore.RegisterModule(_T("TextGl"),     LuaTextGl_Lib_f);
		// sLuaCore.RegisterModule(_T("Party"),      LuaParty_Lib_f);
		// sLuaCore.RegisterModule(_T("ScreenSing"), LuaScreenSing_Lib_f);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Initializing LuaCore"));

		// Lua plugins
		sLog.BenchmarkStart(1);
		sLuaCore.LoadPlugins();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, _T("Loading Lua plugins"));
		sLuaCore.DumpPlugins();

		sLog.BenchmarkEnd(0);
		sLog.Benchmark(0, _T("Loading time"));

		// Prepare software cursor
		// Display::SetCursor();

		// Start background music
		// SoundLib::StartBgMusic();
		
		// Check microphone settings, go to record options if they are incorrect
		/*
		int badPlayer = AudioInputProcessor::ValidateSettings();
		if (badPlayer >= 0)
		{
			ScreenPopupError::ShowPopup(sLanguage.TranslateFormat(_T("ERROR_PLAYER_DEVICE_ASSIGNMENT"), BadPlayer + 1);
			Display::CurrentScreen->FadeTo(&ScreenOptionsRecord);
		}
		*/

		// Start main loop
		sLog.Status(_T("Main loop"), _T("Initialization"));
		usdxMainLoop();
	}
	catch (CriticalException& e)
	{
		_tprintf(_T("Critical exception occurred: %s\n"), e.twhat());
	}
	catch (...)
	{
		_tprintf(_T("Unhandled exception occurred\n"));
	}

	// delete PartyGame::getSingletonPtr();
	// delete Joystick::getSingletonPtr();
	// delete EffectManager::getSingletonPtr();
	// delete PlaylistManager::getSingletonPtr();
	// delete Database::getSingletonPtr();
	// delete CatSongs::getSingletonPtr();
	// delete Songs::getSingletonPtr();
	// delete CatCovers::getSingletonPtr();
	// delete Covers::getSingletonPtr();
	// delete LyricsState::getSingletonPtr();
	delete Ini::getSingletonPtr();
	delete Themes::getSingletonPtr();
	delete Skins::getSingletonPtr();
	delete Language::getSingletonPtr();
	delete Log::getSingletonPtr();
	delete LuaCore::getSingletonPtr();

	FreeGfxResources();
	SDL_Quit();

	return 0;
}

void usdxMainLoop()
{
	/* NOTE: These must be floats to use accurate float division instead of inaccurate integer division. */
	const float MillisecondsInSecond = 1000.0f;
	const float MaxFPS = 100.0f;

	bool done = false;
	uint32 delay, ticksCurrent, ticksBeforeFrame;

	// For some reason this seems to be needed with the SDL timer functions.
	CountSkipTime();

	do
	{
		ticksBeforeFrame = SDL_GetTicks();

		// Do we have a joypad?
		// if (Joystick::getSingletonPtr() != NULL)
		//	sJoystick.Update();

		// Check keyboard events
		// CheckEvents();

		// Display
		// done = !Display::Draw();
		done = true; // TODO: Remove
		// SwapBuffers();
		
		// FPS limiter
		ticksCurrent = SDL_GetTicks();
		delay = (uint32)(MillisecondsInSecond / MaxFPS) - (ticksCurrent - ticksBeforeFrame);

		if (delay > 0)
			SDL_Delay(delay); // dynamic, maximum is 100 fps

		CountSkipTime();
	} while (!done);
}
