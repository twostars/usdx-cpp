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

#include "../menu/Display.h"

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

PFNGLCOPYTEXSUBIMAGE3DPROC	glCopyTexSubImage3D;
PFNGLDRAWRANGEELEMENTSPROC	glDrawRangeElements;
PFNGLTEXIMAGE3DPROC			glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC		glTexSubImage3D;

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

	// TODO
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
	g_surfaces.insert(result);

	return result;
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
		SDL_FreeSurface(*itr);

	if (Screen != NULL)
	{
		SDL_DestroyWindow(Screen);
		SDL_GL_DeleteContext(GLContext);
	}

	delete Display::getSingletonPtr();
}
