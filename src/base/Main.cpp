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
#include "TextureMgr.h"
#include "Database.h"

#include "../menu/Display.h"
#include "../menu/Menu.h"

#include "../screens/ScreenPopup.h"

void CheckEvents(float & mouseX, float & mouseY);
void OnKeyDownEvent(SDL_Keycode keyCode);
void OnMouseEvent(int mouseBtn, bool mouseDown, float mouseX, float mouseY);
void OnTextInputEvent(SDL_Event * event);
void DoQuit();

/* globals */
// TODO: Clean these up

Time USTime;
RelativeTimer VideoBGTimer;
CMDParams Params;

const int MAINTHREAD_EXEC_EVENT = SDL_USEREVENT + 2;

int usdxMain(int argc, char ** argv)
{
	try
	{
		const char * windowTitle = USDXVersionStr();

		if (Platform::TerminateIfAlreadyRunning(windowTitle))
			return 1;

		// fix the locale for string-to-float parsing in C-libs
		Common::SetDefaultNumericLocale();

		// initialize SDL
		// without SDL_INIT_TIMER SDL_GetTicks() might return strange values
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1)
			return 1;

		// initialize SDL_mixer
		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1)
			return 1;

		SDL_StartTextInput();

		// create LuaCore first so other classes can register their events
		new LuaCore();

		// load the command-line arguments
		Params.Load(argc, argv);

		// Setup the logger/benchmarker
		new Log();
		sLog.BenchmarkStart(0);

		// Setup the texture manager
		new TextureMgr();

		// Language
		sLog.BenchmarkStart(1);
		sLog.Status("Initialize Paths", "Initialization");
		InitializePaths();
		sLog.Status("Load Language", "Initialization");
		new Language();

		// Add const values
		sLanguage.AddConst("US_VERSION", USDXVersionStr());
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Language");

		// Skins
		sLog.BenchmarkStart(1);
		sLog.Status("Loading Skin List", "Initialization");
		new Skins();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Skin List");

		// Themes
		sLog.BenchmarkStart(1);
		sLog.Status("Loading Theme List", "Initialization");
		new Themes();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Theme List");

		// INI file
		sLog.BenchmarkStart(1);
		sLog.Status("Loading INI", "Initialization");
		new Ini();
		sIni.Load();

		// Set the language.
		// NOTE: Command-line takes precedence.
		if (!Params.LanguageName.empty())
			sLanguage.ChangeLanguage(Params.LanguageName);
		else
			sLanguage.ChangeLanguage(sIni.LanguageName);
		
		// It is possible that this is the first run, so create an .ini file if necessary.
		sLog.Status("Writing INI", "Initialization");
		sIni.Save();

		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading INI");

		// Sound
		sLog.BenchmarkStart(1);
		sLog.Status("Initialize Sound", "Initialization");
		new SoundLibrary();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Initializing Sound");

		// Lyrics engine with media reference timer
		// new LyricsState();
		
		// Theme
		sLog.BenchmarkStart(1);
		sLog.Status("Load Theme", "Initialization");
		sThemes.LoadTheme(sIni.Theme, sIni.ThemeColor);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Theme");

		// Covers cache
		sLog.BenchmarkStart(1);
		sLog.Status("Creating Covers cache", "Initialization");
		// new Covers();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Covers cache");

		// Category covers
		sLog.BenchmarkStart(1);
		sLog.Status("Creating Category Covers array", "Initialization");
		// new CatCovers();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Category Covers array");

		// Songs
		sLog.BenchmarkStart(1);
		sLog.Status("Creating song array", "Initialization");
		// new Songs();
		sLog.Status("Creating 2nd song array", "Initialization");
		// new CatSongs();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading songs");

		// Graphics
		sLog.BenchmarkStart(1);
		sLog.Status("Initialize 3D", "Initialization");
		Initialize3D(windowTitle);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Initializing 3D");

		// Score saving
		sLog.BenchmarkStart(1);
		sLog.Status("Loading database", "Initialization");
		new Database();
		if (ScoreFile.empty())
		{
			Platform::GetGameUserPath(&ScoreFile);
			ScoreFile /= "Ultrastar.db";
		}
		sDatabase.Init(ScoreFile);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading database");

		// Playlist manager
		sLog.BenchmarkStart(1);
		sLog.Status("Playlist manager", "Initialization");
		// new PlaylistManager();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading playlist manager");

		// Effect manager (for the twinkling golden stars and such)
		sLog.BenchmarkStart(1);
		sLog.Status("Effect manager", "Initialization");
		// new EffectManager();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading effect manager");

		// Joypad
		if (sIni.Joypad || Params.Joypad)
		{
			sLog.BenchmarkStart(1);
			sLog.Status("Joystick", "Initialization");
			// new Joystick();
			sLog.BenchmarkEnd(1);
			sLog.Benchmark(1, "Loading joystick");
		}

		// Party manager
		sLog.BenchmarkStart(1);
		sLog.Status("Party manager", "Initialization");
		// new PartyGame();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading party manager");

		// Lua
		sLog.BenchmarkStart(1);
		// sLuaCore.RegisterModule("Log",        LuaLog_Lib_f);
		// sLuaCore.RegisterModule("Gl",         LuaGl_Lib_f);
		// sLuaCore.RegisterModule("TextGl",     LuaTextGl_Lib_f);
		// sLuaCore.RegisterModule("Party",      LuaParty_Lib_f);
		// sLuaCore.RegisterModule("ScreenSing", LuaScreenSing_Lib_f);
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Initializing LuaCore");

		// Lua plugins
		sLog.BenchmarkStart(1);
		sLuaCore.LoadPlugins();
		sLog.BenchmarkEnd(1);
		sLog.Benchmark(1, "Loading Lua plugins");
		sLuaCore.DumpPlugins();

		sLog.BenchmarkEnd(0);
		sLog.Benchmark(0, "Loading time");

		// Prepare software cursor
		sDisplay.SetCursor();

		// Start background music
		sSoundLib.StartBgMusic();
		
		// Check microphone settings, go to record options if they are incorrect
		/*
		int badPlayer = AudioInputProcessor::ValidateSettings();
		if (badPlayer >= 0)
		{
			ScreenPopupError::ShowPopup("ERROR_PLAYER_DEVICE_ASSIGNMENT", BadPlayer + 1);
			sDisplay.CurrentScreen->FadeTo(&ScreenOptionsRecord);
		}
		*/

		// Start main loop
		sLog.Status("Main loop", "Initialization");
		usdxMainLoop();
	}
	catch (const CriticalException& e)
	{
		printf("Critical exception occurred: %s\n", e.what());
	}
	catch (const std::exception& e)
	{
		printf("Unhandled exception occurred: %s\n", e.what());
	}
	catch (...)
	{
		printf("Unhandled exception occurred.\n");
	}

	FreeGfxResources();

	// delete PartyGame::getSingletonPtr();
	// delete Joystick::getSingletonPtr();
	// delete EffectManager::getSingletonPtr();
	// delete PlaylistManager::getSingletonPtr();
	delete Database::getSingletonPtr();
	// delete CatSongs::getSingletonPtr();
	// delete Songs::getSingletonPtr();
	// delete CatCovers::getSingletonPtr();
	// delete Covers::getSingletonPtr();
	// delete LyricsState::getSingletonPtr();
	delete Ini::getSingletonPtr();
	delete Themes::getSingletonPtr();
	delete Skins::getSingletonPtr();
	delete Language::getSingletonPtr();
	delete TextureMgr::getSingletonPtr();
	delete Log::getSingletonPtr();
	delete LuaCore::getSingletonPtr();
	delete SoundLibrary::getSingletonPtr();

	SDL_StopTextInput();
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}

