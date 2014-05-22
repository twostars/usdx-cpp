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
#include "../base/TextureMgr.h"
#include "MenuBackgroundTexture.h"

MenuBackgroundTexture::MenuBackgroundTexture(const ThemeBackground* themedSettings)
	: MenuBackground(themedSettings)
{
	if (themedSettings->Tex.empty())
		throw MenuBackgroundException(_T("MenuBackgroundTexture::MenuBackgroundTexture(): No texture name present."));

	const boost::filesystem::path * texFilename;

	Color = themedSettings->Color;
	texFilename = sSkins.GetTextureFileName(themedSettings->Tex);

	if (texFilename == NULL)
	{
		throw MenuBackgroundException(_T("MenuBackgroundTexture::MenuBackgroundTexture(): Texture (%s) not found."), 
			themedSettings->Tex.c_str());
	}

	Tex = sTextureMgr.GetTexture(texFilename, TextureType::Plain);
	if (Tex.TexNum)
		throw MenuBackgroundException(_T("MenuBackgroundTexture::MenuBackgroundTexture(): Texture (%s) not loaded."),
		texFilename->native().c_str());
}

void MenuBackgroundTexture::Draw()
{
	// Clear just once when in dual screen mode
	if (ScreenAct == 1)
		glClear(GL_DEPTH_BUFFER_BIT);

	glColorRGB(Color);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, Tex.TexNum);

	glBegin(GL_QUADS);
		glTexCoord2f(Tex.TexX1*Tex.TexW, Tex.TexY1*Tex.TexH);
		glVertex2i(0, 0);

		glTexCoord2f(Tex.TexX1*Tex.TexW, Tex.TexY2*Tex.TexH);
		glVertex2i(0, RenderH);

		glTexCoord2f(Tex.TexX2*Tex.TexW, Tex.TexY2*Tex.TexH);
		glVertex2i(RenderW, RenderH);

		glTexCoord2f(Tex.TexX2*Tex.TexW, Tex.TexY1*Tex.TexH);
		glVertex2i(RenderW, 0);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
