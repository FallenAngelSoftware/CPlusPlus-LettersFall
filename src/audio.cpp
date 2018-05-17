/*
"LettersFall" - Open-source cross-platform educational word spelling game.
Copyright (C) 2018  Jesse Palser

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

#include <fstream>
#include <sstream>

#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "audio.h"

#include "screens.h"
#include "visuals.h"

extern Screens* screens;
extern Visuals* visuals;

//-------------------------------------------------------------------------------------------------
Audio::Audio(void)
{
    MusicVolume = 64;
    EffectsVolume = 64;
    CurrentlySelectedMusicTrack = 0;

    for (int soundIndex = 0; soundIndex < EffectsTotal; soundIndex++)  SoundFX[soundIndex] = NULL;
    for (int musicIndex = 0; musicIndex < MusicTotal; musicIndex++)  MusicTrack[musicIndex] = NULL;

    AudioWorking = false;
}

//-------------------------------------------------------------------------------------------------
Audio::~Audio(void)
{
    if (AudioWorking == false)  return;

    Mix_HaltChannel(-1);
    for (int soundIndex = 0; soundIndex < EffectsTotal; soundIndex++)
    {
        if (SoundFX[soundIndex] != NULL)  Mix_FreeChunk(SoundFX[soundIndex]);
        SoundFX[soundIndex] = NULL;
    }
    Mix_AllocateChannels(0);
    printf("Unloaded all music from memory.\n");

    Mix_HaltMusic();
    for (int musicIndex = 0; musicIndex < MusicTotal; musicIndex++)
    {
        if (MusicTrack[musicIndex] != NULL)  Mix_FreeMusic(MusicTrack[musicIndex]);
        MusicTrack[musicIndex] = NULL;
    }
    printf("Unloaded all sounds from memory.\n");

    Mix_CloseAudio();
    Mix_Quit();
    printf("SDL_mixer closed.\n");
}

//-------------------------------------------------------------------------------------------------
void Audio::GetDataFromFile(const char* fileName, std::stringstream& ss)
{
    std::ifstream file(fileName);
    if(file.is_open())
    {
        ss << file.rdbuf();
    }
    file.close();
}

//-------------------------------------------------------------------------------------------------
void Audio::SetupAudio(void)
{
char *base_path = SDL_GetBasePath();
int musicIndex = 0;
int effectsIndex = 0;

    strcpy(Mix_Init_Error, "SDL2_Mixer: OK");

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048)==-1)
    {
        printf("ERROR: SDL2_Mixer init failed: %s\n", Mix_GetError());

        strcpy(Mix_Init_Error, "SDL2_Mixer ERROR: ");
        strcat( Mix_Init_Error, Mix_GetError() );

        AudioWorking = false;
        return;
    }
    else printf("SDL2_Mixer initialized.\n");

    int flags = MIX_INIT_OGG|MIX_INIT_MOD;
    int initted = Mix_Init(flags);

    if ( flags != initted )
    {
        printf("Mix_Init: Failed to init required ogg and mod support!\n");
        printf("Mix_Init: %s\n", Mix_GetError());

        strcpy(Mix_Init_Error, "SDL2_Mixer ERROR: ");
        strcat( Mix_Init_Error, Mix_GetError() );

        AudioWorking = false;
        return;
    }

    Mix_AllocateChannels(16);
    for (int channel = 0; channel < 16; channel++)
    {
        Mix_Volume(channel, EffectsVolume);
    }

    std::stringstream ss;
    GetDataFromFile("data/music/LoadMusicData.txt", ss);

    std::string data;
    for (int index = 0; index < MusicTotal; index++)
    {
        std::getline(ss, data);
        musicIndex = atoi( data.c_str() );

        std::getline(ss, data);
        char fullPath[512];
        strcpy(fullPath, base_path);
        strcat( fullPath, data.c_str() );

        MusicTrack[musicIndex] = Mix_LoadMUS(fullPath);

        if(!MusicTrack[musicIndex] && MusicTotal != 0)
        {
            printf("ERROR: Couldn't load music %s\n", Mix_GetError());
        }
        else  Mix_VolumeMusic(MusicVolume);
    }
    printf("Loaded all music into memory.\n");

    std::stringstream ssTwo;
    GetDataFromFile("data/effects/LoadEffectsData.txt", ssTwo);

    std::string dataTwo;
    for (int index = 0; index < EffectsTotal; index++)
    {
        std::getline(ssTwo, dataTwo);
        effectsIndex = atoi( dataTwo.c_str() );

        std::getline(ssTwo, dataTwo);
        char fullPath[512];
        strcpy(fullPath, base_path);
        strcat( fullPath, dataTwo.c_str() );

        SoundFX[effectsIndex] = Mix_LoadWAV(fullPath);

        if(!SoundFX[effectsIndex] && EffectsTotal != 0)
        {
            printf("ERROR: Couldn't load effect %s\n", Mix_GetError());
        }
        else  Mix_VolumeChunk(SoundFX[effectsIndex], MIX_MAX_VOLUME);
    }
    printf("Loaded all effects into memory.\n");

    AudioWorking = true;
}

//-------------------------------------------------------------------------------------------------
void Audio::PlayMusic(Uint8 musicIndex, int loop)
{
    if (AudioWorking == false)  return;

    if (MusicTrack[musicIndex] == NULL)  return;

    if (musicIndex >= MusicTotal)  return;

    if (Mix_PlayingMusic() == 1)  Mix_HaltMusic();

    Mix_VolumeMusic(MusicVolume);

    CurrentlySelectedMusicTrack = musicIndex;

    if(Mix_PlayMusic(MusicTrack[musicIndex], loop)==-1)
    {
        printf( "Mix_PlayMusic: %s\n", Mix_GetError() );
    }
}

//-------------------------------------------------------------------------------------------------
void Audio::PlayDigitalSoundFX(Uint8 soundIndex, int loops)
{
    if (AudioWorking == false)  return;

    if (SoundFX[soundIndex] == NULL)  return;

    if (soundIndex >= EffectsTotal)  return;

    if (EffectsVolume == 0)  return;

    for (int index = 1; index < 16; index++)
        Mix_Volume( index, EffectsVolume );

    for (int indexTwo = 0; indexTwo < EffectsTotal; indexTwo++)
        Mix_VolumeChunk(SoundFX[indexTwo], EffectsVolume);

    if(Mix_PlayChannel(-1, SoundFX[soundIndex], loops)==-1)
    {
        printf( "Mix_PlayChannel: %s\n",Mix_GetError() );
    }
}
