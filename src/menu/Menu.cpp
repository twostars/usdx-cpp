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
#include "../base/TextureMgr.h"
#include "../base/Skins.h"
#include "../base/Graphic.h"
#include "../base/Log.h"
#include "Menu.h"
#include "MenuBackgroundNone.h"
#include "MenuBackgroundColor.h"
#include "MenuBackgroundTexture.h"
#include "MenuBackgroundVideo.h"
#include "MenuBackgroundFade.h"
#include "Display.h"
#include "../screens/ScreenPopup.h"
#include "../screens/ScreenSing.h"

static const char * s_supportedTextureExts[] =
{
	".png",
	".bmp",
	".jpg",
	".jpeg",
	".gif",
	".pnm",
	".ppm",
	".pgm",
	".pbm",
	".xpm",
	".lbm",
	".pcx",
	".tga",
	".tiff",
	NULL
};

static const char * s_supportedBackgroundVideoExts[] = 
{
	".avi",
	".mov",
	".divx",
	".mpg",
	".mp4",
	".mpeg",
	".m2v",
	NULL
};

Menu::Menu()
	: Fade(fBlack), ButtonPos(-1), Background(NULL), RightMbESC(true), SelInteraction(0)
{
}

void Menu::AddInteraction(InteractionType type, int num)
{
	MenuInteract interaction;

	interaction.Type = type;
	interaction.Num = num;

	Interactions.push_back(interaction);
	SetInteraction(0);
}

void Menu::SetInteraction(int num)
{
	// Don't bother selecting the already selected control.
	if (SelInteraction == num)
		return;

	int oldNum, newNum, index;
	InteractionType oldType, newType;

	assert(num >= 0 && num < (int) Interactions.size()); 
	assert(SelInteraction >= 0 && SelInteraction < (int) Interactions.size()); 

	oldNum = Interactions[SelInteraction].Num;
	oldType = Interactions[SelInteraction].Type;

	newNum = Interactions[num].Num;
	newType = Interactions[num].Type;
	
	// Set inactive
	switch (oldType)
	{
	case itButton:
		assert(oldNum >= 0 && oldNum < (int) Buttons.size());
		Buttons[oldNum].SetSelected(false);
		break;

	case itText:
		assert(oldNum >= 0 && oldNum < (int) Texts.size());
		Texts[oldNum].SetSelected(false);
		break;

	case itSelectSlide:
		assert(oldNum >= 0 && oldNum < (int) SelectSlides.size());
		SelectSlides[oldNum].SetSelected(false);
		break;

	case itBCollectionChild:
		assert(oldNum >= 0 && oldNum < (int) Buttons.size());
		assert(newNum >= 0 && newNum < (int) Buttons.size());

		Buttons[oldNum].SetSelected(false);

		// deselect collection if next button is not from collection
		if (newType != itButton 
			|| Buttons[newNum].Parent != Buttons[oldNum].Parent)
		{
			index = Buttons[oldNum].Parent - 1;
			assert(index >= 0 && index < (int) ButtonCollections.size());
			ButtonCollections[index].SetSelected(false);
		}
		break;
	}

	// Set active
	SelInteraction = num;
	switch (newType)
	{
	case itButton:
		assert(newNum >= 0 && newNum < (int) Buttons.size());
		Buttons[newNum].SetSelected(true);
		break;

	case itText:
		assert(newNum >= 0 && newNum < (int) Texts.size());
		Texts[newNum].SetSelected(true);
		break;

	case itSelectSlide:
		assert(newNum >= 0 && newNum < (int) SelectSlides.size());
		SelectSlides[newNum].SetSelected(true);
		break;

	case itBCollectionChild:
		assert(newNum >= 0 && newNum < (int) Buttons.size());
		index = Buttons[newNum].Parent - 1;
		assert(index >= 0 && index < (int) ButtonCollections.size());
		Buttons[newNum].SetSelected(true);
		ButtonCollections[index].SetSelected(true);
		break;
	}

	OnInteraction();
}

void Menu::OnInteraction()
{
}

// Load background, texts, statics & button collections from ThemeBasic
void Menu::LoadFromTheme(const ThemeBasic& themeBasic)
{
	// Add button collections
	// Button collections are created when the first child button is created
	PrepareButtonCollections(themeBasic.ButtonCollection);

	AddBackground(themeBasic.Background);

	for (AThemeStatic::const_iterator itr = themeBasic.Static.begin(); itr != themeBasic.Static.end(); ++itr)
		AddStatic(*itr);

	for (AThemeText::const_iterator itr = themeBasic.Text.begin(); itr != themeBasic.Text.end(); ++itr)
		AddText(*itr);
}

void Menu::PrepareButtonCollections(const AThemeButtonCollection& collections)
{
	ButtonCollections.assign(collections.size(), MenuButtonCollection());

	uint8 i = 0;
	for (AThemeButtonCollection::const_iterator itr = collections.begin(); itr != collections.end(); ++itr, ++i)
		AddButtonCollection(*itr, i);
}

