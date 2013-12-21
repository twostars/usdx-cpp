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
#include "Menu.h"
#include "DrawTexture.h"

MenuButton::MenuButton()
{
	Create();
}

MenuButton::MenuButton(const Texture& tex)
{
	Create();
	Tex = tex;
	DeselectTexture = tex;
	Tex.ColRGB.R = 0.0f;
	Tex.ColRGB.G = 0.5f;
	Tex.ColRGB.B = 0.0f;
	Tex.Int = 1.0f;
	Colorized = false;
}

MenuButton::MenuButton(const Texture& tex, const Texture& deselectTexture)
{
	Create();
	Tex = tex;
	DeselectTexture = deselectTexture;
	Tex.ColRGB.R = 1.0f;
	Tex.ColRGB.G = 1.5f;
	Tex.ColRGB.B = 1.0f;
	Tex.Int = 1.0f;
	Colorized = true;
}

void MenuButton::Create()
{
	Visible        = true;
	Selected       = false;
	DeselectType   = 0;
	Selectable     = true;
	Reflection     = false;
	Colorized      = false;

	SelectColRGB.R = 1.0f;
	SelectColRGB.G = 1.0f;
	SelectColRGB.B = 1.0f;

	SelectInt      = 1.0f;
	SelectTInt     = 1.0f;

	DeselectColRGB.R = 1.0f;
	DeselectColRGB.G = 1.0f;
	DeselectColRGB.B = 1.0f;

	DeselectInt    = 0.5f;
	DeselectTInt   = 1.0f;

	Fade           = false;
	FadeTex.TexNum = 0;
	FadeProgress   = 0.0f;
	FadeText       = false;
	SelectW        = DeselectW;
	SelectH        = DeselectH;

	PosX           = 0.0f;
	PosY           = 0.0f;

	Parent         = 0;
}

void MenuButton::SetX(float value)
{
	PosX = value;
	if (FadeTex.TexNum == 0)
		Tex.X = value;
}

void MenuButton::SetY(float value)
{
	PosY = value;
	if (FadeTex.TexNum == 0)
		Tex.Y = value;
}

void MenuButton::SetZ(float value)
{
	Tex.Z = value;
}

void MenuButton::SetW(float value)
{
	if (SelectW == DeselectW)
		SelectW = value;

	DeselectW = value;
	if (!Fade)
		Tex.W = (Selected ? SelectW : DeselectW);
}

void MenuButton::SetH(float value)
{
	if (SelectH == DeselectH)
		SelectH = value;

	DeselectH = value;
	if (!Fade)
		Tex.W = (Selected ? SelectH : DeselectH);
}

void MenuButton::SetSelect(bool value)
{
	Selected = value;

	if (value)
	{
		Tex.ColRGB = Tex2.ColRGB = SelectColRGB;
		Tex.Int = Tex2.Int = SelectInt;

		for (std::vector<MenuText>::iterator itr = Texts.begin(); itr != Texts.end(); ++itr)
                itr->Int = SelectTInt;

		if (Fade)
		{
			if (FadeProgress <= 0.0f)
				FadeProgress = 0.125f;
		}
		else
		{
			Tex.W = SelectW;
			Tex.H = SelectH;
		}
	}
	else
	{
		Tex.ColRGB = Tex2.ColRGB = DeselectColRGB;
		Tex.Int = Tex2.Int = DeselectInt;

		for (std::vector<MenuText>::iterator itr = Texts.begin(); itr != Texts.end(); ++itr)
                itr->Int = DeselectTInt;

		if (Fade)
		{
			if (FadeProgress >= 1.0f)
				FadeProgress = 0.875f;
		}
		else
		{
			Tex.W = DeselectW;
			Tex.H = DeselectH;
		}
	}
}