void usdxMainLoop()
{
	/* NOTE: These must be floats to use accurate float division instead of inaccurate integer division. */
	const float MillisecondsInSecond = 1000.0f;
	const float MaxFPS = 100.0f;

	bool done = false;
	Uint32 ticksCurrent, ticksBeforeFrame;
	Sint32 delay;
	float mouseX = 0.0f, mouseY = 0.0f;

	// For some reason this seems to be needed with the SDL timer functions.
	CountSkipTime();

	do
	{
		ticksBeforeFrame = SDL_GetTicks();

		// Do we have a joypad?
		// if (Joystick::getSingletonPtr() != NULL)
		//	sJoystick.Update();

		// Check keyboard events
		CheckEvents(mouseX, mouseY);

		// Display
		done = !sDisplay.Draw();
		SwapBuffers();
		
		// FPS limiter
		ticksCurrent = SDL_GetTicks();
		delay = (Sint32)(MillisecondsInSecond / MaxFPS) - (ticksCurrent - ticksBeforeFrame);
		if (delay > 0)
			SDL_Delay(delay); // dynamic, maximum is 100 fps

		CountSkipTime();
	} while (!done);
}

void CheckEvents(float & mouseX, float & mouseY)
{
	SDL_Event event;
	bool mouseDown;
	int mouseBtn;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			DoQuit();
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_MAXIMIZED:
					if (event.window.data1 == 0
						|| event.window.data2 == 0)
						continue;

					ScreenW = event.window.data1;
					ScreenH = event.window.data2;

					SDL_SetWindowSize(Screen, ScreenW, ScreenH);
					
					glViewport(0, 0, ScreenW, ScreenH);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glOrtho(0, ScreenW, ScreenH, 0, -1, 1);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					break;
			}
			break;

		case SDL_MOUSEMOTION:
			if (sIni.Mouse)
			{
				mouseDown = false;
				mouseBtn = 0;
			}

			sDisplay.MoveCursor((float) event.button.x * RenderW * Screens / ScreenW,
				(float) event.button.y * RenderH / ScreenH);
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (sIni.Mouse)
			{
				mouseDown = (event.type == SDL_MOUSEBUTTONDOWN);
				mouseBtn = event.button.button;

				if (mouseBtn == SDL_BUTTON_LEFT || mouseBtn == SDL_BUTTON_RIGHT)
					sDisplay.OnMouseButton(mouseDown);
			}
			break;

		case SDL_WINDOWEVENT_RESIZED:
			break;

		case SDL_KEYDOWN:
			OnKeyDownEvent(event.key.keysym.sym);
			break;

		case SDL_MOUSEWHEEL:
			if (event.wheel.y == 0)
				continue;

			mouseDown = true;
			mouseBtn = (event.wheel.y < 0 ? SDL_BUTTON_WHEELDOWN : SDL_BUTTON_WHEELUP);
			break;

		case SDL_JOYAXISMOTION:
			break;

		case SDL_JOYBUTTONDOWN:
			break;

		case SDL_TEXTINPUT:
		case SDL_TEXTEDITING:
			OnTextInputEvent(&event);
			break;

		case MAINTHREAD_EXEC_EVENT:
			break;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
				mouseX = (float) event.button.x;
				mouseY = (float) event.button.y;
				OnMouseEvent(mouseBtn, mouseDown, mouseX, mouseY);
				break;

			case SDL_MOUSEWHEEL:
				OnMouseEvent(mouseBtn, mouseDown, mouseX, mouseY);
				break;
		}
	}
}