void Menu::AddButtonCollection(const ThemeButtonCollection& collection, uint8 num)
{
	uint32 TempCol, TempDCol;

	assert(num >= 0 && num < ButtonCollections.size());

	MenuButtonCollection& dst = ButtonCollections[num];

	TempCol = 0;

	// Colorize hack
	if (collection.Style.Type == TextureType::Colorized)
	{
		TempCol  = collection.Style.ColRGB.ToUInt32();
		TempDCol = collection.Style.DColRGB.ToUInt32();

		// Give encoded color to GetTexture()
		dst = MenuButtonCollection(
			sTextureMgr.GetTexture(sSkins.GetTextureFileName(collection.Style.Tex), TextureType::Colorized, TempCol),
			sTextureMgr.GetTexture(sSkins.GetTextureFileName(collection.Style.Tex), TextureType::Colorized, TempDCol)
		);
	}
	else
	{
		dst = MenuButtonCollection(
			sTextureMgr.GetTexture(sSkins.GetTextureFileName(collection.Style.Tex), collection.Style.Type)
		);
	}

	// Set parent menu
	dst.ScreenButton = &Buttons;

	// Set attributes
	dst.FirstChild = collection.FirstChild;
	dst.ChildCount = collection.ChildCount;
	dst.Parent = num + 1;

	// Set style
	dst.SetX((float) collection.Style.X);
	dst.SetY((float) collection.Style.Y);
	dst.SetW((float) collection.Style.W);
	dst.SetH((float) collection.Style.H);

	if (collection.Style.Type != TextureType::Colorized)
	{
		dst.SelectColRGB = collection.Style.ColRGB;
		dst.DeselectColRGB = collection.Style.DColRGB;
	}

	dst.SelectInt = collection.Style.Int;
	dst.DeselectInt = collection.Style.DInt;

	dst.Tex.TexX1 = 0;
	dst.Tex.TexY1 = 0;
	dst.Tex.TexX2 = 1;
	dst.Tex.TexY2 = 1;

	dst.SetSelected(false);

	dst.Reflection = collection.Style.Reflection;
	dst.ReflectionSpacing = collection.Style.ReflectionSpacing;
	dst.DeselectReflectionSpacing = collection.Style.DeselectReflectionSpacing;

	dst.SetZ(collection.Style.Z);

	dst.SelectH = (float) collection.Style.SelectH;
	dst.SelectW = (float) collection.Style.SelectW;

	dst.Fade = collection.Style.Fade;
	dst.FadeText = collection.Style.FadeText;

	// Hack
	eTextureType texType = TextureType::Colorized;
	{
		dst.FadeTex = sTextureMgr.GetTexture(
			sSkins.GetTextureFileName(collection.Style.FadeTex), 
			TextureType::Colorized, 
			TempCol
		);
	}

	dst.FadeTexPos = collection.Style.FadeTexPos;

	for (AThemeText::const_iterator itr = collection.Style.Text.begin(); itr != collection.Style.Text.end(); ++itr)
		AddButtonText(dst, (float) (*itr).X, (float) (*itr).Y, (*itr).ColRGB, (*itr).Font, (*itr).Size, (*itr).Align, (*itr).Text);
}

void Menu::AddBackground(const ThemeBackground& themedSettings)
{
	if (Background != NULL)
	{
		delete Background;
		Background = NULL;
	}

	switch (themedSettings.BGType)
	{
		case BackgroundType::Auto:
			if (!themedSettings.Tex.empty())
			{
				const path* textureFilename 
					= sSkins.GetTextureFileName(themedSettings.Tex);

				if (textureFilename != NULL)
				{
					// Attempt to identify background type by file extension...
					const char * ext = textureFilename->extension().generic_string().c_str();
					if (IsInStringArrayI(ext, s_supportedTextureExts))
						TryBGCreate<MenuBackgroundTexture>(themedSettings);
					else if (IsInStringArrayI(ext, s_supportedBackgroundVideoExts))
						TryBGCreate<MenuBackgroundVideo>(themedSettings);

					// Couldn't identify background type, so bash our head into the keyboard
					// a few times to see if we can produce any better results.
					if (Background == NULL)
					{
						if (!TryBGCreate<MenuBackgroundTexture>(themedSettings))
							TryBGCreate<MenuBackgroundVideo>(themedSettings);
					}
				}
			}
			break;

		case BackgroundType::Color:
			TryBGCreate<MenuBackgroundColor>(themedSettings);
			break;

		case BackgroundType::Texture:
			TryBGCreate<MenuBackgroundTexture>(themedSettings);
			break;

		case BackgroundType::Video:
			TryBGCreate<MenuBackgroundTexture>(themedSettings);
			break;

		case BackgroundType::Fade:
			TryBGCreate<MenuBackgroundFade>(themedSettings);
			break;

		case BackgroundType::None:
			TryBGCreate<MenuBackgroundNone>(themedSettings);
			break;
	}

	// Fallback to None/Colored
	if (Background == NULL)
	{
		if (themedSettings.BGType == BackgroundType::Color)
			TryBGCreate<MenuBackgroundNone>(themedSettings);
		else
			TryBGCreate<MenuBackgroundColor>(themedSettings);
	}

	assert(Background != NULL);
}

void Menu::LogBackgroundException(const MenuBackgroundException& e)
{
	sLog.Error("TryBGCreate", e.what());
}

int Menu::AddStatic(const ThemeStatic& themeStatic)
{
	return AddStatic(
		(float) themeStatic.X, (float) themeStatic.Y, 
		(float) themeStatic.W, (float) themeStatic.H, themeStatic.Z,
		themeStatic.ColRGB,
		themeStatic.TexX1, themeStatic.TexY1, themeStatic.TexX2, themeStatic.TexY2,
		sSkins.GetTextureFileName(themeStatic.Tex), themeStatic.Type, 0xFFFFFFFF,
		themeStatic.Reflection, themeStatic.ReflectionSpacing);
}

