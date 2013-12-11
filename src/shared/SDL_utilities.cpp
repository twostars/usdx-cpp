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

#include <SDL.h>
#include "SDL_utilities.h"

// List applicable display modes for the specified display.
// NOTE: Unlike the original SDL_ListModes(), be sure to free the returned memory.
SDL_Rect **
SDL_ListModes(int displayIndex)
{
	int nmodes = 0, displayModeCount, currentBpp;
	SDL_Rect **modes = NULL;
	SDL_DisplayMode currentMode;

	SDL_GetCurrentDisplayMode(displayIndex, &currentMode);

	if (SDL_BYTESPERPIXEL(currentMode.format) <= 2)
		currentBpp = SDL_BITSPERPIXEL(currentMode.format);
	else
		currentBpp = SDL_BYTESPERPIXEL(currentMode.format) * 8;

	displayModeCount = SDL_GetNumDisplayModes(displayIndex);
	for (int i = 0; i < displayModeCount; i++)
	{
		SDL_DisplayMode mode;
		int bpp;

		SDL_GetDisplayMode(displayIndex, i, &mode);
		if (!mode.w || !mode.h)
			return (SDL_Rect **) (-1);

		if (SDL_BYTESPERPIXEL(mode.format) <= 2)
			bpp = SDL_BITSPERPIXEL(mode.format);
		else
			bpp = SDL_BYTESPERPIXEL(mode.format) * 8;

		if (bpp != currentBpp)
			continue;

		if (nmodes > 0 && modes[nmodes - 1]->w == mode.w
			&& modes[nmodes - 1]->h == mode.h)
			continue;

		modes = (SDL_Rect **) SDL_realloc(modes, (nmodes + 2) * sizeof(*modes));
		if (modes == NULL)
            return NULL;

		modes[nmodes] = (SDL_Rect *) SDL_malloc(sizeof(SDL_Rect));
		if (modes[nmodes] == NULL)
			return NULL;

		modes[nmodes]->x = 0;
		modes[nmodes]->y = 0;
		modes[nmodes]->w = mode.w;
		modes[nmodes]->h = mode.h;

		++nmodes;
    }

	if (modes != NULL)
		modes[nmodes] = NULL;

	return modes;
}
