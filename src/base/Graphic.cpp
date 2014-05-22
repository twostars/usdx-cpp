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
#include "Graphic.h"
#include "PathUtils.h"
#include "Log.h"
#include "CommandLine.h"
#include "Ini.h"
#include "TextGL.h"

#include "../menu/Display.h"
#include "../menu/Menu.h"

#include "../screens/screens.h"

#define WINDOW_ICON _T("ultrastardx-icon.png")

using namespace boost::filesystem;

extern path ResourcesPath;
extern CMDParams Params;

// TODO: Replace these globals...
SDL_Window  * Screen = NULL;
SDL_GLContext GLContext;

SurfaceCollection g_surfaces;

// Virtual screen size
int RenderW = 800, RenderH = 600;

// Actual display size
int ScreenW, ScreenH;

// Display count
int Screens, ScreenAct, ScreenX;

bool Fullscreen;

// Screens
ScreenLoading			* UILoading = NULL;
ScreenMain				* UIMain = NULL;
ScreenName				* UIName = NULL;
ScreenLevel				* UILevel = NULL;
ScreenSong				* UISong = NULL;
ScreenSing				* UISing = NULL;
ScreenScore				* UIScore = NULL;
ScreenTop5				* UITop5 = NULL;
ScreenOptions			* UIOptions = NULL;
ScreenOptionsGame		* UIOptionsGame = NULL;
ScreenOptionsGraphics	* UIOptionsGraphics = NULL;
ScreenOptionsSound		* UIOptionsSound = NULL;
ScreenOptionsLyrics		* UIOptionsLyrics = NULL;
ScreenOptionsThemes		* UIOptionsThemes = NULL;
ScreenOptionsRecord		* UIOptionsRecord = NULL;
ScreenOptionsAdvanced	* UIOptionsAdvanced = NULL;
ScreenEditSub			* UIEditSub = NULL;
ScreenEdit				* UIEdit = NULL;
ScreenEditConvert		* UIEditConvert = NULL;
ScreenEditHeader		* UIEditHeader = NULL;
ScreenOpen				* UIOpen = NULL;

ScreenSongMenu			* UISongMenu = NULL;
ScreenSongJumpTo		* UISongJumpTo = NULL;

// Party screens
ScreenPartyNewRound		* UIPartyNewRound = NULL;
ScreenPartyScore		* UIPartyScore = NULL;
ScreenPartyWin			* UIPartyWin = NULL;
ScreenPartyOptions		* UIPartyOptions = NULL;
ScreenPartyPlayer		* UIPartyPlayer = NULL;
ScreenPartyRounds		* UIPartyRounds = NULL;

// Stats screens
ScreenStatMain			* UIStatMain = NULL;
ScreenStatDetail		* UIStatDetail = NULL;

// Credits screen
ScreenCredits			* UICredits = NULL;

// Popups
ScreenPopupCheck		* UIPopupCheck = NULL;
ScreenPopupError		* UIPopupError = NULL;
ScreenPopupInfo			* UIPopupInfo = NULL;

typedef std::set<Menu *> ScreenCollection;
ScreenCollection		g_screenCollection;

PFNGLCOPYTEXSUBIMAGE3DPROC	glCopyTexSubImage3D;
PFNGLDRAWRANGEELEMENTSPROC	glDrawRangeElements;
PFNGLTEXIMAGE3DPROC			glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC		glTexSubImage3D;

static const struct SDL_PixelFormat PixelFmt_RGBA =
{
	/*format:*/         SDL_PIXELFORMAT_RGBA8888,
    /*palette:*/        NULL,
    /*BitsPerPixel:*/   32,
    /*BytesPerPixel:*/  4,
	/*padding[2]:*/     {0,0},
    /*Rmask:*/           0x000000ff,
    /*Gmask:*/           0x0000ff00,
    /*Bmask:*/           0x00ff0000,
    /*Amask:*/           0xff000000,
    /*Rloss:*/           0,
    /*Gloss:*/           0,
    /*Bloss:*/           0,
    /*Aloss:*/           0,
    /*Rshift:*/          0,
    /*Gshift:*/          8,
    /*Bshift:*/          16,
    /*Ashift:*/          24,
	/*refcount:*/        0,
	/*next:*/            NULL
};