int Menu::AddStatic(float x, float y, float w, float h, 
					const RGB& colRGB, const path* textureFilename, 
					eTextureType textureType /*= TextureType::Plain*/, uint32 color /*= 0xFFFFFFFF*/)
{
	return AddStatic(x, y, w, h, 0.0f /* z*/, colRGB, textureFilename, textureType, color);
}

int Menu::AddStatic(float x, float y, float w, float h, float z, 
					const RGB& colRGB, const path* textureFilename, 
					eTextureType textureType /*= TextureType::Plain*/, uint32 color /*= 0xFFFFFFFF*/)
{
	return AddStatic(x, y, w, h, z, colRGB, 0.0f, 0.0f, 1.0f, 1.0f, textureFilename, textureType, color);
}

int Menu::AddStatic(float x, float y, float w, float h, float z, 
					const RGB& colRGB, float texX1, float texY1, float texX2, float texY2,
					const path* textureFilename, 
					eTextureType textureType /*= TextureType::Plain*/,
					uint32 color /*= 0xFFFFFFFF*/,
					bool reflection /*= false*/, float reflectionSpacing /*= 0.0f*/)
{
	Texture tex;

	// Colorize hack
	if (textureType == TextureType::Colorized)
		tex = sTextureMgr.GetTexture(textureFilename, textureType, colRGB.ToUInt32());
	else
		tex = sTextureMgr.GetTexture(textureFilename, textureType, 0xFF00FF);

	MenuStatic stat(tex);

	stat.Tex.X = x;
	stat.Tex.Y = y;
	stat.Tex.W = w;
	stat.Tex.H = h;
	stat.Tex.Z = z;

	// Hack
	if (textureType != TextureType::Colorized)
		stat.Tex.ColRGB = colRGB;

	stat.Tex.TexX1 = texX1;
	stat.Tex.TexY1 = texY1;
	stat.Tex.TexX2 = texX2;
	stat.Tex.TexY2 = texY2;
	stat.Tex.Alpha = 1.0f;

	stat.Reflection = reflection;
	stat.ReflectionSpacing = reflectionSpacing;

	stat.Visible = true;

	Statics.push_back(stat);

	return Statics.size() - 1; /* last inserted pos */
}

int Menu::AddText(const ThemeText& themeText)
{
	return AddText(
		(float) themeText.X, (float) themeText.Y, (float) themeText.W,
		themeText.Font, (float) themeText.Size, themeText.ColRGB,
		themeText.Align, themeText.Text, 
		themeText.Reflection, themeText.ReflectionSpacing,
		themeText.Z);
}

int Menu::AddText(float x, float y, const std::string& text)
{
	MenuText menuText(x, y, text);
	Texts.push_back(menuText);
	return Texts.size() - 1;
}

int Menu::AddText(float x, float y, uint32 style, float size, 
					const RGB& colRGB, const std::string& text)
{
	return AddText(x, y, 0.0f, style, size, colRGB, 0, text, false, 0.0f, 0.0f);
}

int Menu::AddText(float x, float y, float w, uint32 style, float size, 
					const RGB& colRGB, int align, const std::string& text,
					bool reflection, float reflectionSpacing, float z)
{
	MenuText menuText(x, y, w, style, size, colRGB, align, text, reflection, reflectionSpacing, z);
	Texts.push_back(menuText);
	return Texts.size() - 1;
}

void Menu::SetButtonLength(size_t length)
{
	if (ButtonPos == -1 && length > 0)
	{
		Buttons.assign(length, MenuButton());
		ButtonPos = 0;
	}
}

int Menu::AddButton(const ThemeButton& themeButton)
{
	int buttonNo = AddButton(
		(float) themeButton.X, (float) themeButton.Y, 
		(float) themeButton.W, (float) themeButton.H,
		themeButton.ColRGB, themeButton.Int, themeButton.DColRGB, themeButton.DInt,
		sSkins.GetTextureFileName(themeButton.Tex), themeButton.Type,
		themeButton.Reflection, themeButton.ReflectionSpacing, 
		themeButton.DeselectReflectionSpacing);

	if (buttonNo >= 0)
	{
		assert(buttonNo < (int) Buttons.size());
		MenuButton& button = Buttons[buttonNo];

		button.Visible = themeButton.Visible;

		button.SetZ(themeButton.Z);
		button.SelectW = (float) themeButton.SelectW;
		button.SelectH = (float) themeButton.SelectH;
		button.Fade = themeButton.Fade;
		button.FadeText = themeButton.FadeText;

		if (themeButton.Type == TextureType::Colorized)
		{
			button.FadeTex = sTextureMgr.GetTexture(
				sSkins.GetTextureFileName(themeButton.FadeTex), 
				themeButton.Type,
				themeButton.ColRGB.ToUInt32()
			);
		}
		else
		{
			button.FadeTex = sTextureMgr.GetTexture(
				sSkins.GetTextureFileName(themeButton.FadeTex), 
				themeButton.Type
			);
		}

		button.FadeTexPos = themeButton.FadeTexPos;

		for (AThemeText::const_iterator itr = themeButton.Text.begin(); itr != themeButton.Text.end(); ++itr)
		{
			AddButtonText((float) (*itr).X, (float) (*itr).Y, 
				(*itr).ColRGB, (*itr).Font, (*itr).Size, (*itr).Align, (*itr).Text);
		}

		// Handle button collections
		if (themeButton.Parent > 0)
		{
			// If collection exists then change interaction to child button
			if ((themeButton.Parent - 1) < (int) ButtonCollections.size())
			{
				assert(!Interactions.empty());

 				Interactions[Interactions.size() - 1].Type = InteractionType::itBCollectionChild;
				button.Visible = false;
				
				for (std::vector<MenuText>::iterator itr = button.Texts.begin(); itr != button.Texts.end(); ++itr)
					(*itr).Alpha = 0.0f;

				button.Parent = themeButton.Parent;
				assert(button.Parent > 0 && button.Parent <= ButtonCollections.size());
				if (ButtonCollections[button.Parent - 1].Fade)
					button.Tex.Alpha = 0.0f;
			}

		}
	}

	return buttonNo;
}

