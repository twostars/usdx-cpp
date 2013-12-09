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
#include "Time.h"

static uint32	s_timeOld = 0, s_timeNew = 0;
static float	s_timeSkip = 0.0f, s_timeMid = 0.0f;
static int64	s_timeMidTemp = 0;

Time::Time()
{
	CountSkipTimeSet();
}

void CountSkipTimeSet()
{
	s_timeNew = SDL_GetTicks();
}

void CountSkipTime()
{
	s_timeOld = s_timeNew;
	CountSkipTimeSet();
	s_timeSkip = (s_timeNew - s_timeOld) / (float) SDLCorrectionRatio;
}

void CountMidTime()
{
	s_timeMidTemp = SDL_GetTicks();
	s_timeMid = (s_timeMidTemp - s_timeNew) / (float) SDLCorrectionRatio;
}

float Time::GetTime()
{
	return SDL_GetTicks() / (float) SDLCorrectionRatio;
}
