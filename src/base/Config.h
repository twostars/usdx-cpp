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

#ifndef _CONFIG_H
#define _CONFIG_H
#pragma once

const int VERSION_MAJOR   = 1000000;
const int VERSION_MINOR   = 1000;
const int VERSION_RELEASE = 1;

/*
 * Current version of UltraStar Deluxe
 */

#define   USDX_VERSION_MAJOR     1
#define	  USDX_VERSION_MINOR     1
#define   USDX_VERSION_RELEASE   0
#define   USDX_VERSION_STATE     _T("")
#define   USDX_IDENTIFIER        _T("ultrastardx")
#define   USDX_STRING            _T("UltraStar Deluxe")
#define   USDX_THEME_VERSION     _T("USD 110")

#define   MAX_PROFILE_NAMES      12
#define   MAX_PLAYERS            6
#define   MAX_PLAYERS_PER_TEAM   4
#define   MAX_ROUNDS             7
#define   MAX_TEAMS              3

#ifdef HAVE_FFMPEG
#define LIBAVCODEC_VERSION		(LIBAVCODEC_VERSION_MAJOR * VERSION_MAJOR) + \
								(LIBAVCODEC_VERSION_MINOR * VERSION_MINOR) + \
								(LIBAVCODEC_VERSION_RELEASE * VERSION_RELEASE)

#define LIBAVFORMAT_VERSION		(LIBAVFORMAT_VERSION_MAJOR * VERSION_MAJOR) + \
								(LIBAVFORMAT_VERSION_MINOR * VERSION_MINOR) + \
								(LIBAVFORMAT_VERSION_RELEASE * VERSION_RELEASE)

#define LIBAVUTIL_VERSION		(LIBAVUTIL_VERSION_MAJOR * VERSION_MAJOR) + \
								(LIBAVUTIL_VERSION_MINOR * VERSION_MINOR) + \
								(LIBAVUTIL_VERSION_RELEASE * VERSION_RELEASE)
#endif

#ifdef HAVE_SWSCALE
#define LIBSWSCALE_VERSION		(LIBSWSCALE_VERSION_MAJOR * VERSION_MAJOR) + \
								(LIBSWSCALE_VERSION_MINOR * VERSION_MINOR) + \
								(LIBSWSCALE_VERSION_RELEASE * VERSION_RELEASE);
#endif

#ifdef HAVE_PROJECTM
#define PROJECTM_VERSION		(PROJECTM_VERSION_MAJOR * VERSION_MAJOR) + \
								(PROJECTM_VERSION_MINOR * VERSION_MINOR) + \
								(PROJECTM_VERSION_RELEASE * VERSION_RELEASE)
#endif

#ifdef HAVE_PORTAUDIO
#define PORTAUDIO_VERSION		(PORTAUDIO_VERSION_MAJOR * VERSION_MAJOR) + \
								(PORTAUDIO_VERSION_MINOR * VERSION_MINOR) + \
								(PORTAUDIO_VERSION_RELEASE * VERSION_RELEASE)
#endif

#ifdef HAVE_LIBSAMPLERATE
#define LIBSAMPLERATE_VERSION	(LIBSAMPLERATE_VERSION_MAJOR * VERSION_MAJOR) + \
								(LIBSAMPLERATE_VERSION_MINOR * VERSION_MINOR) + \
								(LIBSAMPLERATE_VERSION_RELEASE * VERSION_RELEASE)
#endif

#define USDXShortVersionStr()	(USDX_STRING USDX_VERSION_STATE)

#define USDXVersionStr()		(USDX_STRING _T(" V") \
								TSTRINGIFY(USDX_VERSION_MAJOR) _T(".") \
								TSTRINGIFY(USDX_VERSION_MINOR) _T(".") \
								TSTRINGIFY(USDX_VERSION_RELEASE) \
								USDX_VERSION_STATE)

#endif