void OnKeyDownEvent(SDL_Keycode keyCode)
{
	// If there is a visible popup then let it handle input instead of the underlying screen
	// should be done in a way to be sure the topmost popup has preference (maybe error, then check)
	if (UIPopupError != NULL && UIPopupError->Visible)
		UIPopupError->ParseInput(keyCode, keyCode, true);
	else if (UIPopupInfo != NULL && UIPopupInfo->Visible)
		UIPopupInfo->ParseInput(keyCode, keyCode, true);
	else if (UIPopupCheck != NULL && UIPopupCheck->Visible)
		UIPopupCheck->ParseInput(keyCode, keyCode, true);
	// if screen wants to exit
	else if (!sDisplay.ParseInput(keyCode, keyCode, true))
		DoQuit();
}

void OnMouseEvent(int mouseBtn, bool mouseDown, float mouseX, float mouseY)
{
	// Drop input when changing screens
	if (sDisplay.NextScreen != NULL)
		return;

	if (UIPopupError != NULL && UIPopupError->Visible)
		UIPopupError->ParseMouse(mouseBtn, mouseDown, mouseX, mouseY);
	else if (UIPopupInfo != NULL && UIPopupInfo->Visible)
		UIPopupInfo->ParseMouse(mouseBtn, mouseDown, mouseX, mouseY);
	else if (UIPopupCheck != NULL && UIPopupCheck->Visible)
		UIPopupCheck->ParseMouse(mouseBtn, mouseDown, mouseX, mouseY);
	else if (!sDisplay.ParseMouse(mouseBtn, mouseDown, mouseX, mouseY))
		DoQuit();
}

void OnTextInputEvent(SDL_Event * event)
{
	// If there is a visible popup then let it handle input instead of the underlying screen
	// should be done in a way to be sure the topmost popup has preference (maybe error, then check)
	if (UIPopupError != NULL && UIPopupError->Visible)
		UIPopupError->ParseTextInput(event);
	else if (UIPopupInfo != NULL && UIPopupInfo->Visible)
		UIPopupInfo->ParseTextInput(event);
	else if (UIPopupCheck != NULL && UIPopupCheck->Visible)
		UIPopupCheck->ParseTextInput(event);
	// if screen wants to exit
	else if (!sDisplay.ParseTextInput(event))
		DoQuit();
}

void DoQuit()
{
	// If question option is enabled then show exit popup
	if (sIni.AskBeforeDel)
	{
		sDisplay.CurrentScreen->CheckFadeTo(NULL, "MSG_QUIT_USDX");
		return;
	}

	sDisplay.Fade = 0;
	sDisplay.NextScreenWithCheck = NULL;
	sDisplay.CheckOK = true;
}
