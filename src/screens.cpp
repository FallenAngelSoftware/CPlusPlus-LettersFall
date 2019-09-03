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

#include <cstdlib>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "screens.h"

#include "input.h"
#include "visuals.h"
#include "interface.h"
#include "data.h"
#include "logic.h"
#include "audio.h"

extern Input* input;
extern Visuals* visuals;
extern Interface* interface;
extern Data* data;
extern Logic* logic;
extern Audio* audio;

//-------------------------------------------------------------------------------------------------
Screens::Screens(void)
{
    ScreenIsDirty = 1;
    ScreenToDisplay = SixteenBitSoftScreen;

    ScreenFadeTransparency = 255;
    ScreenTransitionStatus = FadeAll;
}

//-------------------------------------------------------------------------------------------------
Screens::~Screens(void)
{

}

//-------------------------------------------------------------------------------------------------
void Screens::ApplyScreenFadeTransition(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        visuals->ClearTextCache();
    }
    else if (ScreenTransitionStatus == FadeIn)
    {
        if (ScreenFadeTransparency > 24)
        {
            ScreenFadeTransparency -= 25;
            ScreenIsDirty = true;
        }
        else if (ScreenFadeTransparency != 0)
        {
            ScreenFadeTransparency = 0;
        }
        else
        {
            ScreenFadeTransparency = 0;
            ScreenTransitionStatus = FadeNone;
        }
    }
    else if (ScreenTransitionStatus == FadeOut)
    {
        if (ScreenFadeTransparency < 231)
        {
            ScreenFadeTransparency += 25;
            ScreenIsDirty = true;
        }
        else if (ScreenFadeTransparency != 255)
        {
            ScreenFadeTransparency = 255;

            interface->DestroyAllButtons();
            interface->DestroyAllArrowSets();
            interface->DestroyAllIcons();
        }
    }

    if (ScreenTransitionStatus != FadeNone)
    {
        visuals->Sprites[0].ScreenX = 320;
        visuals->Sprites[0].ScreenY = 240;
        visuals->Sprites[0].Transparency = ScreenFadeTransparency;
        visuals->DrawSpriteOntoScreenBuffer(0);
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplaySelectedBackround(void)
{
    visuals->Sprites[20+logic->SelectedBackground].ScreenX = 320;
    visuals->Sprites[20+logic->SelectedBackground].ScreenY = 240;
    visuals->DrawSpriteOntoScreenBuffer(20+logic->SelectedBackground);
}

//-------------------------------------------------------------------------------------------------
void Screens::ProcessScreenToDisplay(void)
{
int windowWidth;
int windowHeight;

    if (input->DEBUG == true)  ScreenIsDirty = true;

    SDL_GetWindowSize(visuals->Window, &windowWidth, &windowHeight);
    if (windowWidth != visuals->WindowWidthCurrent || windowHeight != visuals->WindowHeightCurrent)
    {
        visuals->WindowWidthCurrent = windowWidth;
        visuals->WindowHeightCurrent = windowHeight;

        ScreenIsDirty = true;
    }

    if (logic->MusicJukebox == true && Mix_PlayingMusic() == 0)
    {
        logic->SelectedMusicTrack = rand()%9;
        audio->PlayMusic(logic->SelectedMusicTrack, 0);
    }
    else if (Mix_PlayingMusic() == 0)  audio->PlayMusic(logic->SelectedMusicTrack, 0);

    switch(ScreenToDisplay)
    {
        case SixteenBitSoftScreen:
            DisplaySixteenBitSoftScreen();
            break;

        case SDLLogoScreen:
            DisplaySDLLogoScreen();
            break;

        case TitleScreen:
            DisplayTitleScreen();
            break;

        case OptionsScreen:
            DisplayOptionsScreen();
            break;

        case HowToPlayScreen:
            DisplayHowToPlayScreen();
            break;

        case HighScoresScreen:
            DisplayHighScoresScreen();
            break;

        case AboutScreen:
            DisplayAboutScreen();
            break;

        case PlayingGameScreen:
            DisplayPlayingGameScreen();
            break;

		case NameInputMouseKeyboardScreen:
			DisplayNameInputMouseKeyboardScreen();
			break;


        default:
            break;
    }

    ApplyScreenFadeTransition();

    if (logic->MusicJukebox == true)
    {
        if ( Mix_PlayingMusic() == 0 )
        {
            logic->SelectedMusicTrack = rand()%9;
            audio->PlayMusic(logic->SelectedMusicTrack, 1);
        }
    }

    if (input->DEBUG == true)
    {
        ScreenIsDirty = true;

        if ( ScreenToDisplay != PlayingGameScreen || (ScreenToDisplay == PlayingGameScreen && logic->PAUSEgame != true) )
        {
            char temp[256];
            strcpy(visuals->VariableText, "(");
            sprintf(temp, "%i", input->MouseX);
            strcat(visuals->VariableText, temp);
            strcat(visuals->VariableText, ",");
            sprintf(temp, "%i", input->MouseY);
            strcat(visuals->VariableText, temp);
            strcat(visuals->VariableText, ")");
            visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[2], 3, 442
                                              , JustifyLeft, 255, 255, 255, 0, 0, 0);

            sprintf(visuals->VariableText, "%d", visuals->AverageFPS);
            if (ScreenToDisplay != PlayingGameScreen)  strcat(visuals->VariableText, "/60");
            else if (logic->GameMode == ChildMode)  strcat(visuals->VariableText, "/30");
            else if (logic->GameMode == TeenagerMode)  strcat(visuals->VariableText, "/45");
            else if (logic->GameMode == AdultMode)  strcat(visuals->VariableText, "/60");
            else if (logic->GameMode == TimeAttack15MinuteMode)  strcat(visuals->VariableText, "/45");
            else if (logic->GameMode == StoryMode)  strcat(visuals->VariableText, "/45");
            visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[2], 3, 462
                                              , JustifyLeft, 255, 255, 255, 0, 0, 0);
        }
    }

    if (ScreenIsDirty > 0)
    {
        if (visuals->ForceAspectRatio == true)  SDL_RenderSetLogicalSize(visuals->Renderer, 640, 480);

        SDL_RenderPresent(visuals->Renderer);
        if (ScreenTransitionStatus != FadeIn && ScreenTransitionStatus != FadeOut)  ScreenIsDirty--;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplaySixteenBitSoftScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        ScreenDisplayTimer = 390;
        ScreenTransitionStatus = FadeIn;

        input->DelayAllUserInput = 30;
    }

    if (input->KeyOnKeyboardPressedByUser == 'D')
    {
        input->DEBUG = !input->DEBUG;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(1, 0);
    }

    if (input->MouseButtonPressed[0] == true
       || input->SpacebarKeyPressed == true
       || input->EnterKeyPressed == true)
    {
        ScreenDisplayTimer = 0;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }

    if (ScreenDisplayTimer > 0)  ScreenDisplayTimer--;
    else if (ScreenTransitionStatus != FadeIn)  ScreenTransitionStatus = FadeOut;

    if (ScreenIsDirty == 1)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        visuals->Sprites[1].ScreenX = 320;
        visuals->Sprites[1].ScreenY = 240;
        visuals->Sprites[1].ScaleX = .65;
        visuals->Sprites[1].ScaleY = .65;
        visuals->DrawSpriteOntoScreenBuffer(1);

        visuals->DrawTextOntoScreenBuffer("www.FallenAngelSoftware.com", visuals->Font[0]
                                          , 0, 447-3-10, JustifyCenter, 1, 255, 1, 1, 90, 1);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = SDLLogoScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplaySDLLogoScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        ScreenDisplayTimer = 390;
        ScreenTransitionStatus = FadeIn;
    }

    if (input->KeyOnKeyboardPressedByUser == 'L')
    {
        logic->RunLoop = true;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(1, 0);
    }

    if (input->MouseButtonPressed[0] == true
       || input->SpacebarKeyPressed == true
       || input->EnterKeyPressed == true)
    {
        ScreenDisplayTimer = 0;
        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }

    if (ScreenDisplayTimer > 0)  ScreenDisplayTimer--;
    else if (ScreenTransitionStatus != FadeIn)  ScreenTransitionStatus = FadeOut;

    if (ScreenIsDirty == 1)
    {
        visuals->ClearScreenBufferWithColor(200, 200, 255, 255);

        visuals->DrawTextOntoScreenBuffer("100% Cross-Platform Game Engine", visuals->Font[0]
                                          , 0, 50, JustifyCenter, 50, 65, 126, 255, 255, 255);

        visuals->Sprites[9].ScreenX = 320;
        visuals->Sprites[9].ScreenY = 240+20;
        visuals->Sprites[9].ScaleX = .25;
        visuals->Sprites[9].ScaleY = .25;
        visuals->DrawSpriteOntoScreenBuffer(9);

        visuals->DrawTextOntoScreenBuffer("www.LibSDL.org", visuals->Font[4]
                                          , 0, 400, JustifyCenter, 50, 65, 126, 255, 255, 255);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = TitleScreen;

        if (logic->MusicJukebox == true)
        {
            logic->SelectedMusicTrack = rand()%9;
            audio->PlayMusic(logic->SelectedMusicTrack, 1);
        }
        else  audio->PlayMusic(logic->SelectedMusicTrack, 0);
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayTitleScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        int buttonStartY = 198-3;
        int buttonOffsetY = 41;
        interface->CreateButton( 1003, 0, buttonStartY );
        interface->CreateButton( 1004, 1, buttonStartY + (buttonOffsetY*1) );
        interface->CreateButton( 1005, 2, buttonStartY + (buttonOffsetY*2) );
        interface->CreateButton( 1006, 3, buttonStartY + (buttonOffsetY*3) );
        interface->CreateButton( 1007, 4, buttonStartY + (buttonOffsetY*4) );
        interface->CreateButton( 1008, 5, buttonStartY + (buttonOffsetY*5) );

        if (data->NumberOfPlays > 4)  interface->CreateIcon(11, 33, 453);

        ScreenTransitionStatus = FadeIn;
    }

    interface->ProcessAllButtons();
    interface->ProcessAllIcons();

    if (interface->IconSelectedByPlayer == 0)
    {
        interface->IconSelectedByPlayer = -1;
        data->DisplayWebsite();
    }

    if (logic->RunLoop == true)
    {
        interface->ButtonSelectedByPlayer = 0;
        ScreenTransitionStatus = FadeOut;
        ScreenIsDirty = true;
    }

    if (ScreenIsDirty == 1)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        DisplaySelectedBackround();

        visuals->DrawTextOntoScreenBuffer("Retail3 110% v5.0+", visuals->Font[2]
                                          , 0, 0, JustifyCenter, 255, 255, 255, 0, 0, 0);

        visuals->Sprites[3].ScreenX = 320;
        visuals->Sprites[3].ScreenY = 73-3;
        visuals->Sprites[3].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(3);

        char temp[256];
        strcpy(visuals->VariableText, "''");
        strcat(visuals->VariableText, data->HighScoresName[logic->GameMode][0]);
        strcat(visuals->VariableText, "'' Scored: ");
		sprintf(temp, "%d", data->HighScoresScore[logic->GameMode][0]);
        strcat(visuals->VariableText, temp);

        visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 0, 140-5
                                          , JustifyCenter, 255, 255, 0, 0, 0, 0);

        interface->DisplayAllButtonsOntoScreenBuffer();
        interface->DisplayAllIconsOntoScreenBuffer();

		strcpy( visuals->VariableText, "©");
        strcat(visuals->VariableText, "2019, By Team Fallen Angel Software");
        visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0]
                                          , 0, 420-3, JustifyCenter, 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("www.FallenAngelSoftware.com", visuals->Font[0]
                                          , 0, 447-3, JustifyCenter, 255, 255, 255, 0, 0, 0);
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;

        if (interface->ButtonSelectedByPlayer == 1)  ScreenToDisplay = OptionsScreen;
        else if (interface->ButtonSelectedByPlayer == 2)  ScreenToDisplay = HowToPlayScreen;
        else if (interface->ButtonSelectedByPlayer == 3)  ScreenToDisplay = HighScoresScreen;
        else if (interface->ButtonSelectedByPlayer == 4)  ScreenToDisplay = AboutScreen;
        else if (interface->ButtonSelectedByPlayer == 5)  input->EXIT_Game = true;
        else if (interface->ButtonSelectedByPlayer == 0)  ScreenToDisplay = PlayingGameScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayOptionsScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        interface->CreateButton(1009, 0, 454);

        interface->CreateArrowSet(0, 60);
        interface->CreateArrowSet(1, 105);

        interface->CreateArrowSet(2, 160);

        interface->CreateArrowSet(3, 215);

        interface->CreateArrowSet(4, 260);

        interface->CreateArrowSet(5, 305);

        interface->CreateArrowSet(6, 350);


        ScreenTransitionStatus = FadeIn;
    }