static const struct SDL_PixelFormat PixelFmt_RGB =
{
	/*format:*/         SDL_PIXELFORMAT_RGB888,
    /*palette:*/        NULL,
    /*BitsPerPixel:*/   24,
    /*BytesPerPixel:*/  3,
	/*padding[2]:*/     {0,0},
    /*Rmask:*/           0x000000ff,
    /*Gmask:*/           0x0000ff00,
    /*Bmask:*/           0x00ff0000,
    /*Amask:*/           0x00000000,
    /*Rloss:*/           0,
    /*Gloss:*/           0,
    /*Bloss:*/           0,
    /*Aloss:*/           0,
    /*Rshift:*/          0,
    /*Gshift:*/          8,
    /*Bshift:*/          16,
    /*Ashift:*/          0,
	/*refcount:*/        0,
	/*next:*/            NULL
};

static const struct SDL_PixelFormat PixelFmt_BGRA =
{
	/*format:*/         SDL_PIXELFORMAT_BGRA8888,
    /*palette:*/        NULL,
    /*BitsPerPixel:*/   32,
    /*BytesPerPixel:*/  4,
	/*padding[2]:*/     {0,0},
    /*Rmask:*/           0x000000ff,
    /*Gmask:*/           0x0000ff00,
    /*Bmask:*/           0x00ff0000,
    /*Amask:*/           0xff000000,
    /*Rloss:*/           0,
    /*Gloss:*/           0,
    /*Bloss:*/           0,
    /*Aloss:*/           0,
    /*Rshift:*/          16,
    /*Gshift:*/          8,
    /*Bshift:*/          0,
    /*Ashift:*/          24,
	/*refcount:*/        0,
	/*next:*/            NULL
};

static const struct SDL_PixelFormat PixelFmt_BGR =
{
	/*format:*/         SDL_PIXELFORMAT_BGR888,
    /*palette:*/        NULL,
    /*BitsPerPixel:*/   24,
    /*BytesPerPixel:*/  3,
	/*padding[2]:*/     {0,0},
    /*Rmask:*/           0x00ff0000,
    /*Gmask:*/           0x0000ff00,
    /*Bmask:*/           0x000000ff,
    /*Amask:*/           0x00000000,
    /*Rloss:*/           0,
    /*Gloss:*/           0,
    /*Bloss:*/           0,
    /*Aloss:*/           0,
    /*Rshift:*/          16,
    /*Gshift:*/          8,
    /*Bshift:*/          0,
    /*Ashift:*/          0,
	/*refcount:*/        0,
	/*next:*/            NULL
};

enum ImagePixelFormat { ipfRGBA, ipfRGB, ipfBGRA, ipfBGR };