int Menu::AddButton(float x, float y, float w, float h, 
					const path* texturePath,
					eTextureType textureType /*= TextureType::Plain*/,
					bool reflection /*= false*/)
{
	RGB colRGB = {1.0f, 1.0f, 1.0f}, 
		dColRGB = {1.0f, 1.0f, 1.0f};

	return AddButton(x, y, w, h, colRGB, 1.0f, dColRGB, 0.5f, texturePath, textureType, reflection, 15.0f, 15.0f);
}

int Menu::AddButton(float x, float y, float w, float h, 
					const RGB& colRGB, float intensity,
					const RGB& dColRGB, float dIntensity,
					const path* texturePath,
					eTextureType textureType /*= TextureType::Plain*/,
					bool reflection /*= false*/,
					float reflectionSpacing /*= 0.0f*/,
					float deselectReflectionSpacing /*= 0.0f*/)
{
	int buttonNo = (int) Buttons.size();

	// Colorize hack
	if (textureType == TextureType::Colorized)
	{
		Buttons.push_back(MenuButton(
			sTextureMgr.GetTexture(texturePath, textureType, colRGB.ToUInt32()),
			sTextureMgr.GetTexture(texturePath, textureType, dColRGB.ToUInt32())
		));
	}
	else
	{
		Buttons.push_back(MenuButton(
			sTextureMgr.GetTexture(texturePath, textureType)
		));
	}

	MenuButton& button = Buttons[buttonNo];

	button.SetX(x);
	button.SetY(y);
	button.SetW(w);
	button.SetH(h);

	if (textureType != TextureType::Colorized)
	{
		button.SelectColRGB = colRGB;
		button.DeselectColRGB = dColRGB;
	}

	button.SelectInt = intensity;
	button.DeselectInt = dIntensity;

	button.Tex.TexX1 = 0;
	button.Tex.TexY1 = 0;
	button.Tex.TexX2 = 1;
	button.Tex.TexY2 = 1;
	button.SetSelected(false);

	button.Reflection = reflection;
	button.ReflectionSpacing = reflectionSpacing;
	button.DeselectReflectionSpacing = deselectReflectionSpacing;

	// button collection
	button.Parent = 0;

	// Adds interaction
	AddInteraction(InteractionType::itButton, buttonNo);
	SetInteraction(0);

	return buttonNo;
}

void Menu::ClearButtons()
{
	Buttons.clear();
}

void Menu::AddButtonText(float addX, float addY, const std::string& addText)
{
	RGB colRGB = {1.0f, 1.0f, 1.0f};
	AddButtonText(addX, addY, colRGB, addText);
}

void Menu::AddButtonText(float addX, float addY, const RGB& colRGB, 
						const std::string& addText)
{
	if (Buttons.empty())
		return;

	MenuButton& button = Buttons[Buttons.size() - 1];
	MenuText text(button.PosX + addX, button.PosY + addY, addText);
	text.ColRGB = colRGB;
	text.Int = 1.0f;
	button.Texts.push_back(text);
}

void Menu::AddButtonText(float addX, float addY, const RGB& colRGB, 
						int font, int size, int align, const std::string& addText)
{
	if (Buttons.empty())
		return;

	MenuButton& button = Buttons[Buttons.size() - 1];
	AddButtonText(button, addX, addY, colRGB, font, size, align, addText);
}

void Menu::AddButtonText(MenuButton& button, 
						float addX, float addY, const RGB& colRGB, 
						int font, int size, int align, const std::string& addText)
{
	MenuText text(button.PosX + addX, button.PosY + addY, addText);
	text.ColRGB = colRGB;
	text.Int = 1.0f;
	text.Style = font;
	text.Size = (float) size;
	text.Align = align;
	button.Texts.push_back(text);
}

