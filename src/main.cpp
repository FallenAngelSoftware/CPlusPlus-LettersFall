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
/*___________________________________________________________________________________________________________________________
                                                     SDL 2.0.12
                                            Open Source / Cross Platform
  _______ _______     ______   _______    _       _______          _           _______ _                                _ TM
 (_______|_______)   (_____ \ (_______)  (_)     (_______)        | |         (_______) |       Retro Blast Tech!      | |
  _   ___    _  _____ _____) )    _ _ _ _ _ ____     _ _   _  ____| |__   ___  _      | |__  _____  ____ ____ _____  __| |
 | | (_  |  | |(_____)  __  /    | | | | | |  _ \   | | | | |/ ___)  _ \ / _ \| |     |  _ \(____ |/ ___) _  | ___ |/ _  |
 | |___) |  | |      | |  \ \    | | | | | | | | |  | | |_| | |   | |_) ) |_| | |_____| | | / ___ | |  ( (_| | ____( (_| |
  \_____/   |_|      |_|   |_|   |_|\___/|_|_| |_|  |_|____/|_|   |____/ \___/ \______)_| |_\_____|_|   \___ |_____)\____|
                                                                                                       (_____|  Version 2
                                           2-Dimensional Video Game Engine

                              (C)opyright 2019 By Jesse Lee Palser (JessePalser@GMail.com)
.............................................................................................................................

                                                  "LettersFall[TM]"

                                               Retail3 110% v5.0+ Final

                                    (C)opyright 2019 By Team Fallen Angel Software
                                              www.FallenAngelSoftware.com
_____________________________________________________________________________________________________________________________*/
#define SDL_MAIN_HANDLED
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "visuals.h"
#include "input.h"
#include "screens.h"
#include "interface.h"
#include "audio.h"
#include "data.h"
#include "logic.h"

Visuals *visuals;
Input *input;
Screens *screens;
Interface *interface;
Audio *audio;
Data *data;
Logic *logic;

//-------------------------------------------------------------------------------------------------
int main( int argc, char* args[] )
{
    printf("JeZxLee's ''GT-R Twin TurboCharged'' game engine started!\n");
    argc = argc; args = args;

    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        printf( "Unable to initialize SDL2: %s\n", SDL_GetError() );
        return(1);
    }
    else  printf("SDL2 initialized.\n");

    data = new Data();

    visuals = new Visuals();

    if ( visuals->InitializeWindow() != true ) visuals->CoreFailure = true;
    if ( visuals->LoadFontsIntoMemory() != true )  visuals->CoreFailure = true;
    if ( visuals->LoadSpritesAndInitialize() != true )  visuals->CoreFailure = true;

    input = new Input();

    screens = new Screens();

    interface = new Interface();

    srand( (unsigned)time(NULL) ); // Place unique time seed into random number generator.
    logic = new Logic();

    audio = new Audio();
    audio->SetupAudio();

    data->LoadHighScoresAndOptions();

    if (logic->MusicJukebox == true)
    {
        logic->SelectedMusicTrack = rand()%9;
        audio->PlayMusic(logic->SelectedMusicTrack, 1);
    }
    else  audio->PlayMusic(logic->SelectedMusicTrack, 0);

    if (visuals->FullScreenMode == true)  SDL_SetWindowFullscreen(visuals->Window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_StartTextInput();

    if (visuals->CoreFailure == true)  printf("Engine failure!");
    else  printf("MAIN LOOP STARTED...\n");
    //-MAIN-LOOP------------------------------------------------------------------------
    while (visuals->CoreFailure != true && input->EXIT_Game != true)
    {
        input->GetAllUserInput();
        visuals->CalculateFramerate();
        screens->ProcessScreenToDisplay();
        visuals->ProcessFramerate();
        if (visuals->CoreFailure == true)  printf("Engine failed in main loop!");
    }
    //------------------------------------------------------------------------MAIN-LOOP-
    if (visuals->CoreFailure == false)  printf("...MAIN LOOP EXITED\n");

    data->SaveHighScoresAndOptions();

    SDL_StopTextInput();

    delete logic;
    delete data;
    delete audio;
    delete interface;
    delete screens;
    delete input;
    delete visuals;

    SDL_Quit();
    printf("SDL2 closed.\n");
    printf("JeZxLee's ''GT-R Twin TurboCharged'' game engine ended!\n");
    return(0);
}

//                                      TM
// "A 110% By Team Fallen Angel Software!"
