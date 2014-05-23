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
#include "Texture.h"
#include "Graphic.h"

void Texture::Draw()
{
	float	x1, x2, x3, x4,
			y1, y2, y3, y4,
			xt1, xt2, xt3, xt4,
			yt1, yt2, yt3, yt4;

	glColor4f(ColRGB.R * Int, ColRGB.G * Int, ColRGB.B * Int, Alpha);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthRange(0, 10);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, TexNum);

	x1 = X;
	x2 = X;
	x3 = X + W * ScaleW;
	x4 = X + W * ScaleW;
	y1 = Y;
	y2 = Y + H * ScaleH;
	y3 = Y + H * ScaleH;
	y4 = Y;

	if (Rot != 0.0f)
	{
		xt1 = x1 - (X + W/2);
		xt2 = x2 - (X + W/2);
		xt3 = x3 - (X + W/2);
		xt4 = x4 - (X + W/2);
		yt1 = y1 - (Y + H/2);
		yt2 = y2 - (Y + H/2);
		yt3 = y3 - (Y + H/2);
		yt4 = y4 - (Y + H/2);

		x1 = (X + W/2) + xt1 * cos(Rot) - yt1 * sin(Rot);
		x2 = (X + W/2) + xt2 * cos(Rot) - yt2 * sin(Rot);
		x3 = (X + W/2) + xt3 * cos(Rot) - yt3 * sin(Rot);
		x4 = (X + W/2) + xt4 * cos(Rot) - yt4 * sin(Rot);
		
		y1 = (Y + H/2) + yt1 * cos(Rot) + xt1 * sin(Rot);
		y2 = (Y + H/2) + yt2 * cos(Rot) + xt2 * sin(Rot);
		y3 = (Y + H/2) + yt3 * cos(Rot) + xt3 * sin(Rot);
		y4 = (Y + H/2) + yt4 * cos(Rot) + xt4 * sin(Rot);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(TexX1*TexW, TexY1*TexH); glVertex3f(x1, y1, Z);
		glTexCoord2f(TexX1*TexW, TexY2*TexH); glVertex3f(x2, y2, Z);
		glTexCoord2f(TexX2*TexW, TexY2*TexH); glVertex3f(x3, y3, Z);
		glTexCoord2f(TexX2*TexW, TexY1*TexH); glVertex3f(x4, y4, Z);
	glEnd();

	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Texture::DrawReflection(float spacing)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glDepthRange(0, 10);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, TexNum);

	glBegin(GL_QUADS);
		// Top-left
		glColorRGBInt(ColRGB, Alpha - 0.3f, Int);
		glTexCoord2f(TexX1*TexW, TexY2*TexH);
		glVertex3f(X, Y+H*ScaleH + spacing, Z);

		// Bottom-left
		glColorRGBInt(ColRGB, 0.0f, Int);
		glTexCoord2f(TexX1*TexW, TexY1+TexH*0.5f);
		glVertex3f(X, Y+H*ScaleH + H*ScaleH/2 + spacing, Z);

		// Bottom-right
		glColorRGBInt(ColRGB, 0.0f, Int);
		glTexCoord2f(TexX2*TexW, TexY1+TexH*0.5f);
		glVertex3f(X+W*ScaleW, Y+H*ScaleH + H*ScaleH/2 + spacing, Z);

		// Top-right
		glColorRGBInt(ColRGB, Alpha-0.3f, Int);
		glTexCoord2f(TexX2*TexW, TexY2*TexH);
		glVertex3f(X+W*ScaleW, Y+H*ScaleH + spacing, Z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}