int Menu::AddSelectSlide(const ThemeSelectSlide& themeSelectSlide, uint32 * pData,
	const std::string* values, size_t valueCount)
{
	int result = AddSelectSlide((float) themeSelectSlide.X, (float) themeSelectSlide.Y, 
		(float) themeSelectSlide.W, (float) themeSelectSlide.H,
		(float) themeSelectSlide.SkipX, (float) themeSelectSlide.SBGW,
		themeSelectSlide.ColRGB, themeSelectSlide.Int,
		themeSelectSlide.DColRGB, themeSelectSlide.DInt,
		themeSelectSlide.TColRGB, themeSelectSlide.TInt,
		themeSelectSlide.TDColRGB, themeSelectSlide.TDInt,
		themeSelectSlide.SBGColRGB, themeSelectSlide.SBGInt,
		themeSelectSlide.SBGDColRGB, themeSelectSlide.SBGDInt,
		themeSelectSlide.STColRGB, themeSelectSlide.STInt,
		themeSelectSlide.STDColRGB, themeSelectSlide.STDInt,
		sSkins.GetTextureFileName(themeSelectSlide.Tex), themeSelectSlide.Type,
		sSkins.GetTextureFileName(themeSelectSlide.TexSBG), themeSelectSlide.TypeSBG,
		themeSelectSlide.Text, pData);

	if (result < 0)
		return result;

	for (size_t i = 0; i < valueCount; i++)
		AddSelectSlideOption(values[i]);

	MenuSelectSlide& slide = SelectSlides[SelectSlides.size() - 1];

	slide.Text.Size = (float) themeSelectSlide.TextSize;
	slide.Tex.Z = themeSelectSlide.Z;
	slide.TexSBG.Z = themeSelectSlide.Z;
	slide.Tex_SelectS_ArrowL.Z = themeSelectSlide.Z;
	slide.Tex_SelectS_ArrowR.Z = themeSelectSlide.Z;

	slide.ShowArrows = themeSelectSlide.ShowArrows;
	slide.OneItemOnly = themeSelectSlide.OneItemOnly;

	// Generate lines
	slide.GenerateLines();
	slide.SetSelectOpt(slide.GetSelectOpt()); // refresh

	return result;
}

int Menu::AddSelectSlide(float x, float y, float w, float h, float skipX, float sbgw,
	const RGB& colRGB, float intensity,
	const RGB& dColRGB, float dIntensity,
	const RGB& tColRGB, float tIntensity,
	const RGB& tdColRGB, float tdIntensity,
	const RGB& sbgColRGB, float sbgIntensity,
	const RGB& sbgdColRGB, float sbgdIntensity,
	const RGB& stColRGB, float stIntensity,
	const RGB& stdColRGB, float stdIntensity,
	const path* texturePath,
	eTextureType textureType,
	const path* sbgTexturePath,
	eTextureType sbgTextureType,
	const std::string& caption, uint32 * pData)
{
	int slideNo = (int) SelectSlides.size();

	SelectSlides.push_back(MenuSelectSlide());
	MenuSelectSlide& slide = SelectSlides[slideNo];

	// Colorize hack
	if (textureType == TextureType::Colorized)
	{
		slide.Colorized = true;
		slide.Tex = sTextureMgr.GetTexture(texturePath, textureType, colRGB.ToUInt32());
		slide.DeselectTexture = sTextureMgr.GetTexture(texturePath, textureType, dColRGB.ToUInt32());
	}
	else
	{
		slide.Colorized = false;
		slide.Tex = sTextureMgr.GetTexture(texturePath, textureType);
		slide.ColRGB = colRGB;
		slide.DColRGB = dColRGB;
	}

	slide.Int = intensity;
	slide.DInt = dIntensity;

	slide.Tex.X = x;
	slide.Tex.Y = y;
	slide.Tex.W = w;
	slide.Tex.H = h;

	if (textureType == TextureType::Colorized)
	{
		slide.ColorizedSBG = true;
		slide.TexSBG = sTextureMgr.GetTexture(sbgTexturePath, sbgTextureType, sbgColRGB.ToUInt32());
		slide.DeselectTextureSBG = sTextureMgr.GetTexture(sbgTexturePath, sbgTextureType, sbgdColRGB.ToUInt32());
	}
	else
	{
		slide.ColorizedSBG = false;
		slide.TexSBG = sTextureMgr.GetTexture(sbgTexturePath, sbgTextureType);
		slide.SBGColRGB = sbgColRGB;
		slide.SBGDColRGB = sbgdColRGB;
	}

	slide.SBGInt = sbgIntensity;
	slide.SBGDInt = sbgdIntensity;

	slide.Tex_SelectS_ArrowL = TexSelectSArrowL;
	slide.Tex_SelectS_ArrowL.X = x + w + skipX;
	slide.Tex_SelectS_ArrowL.Y = y + (h - TexSelectSArrowL.H) / 2;
	slide.Tex_SelectS_ArrowL.W = TexSelectSArrowL.W;
	slide.Tex_SelectS_ArrowL.H = TexSelectSArrowL.H;

	slide.Tex_SelectS_ArrowR = TexSelectSArrowR;
	slide.Tex_SelectS_ArrowR.X = x + w + skipX + sbgw - TexSelectSArrowR.W;
	slide.Tex_SelectS_ArrowR.Y = y + (h - TexSelectSArrowR.H) / 2;
	slide.Tex_SelectS_ArrowR.W = TexSelectSArrowR.W;
	slide.Tex_SelectS_ArrowR.H = TexSelectSArrowR.H;

	slide.TexSBG.X = x + w + skipX;
	slide.TexSBG.Y = y;
	slide.TexSBG.W = sbgw;
	slide.TexSBG.H = h;

	slide.Text.X = x + 20.0f;
	slide.Text.Y = y + (slide.TexSBG.H / 2) - 15.0f;
	slide.Text.SetText(caption);
	slide.Text.Size = 30.0f;
	slide.Text.Visible = true;

	slide.TColRGB = tColRGB;
	slide.TInt = tIntensity;
	slide.TDColRGB = tdColRGB;
	slide.TDInt = tdIntensity;

	slide.STColRGB = stColRGB;
	slide.STInt = stIntensity;
	slide.STDColRGB = stdColRGB;
	slide.STDInt = stdIntensity;

	slide.Tex.TexX1 = 0;
	slide.Tex.TexY1 = 0;
	slide.Tex.TexX2 = 1;
	slide.Tex.TexY2 = 1;

	slide.TexSBG.TexX1 = 0;
	slide.TexSBG.TexY1 = 0;
	slide.TexSBG.TexX2 = 1;
	slide.TexSBG.TexY2 = 1;
	
	slide.pData = pData;
	slide.SetSelectOpt(*pData);

	// Disables default selection
	slide.SetSelected(false);

	// Adds interaction
	AddInteraction(InteractionType::itSelectSlide, slideNo);

	return slideNo;
}