/*
    if (input->CheckKeyCodeName("[F2]") == true && input->JoystickSetupProcess == JoySetupNotStarted)
    {
        ScreenIsDirty = true;
        audio->PlayDigitalSoundFX(1, 0);

        if (input->KeyboardSetupProcess == KeyboardSetupNotStarted)
        {
            input->KeyboardSetupProcess = KeyboardSetupPressOne;
        }
        else
        {
            input->KeyboardSetupProcess = KeyboardSetupNotStarted;

            input->UserDefinedKeyButtonOne = -1;
            input->UserDefinedKeyButtonTwo = -1;
            input->UserDefinedKeyUP = -1;
            input->UserDefinedKeyRIGHT = -1;
            input->UserDefinedKeyDOWN = -1;
            input->UserDefinedKeyLEFT = -1;
            input->UserDefinedKeyPause = -1;
        }
    }

    if (input->KeyboardSetupProcess == KeyboardSetupPressOne && input->CheckKeyCodeName("[F2]") != true
        && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyButtonOne = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupPressTwo;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }
    else if (input->KeyboardSetupProcess == KeyboardSetupPressTwo && input->CheckKeyCodeName("[F2]") != true
             && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyButtonTwo = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupPressUP;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }
    else if (input->KeyboardSetupProcess == KeyboardSetupPressUP && input->CheckKeyCodeName("[F2]") != true
             && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyUP = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupPressRIGHT;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }
    else if (input->KeyboardSetupProcess == KeyboardSetupPressRIGHT && input->CheckKeyCodeName("[F2]") != true
             && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyRIGHT = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupPressDOWN;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }
    else if (input->KeyboardSetupProcess == KeyboardSetupPressDOWN && input->CheckKeyCodeName("[F2]") != true
             && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyDOWN = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupPressLEFT;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }
    else if (input->KeyboardSetupProcess == KeyboardSetupPressLEFT && input->CheckKeyCodeName("[F2]") != true
             && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyLEFT = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupPressPause;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }
    else if (input->KeyboardSetupProcess == KeyboardSetupPressPause && input->CheckKeyCodeName("[F2]") != true
             && input->KeyOnKeyboardPressedByUser != -1)
    {
        input->UserDefinedKeyPause = input->KeyOnKeyboardPressedByUser;
        input->KeyboardSetupProcess = KeyboardSetupNotStarted;

        audio->PlayDigitalSoundFX(1, 0);
        input->DelayAllUserInput = 20;
        ScreenIsDirty = true;
    }

    if ( input->CheckKeyCodeName("[F1]") == true && input->JoystickDeviceOne != NULL
        && input->KeyboardSetupProcess == KeyboardSetupNotStarted )
    {
        ScreenIsDirty = true;
        audio->PlayDigitalSoundFX(1, 0);

        if (input->JoystickSetupProcess == JoySetupNotStarted)
        {
            input->JoystickSetupProcess = Joy1SetupPressUP;
        }
        else
        {
            input->JoystickSetupProcess = JoySetupNotStarted;

            for (int joy = 0; joy < 2; joy++)
            {
                input->JoyUP[joy] = Axis1;
                input->JoyDOWN[joy] = Axis1;
                input->JoyLEFT[joy] = Axis0;
                input->JoyRIGHT[joy] = Axis0;
                input->JoyButton1[joy] = Button0;
                input->JoyButton2[joy] = Button1;
            }
        }
    }

    int joyAction = -1;
    if (input->JoystickSetupProcess > Joy2SetupPressBUTTONTwo)
        joyAction = input->QueryJoysticksForAction(2);
    else if (input->JoystickSetupProcess > Joy1SetupPressBUTTONTwo)
        joyAction = input->QueryJoysticksForAction(1);
    else if (input->JoystickSetupProcess > JoySetupNotStarted)
        joyAction = input->QueryJoysticksForAction(0);

    if (joyAction > -1)
    {
        if (input->JoystickSetupProcess == Joy1SetupPressUP)
        {
            input->JoyUP[0] = joyAction;
            input->JoystickSetupProcess = Joy1SetupPressDOWN;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy1SetupPressDOWN)
        {
            input->JoyDOWN[0] = joyAction;
            input->JoystickSetupProcess = Joy1SetupPressLEFT;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy1SetupPressLEFT)
        {
            input->JoyLEFT[0] = joyAction;
            input->JoystickSetupProcess = Joy1SetupPressRIGHT;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy1SetupPressRIGHT)
        {
            input->JoyRIGHT[0] = joyAction;
            input->JoystickSetupProcess = Joy1SetupPressBUTTONOne;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy1SetupPressBUTTONOne)
        {
            input->JoyButton1[0] = joyAction;
            input->JoystickSetupProcess = Joy1SetupPressBUTTONTwo;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy1SetupPressBUTTONTwo)
        {
            input->JoyButton2[0] = joyAction;

            if (input->JoystickDeviceTwo)
                input->JoystickSetupProcess = Joy2SetupPressUP;
            else
                input->JoystickSetupProcess = JoySetupNotStarted;

            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy2SetupPressUP)
        {
            input->JoyUP[1] = joyAction;
            input->JoystickSetupProcess = Joy2SetupPressDOWN;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy2SetupPressDOWN)
        {
            input->JoyDOWN[1] = joyAction;
            input->JoystickSetupProcess = Joy2SetupPressLEFT;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy2SetupPressLEFT)
        {
            input->JoyLEFT[1] = joyAction;
            input->JoystickSetupProcess = Joy2SetupPressRIGHT;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy2SetupPressRIGHT)
        {
            input->JoyRIGHT[1] = joyAction;
            input->JoystickSetupProcess = Joy2SetupPressBUTTONOne;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy2SetupPressBUTTONOne)
        {
            input->JoyButton1[1] = joyAction;
            input->JoystickSetupProcess = Joy2SetupPressBUTTONTwo;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy2SetupPressBUTTONTwo)
        {
            input->JoyButton2[1] = joyAction;

            if (input->JoystickDeviceThree)
                input->JoystickSetupProcess = Joy3SetupPressUP;
            else
                input->JoystickSetupProcess = JoySetupNotStarted;

            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy3SetupPressUP)
        {
            input->JoyUP[2] = joyAction;
            input->JoystickSetupProcess = Joy3SetupPressDOWN;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy3SetupPressDOWN)
        {
            input->JoyDOWN[2] = joyAction;
            input->JoystickSetupProcess = Joy3SetupPressLEFT;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy3SetupPressLEFT)
        {
            input->JoyLEFT[2] = joyAction;
            input->JoystickSetupProcess = Joy3SetupPressRIGHT;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy3SetupPressRIGHT)
        {
            input->JoyRIGHT[2] = joyAction;
            input->JoystickSetupProcess = Joy3SetupPressBUTTONOne;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy3SetupPressBUTTONOne)
        {
            input->JoyButton1[2] = joyAction;
            input->JoystickSetupProcess = Joy3SetupPressBUTTONTwo;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
        else if (input->JoystickSetupProcess == Joy3SetupPressBUTTONTwo)
        {
            input->JoyButton2[2] = joyAction;
            input->JoystickSetupProcess = JoySetupNotStarted;
            audio->PlayDigitalSoundFX(0, 0);
            input->DelayAllUserInput = 20;
            ScreenIsDirty = true;
        }
    }
*/
    if (input->JoystickSetupProcess == JoySetupNotStarted
     && input->KeyboardSetupProcess == KeyboardSetupNotStarted)
    {
        interface->ProcessAllButtons();
        interface->ProcessAllArrowSets();

        if (interface->ArrowSetArrowSelectedByPlayer != -1)
        {
            if (interface->ArrowSetArrowSelectedByPlayer == 0)
            {
                if (audio->MusicVolume > 0)  audio->MusicVolume-=32;
                else  audio->MusicVolume = 128;

                Mix_VolumeMusic(audio->MusicVolume);
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 0.5)
            {
                if (audio->MusicVolume < 128)  audio->MusicVolume+=32;
                else  audio->MusicVolume = 0;

                Mix_VolumeMusic(audio->MusicVolume);
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 1)
            {
                if (audio->EffectsVolume > 0)  audio->EffectsVolume-=32;
                else  audio->EffectsVolume = 128;

                Mix_HaltChannel(-1);
                audio->PlayDigitalSoundFX(1, 0);
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 1.5)
            {
                if (audio->EffectsVolume < 128)  audio->EffectsVolume+=32;
                else  audio->EffectsVolume = 0;

                Mix_HaltChannel(-1);
                audio->PlayDigitalSoundFX(1, 0);
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 2)
            {
                visuals->FullScreenMode = !visuals->FullScreenMode;

                for (int index = 0; index < NumberOfSprites; index++)
                {
                    if (visuals->Sprites[index].Texture != NULL)
                    {
                        SDL_DestroyTexture(visuals->Sprites[index].Texture);
                    }
                }
                visuals->TotalNumberOfLoadedStaffTexts = 0;
                printf("Unloaded all sprite images from memory.\n");

                if (visuals->FullScreenMode == false)  SDL_SetWindowFullscreen(visuals->Window, 0);
                else if (visuals->FullScreenMode == true)  SDL_SetWindowFullscreen(visuals->Window, SDL_WINDOW_FULLSCREEN_DESKTOP);

                if ( visuals->LoadSpritesAndInitialize() != true )  visuals->CoreFailure = true;
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 2.5)
            {
                visuals->FullScreenMode = !visuals->FullScreenMode;

                for (int index = 0; index < NumberOfSprites; index++)
                {
                    if (visuals->Sprites[index].Texture != NULL)
                    {
                        SDL_DestroyTexture(visuals->Sprites[index].Texture);
                    }
                }
                visuals->TotalNumberOfLoadedStaffTexts = 0;
                printf("Unloaded all sprite images from memory.\n");

                if (visuals->FullScreenMode == false)  SDL_SetWindowFullscreen(visuals->Window, 0);
                else if (visuals->FullScreenMode == true)  SDL_SetWindowFullscreen(visuals->Window, SDL_WINDOW_FULLSCREEN_DESKTOP);

                if ( visuals->LoadSpritesAndInitialize() != true )  visuals->CoreFailure = true;
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 3)
            {
                if (logic->GameMode > 0)  logic->GameMode-=1;
                else  logic->GameMode = 3;
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 3.5)
            {
                if (logic->GameMode < 3)  logic->GameMode+=1;
                else  logic->GameMode = 0;
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 4)
            {
                if (logic->SelectedBackground > 0)  logic->SelectedBackground-=1;
                else  logic->SelectedBackground = 5;
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 4.5)
            {
                if (logic->SelectedBackground < 5)  logic->SelectedBackground+=1;
                else  logic->SelectedBackground = 0;
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 5)
            {
                if (audio->MusicVolume > 0 && logic->MusicJukebox == false)
                {
                    if (logic->SelectedMusicTrack > 0)  logic->SelectedMusicTrack-=1;
                    else  logic->SelectedMusicTrack = 8;

                    audio->PlayMusic(logic->SelectedMusicTrack, 0);
                }
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 5.5)
            {
                if (audio->MusicVolume > 0 && logic->MusicJukebox == false)
                {
                    if (logic->SelectedMusicTrack < 8)  logic->SelectedMusicTrack+=1;
                    else  logic->SelectedMusicTrack = 0;

                    audio->PlayMusic(logic->SelectedMusicTrack, 0);
                }
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 6)
            {
                if (audio->MusicVolume > 0)
                {
                    logic->MusicJukebox = !logic->MusicJukebox;

                    if (logic->MusicJukebox == true)
                    {
                        logic->SelectedMusicTrack = rand()%9;
                        audio->PlayMusic(logic->SelectedMusicTrack, 1);
                    }
                }
            }
            else if (interface->ArrowSetArrowSelectedByPlayer == 6.5)
            {
                if (audio->MusicVolume > 0)
                {
                    logic->MusicJukebox = !logic->MusicJukebox;

                    if (logic->MusicJukebox == true)
                    {
                        logic->SelectedMusicTrack = rand()%9;
                        audio->PlayMusic(logic->SelectedMusicTrack, 1);
                    }
                }
            }

            interface->ArrowSetArrowSelectedByPlayer = -1;
        }
    }

    if (ScreenIsDirty == 1)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        DisplaySelectedBackround();

        visuals->Sprites[0].ScreenX = 320;
        visuals->Sprites[0].ScreenY = 240;
        visuals->Sprites[0].Transparency = 150;
        visuals->DrawSpriteOntoScreenBuffer(0);

        visuals->DrawTextOntoScreenBuffer("O P T I O N S:", visuals->Font[0]
                                          , 0, 1, JustifyCenter, 255, 255, 1, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 7-1, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        interface->DisplayAllArrowSetsOntoScreenBuffer();

        visuals->DrawTextOntoScreenBuffer("Music Volume:"
                                          , visuals->Font[0], 60, 60-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);
        if (audio->MusicVolume == 0)
            visuals->DrawTextOntoScreenBuffer("OFF", visuals->Font[0], 60, 60-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->MusicVolume == 32)
            visuals->DrawTextOntoScreenBuffer("25%", visuals->Font[0], 60, 60-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->MusicVolume == 64)
            visuals->DrawTextOntoScreenBuffer("50%", visuals->Font[0], 60, 60-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->MusicVolume == 96)
            visuals->DrawTextOntoScreenBuffer("75%", visuals->Font[0], 60, 60-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->MusicVolume == 128)
            visuals->DrawTextOntoScreenBuffer("100%", visuals->Font[0], 60, 60-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Sound Effects Volume:"
                                          , visuals->Font[0], 60, 105-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);
        if (audio->EffectsVolume == 0)
            visuals->DrawTextOntoScreenBuffer("OFF", visuals->Font[0], 60, 105-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->EffectsVolume == 32)
            visuals->DrawTextOntoScreenBuffer("25%", visuals->Font[0], 60, 105-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->EffectsVolume == 64)
            visuals->DrawTextOntoScreenBuffer("50%", visuals->Font[0], 60, 105-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->EffectsVolume == 96)
            visuals->DrawTextOntoScreenBuffer("75%", visuals->Font[0], 60, 105-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (audio->EffectsVolume == 128)
            visuals->DrawTextOntoScreenBuffer("100%", visuals->Font[0], 60, 105-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 107-6, JustifyCenter
                                          , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Full Screen Mode:"
                                          , visuals->Font[0], 60, 160-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);
        if (visuals->FullScreenMode == false)
            visuals->DrawTextOntoScreenBuffer("OFF", visuals->Font[0], 60, 160-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (visuals->FullScreenMode == true)
            visuals->DrawTextOntoScreenBuffer("ON", visuals->Font[0], 60, 160-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 162-6, JustifyCenter
                                          , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Game Mode:"
                                          , visuals->Font[0], 60, 215-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);

        if (logic->GameMode == 0)
            visuals->DrawTextOntoScreenBuffer("Neverending Child", visuals->Font[0], 60, 215-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == 1)
            visuals->DrawTextOntoScreenBuffer("Neverending Teenager", visuals->Font[0], 60, 215-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == 2)
            visuals->DrawTextOntoScreenBuffer("Neverending Adult", visuals->Font[0], 60, 215-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == 3)
            visuals->DrawTextOntoScreenBuffer("Time Attack 15 Minute", visuals->Font[0], 60, 215-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == 4)
            visuals->DrawTextOntoScreenBuffer("Story", visuals->Font[0], 60, 215-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Background Image:"
                                          , visuals->Font[0], 60, 260-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);

        if (logic->SelectedBackground == 0)
            visuals->DrawTextOntoScreenBuffer("Waterfall", visuals->Font[0], 60, 260-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedBackground == 1)
            visuals->DrawTextOntoScreenBuffer("Water Droplets", visuals->Font[0], 60, 260-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedBackground == 2)
            visuals->DrawTextOntoScreenBuffer("Kittens", visuals->Font[0], 60, 260-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedBackground == 3)
            visuals->DrawTextOntoScreenBuffer("Puppies", visuals->Font[0], 60, 260-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedBackground == 4)
            visuals->DrawTextOntoScreenBuffer("Chalkboard", visuals->Font[0], 60, 260-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedBackground == 5)
            visuals->DrawTextOntoScreenBuffer("Sunset", visuals->Font[0], 60, 260-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Music Track:"
                                          , visuals->Font[0], 60, 305-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);

        if (audio->MusicVolume == 0)
            visuals->DrawTextOntoScreenBuffer("Music Volume OFF", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->MusicJukebox == true)
            visuals->DrawTextOntoScreenBuffer("Random", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 0)
            visuals->DrawTextOntoScreenBuffer("Track #1", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 1)
            visuals->DrawTextOntoScreenBuffer("Track #2", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 2)
            visuals->DrawTextOntoScreenBuffer("Track #3", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 3)
            visuals->DrawTextOntoScreenBuffer("Track #4", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 4)
            visuals->DrawTextOntoScreenBuffer("Track #5", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 5)
            visuals->DrawTextOntoScreenBuffer("Track #6", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 6)
            visuals->DrawTextOntoScreenBuffer("Track #7", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 7)
            visuals->DrawTextOntoScreenBuffer("Track #8", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->SelectedMusicTrack == 8)
            visuals->DrawTextOntoScreenBuffer("Track #9", visuals->Font[0], 60, 305-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Music Jukebox:"
                                          , visuals->Font[0], 60, 350-19, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);

        if (audio->MusicVolume == 0)
            visuals->DrawTextOntoScreenBuffer("Music Volume OFF", visuals->Font[0], 60, 350-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->MusicJukebox == true)
            visuals->DrawTextOntoScreenBuffer("Random Music Track", visuals->Font[0], 60, 350-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->MusicJukebox == false)
            visuals->DrawTextOntoScreenBuffer("Repeat Above Track", visuals->Font[0], 60, 350-19, JustifyRight
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 342, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        int index = 307;
        int screenX = 75;
        int offsetX = 70;
        for (int letter = 0; letter < 8; letter++)
        {
            if (letter == 0)  index = 307;
            else if (letter == 1)  index = 300;
            else if (letter == 2)  index = 321;
            else if (letter == 3)  index = 304;
            else if (letter == 4)  index = 307;
            else if (letter == 5)  index = 305;
            else if (letter == 6)  index = 320;
            else if (letter == 7)  index = 313;

            visuals->Sprites[index].ScreenX = screenX + (letter * offsetX);
            visuals->Sprites[index].ScreenY = 404;
            visuals->Sprites[index].ScaleX = (float)1.6;
            visuals->Sprites[index].ScaleY = (float)1.6;
            visuals->Sprites[index].Transparency = 128;
            if (letter != 4)  visuals->DrawSpriteOntoScreenBuffer(index);
            visuals->Sprites[index].RedHue = 255;
            visuals->Sprites[index].BlueHue = 255;
            visuals->Sprites[index].Transparency = 255;
            visuals->Sprites[index].ScaleX = 1;
            visuals->Sprites[index].ScaleY = 1;
        }

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 404-6, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        interface->DisplayAllButtonsOntoScreenBuffer();
/*
        if (input->KeyboardSetupProcess > KeyboardSetupNotStarted)
        {
            visuals->ClearScreenBufferWithColor(0.0f, 0.0f, 0.0f, 1.0f);

            if (input->KeyboardSetupProcess == KeyboardSetupPressOne)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [ROTATE1] now!"
                                                  ,visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            else if (input->KeyboardSetupProcess == KeyboardSetupPressTwo)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [ROTATE2] now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            else if (input->KeyboardSetupProcess == KeyboardSetupPressUP)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [UP] now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            else if (input->KeyboardSetupProcess == KeyboardSetupPressRIGHT)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [RIGHT] now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            else if (input->KeyboardSetupProcess == KeyboardSetupPressDOWN)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [DOWN] now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            else if (input->KeyboardSetupProcess == KeyboardSetupPressLEFT)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [LEFT] now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            else if (input->KeyboardSetupProcess == KeyboardSetupPressPause)
                visuals->DrawTextOntoScreenBuffer("Press key on keyboard for [Pause] now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            visuals->DrawTextOntoScreenBuffer("Press [F2] key on keyboard to quit setup."
                                              , visuals->Font[0]
                                              , 0, 380, JustifyCenter, 200, 200, 200, 1, 1, 1);

            visuals->DrawTextOntoScreenBuffer("(Will setup keyboard with default mappings.)"
                                              , visuals->Font[1]
                                              , 0, 410, JustifyCenter, 200, 200, 200, 1, 1, 1);
        }

        if (input->JoystickSetupProcess > JoySetupNotStarted)
        {
            visuals->ClearScreenBufferWithColor(0.0f, 0.0f, 0.0f, 1.0f);

            if (input->JoystickSetupProcess == Joy1SetupPressUP)
                visuals->DrawTextOntoScreenBuffer("Press [UP] on joystick #1 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy1SetupPressDOWN)
                visuals->DrawTextOntoScreenBuffer("Press [DOWN] on joystick #1 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy1SetupPressLEFT)
                visuals->DrawTextOntoScreenBuffer("Press [LEFT] on joystick #1 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy1SetupPressRIGHT)
                visuals->DrawTextOntoScreenBuffer("Press [RIGHT] on joystick #1 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy1SetupPressBUTTONOne)
                visuals->DrawTextOntoScreenBuffer("Press [BUTTON1] on joystick #1 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy1SetupPressBUTTONTwo)
                visuals->DrawTextOntoScreenBuffer("Press [BUTTON2] on joystick #1 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy2SetupPressUP)
                visuals->DrawTextOntoScreenBuffer("Press [UP] on joystick #2 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy2SetupPressDOWN)
                visuals->DrawTextOntoScreenBuffer("Press [DOWN] on joystick #2 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy2SetupPressLEFT)
                visuals->DrawTextOntoScreenBuffer("Press [LEFT] on joystick #2 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy2SetupPressRIGHT)
                visuals->DrawTextOntoScreenBuffer("Press [RIGHT] on joystick #2 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy2SetupPressBUTTONOne)
                visuals->DrawTextOntoScreenBuffer("Press [BUTTON1] on joystick #2 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy2SetupPressBUTTONTwo)
                visuals->DrawTextOntoScreenBuffer("Press [BUTTON2] on joystick #2 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy3SetupPressUP)
                visuals->DrawTextOntoScreenBuffer("Press [UP] on joystick #3 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy3SetupPressDOWN)
                visuals->DrawTextOntoScreenBuffer("Press [DOWN] on joystick #3 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy3SetupPressLEFT)
                visuals->DrawTextOntoScreenBuffer("Press [LEFT] on joystick #3 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy3SetupPressRIGHT)
                visuals->DrawTextOntoScreenBuffer("Press [RIGHT] on joystick #3 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy3SetupPressBUTTONOne)
                visuals->DrawTextOntoScreenBuffer("Press [BUTTON1] on joystick #3 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);
            else if (input->JoystickSetupProcess == Joy3SetupPressBUTTONTwo)
                visuals->DrawTextOntoScreenBuffer("Press [BUTTON2] on joystick #3 now!"
                                                  , visuals->Font[0]
                                                  , 0, 214, JustifyCenter, 0, 255, 0, 1, 1, 1);

            visuals->DrawTextOntoScreenBuffer("Press [F1] key on keyboard to quit setup.", visuals->Font[0]
                                              , 0, 380, JustifyCenter, 200, 200, 200, 1, 1, 1);

            visuals->DrawTextOntoScreenBuffer("(Will setup joystick(s) with default mappings.)", visuals->Font[1]
                                              , 0, 410, JustifyCenter, 200, 200, 200, 1, 1, 1);
        }*/
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = TitleScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayHowToPlayScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        interface->CreateButton(1009, 0, 454);

        ScreenTransitionStatus = FadeIn;
    }

    interface->ProcessAllButtons();

    if (ScreenIsDirty == 1)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        DisplaySelectedBackround();

        visuals->Sprites[0].ScreenX = 320;
        visuals->Sprites[0].ScreenY = 240;
        visuals->Sprites[0].Transparency = 150;
        visuals->DrawSpriteOntoScreenBuffer(0);

        visuals->Sprites[4].ScreenX = 320;

        if (input->UserDefinedKeyButtonOne == -1 && input->UserDefinedKeyButtonTwo == -1)
            visuals->Sprites[4].ScreenY = 240;
        else
            visuals->Sprites[4].ScreenY = 160;

        visuals->DrawSpriteOntoScreenBuffer(4);

        if (input->UserDefinedKeyButtonOne != -1 && input->UserDefinedKeyButtonTwo != -1)
        {
            visuals->Sprites[0].ScreenX = 320;
            visuals->Sprites[0].ScreenY = 240;
            visuals->Sprites[0].Transparency = 100;
            visuals->DrawSpriteOntoScreenBuffer(0);
        }

        visuals->DrawTextOntoScreenBuffer("H O W   T O   P L A Y:", visuals->Font[0]
                                          , 0, 1, JustifyCenter, 255, 255, 1, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 7-1, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 404-6, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        interface->DisplayAllButtonsOntoScreenBuffer();
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = TitleScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayHighScoresScreen(void)
{
Uint8 greenBlueColorValue;

    if (ScreenTransitionStatus == FadeAll)
    {
        interface->CreateButton(1009, 0, 454);

        interface->CreateArrowSet(0, 65);

        ScreenTransitionStatus = FadeIn;
    }

    interface->ProcessAllButtons();

    interface->ProcessAllArrowSets();

    if (interface->ArrowSetArrowSelectedByPlayer != -1)
    {
        if (interface->ArrowSetArrowSelectedByPlayer == 0)
        {
            if (logic->GameMode > 0)  logic->GameMode-=1;
            else  logic->GameMode = 3;

            visuals->ClearTextCache();
        }
        else if (interface->ArrowSetArrowSelectedByPlayer == 0.5)
        {
            if (logic->GameMode < 3)  logic->GameMode+=1;
            else  logic->GameMode = 0;

            visuals->ClearTextCache();
        }

        interface->ArrowSetArrowSelectedByPlayer = -1;
    }

    if (input->KeyOnKeyboardPressedByUser == 'C')
    {
        data->ClearHighScores();
        ScreenIsDirty = true;
        audio->PlayDigitalSoundFX(1, 0);
    }

    if (logic->RunLoop == true)
    {
        interface->ButtonSelectedByPlayer = 0;
        ScreenTransitionStatus = FadeOut;
        ScreenIsDirty = true;
    }

//    if (ScreenIsDirty == 1)
    {
        visuals->ClearScreenBufferWithColor(0, 0, 0, 255);

        DisplaySelectedBackround();

        visuals->Sprites[0].ScreenX = 320;
        visuals->Sprites[0].ScreenY = 240;
        visuals->Sprites[0].Transparency = 150;
        visuals->DrawSpriteOntoScreenBuffer(0);

        visuals->DrawTextOntoScreenBuffer("H I G H   S C O R E S:", visuals->Font[0]
                                          , 0, 1, JustifyCenter, 255, 255, 1, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 7-1, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        interface->DisplayAllArrowSetsOntoScreenBuffer();

        if (logic->GameMode == ChildMode)
            visuals->DrawTextOntoScreenBuffer("Neverending Child Mode", visuals->Font[0]
                                              , 0, 65-19, JustifyCenter
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == TeenagerMode)
            visuals->DrawTextOntoScreenBuffer("Neverending Teenager Mode", visuals->Font[0]
                                              , 0, 65-19, JustifyCenter
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == AdultMode)
            visuals->DrawTextOntoScreenBuffer("Neverending Adult Mode", visuals->Font[0]
                                              , 0, 65-19, JustifyCenter
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == TimeAttack15MinuteMode)
            visuals->DrawTextOntoScreenBuffer("Time Attack 15 Minute Mode", visuals->Font[0]
                                              , 0, 65-19, JustifyCenter
                                              , 255, 255, 255, 0, 0, 0);
        else if (logic->GameMode == StoryMode)
            visuals->DrawTextOntoScreenBuffer("Story Mode", visuals->Font[0]
                                              , 0, 65-19, JustifyCenter
                                              , 255, 255, 255, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Name:", visuals->Font[1],
                                          30, 88, JustifyLeft, 150, 150, 150, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Level:", visuals->Font[1],
                                          420, 88, JustifyLeft, 150, 150, 150, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("Score:", visuals->Font[1],
                                          510, 88, JustifyLeft, 150, 150, 150, 0, 0, 0);

        visuals->DrawTextOntoScreenBuffer("1.", visuals->Font[1], 1, 112+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("2.", visuals->Font[1], 1, 142+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("3.", visuals->Font[1], 1, 172+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("4.", visuals->Font[1], 1, 202+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("5.", visuals->Font[1], 1, 232+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("6.", visuals->Font[1], 1, 263+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("7.", visuals->Font[1], 1, 292+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("8.", visuals->Font[1], 1, 322+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("9.", visuals->Font[1], 1, 352+4, JustifyLeft, 150, 150, 150, 0, 0, 0);
        visuals->DrawTextOntoScreenBuffer("10.", visuals->Font[1], 1, 382+4, JustifyLeft, 150, 150, 150, 0, 0, 0);

        int rankY = 112;
        int offsetY = 30;

        for (Uint8 index = 0; index < 10; index++)
        {
            greenBlueColorValue = 255;

            if (data->HighScoresScore[logic->GameMode][index] == logic->Score
                && data->HighScoresLevel[logic->GameMode][index] == logic->Level)
            {
                greenBlueColorValue = 0;
            }

            visuals->DrawTextOntoScreenBuffer(data->HighScoresName[logic->GameMode][index], visuals->Font[0]
                                              , 30, rankY, JustifyLeft, 255, greenBlueColorValue, greenBlueColorValue, 0, 0, 0);

            if (logic->GameMode == StoryMode && data->HighScoresLevel[logic->GameMode][index] == 10)
            {
                visuals->DrawTextOntoScreenBuffer("Won!", visuals->Font[0], 420, rankY
                                                  , JustifyLeft, 255, greenBlueColorValue, greenBlueColorValue, 0, 0, 0);
            }
            else
            {
                sprintf(visuals->VariableText, "%d", data->HighScoresLevel[logic->GameMode][index]);
                visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 420, rankY
                                                  , JustifyLeft, 255, greenBlueColorValue, greenBlueColorValue, 0, 0, 0);
            }

            sprintf(visuals->VariableText, "%d", data->HighScoresScore[logic->GameMode][index]);
            visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 510, rankY, JustifyLeft
                                              , 255, greenBlueColorValue, greenBlueColorValue, 0, 0, 0);

            rankY += offsetY;
        }

        visuals->DrawTextOntoScreenBuffer("_____________________________________"
                                          , visuals->Font[3], 0, 404-6, JustifyCenter
                                          , 255, 255, 1, 0, 0, 0);

        interface->DisplayAllButtonsOntoScreenBuffer();
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = TitleScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayAboutScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        for (int index = 1100; index < (1100+visuals->TotalNumberOfLoadedStaffTexts); index++)
        {
            if (index == 1100)
            {
                visuals->Sprites[1100].ScreenX = 385-5;
                visuals->Sprites[1100].ScreenY = 491+5;
            }
            else if (index == 1101)
            {
                visuals->Sprites[1101].ScreenX = 320;
                visuals->Sprites[1101].ScreenY = 510;
            }
            else if (index == 1102)
            {
                visuals->Sprites[1102].ScreenX = 320;
                visuals->Sprites[1102].ScreenY = 540;
            }
            else if ( index < (1100 + visuals->TotalNumberOfLoadedStaffTexts-1) )
            {
                if (visuals->Sprites[index-1].BlueHue == 255
                 && visuals->Sprites[index].BlueHue == 0)
                {
                    visuals->Sprites[index].ScreenX = 320;
                    visuals->Sprites[index].ScreenY = visuals->Sprites[index-1].ScreenY+90;
                }
                else
                {
                    visuals->Sprites[index].ScreenX = 320;
                    visuals->Sprites[index].ScreenY = visuals->Sprites[index-1].ScreenY+30;
                }
            }
            else if (index == 1100 + visuals->TotalNumberOfLoadedStaffTexts-1)
            {
                visuals->Sprites[index].ScreenX = 320;
                visuals->Sprites[index].ScreenY = visuals->Sprites[index-2].ScreenY+270;
            }
        }

        ScreenTransitionStatus = FadeIn;
    }

    if ( input->SpacebarKeyPressed == true
        || input->EnterKeyPressed == true
        || input->MouseButtonPressed[0] == true
        || input->JoystickButtonOne[Any] == ON)
    {
        ScreenTransitionStatus = FadeOut;
        input->DelayAllUserInput = 60;
        audio->PlayDigitalSoundFX(0, 0);
    }

    for (int index = 1100; index < (1100+visuals->TotalNumberOfLoadedStaffTexts); index++)
    {
        visuals->Sprites[index].ScreenY-=1;
    }

    if (visuals->Sprites[1100+visuals->TotalNumberOfLoadedStaffTexts-1].ScreenY < -20)
        ScreenTransitionStatus = FadeOut;

//    if (ScreenIsDirty == true)
    {
        DisplaySelectedBackround();

        visuals->Sprites[0].ScreenX = 320;
        visuals->Sprites[0].ScreenY = 240;
        visuals->Sprites[0].Transparency = 150;
        visuals->DrawSpriteOntoScreenBuffer(0);

        for (  int index = 1100; index < ( (1100 + visuals->TotalNumberOfLoadedStaffTexts) ); index++  )
        {
            visuals->DrawSpriteOntoScreenBuffer(index);
        }

        ScreenIsDirty = true;
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;
        ScreenToDisplay = TitleScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayPlayingGameScreen(void)
{
    if (ScreenTransitionStatus == FadeAll)
    {
        logic->SetupForNewGame();

        data->NumberOfPlays++;

        input->DelayAllUserInput = 20;

        ScreenTransitionStatus = FadeIn;
    }

    if (input->SpacebarKeyPressed == true)
    {
        logic->PAUSEgame = !logic->PAUSEgame;

        if (logic->PAUSEgame == false)  Mix_ResumeMusic();
        else  Mix_PauseMusic();

        logic->DrawEntirePlayingGameScreen = 2;

        input->DelayAllUserInput = 20;
        audio->PlayDigitalSoundFX(0, 0);
    }

    if (logic->PAUSEgame == false && logic->GameOver == -1)  logic->RunGameplayEngine();

    SDL_Rect rect;
    rect.x = logic->FallingLettersScreenX[0] - 21;
    rect.w = 42;
    rect.y = logic->FallingLettersScreenY[0]-80;
    rect.h = 122;
    if (visuals->WindowWidthCurrent != 640 || visuals->WindowHeightCurrent != 480)  logic->DrawEntirePlayingGameScreen = 2;

    if (logic->DrawEntirePlayingGameScreen > 0 || ScreenTransitionStatus != FadeNone
       || logic->GameOver > -1 || logic->PAUSEgame == true)
    {
        SDL_RenderSetClipRect(visuals->Renderer, NULL);
        if (logic->DrawEntirePlayingGameScreen == 0)  logic->DrawEntirePlayingGameScreen = 2;
    }
    else  SDL_RenderSetClipRect(visuals->Renderer, &rect);

    visuals->Sprites[20+logic->SelectedBackground].ScreenX = 320;
    visuals->Sprites[20+logic->SelectedBackground].ScreenY = 240;
    visuals->Sprites[20+logic->SelectedBackground].Smooth = true;
    visuals->DrawSpriteOntoScreenBuffer(20+logic->SelectedBackground);

    visuals->Sprites[60].ScreenX = 320;
    visuals->Sprites[60].ScreenY = 218;
    visuals->Sprites[60].Transparency = 200;
    visuals->Sprites[60].Smooth = true;
    visuals->DrawSpriteOntoScreenBuffer(60);

    if (logic->FallingLetters[0] == 255 && logic->ExplosionScale == -1)
    {
        visuals->Sprites[90].ScreenX = logic->FallingLettersScreenX[0];
        visuals->Sprites[90].ScreenY = logic->FallingLettersScreenY[0];
        visuals->Sprites[90].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(90);
    }
    else if (logic->FallingLetters[1] == 0)
    {
        visuals->Sprites[ 299+logic->FallingLetters[0] ].ScreenX = logic->FallingLettersScreenX[0];
        visuals->Sprites[ 299+logic->FallingLetters[0] ].ScreenY = logic->FallingLettersScreenY[0];
        visuals->Sprites[ 299+logic->FallingLetters[0] ].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(299+logic->FallingLetters[0]);
    }
    else if (logic->FallingLetters[2] == 0)
    {
        visuals->Sprites[ 299+logic->FallingLetters[1] ].ScreenX = logic->FallingLettersScreenX[1];
        visuals->Sprites[ 299+logic->FallingLetters[1] ].ScreenY = logic->FallingLettersScreenY[1];
        visuals->Sprites[ 299+logic->FallingLetters[1] ].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(299+logic->FallingLetters[1]);

        visuals->Sprites[ 299+logic->FallingLetters[0] ].ScreenX = logic->FallingLettersScreenX[0];
        visuals->Sprites[ 299+logic->FallingLetters[0] ].ScreenY = logic->FallingLettersScreenY[0];
        visuals->Sprites[ 299+logic->FallingLetters[0] ].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(299+logic->FallingLetters[0]);
    }
    else // 3 letters falling...
    {
        visuals->Sprites[ 299+logic->FallingLetters[2] ].ScreenX = logic->FallingLettersScreenX[2];
        visuals->Sprites[ 299+logic->FallingLetters[2] ].ScreenY = logic->FallingLettersScreenY[2];
        visuals->Sprites[ 299+logic->FallingLetters[2] ].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(299+logic->FallingLetters[2]);

        visuals->Sprites[ 299+logic->FallingLetters[1] ].ScreenX = logic->FallingLettersScreenX[1];
        visuals->Sprites[ 299+logic->FallingLetters[1] ].ScreenY = logic->FallingLettersScreenY[1];
        visuals->Sprites[ 299+logic->FallingLetters[1] ].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(299+logic->FallingLetters[1]);

        visuals->Sprites[ 299+logic->FallingLetters[0] ].ScreenX = logic->FallingLettersScreenX[0];
        visuals->Sprites[ 299+logic->FallingLetters[0] ].ScreenY = logic->FallingLettersScreenY[0];
        visuals->Sprites[ 299+logic->FallingLetters[0] ].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(299+logic->FallingLetters[0]);
    }

//    if (logic->DrawEntirePlayingGameScreen > 0)
    {
        int tileX = 27;
        int tileY = 26-(30*3);
        for (int y = 0; y < 17; y++)
        {
            for (int x = 0; x < 11; x++)
            {
                if (logic->Playfield[x][y] > 0)
                {
                    for (int index = 0; index < 11; index++)
                    {
                        if (logic->SelectedLetterPlayfieldX[index] == x && logic->SelectedLetterPlayfieldY[index] == y)
                        {
                            visuals->Sprites[ 299+logic->Playfield[x][y] ].RedHue = 0;
                            visuals->Sprites[ 299+logic->Playfield[x][y] ].GreenHue = 255;
                            visuals->Sprites[ 299+logic->Playfield[x][y] ].BlueHue = 0;
                            visuals->Sprites[ 299+logic->Playfield[x][y] ].Transparency = logic->ClearWordTransparency;
                        }
                    }

                    visuals->Sprites[ 299+logic->Playfield[x][y] ].ScreenX = tileX;
                    visuals->Sprites[ 299+logic->Playfield[x][y] ].ScreenY = tileY;
                    visuals->Sprites[ 299+logic->Playfield[x][y] ].Smooth = true;
                    visuals->DrawSpriteOntoScreenBuffer(299+logic->Playfield[x][y]);

                    visuals->Sprites[ 299+logic->Playfield[x][y] ].RedHue = 255;
                    visuals->Sprites[ 299+logic->Playfield[x][y] ].GreenHue = 255;
                    visuals->Sprites[ 299+logic->Playfield[x][y] ].BlueHue = 255;
                    visuals->Sprites[ 299+logic->Playfield[x][y] ].Transparency = 255;
                }

                tileX+=41;
            }

            tileX = 27;
            tileY+=30;
        }

        visuals->Sprites[65].ScreenX = 550;
        visuals->Sprites[65].ScreenY = 22+20+10;
        visuals->Sprites[65].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(65);

        if (logic->NextLetters[0] == 255)
        {
            visuals->Sprites[90].ScreenX = 550;
            visuals->Sprites[90].ScreenY = 58+20+10-3;
            visuals->Sprites[90].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(90);
        }
        else if (logic->NextLetters[1] == 0)
        {
            visuals->Sprites[ 299+logic->NextLetters[0] ].ScreenX = 550;
            visuals->Sprites[ 299+logic->NextLetters[0] ].ScreenY = 58+20+10-3;
            visuals->Sprites[ 299+logic->NextLetters[0] ].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(299+logic->NextLetters[0]);
        }
        else if (logic->NextLetters[2] == 0)
        {
            visuals->Sprites[ 299+logic->NextLetters[0] ].ScreenX = 550-20;
            visuals->Sprites[ 299+logic->NextLetters[0] ].ScreenY = 58+20+10-3;
            visuals->Sprites[ 299+logic->NextLetters[0] ].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(299+logic->NextLetters[0]);

            visuals->Sprites[ 299+logic->NextLetters[1] ].ScreenX = 550+20;
            visuals->Sprites[ 299+logic->NextLetters[1] ].ScreenY = 58+20+10-3;
            visuals->Sprites[ 299+logic->NextLetters[1] ].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(299+logic->NextLetters[1]);
        }
        else // 3 next letters...
        {
            visuals->Sprites[ 299+logic->NextLetters[0] ].ScreenX = 550-40;
            visuals->Sprites[ 299+logic->NextLetters[0] ].ScreenY = 58+20+10-3;
            visuals->Sprites[ 299+logic->NextLetters[0] ].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(299+logic->NextLetters[0]);

            visuals->Sprites[ 299+logic->NextLetters[1] ].ScreenX = 550;
            visuals->Sprites[ 299+logic->NextLetters[1] ].ScreenY = 58+20+10-3;
            visuals->Sprites[ 299+logic->NextLetters[1] ].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(299+logic->NextLetters[1]);

            visuals->Sprites[ 299+logic->NextLetters[2] ].ScreenX = 550+40;
            visuals->Sprites[ 299+logic->NextLetters[2] ].ScreenY = 58+20+10-3;
            visuals->Sprites[ 299+logic->NextLetters[2] ].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(299+logic->NextLetters[2]);
        }

        visuals->Sprites[70].ScreenX = 550;
        visuals->Sprites[70].ScreenY = 102+20+1;
        visuals->Sprites[70].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(70);

        char temp[256];
		sprintf(temp, "%d", logic->Level);
        strcpy(visuals->VariableText, temp);
        visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 550, 115+20+1, JustifyCenterOnPoint
                                          , 255, 255, 255, 0, 0, 0);

        visuals->Sprites[75].ScreenX = 550;
        visuals->Sprites[75].ScreenY = 172+20-4;
        visuals->Sprites[75].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(75);

		sprintf(temp, "%d", logic->Score);
        strcpy(visuals->VariableText, temp);
        visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 550, 185+20-4, JustifyCenterOnPoint
                                          , 255, 255, 255, 0, 0, 0);

        visuals->Sprites[80].ScreenX = 550;
        visuals->Sprites[80].ScreenY = 250;
        visuals->Sprites[80].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(80);

        for (int greenLineX = 477; greenLineX < (477+logic->BombBonusMeterValue); greenLineX++)
        {
            visuals->Sprites[85].ScreenX = greenLineX;
            visuals->Sprites[85].ScreenY = 250;
            visuals->Sprites[85].Smooth = false;
            visuals->DrawSpriteOntoScreenBuffer(85);
        }

        visuals->Sprites[90].ScreenX = 496;
        visuals->Sprites[90].ScreenY = 290;
        visuals->Sprites[90].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(90);

        strcpy(visuals->VariableText, "x");
        sprintf(temp, "%d", logic->Bombs);
        strcat(visuals->VariableText, temp);
        visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 514, 270, JustifyLeft
                                          , 255, 255, 255, 0, 0, 0);

        if (logic->Bombs > 0)
        {
            visuals->Sprites[95].ScreenX = 598;
            visuals->Sprites[95].ScreenY = 290;
            visuals->Sprites[95].ScaleX = logic->BombButtonScale;
            visuals->Sprites[95].ScaleY = logic->BombButtonScale;
            visuals->Sprites[95].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(95);
        }

        visuals->Sprites[105].ScreenX = 550;
        visuals->Sprites[105].ScreenY = 350;
        visuals->Sprites[105].ScaleX = logic->DownArrowScale;
        visuals->Sprites[105].ScaleY = logic->DownArrowScale;
        visuals->Sprites[105].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(105);

        if (logic->FallingLetters[0] == 255)
        {
            visuals->Sprites[100].ScreenX = 495;
            visuals->Sprites[100].ScreenY = 336;
            visuals->Sprites[100].ScaleX = logic->LeftArrowScale;
            visuals->Sprites[100].ScaleY = logic->LeftArrowScale;
            visuals->Sprites[100].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(100);

            visuals->Sprites[110].ScreenX = 605;
            visuals->Sprites[110].ScreenY = 336;
            visuals->Sprites[110].ScaleX = logic->RightArrowScale;
            visuals->Sprites[110].ScaleY = logic->RightArrowScale;
            visuals->Sprites[110].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(110);
        }

        if (logic->SelectedLetterPlayfieldX[1] != -1 && logic->SelectedLetterPlayfieldY[1] != -1)
        {
            visuals->Sprites[115].ScreenX = 550;
            visuals->Sprites[115].ScreenY = 406;
            visuals->Sprites[115].ScaleX = logic->ClearButtonScale;
            visuals->Sprites[115].ScaleY = logic->ClearButtonScale;
            visuals->Sprites[115].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(115);
        }

        if (logic->SelectedLetterPlayfieldX[0] != -1 && logic->SelectedLetterPlayfieldY[0] != -1)
        {
            visuals->Sprites[125].ScreenX = 496;
            visuals->Sprites[125].ScreenY = 457;
            visuals->Sprites[125].ScaleX = logic->UndoButtonScale;
            visuals->Sprites[125].ScaleY = logic->UndoButtonScale;
            visuals->Sprites[125].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(125);
        }

        if (logic->SelectedLetterPlayfieldX[2] != -1 && logic->SelectedLetterPlayfieldY[2] != -1)
        {
            visuals->Sprites[120].ScreenX = 598;
            visuals->Sprites[120].ScreenY = 457;
            visuals->Sprites[120].ScaleX = logic->CheckButtonScale;
            visuals->Sprites[120].ScaleY = logic->CheckButtonScale;
            visuals->Sprites[120].RedHue = logic->CheckButtonRedHue;
            visuals->Sprites[120].GreenHue = logic->CheckButtonGreenHue;
            visuals->Sprites[120].BlueHue = logic->CheckButtonBlueHue;
            visuals->Sprites[120].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(120);
        }

        int screenX = 27;
        for (int index = 0; index < 11; index++)
        {
            if (logic->SelectedLetterPlayfieldX[index] != -1 && logic->SelectedLetterPlayfieldY[index] != -1)
            {
                visuals->Sprites[ 299+logic->Playfield[logic->SelectedLetterPlayfieldX[index]][logic->SelectedLetterPlayfieldY[index]] ].ScreenX = screenX;
                visuals->Sprites[ 299+logic->Playfield[logic->SelectedLetterPlayfieldX[index]][logic->SelectedLetterPlayfieldY[index]] ].ScreenY = 457;
                visuals->Sprites[ 299+logic->Playfield[logic->SelectedLetterPlayfieldX[index]][logic->SelectedLetterPlayfieldY[index]] ].Transparency = logic->ClearWordTransparency;
                visuals->Sprites[ 299+logic->Playfield[logic->SelectedLetterPlayfieldX[index]][logic->SelectedLetterPlayfieldY[index]] ].Smooth = true;
                visuals->DrawSpriteOntoScreenBuffer(299+logic->Playfield[logic->SelectedLetterPlayfieldX[index]][logic->SelectedLetterPlayfieldY[index]]);

                visuals->Sprites[ 299+logic->Playfield[logic->SelectedLetterPlayfieldX[index]][logic->SelectedLetterPlayfieldY[index]] ].Transparency = 255;
            }
            else index = 999;

            screenX+=41;
        }

        if (logic->ExplosionScale > -1)
        {
            visuals->Sprites[130].ScreenX = logic->FallingLettersScreenX[0];
            visuals->Sprites[130].ScreenY = logic->FallingLettersScreenY[0];
            visuals->Sprites[130].ScaleX = logic->ExplosionScale;
            visuals->Sprites[130].ScaleY = logic->ExplosionScale;
            visuals->Sprites[130].Transparency = logic->ExplosionTransparency;
            visuals->Sprites[130].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(130);
        }

        if (logic->GameOver > 1)  logic->GameOver--;
        if (logic->GameOver > 0)
        {
            visuals->Sprites[135].ScreenX = 320;
            visuals->Sprites[135].ScreenY = 240;
            visuals->Sprites[135].Smooth = true;
            visuals->DrawSpriteOntoScreenBuffer(135);
        }

        if (logic->GameOver == 1)
        {
            ScreenTransitionStatus = FadeOut;
        }

        if (logic->GameMode == TimeAttack15MinuteMode)
        {
            char temp[256];
            sprintf(temp, "%i", logic->TimeAttackMinutes);
            strcpy(visuals->VariableText, temp);
            strcat(visuals->VariableText, ":");
            if (logic->TimeAttackSeconds < 10)  strcat(visuals->VariableText, "0");
            sprintf(temp, "%i", logic->TimeAttackSeconds);
            strcat(visuals->VariableText, temp);
            visuals->DrawTextOntoScreenBuffer(visuals->VariableText, visuals->Font[0], 550, -3, JustifyCenterOnPoint
                                              , 255, 255, 255, 0, 0, 0);
        }

        visuals->Sprites[55].ScreenX = 640-16;
        visuals->Sprites[55].ScreenY = 0+16;
        visuals->Sprites[55].Smooth = true;
        visuals->DrawSpriteOntoScreenBuffer(55);

        if (logic->PAUSEgame == true && input->DEBUG == false)
        {
            visuals->Sprites[0].ScreenX = 320;
            visuals->Sprites[0].ScreenY = 240;
            visuals->Sprites[0].Transparency = 255;//200;
            visuals->DrawSpriteOntoScreenBuffer(0);

            visuals->DrawTextOntoScreenBuffer("G A M E   P A U S E D", visuals->Font[0]
                                              , 0, 221, JustifyCenter, 255, 255, 255, 90, 90, 90);

            visuals->DrawTextOntoScreenBuffer("Press [SpaceBar] On Keyboard To Continue!", visuals->Font[2]
                                              , 0, 261, JustifyCenter, 255, 255, 255, 90, 90, 90);
        }

        ScreenIsDirty = true;

        if (logic->DrawEntirePlayingGameScreen > 0)  logic->DrawEntirePlayingGameScreen--;
    }

    if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
    {
        ScreenTransitionStatus = FadeAll;

        SDL_RenderSetClipRect(visuals->Renderer, NULL);

        for (int index = 55; index < 136; index++)
        {
            visuals->Sprites[index].ScaleX = 1;
            visuals->Sprites[index].ScaleY = 1;
            visuals->Sprites[index].Transparency = 255;
            visuals->Sprites[index].RedHue = 255;
            visuals->Sprites[index].GreenHue = 255;
            visuals->Sprites[index].BlueHue = 255;
        }

        for (int index = 300; index < 347; index++)
        {
            visuals->Sprites[index].ScaleX = 1;
            visuals->Sprites[index].ScaleY = 1;
            visuals->Sprites[index].Transparency = 255;
            visuals->Sprites[index].RedHue = 255;
            visuals->Sprites[index].GreenHue = 255;
            visuals->Sprites[index].BlueHue = 255;
        }

        visuals->FrameLock = 16;

        if (logic->PAUSEgame == true)
        {
            logic->PAUSEgame = false;
            Mix_ResumeMusic();
        }

        if (logic->GameOver > 0)
        {
            data->CheckForNewHighScore();
            if (data->PlayerRankOnGameOver < 10)  ScreenToDisplay = NameInputMouseKeyboardScreen;
            else  ScreenToDisplay = HighScoresScreen;
        }
        else  ScreenToDisplay = TitleScreen;
    }
}

//-------------------------------------------------------------------------------------------------
void Screens::DisplayNameInputMouseKeyboardScreen(void)
{
	if (ScreenTransitionStatus == FadeAll)
	{
		interface->CreateButton(1008, 0, 454);

		input->DelayAllUserInput = 20;

		ScreenTransitionStatus = FadeIn;

		for (int index = 0; index < 73; index++)
		{
			NameInputButtons[index].Scale = 1;
			NameInputButtons[index].ScaleTimer = -1;
		}
	}

	interface->ProcessAllButtons();

	char characterArray[73] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
		, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
		, '!', '@', '#', '$', '%', '^', '&', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', ' ' };

	if (input->KeyOnKeyboardPressedByUser != -1)
	{
		if ( input->BackSpaceKeyPressed == true )
		{
			NameInputButtons[71].Scale = .75;
			NameInputButtons[71].ScaleTimer = 10;

			input->DelayAllUserInput = 20;

			audio->PlayDigitalSoundFX(0, 0);
			ScreenIsDirty = true;
		}
		else if (input->SpacebarKeyPressed == true )
		{
			NameInputButtons[70].Scale = .75;
			NameInputButtons[70].ScaleTimer = 10;

			input->DelayAllUserInput = 20;

			audio->PlayDigitalSoundFX(0, 0);
			ScreenIsDirty = true;
		}
		else
		{
			int charTyped = 70;
			for (int index = 0; index < 70; index++)
			{
				if (input->KeyOnKeyboardPressedByUser == characterArray[index])
				{
					charTyped = index;

					NameInputButtons[charTyped].Scale = .75;
					NameInputButtons[charTyped].ScaleTimer = 10;

					input->DelayAllUserInput = 20;

					audio->PlayDigitalSoundFX(0, 0);
					ScreenIsDirty = true;

					break;
				}
			}
		}
	}

	if (input->MouseButtonPressed[0] == true)
	{
		for (int index = 0; index < 73; index++)
		{
			if (input->MouseY >(NameInputButtons[index].ScreenY - 15)
				&& input->MouseY < (NameInputButtons[index].ScreenY + 15)
				&& input->MouseX >(NameInputButtons[index].ScreenX - 20)
				&& input->MouseX < (NameInputButtons[index].ScreenX + 20))
			{
				NameInputButtons[index].Scale = .75;
				NameInputButtons[index].ScaleTimer = 10;

				input->DelayAllUserInput = 10;

				ScreenIsDirty = true;

				audio->PlayDigitalSoundFX(0, 0);
			}
		}
	}

	for (int index = 0; index < 73; index++)
	{
		if (NameInputButtons[index].ScaleTimer > 0)  NameInputButtons[index].ScaleTimer--;
		else if (NameInputButtons[index].ScaleTimer == 0)
		{
			NameInputButtons[index].ScaleTimer = -1;
			NameInputButtons[index].Scale = 1;

			if (index < 71)
			{
				if (data->NameInputArayIndex < 18)
					data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][data->NameInputArayIndex]
					= characterArray[index];

				if (data->NameInputArayIndex < 19)  data->NameInputArayIndex++;
			}
			else if (index == 71)
			{
				if (data->NameInputArayIndex == 19)
				{
					data->NameInputArayIndex--;
					data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][data->NameInputArayIndex] = '\0';
				}

				data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][data->NameInputArayIndex] = '\0';
				if (data->NameInputArayIndex > 0)  data->NameInputArayIndex--;
				data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][data->NameInputArayIndex] = '\0';

				audio->PlayDigitalSoundFX(2, 0);
			}
			else if (index == 72)  ScreenTransitionStatus = FadeOut;

			ScreenIsDirty = true;
		}
	}

	if (ScreenIsDirty == 1)
	{
		visuals->Sprites[20 + logic->SelectedBackground].ScreenX = 320;
		visuals->Sprites[20 + logic->SelectedBackground].ScreenY = 240;
		visuals->Sprites[20 + logic->SelectedBackground].Smooth = true;
		visuals->DrawSpriteOntoScreenBuffer(20 + logic->SelectedBackground);

		visuals->Sprites[0].ScreenX = 320;
		visuals->Sprites[0].ScreenY = 240;
		visuals->Sprites[0].Transparency = 128;
		visuals->DrawSpriteOntoScreenBuffer(0);

		visuals->DrawTextOntoScreenBuffer("NEW HIGH SCORE NAME INPUT:", visuals->Font[0]
			, 0, 1, JustifyCenter, 255, 255, 1, 90, 90, 1);

		visuals->DrawTextOntoScreenBuffer("_____________________________________"
			, visuals->Font[0], 0, 7-1, JustifyCenter
			, 255, 255, 1, 90, 90, 1);

		visuals->DrawTextOntoScreenBuffer("You Achieved A New High Score!", visuals->Font[0]
			, 0, 40, JustifyCenter, 255, 255, 255, 90, 90, 90);

		visuals->DrawTextOntoScreenBuffer("Enter Your Name Using Your Keyboard Or Mouse:", visuals->Font[1]
			, 0, 75, JustifyCenter, 255, 255, 255, 90, 90, 90);

		if (data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][0] != (char)NULL)
			visuals->DrawTextOntoScreenBuffer(data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver],
			visuals->Font[0], 0, 125, JustifyCenter, 255, 255, 255, 90, 90, 90);

		int intScreenY = 216;
		int offsetY = 30;
		int screenX = 69;
		for (int index = 300; index < 313; index++)
		{
			visuals->Sprites[index].ScreenX = screenX;
			visuals->Sprites[index].ScreenY = intScreenY;
			visuals->Sprites[index].ScaleX = NameInputButtons[index - 300].Scale;
			visuals->Sprites[index].ScaleY = NameInputButtons[index - 300].Scale;
			visuals->DrawSpriteOntoScreenBuffer(index);

			NameInputButtons[index - 300].ScreenX = screenX;
			NameInputButtons[index - 300].ScreenY = visuals->Sprites[index].ScreenY;

			screenX += 41;
		}

		intScreenY += offsetY;
		screenX = 69;
		for (int index = 313; index < 326; index++)
		{
			visuals->Sprites[index].ScreenX = screenX;
			visuals->Sprites[index].ScreenY = intScreenY;
			visuals->Sprites[index].ScaleX = NameInputButtons[index - 300].Scale;
			visuals->Sprites[index].ScaleY = NameInputButtons[index - 300].Scale;
			visuals->DrawSpriteOntoScreenBuffer(index);

			NameInputButtons[index - 300].ScreenX = screenX;
			NameInputButtons[index - 300].ScreenY = visuals->Sprites[index].ScreenY;

			screenX += 41;
		}

		intScreenY += offsetY;
		screenX = 69;

		for (int index = 326; index < 339; index++)
		{
			visuals->Sprites[index].ScreenX = screenX;
			visuals->Sprites[index].ScreenY = intScreenY;
			visuals->Sprites[index].ScaleX = NameInputButtons[index - 300].Scale;
			visuals->Sprites[index].ScaleY = NameInputButtons[index - 300].Scale;
			visuals->DrawSpriteOntoScreenBuffer(index);

			NameInputButtons[index - 300].ScreenX = screenX;
			NameInputButtons[index - 300].ScreenY = visuals->Sprites[index].ScreenY;

			screenX += 41;
		}

		intScreenY += offsetY;
		screenX = 69;
		for (int index = 339; index < 352; index++)
		{
			visuals->Sprites[index].ScreenX = screenX;
			visuals->Sprites[index].ScreenY = intScreenY;
			visuals->Sprites[index].ScaleX = NameInputButtons[index - 300].Scale;
			visuals->Sprites[index].ScaleY = NameInputButtons[index - 300].Scale;
			visuals->DrawSpriteOntoScreenBuffer(index);

			NameInputButtons[index - 300].ScreenX = screenX;
			NameInputButtons[index - 300].ScreenY = visuals->Sprites[index].ScreenY;

			screenX += 41;
		}

		intScreenY += offsetY;
		screenX = 69 + (3 * 41);
		for (int index = 352; index < 359; index++)
		{
			visuals->Sprites[index].ScreenX = screenX;
			visuals->Sprites[index].ScreenY = intScreenY;
			visuals->Sprites[index].ScaleX = NameInputButtons[index - 300].Scale;
			visuals->Sprites[index].ScaleY = NameInputButtons[index - 300].Scale;
			visuals->DrawSpriteOntoScreenBuffer(index);

			NameInputButtons[index - 300].ScreenX = screenX;
			NameInputButtons[index - 300].ScreenY = visuals->Sprites[index].ScreenY;

			screenX += 41;
		}

		intScreenY += offsetY;
		screenX = 69;
		for (int index = 359; index < 372; index++)
		{
			visuals->Sprites[index].ScreenX = screenX;
			visuals->Sprites[index].ScreenY = intScreenY;
			visuals->Sprites[index].ScaleX = NameInputButtons[index - 300].Scale;
			visuals->Sprites[index].ScaleY = NameInputButtons[index - 300].Scale;
			visuals->DrawSpriteOntoScreenBuffer(index);

			NameInputButtons[index - 300].ScreenX = screenX;
			NameInputButtons[index - 300].ScreenY = visuals->Sprites[index].ScreenY;

			screenX += 41;
		}

		intScreenY += offsetY;

		visuals->Sprites[372].ScreenX = 69 + (6 * 41);
		visuals->Sprites[372].ScreenY = intScreenY;
		visuals->Sprites[372].ScaleX = NameInputButtons[72].Scale;
		visuals->Sprites[372].ScaleY = NameInputButtons[72].Scale;
		visuals->DrawSpriteOntoScreenBuffer(372);

		NameInputButtons[72].ScreenX = visuals->Sprites[372].ScreenX;
		NameInputButtons[72].ScreenY = visuals->Sprites[372].ScreenY;

		visuals->DrawTextOntoScreenBuffer("_____________________________________"
			, visuals->Font[0], 0, 404-6, JustifyCenter
			, 255, 255, 1, 90, 90, 1);

		interface->DisplayAllButtonsOntoScreenBuffer();
	}

	if (ScreenTransitionStatus == FadeOut && ScreenFadeTransparency == 255)
	{
		ScreenTransitionStatus = FadeAll;
		ScreenToDisplay = HighScoresScreen;

		for (int index = 300; index < 347; index++)
		{
			visuals->Sprites[index].ScaleX = 1;
			visuals->Sprites[index].ScaleY = 1;
		}

		if (data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][0] == '\0')
			data->HighScoresName[logic->GameMode][data->PlayerRankOnGameOver][0] = ' ';
	}
}
