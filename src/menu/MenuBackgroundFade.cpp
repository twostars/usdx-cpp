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
#include "../base/Graphic.h"
#include "../base/ThemeDefines.h"
#include "../base/Skins.h"
#include "../base/Texture.h"
#include "MenuBackgroundFade.h"

const Uint32 FADEINTIME = 1500; // Time the bg fades in

MenuBackgroundFade::MenuBackgroundFade(const ThemeBackground * themedSettings)
	: MenuBackgroundTexture(themedSettings, true)
{
	FadeTime = 0;

	Alpha = themedSettings->Alpha;
	if (themedSettings->Tex.empty()
		|| Tex.TexNum == 0)
		UseTexture = false;
}

void MenuBackgroundFade::OnShow()
{
	FadeTime = SDL_GetTicks();
}

void MenuBackgroundFade::Draw()
{
	float Progress;
	if (FadeTime == 0)
		Progress = Alpha;
	else
		Progress = Alpha * (SDL_GetTicks() - FadeTime) / FADEINTIME;

	if (Progress > Alpha)
	{
		FadeTime = 0;
		Progress = Alpha;
	}

	if (UseTexture)
		return MenuBackgroundTexture::Draw();

	// Clear just once when in dual screen mode
	if (ScreenAct == 1)
		glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorRGB(Color, Progress);

	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, RenderH);
		glVertex2i(RenderW, RenderH);
		glVertex2i(RenderW, 0);
	glEnd();
	glDisable(GL_BLEND);
}
