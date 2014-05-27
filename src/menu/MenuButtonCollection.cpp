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
#include "../base/Texture.h"

MenuButtonCollection::MenuButtonCollection()
	: MenuButton(), ScreenButton(NULL), FirstChild(0), ChildCount(0)
{
}

MenuButtonCollection::MenuButtonCollection(const Texture& tex)
	: MenuButton(tex), ScreenButton(NULL), FirstChild(0), ChildCount(0)
{
}

MenuButtonCollection::MenuButtonCollection(const Texture& tex, const Texture& deselectTexture)
	: MenuButton(tex, deselectTexture), ScreenButton(NULL), FirstChild(0), ChildCount(0)
{
}

void MenuButtonCollection::SetSelected(bool value)
{
	MenuButton::SetSelected(value);

	if (!Fade)
	{
		assert(ScreenButton != NULL);

		// Set the visibility state for every button that is a child of this collection.
		for (std::vector<MenuButton>::iterator itr = ScreenButton->begin(); itr != ScreenButton->end(); ++itr)
		{
			if ((*itr).Parent == Parent)
				(*itr).Visible = value;
		}
	}
}

void MenuButtonCollection::Draw()
{
	if (!Visible)
		return;

	MenuButton::Draw();

	// If fading is activated, fade child buttons
	if (Fade)
	{
		assert(ScreenButton != NULL);

		bool isFading = (FadeProgress >= 0.5f);
		float currentFadeProgress = (FadeProgress - 0.666f) * 3;

		for (std::vector<MenuButton>::iterator itr = ScreenButton->begin(); itr != ScreenButton->end(); ++itr)
		{
			if ((*itr).Parent != Parent)
				continue;

			if (!isFading)
			{
				(*itr).Visible = Selected;
				for (std::vector<MenuText>::iterator textItr = (*itr).Texts.begin(); textItr != (*itr).Texts.end(); ++textItr)
					(*textItr).Visible = Selected;
			}
			else
			{
				(*itr).Tex.Alpha = currentFadeProgress;
				for (std::vector<MenuText>::iterator textItr = (*itr).Texts.begin(); textItr != (*itr).Texts.end(); ++textItr)
					(*textItr).Alpha = currentFadeProgress;
			}
		}
	}
}