void Initialize3D(const TCHAR * windowTitle)
{
	char * utf8Title;
	SDL_Surface * icon;

	// Set window title to applicable
	sLog.Status(_T("SDL_InitSubSystem"), _T("Initialize3D"));

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		return sLog.Critical(_T("Initialize3D"), _T("SDL_InitSubSystem() failed."));

	// Set screen/display count
	if (Params.Screens > 0)
		Screens = Params.Screens;
	else
		Screens = sIni.Screens + 1;

	// Load resolution
	ResolutionWH resolution;
	if (Params.Resolution.empty()
		|| !ExtractResolution(Params.Resolution, &resolution.first, &resolution.second))
		resolution = sIni.Resolution;

	// Handle width adjustment for multiple displays setup horizontally.
	if (Screens > 1)
		resolution.first *= Screens; /* assume they're spread out horizontally... */

	// Specify fullscreen mode
	Fullscreen = (Params.ScreenMode != scmWindowed) || (sIni.FullScreen == Switch::On);

	sLog.Status(_T("Initialize3D"), _T("SDL_CreateWindow (%s)"), Fullscreen ? _T("fullscreen") : _T("windowed"));
	uint32 flags = SDL_WINDOW_OPENGL;
	if (Fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;
	else
		flags |= SDL_WINDOW_RESIZABLE;

	utf8Title = StringToUTF8(windowTitle);
	Screen = SDL_CreateWindow(utf8Title, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		0, 0, 
		flags
	);
	SDL_free(utf8Title);

	if (Screen == NULL)
		return sLog.Critical(_T("Initialize3D"), _T("SDL_CreateWindow() failed."));

	icon = LoadSurfaceFromFile(ResourcesPath / ICONS_DIR / WINDOW_ICON);
	SDL_SetWindowIcon(Screen, icon);

	// Create an OpenGL context
	GLContext = SDL_GL_CreateContext(Screen);

	// Set minimum color component sizes
	// Note: do not request an alpha plane with SDL_GL_ALPHA_SIZE here as
	// some cards/implementations do not support them (SDL_SetVideoMode fails).
	// We do not the alpha plane anymore since offscreen rendering in back-buffer
	// was removed.
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,      5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,    5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,     5);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,    16); // Z-Buffer depth
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,  1);

	// Hide cursor
	SDL_ShowCursor(0);

	// Load extensions
	LoadOpenGLExtensions();

	ScreenW = resolution.first;
	ScreenH = resolution.second;

	// Clear screen once window is shown
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers();

	new Display();

	sLog.Status(_T("Initialize3D"), _T("Loading font textures"));
	LoadFontTextures();

	sLog.Status(_T("Initialize3D"), _T("Loading load screen"));
	LoadLoadingScreen();

	sLog.Status(_T("Initialize3D"), _T("Loading textures"));
	LoadTextures();

	sLog.Status(_T("Initialize3D"), _T("Loading screens"));
	LoadScreens();

	// TODO:
	// Here should be a loop which
	// * draws the loading screen (form time to time)
	// * controlls the "process of the loading screen"
	// * checks if the loadingthread has loaded textures (check mutex) and
	//   * load the textures into opengl
	//   * tells the loadingthread, that the memory for the texture can be reused
	//     to load the next texture (over another mutex)
	// * runs as long as the loadingthread tells, that everything is loaded and ready (using a third mutex)
	//
	// therefore LoadTextures() has to be changed, instead of caling some opengl functions
	// for itself, it should change mutex
	// the mainthread have to know somehow what opengl function have to be called with which parameters like
	// texturetype, textureobject, texture-buffer-adress, ...
	
	assert(sDisplay.CurrentScreen != NULL);
	sDisplay.CurrentScreen->FadeTo(UIMain);

	sLog.BenchmarkEnd(2);
	sLog.Benchmark(2, _T("Loading screens"));

	sLog.Status(_T("Initialize3D"), _T("Finished"));
}

class OpenGL12
{
public:
	static bool Load()
	{
		sLog.Status(_T("OpenGL"), _T("Loading OpenGL 1.2 extensions..."));

		glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) SDL_GL_GetProcAddress("glCopyTexSubImage3D");
		glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) SDL_GL_GetProcAddress("glDrawRangeElements");
		glTexImage3D        = (PFNGLTEXIMAGE3DPROC       ) SDL_GL_GetProcAddress("glTexImage3D");
		glTexSubImage3D     = (PFNGLTEXSUBIMAGE3DPROC    ) SDL_GL_GetProcAddress("glTexSubImage3D");

		if (glCopyTexSubImage3D == NULL
			|| glDrawRangeElements == NULL
			|| glTexImage3D == NULL
			|| glTexSubImage3D == NULL)
			return false;

		sLog.Status(_T("OpenGL"), _T("Loaded OpenGL 1.2 extensions."));
		return true;
	}
};

void LoadOpenGLExtensions()
{
	if (!OpenGL12::Load())
		return sLog.Critical(_T("LoadOpenGLExtensions"), _T("Unable to load OpenGL 1.2"));
}

void LoadFontTextures()
{
	sLog.Status(_T("LoadFontTextures"), _T("Building fonts"));
	BuildFonts();
}

void UnloadFontTextures()
{
	sLog.Status(_T("UnloadFontTextures"), _T("Killing fonts"));
	KillFonts();
}

