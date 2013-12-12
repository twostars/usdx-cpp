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

#include <SDL_image.h>

#define WINDOW_ICON _T("ultrastardx-icon.png")

using namespace boost::filesystem;

extern path ResourcesPath;

typedef std::set<SDL_Surface *> SurfaceCollection;

SDL_Window * g_window = NULL;
SurfaceCollection g_surfaces;

void Initialize3D(const TCHAR * windowTitle)
{
	char * utf8Title;
	SDL_Surface * icon;

	// Set window title to applicable
	sLog.Status(_T("SDL_InitSubSystem"), _T("Initialize3D"));

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		return sLog.Critical(_T("Initialize3D"), _T("SDL_InitSubSystem() failed."));

	utf8Title = StringToUTF8(windowTitle);
	g_window = SDL_CreateWindow(utf8Title, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		0, 0, 
		0
	);
	free(utf8Title);

	if (g_window == NULL)
		return sLog.Critical(_T("Initialize3D"), _T("SDL_CreateWindow() failed."));

	icon = LoadSurfaceFromFile(ResourcesPath / ICONS_DIR / WINDOW_ICON);
	SDL_SetWindowIcon(g_window, icon);

	// InitializeScreen();

	// Note: do not initialize video modules earlier. They might depend on some
	// SDL video functions or OpenGL extensions initialized in InitializeScreen()
	// InitializeVideo();

	// TODO
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

void FreeGfxResources()
{
	for (SurfaceCollection::const_iterator itr = g_surfaces.begin(); itr != g_surfaces.end(); ++itr)
		SDL_FreeSurface(*itr);

	SDL_DestroyWindow(g_window);
}
