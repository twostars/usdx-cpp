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

#ifndef _MENUINTERACT_H
#define _MENUINTERACT_H
#pragma once

enum InteractionType
{
	itButton			= 0,
	itSelect			= 1,
	itText				= 2,
	itSelectSlide		= 3,
	itBCollectionChild	= 5
};

/* For moving through a menu */
struct MenuInteract
{
	InteractionType Type;
	int Num;  // number of this item in proper list like buttons, selects
};

// to handle the area where the mouse is over a control
struct MouseOverRect
{
	float X, Y;
	float W, H;
};

// to handle the on click action
enum MouseClickAction 
{
	maNone, maReturn, maLeft, maRight
};

#endif
