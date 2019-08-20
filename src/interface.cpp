/*
"LettersFall" - Open-source cross-platform educational word spelling game.
Copyright (C) 2019  Jesse Palser

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Email the author at: "JessePalser@Gmail.com"
*/
#include <stdio.h>
#include <cstring>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "interface.h"

#include "input.h"
#include "visuals.h"
#include "screens.h"
#include "audio.h"
#include "logic.h"

extern Input* input;
extern Visuals* visuals;
extern Screens* screens;
extern Audio* audio;
extern Logic* logic;

//-------------------------------------------------------------------------------------------------
Interface::Interface(void)
{
    for (int index = 0; index < NumberOfButtons; index++)
    {
        ButtonGUIs[index].SpriteIndex = 0;
        ButtonGUIs[index].ScreenIndex = -1;
        ButtonGUIs[index].Scale = 1;
        ButtonGUIs[index].ScreenY = -100;
        ButtonGUIs[index].AnimationTimer = -1;
    }

    for (int index = 0; index < NumberOfArrowSets; index++)
    {
        ArrowSetGUIs[index].LeftArrowScreenIndex = -1;
        ArrowSetGUIs[index].LeftArrowScale = 1;
        ArrowSetGUIs[index].RightArrowScale = 1;
        ArrowSetGUIs[index].ScreenY = -100;
        ArrowSetGUIs[index].AnimationTimer = -1;
    }

    for (int index = 0; index < NumberOfIcons; index++)
    {
        IconGUIs[index].Index = -1;
        IconGUIs[index].SpriteIndex = -1;
        IconGUIs[index].Scale = 1;
        IconGUIs[index].ScreenX = -100;
        IconGUIs[index].ScreenY = -100;
        IconGUIs[index].AnimationTimer = -1;
    }

    ButtonSelectedByKeyboard = -1;
    ButtonSelectedByPlayer = -1;

    ArrowSetSelectedByKeyboard = -1;
    ArrowSetArrowSelectedByPlayer = -1;

    IconSelectedByPlayer = -1;
}

//-------------------------------------------------------------------------------------------------
Interface::~Interface(void)
{

}