#define LOAD_SCREEN(name) \
	LoadScreen(UI ## name, _T(#name))

template <typename T>
void LoadScreen(T *& p, TCHAR * name)
{
	assert(p == NULL);
	sLog.BenchmarkStart(3);
	sLog.Benchmark(3, _T("====> Screen %s"), name);
	p = new T();
	sLog.BenchmarkEnd(3);

	// Add screen to collection.
	g_screenCollection.insert(p);
}

void LoadLoadingScreen()
{
	assert(UILoading == NULL);

	LOAD_SCREEN(Loading);
	UILoading->OnShow();

	sDisplay.CurrentScreen = UILoading;
	SwapBuffers();

	// Is this necessary? Original code does this, but Display::Draw() should call it appropriately..
	// UILoading->Draw();

	sDisplay.Draw();
	SwapBuffers();
}

void LoadTextures()
{
	// TODO
}

void LoadScreens()
{
	LOAD_SCREEN(Main);
	LOAD_SCREEN(Name);
	LOAD_SCREEN(Level);
	LOAD_SCREEN(Song);
	LOAD_SCREEN(SongMenu);
	LOAD_SCREEN(Sing);
	LOAD_SCREEN(Score);
	LOAD_SCREEN(Top5);
	LOAD_SCREEN(Options);
	LOAD_SCREEN(OptionsGame);
	LOAD_SCREEN(OptionsGraphics);
	LOAD_SCREEN(OptionsSound);
	LOAD_SCREEN(OptionsLyrics);
	LOAD_SCREEN(OptionsThemes);
	LOAD_SCREEN(OptionsRecord);
	LOAD_SCREEN(OptionsAdvanced);
	LOAD_SCREEN(EditSub);
	LOAD_SCREEN(Edit);
	LOAD_SCREEN(EditConvert);
	LOAD_SCREEN(Open);
	LOAD_SCREEN(SongJumpTo);
	LOAD_SCREEN(PopupCheck);
	LOAD_SCREEN(PopupError);
	LOAD_SCREEN(PopupInfo);
	LOAD_SCREEN(PartyNewRound);
	LOAD_SCREEN(PartyScore);
	LOAD_SCREEN(PartyWin);
	LOAD_SCREEN(PartyOptions);
	LOAD_SCREEN(PartyPlayer);
	LOAD_SCREEN(PartyRounds);
	LOAD_SCREEN(StatMain);
	LOAD_SCREEN(StatDetail);
	LOAD_SCREEN(Credits);
}

void SwapBuffers()
{
	SDL_GL_SwapWindow(Screen);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, RenderW, RenderH, 0, -1, 100);
	glMatrixMode(GL_MODELVIEW);
}

SDL_Surface * LoadSurfaceFromFile(const path& filename)
{
	if (!filename.has_extension()
		// need at least 2 characters, as we need to skip the . prefix
		|| filename.extension().native().size() < 2)
	{
		sLog.Error(_T("LoadSurfaceFromFile"), _T("%s has no extension."), filename.native().c_str());
		return NULL;
	}

	FILE * fp = _tfopen(filename.c_str(), _T("rb"));
	if (fp == NULL)
	{
		sLog.Error(_T("LoadSurfaceFromFile"), _T("fopen() failed on %s (file not found, or no permission to access)."), filename.native().c_str());
		return NULL;
	}

	const char * ext = filename.extension().generic_string().c_str() + 1;
	SDL_RWops * src = SDL_RWFromFP(fp, SDL_TRUE);

	SDL_Surface * result = IMG_LoadTyped_RW(src, 1, ext);
	if (result != nullptr)
		g_surfaces.insert(result);

	return result;
}

void UnloadSurface(SDL_Surface * texSurface)
{
	if (texSurface == NULL)
		return;

	g_surfaces.erase(texSurface);
	SDL_FreeSurface(texSurface);
}

void AdjustPixelFormat(SDL_Surface*& texSurface, eTextureType textureType)
{
	if (texSurface == NULL)
		return;

	const SDL_PixelFormat * pixelFormat = NULL;
	switch (textureType)
	{
		case TextureType::Transparent:
		case TextureType::Colorized:
			pixelFormat = &PixelFmt_RGBA;
			break;

		default:
			pixelFormat = &PixelFmt_RGB;
			break;
	}

	if (!PixelFormatEquals(texSurface->format, pixelFormat))
	{
		SDL_Surface * tempSurface = texSurface;
		texSurface = SDL_ConvertSurface(tempSurface, pixelFormat, SDL_SWSURFACE);

		// Remove old surface from collection & free it from memory
		UnloadSurface(tempSurface);

		// Insert new surface to collection
		g_surfaces.insert(texSurface);
	}
}

