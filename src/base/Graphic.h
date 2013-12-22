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

#ifndef _GRAPHIC_H
#define _GRAPHIC_H
#pragma once

#include "Texture.h"

void Initialize3D(const TCHAR * windowTitle);
void LoadOpenGLExtensions();
void LoadFontTextures();
void UnloadFontTextures();
void LoadLoadingScreen();
void LoadTextures();
void LoadScreens();
void SwapBuffers();

SDL_Surface * LoadSurfaceFromFile(const boost::filesystem::path& filename);
void UnloadSurface(SDL_Surface * texSurface);

void AdjustPixelFormat(SDL_Surface *& texSurface, eTextureType textureType);
bool PixelFormatEquals(SDL_PixelFormat * fmt1, const SDL_PixelFormat * fmt2);
void ScaleImage(SDL_Surface * imgSurface, uint32 width, uint32 height);
void FitImage(SDL_Surface *& imgSurface, uint32 width, uint32 height);
void ColorizeImage(SDL_Surface * imgSurface, uint32 newColor);

void glColorRGB(const RGB& color);
void glColorRGB(const RGB& color, float alpha);
void glColorRGB(const RGBA& color);
void glColorRGB(const RGBA& color, float alpha);
void glColorRGBInt(const RGB& color, float intensity);
void glColorRGBInt(const RGB& color, float alpha, float intensity);
void glColorRGBInt(const RGBA& color, float intensity);
void glColorRGBInt(const RGBA& color, float alpha, float intensity);

void FreeGfxResources();

/* TODO: Clean up these globals */

typedef std::set<SDL_Surface *> SurfaceCollection;

extern SDL_Window  * Screen;
extern SDL_GLContext GLContext;

extern SurfaceCollection g_surfaces;

extern int RenderW, RenderH;
extern int ScreenW, ScreenH;
extern int Screens, ScreenAct, ScreenX;

extern bool Fullscreen;

extern PFNGLCOPYTEXSUBIMAGE3DPROC	glCopyTexSubImage3D;
extern PFNGLDRAWRANGEELEMENTSPROC	glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC			glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC		glTexSubImage3D;

#endif
