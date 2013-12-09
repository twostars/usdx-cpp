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
#include "RelativeTimer.h"
#include "Time.h"

RelativeTimer::RelativeTimer()
	: _state(rtsStopped), _absoluteTime(0), _relativeTime(0.0f), _triggerMode(false)
{
}

/**
 * Starts the timer.
 * If WaitForTrigger is false the timer will be started immediately.
 * If WaitForTrigger is true the timer will be started when a trigger event
 * occurs. A trigger event is a call of one of the Get-/SetTime() methods.
 * In addition the timer can be started by calling this method again with
 * WaitForTrigger set to false.
 */
void RelativeTimer::Start(bool WaitForTrigger /*= false*/)
{
	switch (GetState())
	{
	case rtsStopped:
	case rtsPaused:
		if (WaitForTrigger)
		{
			SetState(rtsWait);
		}
		else
		{
			SetState(rtsRunning);
			_absoluteTime = SDL_GetTicks();
		}
		break;

	case rtsWait:
		if (!WaitForTrigger)
		{
			SetState(rtsRunning);
			_absoluteTime = SDL_GetTicks();
			_relativeTime = 0.0f;
		}
		break;
	}
}

/**
 * Pauses the timer and leaves the counter untouched.
 */
void RelativeTimer::Pause()
{
	// Important: GetTime() must be called in running state
	if (GetState() != rtsRunning)
		return;

	_relativeTime = GetTime();
	SetState(rtsPaused);
}

/**
 * Stops the timer and sets its counter to 0.
 */
void RelativeTimer::Stop()
{
	if (GetState() == rtsStopped)
		return;

	SetState(rtsStopped);
	_relativeTime = 0.0f;
}

/**
 * Returns the current counter of the timer.
 * If WaitForTrigger was true in Start() the timer will be started
 * if it was not already running.
 */
float RelativeTimer::GetTime()
{
	switch (GetState())
	{
	case rtsStopped:
	case rtsPaused:
		return _relativeTime;

	case rtsRunning:
		return _relativeTime + (SDL_GetTicks() - _absoluteTime) / (float) SDLCorrectionRatio;

	// start triggered
	case rtsWait:
		SetState(rtsRunning);
		_absoluteTime = SDL_GetTicks();
		return _relativeTime;

	default: // this shouldn't occur
		SDL_assert(0);
		return 0.0f;
	}
}

/**
 * Sets the counter of the timer.
 * If WaitForTrigger was true in Start() the timer will be started
 * if it was not already running.
 */
void RelativeTimer::SetTime(float time)
{
	_relativeTime = time;
	_absoluteTime = SDL_GetTicks();

	// start triggered
	if (GetState() == rtsWait)
		SetState(rtsRunning);
}