bool PixelFormatEquals(SDL_PixelFormat * fmt1, const SDL_PixelFormat * fmt2)
{
	assert(fmt1 != NULL);
	assert(fmt2 != NULL);

	/*	couldn't we just check the declared pixel format? 
		it's not like we use special pixel formats... just stock RGB/RGBA/BGR/BGRA...
	*/
	return (fmt1->BitsPerPixel == fmt2->BitsPerPixel
			&& fmt1->BytesPerPixel == fmt2->BytesPerPixel
			&& fmt1->Rloss == fmt2->Rloss
			&& fmt1->Gloss == fmt2->Gloss
			&& fmt1->Bloss == fmt2->Bloss
			&& fmt1->Rmask == fmt2->Rmask
			&& fmt1->Gmask == fmt2->Gmask
			&& fmt1->Bmask == fmt2->Bmask
			&& fmt1->Rshift == fmt2->Rshift
			&& fmt1->Gshift == fmt2->Gshift
			&& fmt1->Bshift == fmt2->Bshift);
}

void ScaleImage(SDL_Surface * imgSurface, uint32 width, uint32 height)
{
/*
	// TODO
	SDL_Surface * tempSurface = imgSurface;
	imgSurface = SDL_ScaleSurfaceRect(tempSurface,
                  0, 0, tempSurface->w, tempSurface->h,
                  width, height);
	UnloadSurface(tempSurface);
	g_surfaces.insert(imgSurface);
*/
}

void FitImage(SDL_Surface *& imgSurface, uint32 width, uint32 height)
{
	SDL_Surface * tempSurface = imgSurface;
	SDL_PixelFormat * imgFmt;

	// create a new surface with given width and height
	imgFmt = tempSurface->format;
	imgSurface = SDL_CreateRGBSurface(
		SDL_SWSURFACE, width, height, imgFmt->BitsPerPixel,
		imgFmt->Rmask, imgFmt->Gmask, imgFmt->Bmask, imgFmt->Amask);

	// copy image from temp- to new surface
	SDL_SetSurfaceAlphaMod(imgSurface, 255);
	SDL_SetSurfaceAlphaMod(tempSurface, 255);
	SDL_BlitSurface(tempSurface, NULL, imgSurface, NULL);

	UnloadSurface(tempSurface);
	g_surfaces.insert(imgSurface);
}

void ColorizeImage(SDL_Surface * imgSurface, uint32 newColor)
{
	// TODO
}

void glColorRGB(const RGB& color)
{
	glColor3f(color.R, color.G, color.B);
}

void glColorRGB(const RGB& color, float alpha)
{
	glColor4f(color.R, color.G, color.B, alpha);
}

void glColorRGB(const RGBA& color)
{
	glColor4f(color.R, color.G, color.B, color.A);
}

void glColorRGB(const RGBA& color, float alpha)
{
	glColor4f(color.R, color.G, color.B, std::min(color.A, alpha));
}

void glColorRGBInt(const RGB& color, float intensity)
{
	glColor3f(color.R * intensity, color.G * intensity, color.B * intensity);
}

void glColorRGBInt(const RGB& color, float alpha, float intensity)
{
	glColor4f(color.R * intensity, color.G * intensity, color.B * intensity, alpha);
}

void glColorRGBInt(const RGBA& color, float intensity)
{
	glColor4f(color.R * intensity, color.G * intensity, color.B * intensity, color.A);
}

void glColorRGBInt(const RGBA& color, float alpha, float intensity)
{
	glColor4f(color.R * intensity, color.G * intensity, color.B * intensity, std::min(color.A, alpha));
}

void FreeGfxResources()
{
	for (SurfaceCollection::const_iterator itr = g_surfaces.begin(); itr != g_surfaces.end(); ++itr)
		UnloadSurface(*itr);
	g_surfaces.clear();

	for (ScreenCollection::const_iterator itr = g_screenCollection.begin(); itr != g_screenCollection.end(); ++itr)
		delete (*itr);
	g_screenCollection.clear();

	if (Screen != NULL)
	{
		SDL_DestroyWindow(Screen);
		SDL_GL_DeleteContext(GLContext);
	}

	delete Display::getSingletonPtr();
}