void Menu::AddSelectSlideOption(const std::string& addText)
{
	AddSelectSlideOption(SelectSlides.size() - 1, addText);
}

void Menu::AddSelectSlideOption(uint32 selectNum, const std::string& addText)
{
	assert(selectNum < SelectSlides.size());
	SelectSlides[selectNum].TextOptionNames.push_back(addText);
}

void Menu::UpdateSelectSlideOptions(const ThemeSelectSlide& themeSelectSlide, uint32 selectNum, 
									const std::vector<std::string>& values, int data)
{
	assert(selectNum < SelectSlides.size());
	MenuSelectSlide& slide = SelectSlides[selectNum];
	for (std::vector<std::string>::const_iterator itr = values.begin(); itr != values.end(); ++itr)
		AddSelectSlideOption(selectNum, *itr);

	slide.GenerateLines();
	slide.SetSelectOpt(data);
}

void Menu::FadeTo(Menu* screen)
{
	sDisplay.Fade = 0.0f;
	sDisplay.NextScreen = screen;
}

void Menu::FadeTo(Menu* screen, /*AudioPlaybackStream*/void * playbackStream)
{
	FadeTo(screen);

	// TODO
	// AudioPlayback.PlaySound(playbackStream);
}

void Menu::CheckFadeTo(Menu* screen, const char * message)
{
	sDisplay.Fade = 0.0f;
	sDisplay.NextScreenWithCheck = screen;
	sDisplay.CheckOK = false;

	UIPopupCheck->ShowPopup(message, &Menu::CheckFadeToCallback);
	// ScreenPopupCheck.ShowPopup(message, OnSaveEncodingError, NULL, false);
}

void Menu::CheckFadeToCallback(bool value)
{
	sDisplay.CheckOK = value;
	if (!value)
	{
		sDisplay.NextScreenWithCheck = NULL;
		return;
	}

	// Hack to finish sing screen correctly on exit with Q shortcut
	if (sDisplay.NextScreenWithCheck == NULL)
	{
		if (sDisplay.CurrentScreen == UISing)
			UISing->Finish();
	}
}

void Menu::DrawBG()
{
	assert(Background != NULL);
	Background->Draw();
}

void Menu::DrawFG()
{
	for (std::vector<MenuStatic>::iterator itr = Statics.begin(); itr != Statics.end(); ++itr)
		(*itr).Draw();

	for (std::vector<MenuText>::iterator itr = Texts.begin(); itr != Texts.end(); ++itr)
		(*itr).Draw();

	for (std::vector<MenuButtonCollection>::iterator itr = ButtonCollections.begin(); itr != ButtonCollections.end(); ++itr)
		(*itr).Draw();

	for (std::vector<MenuButton>::iterator itr = Buttons.begin(); itr != Buttons.end(); ++itr)
		(*itr).Draw();

	for (std::vector<MenuSelectSlide>::iterator itr = SelectSlides.begin(); itr != SelectSlides.end(); ++itr)
		(*itr).Draw();
}

void Menu::Draw()
{
	DrawBG();
	DrawFG();
}

bool Menu::ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	/* do nothing */
	return true;
}

bool Menu::ParseTextInput(Uint32 inputType, SDL_Event * event)
{
	/* do nothing */
	return true;
}

// default mouse parsing: clicking generates return keypress,
// mousewheel selects in select slide
// override ParseMouse to customize
bool Menu::ParseMouse(int mouseButton, bool btnDown, float x, float y)
{
	bool result = true;

	// // If RightMbESC is set, send ESC keypress
	if (RightMbESC 
		&& mouseButton == SDL_BUTTON_RIGHT
		&& btnDown)
	{
		result = ParseInput(SDLK_ESCAPE, 0, true); // FIXME: SDLK_ESCAPE is a keycode...
	}

	// Transfer mouse-coords to the render raster we use to draw (800x600 by default)
	x = (x / (ScreenW / Screens)) * RenderW;
	if (x > RenderW)
		x -= RenderW;
	y = (y / ScreenH) * RenderH;
	if (y > RenderH)
		y -= RenderH;

	// Always go to the next screen if we don't have any interactions
	if (Interactions.empty())
	{
		if (btnDown && mouseButton == SDL_BUTTON_LEFT)
			result = ParseInput(SDLK_RETURN, 0, true); // FIXME: SDLK_RETURN is a keycode...
	}
	else
	{
		int nBut = InteractAt(x, y);
		if (nBut >= 0)
		{
			// Select on mouse-over
			if (nBut != SelInteraction)
				SetInteraction(nBut);

			assert(nBut < (int) Interactions.size());

			MouseClickAction action = maNone;
			MenuInteract& interact = Interactions[nBut];

			if (btnDown)
			{
				switch (mouseButton)
				{
				// Click button or select slider
				case SDL_BUTTON_LEFT:
					if (interact.Type == InteractionType::itSelectSlide)
					{
						assert(interact.Num >= 0 && interact.Num < (int) SelectSlides.size());
						action = SelectSlides[interact.Num].OnClick((float) x, (float) y);
					}
					else
					{
						action = maReturn;
					}
					break;

				// NOTE: As of SDL1.3/2, the mouse wheel is no longer considered a button; 
				// it has its own event which needs to be (and is) handled. These are user events.

				// Forward on select slide with mousewheel
				case SDL_BUTTON_WHEELDOWN:
					if (interact.Type == InteractionType::itSelectSlide)
						action = maRight;
					break;

				// Backward on select slide with mousewheel
				case SDL_BUTTON_WHEELUP:
					if (interact.Type == InteractionType::itSelectSlide)
						action = maLeft;
					break;
				}
			}

			switch (action)
			{
			// FIXME: SDLK_RETURN/SDLK_LEFT/SDLK_RIGHT are keycodes...
			case maReturn: return ParseInput(SDLK_RETURN, 0, true);
			case maLeft:   return ParseInput(SDLK_LEFT, 0, true);
			case maRight:  return ParseInput(SDLK_RIGHT, 0, true);
			}
		}
		else // not hovering over anything we could identify so far
		{
			// If hovering over button collection that is not already selected
			// we should select the first child but not allow clicks.
			nBut = CollectionAt((float) x, (float) y);
			if (nBut >= 0)
			{
				assert(nBut < (int) ButtonCollections.size());
				MenuButtonCollection& collection = ButtonCollections[nBut];
				if (!collection.IsSelected())
				{
					nBut = collection.FirstChild - 1;
					if (nBut != SelInteraction)
						SetInteraction(nBut);
				}
			}
		}
	}

	return result;
}