//-------------------------------------------------------------------------------------------------
void Interface::CreateButton(Uint16 SpriteIndex, Uint8 ScreenIndex, int ScreenY)
{
    ButtonSelectedByKeyboard = 0;
    ButtonSelectedByPlayer = -1;

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (ButtonGUIs[index].ScreenIndex == -1)
        {
            ButtonGUIs[index].SpriteIndex = SpriteIndex;
            ButtonGUIs[index].ScreenIndex = ScreenIndex;
            ButtonGUIs[index].Scale = 1;
            ButtonGUIs[index].ScreenY = ScreenY;
            ButtonGUIs[index].AnimationTimer = -1;

            index = NumberOfButtons;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::DisplayAllButtonsOntoScreenBuffer(void)
{
    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (ButtonGUIs[index].ScreenIndex != -1)
        {
            visuals->Sprites[ ButtonGUIs[index].SpriteIndex ].ScreenX = 320;
            visuals->Sprites[ ButtonGUIs[index].SpriteIndex ].ScreenY = ButtonGUIs[index].ScreenY;
            visuals->Sprites[ ButtonGUIs[index].SpriteIndex ].ScaleX = ButtonGUIs[index].Scale;
            visuals->Sprites[ ButtonGUIs[index].SpriteIndex ].ScaleY = ButtonGUIs[index].Scale;
            visuals->DrawSpriteOntoScreenBuffer(ButtonGUIs[index].SpriteIndex);

            if (ButtonSelectedByKeyboard == ButtonGUIs[index].ScreenIndex && ButtonGUIs[1].ScreenIndex != -1)
            {
                visuals->Sprites[1000].ScreenX = 320-150;
                visuals->Sprites[1000].ScreenY = ButtonGUIs[index].ScreenY;
                visuals->Sprites[1000].ScaleX = 1;
                visuals->Sprites[1000].ScaleY = 1;
                visuals->DrawSpriteOntoScreenBuffer(1000);

                visuals->Sprites[1001].ScreenX = 320+150;
                visuals->Sprites[1001].ScreenY = ButtonGUIs[index].ScreenY;
                visuals->Sprites[1001].ScaleX = 1;
                visuals->Sprites[1001].ScaleY = 1;
                visuals->DrawSpriteOntoScreenBuffer(1001);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::ProcessAllButtons(void)
{
    Sint8 MinButtonIndex = 0;
    Sint8 MaxButtonIndex = -1;
    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (ButtonGUIs[index].ScreenIndex > MaxButtonIndex)
            MaxButtonIndex = ButtonGUIs[index].ScreenIndex;
    }

    if (input->JoystickDirectionVertical[Any] == UP)
    {
        if (ButtonSelectedByKeyboard > MinButtonIndex)  ButtonSelectedByKeyboard--;
        else  ButtonSelectedByKeyboard = MaxButtonIndex;

        screens->ScreenIsDirty = true;
        input->DelayAllUserInput = 10;
        audio->PlayDigitalSoundFX(0, 0);
    }
    else if (input->JoystickDirectionVertical[Any] == DOWN)
    {
        if (ButtonSelectedByKeyboard < MaxButtonIndex)  ButtonSelectedByKeyboard++;
        else  ButtonSelectedByKeyboard = MinButtonIndex;

        screens->ScreenIsDirty = true;
        input->DelayAllUserInput = 10;
        audio->PlayDigitalSoundFX(0, 0);
    }
    else if (  input->JoystickButtonOne[Any] == ON ||
            ( (input->SpacebarKeyPressed == true || input->EnterKeyPressed == true) )  )
    {
        if (input->SpacebarKeyPressed == true && screens->ScreenToDisplay == NameInputMouseKeyboardScreen)
        {
        }
        else
        {
            ButtonGUIs[ButtonSelectedByKeyboard].Scale = 0.9f;
            ButtonGUIs[ButtonSelectedByKeyboard].AnimationTimer = 10;
            screens->ScreenIsDirty = true;
            input->DelayAllUserInput = 50;

            audio->PlayDigitalSoundFX(0, 0);

        }
    }

    if (input->MouseButtonPressed[0] == true)
    {
        for (int index = 0; index < NumberOfButtons; index++)
        {
            if (ButtonGUIs[index].ScreenIndex != -1)
            {
                if (  ( input->MouseY > (ButtonGUIs[index].ScreenY - 20) )
                   && ( input->MouseY < (ButtonGUIs[index].ScreenY + 20) )
                   && ( input->MouseX > (320 - 125) )
                   && ( input->MouseX < (320 + 125) )  )
                {
                    ButtonSelectedByKeyboard = ButtonGUIs[index].ScreenIndex;
                    ButtonGUIs[index].Scale = 0.9f;
                    ButtonGUIs[index].AnimationTimer = 10;
                    screens->ScreenIsDirty = true;
                    input->DelayAllUserInput = 20;

                    audio->PlayDigitalSoundFX(0, 0);

                }
            }
        }
    }

    for (int index = 0; index < NumberOfButtons; index++)
    {
        if (ButtonGUIs[index].ScreenIndex != -1)
        {
            if (ButtonGUIs[index].AnimationTimer > -1)
            {
                ButtonGUIs[index].AnimationTimer--;

                if (ButtonGUIs[index].AnimationTimer == -1)
                {
                    ButtonSelectedByPlayer = ButtonSelectedByKeyboard;
                    screens->ScreenTransitionStatus = FadeOut;
                    ButtonGUIs[index].Scale = 1;
                    screens->ScreenIsDirty = true;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::DestroyAllButtons(void)
{
    for (int index = 0; index < NumberOfButtons; index++)
    {
        ButtonGUIs[index].SpriteIndex = 0;
        ButtonGUIs[index].ScreenIndex = -1;
        ButtonGUIs[index].Scale = 1;
        ButtonGUIs[index].ScreenY = -100;
        ButtonGUIs[index].AnimationTimer = -1;
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::CreateArrowSet(float ScreenIndex, int ScreenY)
{
    ArrowSetSelectedByKeyboard = 0;
    ArrowSetArrowSelectedByPlayer = -1;

    for (int index = 0; index < NumberOfArrowSets; index++)
    {
        if (ArrowSetGUIs[index].LeftArrowScreenIndex == -1)
        {
            ArrowSetGUIs[index].LeftArrowScreenIndex = ScreenIndex;
            ArrowSetGUIs[index].LeftArrowScale = 1;
            ArrowSetGUIs[index].RightArrowScale = 1;
            ArrowSetGUIs[index].ScreenY = ScreenY;
            ArrowSetGUIs[index].AnimationTimer = -1;

            index = NumberOfArrowSets;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::DisplayAllArrowSetsOntoScreenBuffer(void)
{
    for (int index = 0; index < NumberOfArrowSets; index++)
    {
        if (ArrowSetGUIs[index].LeftArrowScreenIndex != -1)
        {
            if (ArrowSetSelectedByKeyboard == ArrowSetGUIs[index].LeftArrowScreenIndex)
            {
                visuals->Sprites[1002].ScreenX = 320;
                visuals->Sprites[1002].ScreenY = ArrowSetGUIs[index].ScreenY;
                visuals->Sprites[1002].Smooth = true;
                visuals->Sprites[1002].Transparency = 150;
                visuals->DrawSpriteOntoScreenBuffer(1002);
            }

            visuals->Sprites[1001].ScreenX = 320-295;
            visuals->Sprites[1001].ScreenY = ArrowSetGUIs[index].ScreenY;
            visuals->Sprites[1001].ScaleX = ArrowSetGUIs[index].LeftArrowScale;
            visuals->Sprites[1001].ScaleY = ArrowSetGUIs[index].LeftArrowScale;
            visuals->DrawSpriteOntoScreenBuffer(1001);

            visuals->Sprites[1000].ScreenX = 320+295;
            visuals->Sprites[1000].ScreenY = ArrowSetGUIs[index].ScreenY;
            visuals->Sprites[1000].ScaleX = ArrowSetGUIs[index].RightArrowScale;
            visuals->Sprites[1000].ScaleY = ArrowSetGUIs[index].RightArrowScale;
            visuals->DrawSpriteOntoScreenBuffer(1000);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::ProcessAllArrowSets(void)
{
    Sint8 MinArrowSetIndex = 0;
    Sint8 MaxArrowSetIndex = -1;
    for (int index = 0; index < NumberOfArrowSets; index++)
    {
        if (ArrowSetGUIs[index].LeftArrowScreenIndex > MaxArrowSetIndex)
            MaxArrowSetIndex = static_cast<Sint8>(ArrowSetGUIs[index].LeftArrowScreenIndex);
    }

    if (input->JoystickDirectionVertical[Any] == UP)
    {
        if (ArrowSetSelectedByKeyboard > MinArrowSetIndex)  ArrowSetSelectedByKeyboard--;
        else  ArrowSetSelectedByKeyboard = MaxArrowSetIndex;

        screens->ScreenIsDirty = true;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }
    else if (input->JoystickDirectionVertical[Any] == DOWN)
    {
        if (ArrowSetSelectedByKeyboard < MaxArrowSetIndex)  ArrowSetSelectedByKeyboard++;
        else  ArrowSetSelectedByKeyboard = MinArrowSetIndex;

        screens->ScreenIsDirty = true;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }
    else if (input->JoystickDirectionHorizonal[Any] == LEFT)
    {
        ArrowSetGUIs[ArrowSetSelectedByKeyboard].LeftArrowScale = 0.8f;
        ArrowSetGUIs[ArrowSetSelectedByKeyboard].AnimationTimer = 10;

        screens->ScreenIsDirty = true;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }
    else if (input->JoystickDirectionHorizonal[Any] == RIGHT)
    {
        ArrowSetGUIs[ArrowSetSelectedByKeyboard].RightArrowScale = 0.8f;
        ArrowSetGUIs[ArrowSetSelectedByKeyboard].AnimationTimer = 10;

        screens->ScreenIsDirty = true;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }

    if (input->MouseButtonPressed[0] == true)
    {
        for (int index = 0; index < NumberOfArrowSets; index++)
        {
            if (ArrowSetGUIs[index].LeftArrowScreenIndex != -1)
            {
                if (  ( input->MouseY > (ArrowSetGUIs[index].ScreenY - 21) )
                   && ( input->MouseY < (ArrowSetGUIs[index].ScreenY + 21) )
                   && ( input->MouseX > (320-295-25) )
                   && ( input->MouseX < (320-295+25) )  )
                {
                    ArrowSetSelectedByKeyboard = static_cast<Uint8>(ArrowSetGUIs[index].LeftArrowScreenIndex);
                    ArrowSetGUIs[index].LeftArrowScale = 0.8f;
                    ArrowSetGUIs[index].AnimationTimer = 10;
                    screens->ScreenIsDirty = true;
                    input->DelayAllUserInput = 20;
                    audio->PlayDigitalSoundFX(0, 0);
                }
                else if (  ( input->MouseY > (ArrowSetGUIs[index].ScreenY - 21) )
                   && ( input->MouseY < (ArrowSetGUIs[index].ScreenY + 21) )
                   && ( input->MouseX > (320+295-25) )
                   && ( input->MouseX < (320+295+25) )  )
                {
                    ArrowSetSelectedByKeyboard = static_cast<Uint8>(ArrowSetGUIs[index].LeftArrowScreenIndex);
                    ArrowSetGUIs[index].RightArrowScale = 0.8f;
                    ArrowSetGUIs[index].AnimationTimer = 10;
                    screens->ScreenIsDirty = true;
                    input->DelayAllUserInput = 20;
                    audio->PlayDigitalSoundFX(0, 0);
                }
            }
        }
    }

    for (int index = 0; index < NumberOfArrowSets; index++)
    {
        if (ArrowSetGUIs[index].LeftArrowScreenIndex != -1)
        {
            if (ArrowSetGUIs[index].AnimationTimer > 0)  ArrowSetGUIs[index].AnimationTimer--;
            else if (ArrowSetGUIs[index].AnimationTimer == 0)
            {
                if (ArrowSetGUIs[index].LeftArrowScale < 1)
                {
                    ArrowSetGUIs[index].LeftArrowScale = 1;
                    ArrowSetArrowSelectedByPlayer = (float)ArrowSetSelectedByKeyboard;
                }
                else if (ArrowSetGUIs[index].RightArrowScale < 1)
                {
                    ArrowSetGUIs[index].RightArrowScale = 1;
                    ArrowSetArrowSelectedByPlayer = (float)ArrowSetSelectedByKeyboard+0.5f;
                }

                ArrowSetGUIs[index].AnimationTimer = -1;
                screens->ScreenIsDirty = true;
            }
        }
    }

}

//-------------------------------------------------------------------------------------------------
void Interface::DestroyAllArrowSets(void)
{
    for (int index = 0; index < NumberOfArrowSets; index++)
    {
        ArrowSetGUIs[index].LeftArrowScreenIndex = -1;
        ArrowSetGUIs[index].LeftArrowScale = 1;
        ArrowSetGUIs[index].RightArrowScale = 1;
        ArrowSetGUIs[index].ScreenY = -100;
        ArrowSetGUIs[index].AnimationTimer = -1;
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::CreateIcon(int spriteIndex, int screenX, int screenY)
{
    IconSelectedByPlayer = -1;

    for (int index = 0; index < NumberOfIcons; index++)
    {
        if (IconGUIs[index].Index == -1)
        {
            IconGUIs[index].Index = index;
            IconGUIs[index].SpriteIndex = spriteIndex;
            IconGUIs[index].ScreenX = screenX;
            IconGUIs[index].ScreenY = screenY;
            IconGUIs[index].AnimationTimer = -1;

            index = (NumberOfIcons+1);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::DisplayAllIconsOntoScreenBuffer(void)
{
    for (int index = 0; index < NumberOfIcons; index++)
    {
        if (IconGUIs[index].Index != -1)
        {
            visuals->Sprites[ IconGUIs[index].SpriteIndex ].ScreenX = IconGUIs[index].ScreenX;
            visuals->Sprites[ IconGUIs[index].SpriteIndex ].ScreenY = IconGUIs[index].ScreenY;
            visuals->Sprites[ IconGUIs[index].SpriteIndex ].ScaleX = IconGUIs[index].Scale;
            visuals->Sprites[ IconGUIs[index].SpriteIndex ].ScaleY = IconGUIs[index].Scale;
            visuals->DrawSpriteOntoScreenBuffer(IconGUIs[index].SpriteIndex);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::ProcessAllIcons(void)
{
    if (input->MouseButtonPressed[0] == true)
    {
        for (int index = 0; index < NumberOfIcons; index++)
        {
            if (IconGUIs[index].Index != -1)
            {
                if (   (  input->MouseY > ( IconGUIs[index].ScreenY - (visuals->Sprites[ IconGUIs[index].SpriteIndex ].TextureHeightOriginal/2) )  )
                   && (  input->MouseY < ( IconGUIs[index].ScreenY + (visuals->Sprites[ IconGUIs[index].SpriteIndex ].TextureHeightOriginal/2) )  )
                   && (  input->MouseX > ( IconGUIs[index].ScreenX - (visuals->Sprites[ IconGUIs[index].SpriteIndex ].TextureWidthOriginal/2) )  )
                   && (  input->MouseX < ( IconGUIs[index].ScreenX + (visuals->Sprites[ IconGUIs[index].SpriteIndex ].TextureWidthOriginal/2) )  )   )
                {
                    IconGUIs[index].Scale = 0.9f;
                    IconGUIs[index].AnimationTimer = 10;
                    screens->ScreenIsDirty = true;
                    input->DelayAllUserInput = 20;

                    audio->PlayDigitalSoundFX(0, 0);
                }
            }
        }
    }

    for (int index = 0; index < NumberOfIcons; index++)
    {
        if (IconGUIs[index].Index != -1)
        {
            if (IconGUIs[index].AnimationTimer > -1)
            {
                IconGUIs[index].AnimationTimer--;

                if (IconGUIs[index].AnimationTimer == -1)
                {
                    IconSelectedByPlayer = IconGUIs[index].Index;

                    IconGUIs[index].Scale = 1;
                    screens->ScreenIsDirty = true;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Interface::DestroyAllIcons()
{
    for (int index = 0; index < NumberOfIcons; index++)
    {
        IconGUIs[index].Index = -1;
        IconGUIs[index].SpriteIndex = -1;
        IconGUIs[index].Scale = 1;
        IconGUIs[index].ScreenX = -100;
        IconGUIs[index].ScreenY = -100;
        IconGUIs[index].AnimationTimer = -1;
    }
}
