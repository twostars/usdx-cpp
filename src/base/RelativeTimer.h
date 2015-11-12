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

#ifndef _RELATIVETIMER_H
#define _RELATIVETIMER_H
#pragma once

enum RelativeTimerState
{
	rtsStopped,
	rtsWait,
	rtsPaused,
	rtsRunning
};

class RelativeTimer
{
public:
	RelativeTimer();

	void Start(bool WaitForTrigger = false);
	void Pause();
	void Stop();

	float GetTime();
	void SetTime(float time);

	INLINE void SetState(RelativeTimerState state) {
		_state = state;
	}

	INLINE RelativeTimerState GetState() const {
		return _state;
	}

private:
	RelativeTimerState	_state;
	Sint64				_absoluteTime;
	float				_relativeTime;
	bool				_triggerMode;
};

#endif