bool Menu::InRegion(float x, float y, const MouseOverRect& rect)
{
	// check whether the mouse over rect contains X and Y
	return (x >= rect.X) && (x <= rect.X + rect.W) && (y >= rect.Y) && (y <= rect.Y + rect.H);
}

// Takes x,y coordinates and returns the interaction number 
// of the control at this position
int Menu::InteractAt(float x, float y)
{
	int index = 0;
	for (std::vector<MenuInteract>::iterator itr = Interactions.begin(); itr != Interactions.end(); ++itr, ++index)
	{
		switch ((*itr).Type)
		{
			case InteractionType::itButton:
			{
				assert((*itr).Num >= 0 && (*itr).Num < (int) Buttons.size());
				MenuButton& button = Buttons[(*itr).Num];
				if (button.Visible
					&& InRegion(x, y, button.GetMouseOverRect()))
					return index;
			} break;

			case InteractionType::itBCollectionChild:
			{
				assert((*itr).Num >= 0 && (*itr).Num < (int) Buttons.size());
				MenuButton& button = Buttons[(*itr).Num];
				if (InRegion(x, y, button.GetMouseOverRect()))
					return index;
			} break;

			case InteractionType::itSelectSlide:
			{
				assert((*itr).Num >= 0 && (*itr).Num < (int) SelectSlides.size());
				MenuSelectSlide& slide = SelectSlides[(*itr).Num];
				if (InRegion(x, y, slide.GetMouseOverRect()))
					return index;
			} break;
		}
	}

	return -1;
}

// Takes x,y coordinates and returns the button collection ID
int Menu::CollectionAt(float x, float y)
{
	int index = 0;
	for (std::vector<MenuButtonCollection>::iterator itr = ButtonCollections.begin(); itr != ButtonCollections.end(); ++itr, ++index)
	{
		if ((*itr).Visible
			&& InRegion(x, y, (*itr).GetMouseOverRect()))
			return index;
	}

	return -1;
}

void Menu::OnShow()
{
  // FIXME: this needs some work. First, there should be a variable like
  // VideoBackground so we can check whether a video-background is enabled or not.
  // Second, a video should be stopped if the screen is hidden, but the Video.Stop()
  // method is not implemented by now. This is necessary for theme-switching too.
  // At the moment videos cannot be turned off without restarting USDX.

	if (Background == NULL)
		AddBackground(DEFAULT_BACKGROUND);

	assert(Background != NULL);
	Background->OnShow();
}

void Menu::OnShowFinish()
{
	/* do nothing */
}

void Menu::OnHide()
{
	assert(Background != NULL);
	Background->OnFinish();
}

void Menu::SetAnimationProgress(float progress)
{
	/* do nothing */
}

bool Menu::IsSelectable(uint32 index)
{
	assert(index < Interactions.size());
	MenuInteract& interact = Interactions[index];
	
	switch (interact.Type)
	{
	case InteractionType::itButton:
		assert(interact.Num >= 0 && interact.Num < (int) Buttons.size());
		return Buttons[interact.Num].Visible && Buttons[interact.Num].Selectable;

	case InteractionType::itSelectSlide:
		assert(interact.Num >= 0 && interact.Num < (int) SelectSlides.size());
		return SelectSlides[interact.Num].Visible;

	case InteractionType::itBCollectionChild:
		assert(interact.Num >= 0 && interact.Num < (int) Buttons.size());
		assert(Buttons[interact.Num].Parent - 1 >= 0 && Buttons[interact.Num].Parent - 1 < (int) ButtonCollections.size());
		if (ButtonCollections[Buttons[interact.Num].Parent - 1].FirstChild - 1 != index)
			return false;

		if ((interact.Type != InteractionType::itBCollectionChild
			|| Buttons[interact.Num].Parent != Buttons[Interactions[SelInteraction].Num].Parent))
			return true;

		return false;

	}

	return true; // should this default to false? officially it defaults to true.
}

