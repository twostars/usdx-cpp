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

MenuButtonCollection::MenuButtonCollection(Texture* tex)
	: MenuButton(tex), ScreenButton(NULL), FirstChild(0), ChildCount(0)
{
}

MenuButtonCollection::MenuButtonCollection(Texture* tex, Texture* deselectTexture)
	: MenuButton(tex, deselectTexture), ScreenButton(NULL), FirstChild(0), ChildCount(0)
{
}

void MenuButtonCollection::SetSelect(bool value)
{
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
	// If fading is activated, fade child buttons
	if (Fade)
	{
		assert(ScreenButton != NULL);

		float fadeProgress = 0.0f;
		if (FadeProgress >= 0.5f)
			fadeProgress = (FadeProgress - 0.666f) * 3;

		for (std::vector<MenuButton>::iterator itr = ScreenButton->begin(); itr != ScreenButton->end(); ++itr)
		{
			if ((*itr).Parent != Parent)
				continue;

			if (FadeProgress < 0.5f)
			{
				(*itr).Visible = Selected;
				for (std::vector<MenuText>::iterator textItr = (*itr).Texts.begin(); textItr != (*itr).Texts.end(); ++textItr)
					(*textItr).Visible = Selected;
			}
			else
			{
				assert((*itr).Tex != NULL);
				(*itr).Tex->Alpha = fadeProgress;
				for (std::vector<MenuText>::iterator textItr = (*itr).Texts.begin(); textItr != (*itr).Texts.end(); ++textItr)
					(*textItr).Alpha = fadeProgress;
			}
		}
	}
}