void MenuButton::Draw()
{
	if (!Visible)
		return;

	uint32 tick = 0;
	float spacing;

	if (Fade)
	{
		if (FadeProgress > 0.0f && FadeProgress < 1.0f)
		{
			tick = SDL_GetTicks() / 16;
			if (tick != FadeLastTick)
			{
				FadeLastTick = tick;
				if (Selected)
					FadeProgress += 0.125f;
				else
					FadeProgress -= 0.125f;

				if (FadeText)
				{
					for (std::vector<MenuText>::iterator itr = Texts.begin(); itr != Texts.end(); ++itr)
					{
						itr->MoveX = (SelectW - DeselectW) * FadeProgress;
						itr->MoveY = (SelectH - DeselectH) * FadeProgress;
					}
				}
			}
		}

		// Method without fade texture
		if (FadeTex.TexNum == 0)
		{
			DeselectTexture.W = Tex.W = DeselectW + (SelectW - DeselectW) * FadeProgress;
			DeselectTexture.H = Tex.H = DeselectH + (SelectH - DeselectH) * FadeProgress;
		}
		// Method with fade texture
		else
		{
			DeselectTexture.W = Tex.W = DeselectW;
			DeselectTexture.H = Tex.H = DeselectH;

			FadeTex.ColRGB = Tex.ColRGB;
			FadeTex.Int = Tex.Int;
			FadeTex.Z = Tex.Z;

			FadeTex.Alpha = Tex.Alpha;
			FadeTex.TexX1 = 0;
			FadeTex.TexX2 = 1;
			FadeTex.TexY1 = 0;
			FadeTex.TexY1 = 1;

			switch (FadeTexPos)
			{
			// Fade tex on top
			case 0:
				// Standard texture
				DeselectTexture.X = Tex.X = PosX;
				DeselectTexture.Y = Tex.Y = PosY + (SelectH - DeselectH) * FadeProgress;

				// Fade texture
				FadeTex.X = PosX;
				FadeTex.Y = PosY;
				FadeTex.W = Tex.W;
				FadeTex.H = (SelectH - DeselectH) * FadeProgress;
				FadeTex.ScaleW = Tex.ScaleW;

				// Hack to fix a little space between both textures
				FadeTex.TexY2 = 0.9f;
				break;

			// Fade tex on left
			case 1:
				// Standard texture
				DeselectTexture.X = Tex.X = PosX + (SelectW - DeselectW) * FadeProgress;
				DeselectTexture.Y = Tex.Y = PosY;

				// Fade texture
				FadeTex.X = PosX;
				FadeTex.Y = PosY;
				FadeTex.W = (SelectW - DeselectW) * FadeProgress;
				FadeTex.H = Tex.H;
				FadeTex.ScaleH = Tex.ScaleH;

				// Hack to fix a little space between both textures
				FadeTex.TexX2 = 0.9f;
				break;

			// Fade tex on bottom
			case 2:
				// Standard texture
				DeselectTexture.X = Tex.X = PosX;
				DeselectTexture.Y = Tex.Y = PosY;

				// Fade texture
				FadeTex.X = PosX;
				FadeTex.Y = PosY + (SelectH - DeselectH) * FadeProgress;
				FadeTex.W = Tex.W;
				FadeTex.H = (SelectH - DeselectH) * FadeProgress;
				FadeTex.ScaleW = Tex.ScaleW;

				// Hack to fix a little space between both textures
				FadeTex.TexY1 = 0.1f;
				break;

			// Fade tex on right
			case 3:
				// Standard texture
				DeselectTexture.X = Tex.X = PosX;
				DeselectTexture.Y = Tex.Y = PosY;

				// Fade texture
				FadeTex.X = PosX + (SelectW - DeselectW) * FadeProgress;
				FadeTex.Y = PosY;
				FadeTex.W = (SelectW - DeselectW) * FadeProgress;
				FadeTex.H = Tex.H;
				FadeTex.ScaleW = Tex.ScaleW;

				// Hack to fix a little space between both textures
				FadeTex.TexX1 = 0.1f;
				break;
			}
		}
	}
	else if (FadeText)
	{
		if (!Texts.empty())
		{
			Texts[0].MoveX = (SelectW - DeselectW);
			Texts[0].MoveY = (SelectH - DeselectH);
		}
	}

	if (Selected || FadeProgress > 0.0f || !Colorized)
	{
		Tex.Draw();
	}
	else
	{
		DeselectTexture.X = Tex.X;
		DeselectTexture.Y = Tex.Y;
		DeselectTexture.W = Tex.W;
		DeselectTexture.H = Tex.H;
		DeselectTexture.Draw();
	}

	if (FadeTex.TexNum > 0)
		FadeTex.Draw();

	if (Tex2.Alpha > 0.0f)
	{
		Tex2.ScaleW = Tex.ScaleW;
		Tex2.ScaleH = Tex.ScaleH;

		Tex2.X = Tex.X;
		Tex2.Y = Tex.Y;
		Tex2.W = Tex.W;
		Tex2.H = Tex.H;

		Tex2.ColRGB = Tex.ColRGB;
		Tex2.Int = Tex.Int;

		Tex2.Draw();
	}

	// Draw reflections
	if (Reflection)
	{
		if (FadeProgress > 0.0f && FadeProgress < 1.0f)
			spacing = DeselectReflectionSpacing - (DeselectReflectionSpacing - ReflectionSpacing) * FadeProgress;
		else if (Selected)
			spacing = ReflectionSpacing;
		else
			spacing = DeselectReflectionSpacing;

		if (Selected || !Colorized)
			Tex.DrawReflection(spacing);
		else
			DeselectTexture.DrawReflection(spacing);
	}

	for (std::vector<MenuText>::iterator itr = Texts.begin(); itr != Texts.end(); ++itr)
		itr->Draw();
}

MouseOverRect MenuButton::GetMouseOverRect()
{
	MouseOverRect rect = {0};
	if (FadeTex.TexNum == 0)
	{
		rect.X = Tex.X;
		rect.Y = Tex.Y;
		rect.W = Tex.W;
		rect.H = Tex.H;
		return rect;
	}

	switch (FadeTexPos)
	{
	// Fade tex on top
	case 0:
		rect.X = Tex.X;
		rect.Y = FadeTex.Y;
		rect.W = Tex.W;
		rect.H = FadeTex.H + Tex.H;
		break;

	// Fade tex on left
	case 1:
		rect.X = FadeTex.X;
		rect.Y = Tex.Y;
		rect.W = FadeTex.W + Tex.W;
		rect.H = Tex.H;
		break;

	// Fade tex on bottom
	case 2:
		rect.X = Tex.X;
		rect.Y = Tex.Y;
		rect.W = Tex.W;
		rect.H = FadeTex.H + Tex.H;
		break;

	// Fade text on right
	case 3:
		rect.X = FadeTex.Y;
		rect.Y = Tex.Y;
		rect.W = FadeTex.W + Tex.W;
		rect.H = Tex.H;
		break;
	}

	return rect;
}