void Menu::InteractNext()
{
	int selInt = SelInteraction;

	// change interaction as long as it's needed
	do
	{
		selInt = (selInt + 1) % Interactions.size();

		// If no Interaction is selectable simply select next
		if (selInt == SelInteraction)
			break;

	} while (!IsSelectable(selInt));

	SetInteraction(selInt);
}

void Menu::InteractCustom(int customSwitch)
{
	// FIXME: Original doesn't work with button collections, so it (the original) was commented out.
}

void Menu::InteractPrev()
{
	int selInt = SelInteraction;

	// change interaction as long as it's needed
	do
	{
		if (--selInt < 0)
			selInt = Interactions.size() - 1;

		// If no Interaction is selectable simply select next
		if (selInt == SelInteraction)
			break;

	} while (!IsSelectable(selInt));

	SetInteraction(selInt);
}

void Menu::InteractInc()
{
	int num, value;

	assert(SelInteraction >= 0 && SelInteraction < (int) Interactions.size());
	MenuInteract& interact = Interactions[SelInteraction];

	switch (interact.Type)
	{
	case InteractionType::itSelectSlide:
		num = interact.Num;
		assert(num >= 0 && num < (int) SelectSlides.size());
		value = SelectSlides[num].GetSelectOpt() + 1;
		if (value < (int) SelectSlides[num].TextOptionNames.size())
			SelectSlides[num].SetSelectOpt(value);
		break;

	case InteractionType::itBCollectionChild:
		// Select next button in collection
		num = 1;
		for (std::vector<MenuButton>::iterator itr = Buttons.begin(); itr != Buttons.end(); ++itr, ++num)
		{
			value = (SelInteraction + num) % Buttons.size();
			assert(value >= 0 && value < (int) Buttons.size());

			if (value == 0)
			{
				InteractNext();
				break;
			}

			if (Buttons[value].Parent == Buttons[SelInteraction].Parent)
			{
				SetInteraction(value);
				break;
			}
		}
		break;

	default:
		// Interact next if there is nothing to change
		InteractNext();
		break;
	}
}

void Menu::InteractDec()
{
	int num, value;
	uint8 parent;

	assert(SelInteraction >= 0 && SelInteraction < (int) Interactions.size());
	MenuInteract& interact = Interactions[SelInteraction];

	switch (interact.Type)
	{
	case InteractionType::itSelectSlide:
		num = interact.Num;
		assert(num >= 0 && num < (int) SelectSlides.size());
		value = SelectSlides[num].GetSelectOpt() - 1;
		if (value >= 0)
			SelectSlides[num].SetSelectOpt(value);
		break;

	case InteractionType::itBCollectionChild:
		// Select previous button in collection
		num = Buttons.size() - 1;
		for (std::vector<MenuButton>::reverse_iterator itr = Buttons.rbegin(); itr != Buttons.rend(); ++itr, --num)
		{
			value = (SelInteraction + num) % Buttons.size();
			assert(value >= 0 && value < (int) Buttons.size());

			if (value == (Buttons.size() - 1))
			{
				InteractPrev();
				break;
			}

			if (Buttons[value].Parent == Buttons[SelInteraction].Parent)
			{
				SetInteraction(value);
				break;
			}
		}
		break;

	default:
		// Interact prev if there is nothing to change
		InteractPrev();

		// If the button collection has more than one entry than select the last entry
		assert(SelInteraction >= 0 && SelInteraction < (int) Interactions.size());
		num = interact.Num;
		assert(num >= 0 && num < (int) Buttons.size());
		if (Buttons[num].Parent != 0)
		{
			parent = Buttons[num].Parent;
			assert(parent > 0 && parent <= ButtonCollections.size());
			if (ButtonCollections[parent - 1].ChildCount > 1)
			{
				// Select last child
				num = Buttons.size() - 1;
				for (std::vector<MenuButton>::reverse_iterator itr = Buttons.rbegin(); itr != Buttons.rend(); ++itr, --num)
				{
					value = (SelInteraction + num) % Buttons.size();
					assert(value >= 0 && value < (int) Buttons.size());

					if (Buttons[value].Parent == Buttons[SelInteraction].Parent)
					{
						SetInteraction(value);
						break;
					}
				}
			}
		}
		break;
	}
}

void Menu::InteractNextRow()
{
	int selInt = SelInteraction + (int) std::ceil(Interactions.size() * 0.50f);

	// set interaction
	if (selInt >= 0 && selInt < (int) Interactions.size())
		SetInteraction(selInt);
}

// implemented for the sake of usablility
// [cursor down] picks the button left to the actual atm
// this behaviour doesn't make sense for two rows of buttons
void Menu::InteractPrevRow()
{
	int selInt = SelInteraction - (int) std::ceil(Interactions.size() * 0.50f);

	// set interaction
	if (selInt >= 0 && selInt < (int) Interactions.size())
		SetInteraction(selInt);
}

void Menu::AddBox(float x, float y, float w, float h)
{
	RGB colRGB1 = {0.0f, 0.0f, 0.0f}, colRGB2 = {1.0f, 1.0f, 1.0f};
	const path* textureFilename = sSkins.GetTextureFileName("MainBar");

	AddStatic(x,   y,   w,   h,   colRGB1, textureFilename, TextureType::Colorized);
	AddStatic(x+2, y+2, w-4, h-4, colRGB2, textureFilename, TextureType::Colorized);
}

Menu::~Menu()
{
}
