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

#ifndef _STDAFX_H
#define _STDAFX_H
#pragma once

#if defined(WIN32)
#	define WIN32_LEAN_AND_MEAN
#	define VC_EXTRALEAN
#endif

#include <exception>
#include <string>
#include <iostream>
#include <cassert>
#include <map>
#include <set>
#include <vector>
#include <cstdarg>
#include <algorithm>
#include <cctype>
#include <cmath>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using boost::filesystem::path;

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "SDL_utilities.h"

#include <GL/glu.h>

#include "../lib/simpleini/SimpleIni.h"

#include "unicode.h"
#include "types.h"
#include "string_utils.h"
#include "math_utils.h"
#include "misc_utils.h"

#include "exceptions.h"
#include "Singleton.h"

#include "enumerations.h"

#include "../base/Config.h"
#include "../base/TextEncoding.h"
#include "../base/Common.h"

#endif
