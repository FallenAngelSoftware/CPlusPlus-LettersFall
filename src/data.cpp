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
#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h>
#else
    #include <stdio.h>
    #include <stdlib.h>
#endif

#include <string.h>

#include <fstream>
#include <sstream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "data.h"

#include "audio.h"
#include "input.h"
#include "logic.h"
#include "visuals.h"

extern Audio* audio;
extern Input* input;
extern Logic* logic;
extern Visuals* visuals;

//-------------------------------------------------------------------------------------------------
Data::Data(void)
{
    ClearHighScores();

    NumberOfPlays = 0;

    PlayerRankOnGameOver = 10;
    NameInputJoyCharX = 0;
    NameInputJoyCharY = 0;
    NameInputJoyChar = (char)NULL;
    NewHighScoreRegistered = false;
}

//-------------------------------------------------------------------------------------------------
Data::~Data(void)
{

}

//-------------------------------------------------------------------------------------------------
void Data::CheckForNewHighScore(void)
{
int playerRank = 10;

    for (int rank = 0; rank < 10; rank++)
    {
        if (logic->Score >= HighScoresScore[logic->GameMode][rank])
        {
            playerRank = rank;

            for (int index = 9; index > rank; index--)
            {
                strcpy(HighScoresName[logic->GameMode][index], HighScoresName[logic->GameMode][index-1]);
                HighScoresLevel[logic->GameMode][index] = HighScoresLevel[logic->GameMode][index-1];
                HighScoresScore[logic->GameMode][index] = HighScoresScore[logic->GameMode][index-1];
            }

            char erase[20];
            for (int letter = 0; letter < 20; letter++)  erase[letter] = '\0';
            for (int letterTwo = 0; letterTwo < 20; letterTwo++)  HighScoresName[logic->GameMode][rank][letterTwo] = '\0';
            strcpy(HighScoresName[logic->GameMode][rank], erase);

            HighScoresLevel[logic->GameMode][rank] = (Uint8)logic->Level;
            HighScoresScore[logic->GameMode][rank] = logic->Score;

            rank = 10;
        }
    }

    HighScoresName[logic->GameMode][playerRank][0] = '\0';
	NameInputArayIndex = 0;

    PlayerRankOnGameOver = playerRank;

    NameInputJoyCharX = 0;
    NameInputJoyCharY = 0;
    NameInputJoyChar = 'A';
}

//-------------------------------------------------------------------------------------------------
void Data::ClearHighScores(void)
{
std::string name[10];

    name[0] += "The Fallen Angel";
    name[1] += "Merkredy";
    name[2] += "Daotheman";
    name[3] += "DJ Fading Twilight";
    name[4] += "Ronald P.";
    name[5] += "You!";
    name[6] += "You!";
    name[7] += "You!";
    name[8] += "You!";
    name[9] += "You!";

    for (int gameMode = 0; gameMode < 5; gameMode++)
    {
        for (int loop = 0; loop < 10; loop++)
        {
            switch (loop)
            {
                case 0:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 10;
                    HighScoresScore[gameMode][loop] = 10000;
                    break;

                case 1:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 9;
                    HighScoresScore[gameMode][loop] = 9000;
                    break;

                case 2:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 8;
                    HighScoresScore[gameMode][loop] = 8000;
                    break;

                case 3:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 7;
                    HighScoresScore[gameMode][loop] = 7000;
                    break;

                case 4:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 6;
                    HighScoresScore[gameMode][loop] = 6000;
                    break;

                case 5:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 5;
                    HighScoresScore[gameMode][loop] = 5000;
                    break;

                case 6:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 4;
                    HighScoresScore[gameMode][loop] = 4000;
                    break;

                case 7:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 3;
                    HighScoresScore[gameMode][loop] = 3000;
                    break;

                case 8:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 2;
                    HighScoresScore[gameMode][loop] = 2000;
                    break;

                case 9:
                    strcpy( HighScoresName[gameMode][loop], name[loop].c_str() );
                    HighScoresLevel[gameMode][loop] = 1;
                    HighScoresScore[gameMode][loop] = 1000;
                    break;

                default:
                    break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Data::GetDataFromFile(const char* fileName, std::stringstream& ss)
{
    std::ifstream file(fileName);
    if(file.is_open())
    {
        ss << file.rdbuf();
    }
    file.close();
}

//-------------------------------------------------------------------------------------------------
void Data::LoadHighScoresAndOptions(void)
{
char fileName[1024];
char *base_path = SDL_GetPrefPath("FallenAngelSoftware", "LettersFall-Retail3");
char *data_path = NULL;

    if (base_path)
    {
        data_path = SDL_strdup(base_path);
        SDL_free(base_path);
    }
    else  return;

	strcpy(fileName, data_path);
	strcat(fileName, "LettersFall-5-Retail3a-Data");

    std::stringstream ss;
    std::string data;
    GetDataFromFile(fileName, ss);
    if ( std::getline(ss, data) )  NumberOfPlays = atoi( data.c_str() );
    if ( std::getline(ss, data) )  audio->MusicVolume = atoi( data.c_str() );
    if ( std::getline(ss, data) )  audio->EffectsVolume = atoi( data.c_str() );
    if ( std::getline(ss, data) )  visuals->FullScreenMode = atoi( data.c_str() );
    if ( std::getline(ss, data) )  logic->GameMode = atoi( data.c_str() );
    if ( std::getline(ss, data) )  logic->SelectedBackground = atoi( data.c_str() );
    if ( std::getline(ss, data) )  logic->SelectedMusicTrack = atoi( data.c_str() );
    if ( std::getline(ss, data) )  logic->MusicJukebox = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyButtonOne = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyButtonTwo = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyUP = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyRIGHT = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyDOWN = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyLEFT = atoi( data.c_str() );
    if ( std::getline(ss, data) )  input->UserDefinedKeyPause = atoi( data.c_str() );

    for (int index = 0; index < 3; index++)
    {
        if ( std::getline(ss, data) )  input->JoyUP[index] = atoi( data.c_str() );
        if ( std::getline(ss, data) )  input->JoyDOWN[index] = atoi( data.c_str() );
        if ( std::getline(ss, data) )  input->JoyLEFT[index] = atoi( data.c_str() );
        if ( std::getline(ss, data) )  input->JoyRIGHT[index] = atoi( data.c_str() );
        if ( std::getline(ss, data) )  input->JoyButton1[index] = atoi( data.c_str() );
        if ( std::getline(ss, data) )  input->JoyButton2[index] = atoi( data.c_str() );
    }

    for (int gameMode = 0; gameMode < 5; gameMode++)
    {
        for (Uint8 rank = 0; rank < 10; rank++)
        {
            if ( std::getline(ss, data) )  strcpy( HighScoresName[gameMode][rank], data.c_str() );
            if ( std::getline(ss, data) )  HighScoresLevel[gameMode][rank] = atoi( data.c_str() );
            if ( std::getline(ss, data) )  HighScoresScore[gameMode][rank] = atoi( data.c_str() );
        }
    }
}

//-------------------------------------------------------------------------------------------------
void Data::SaveHighScoresAndOptions(void)
{
char fileName[1024];
char *base_path = SDL_GetPrefPath("FallenAngelSoftware", "LettersFall-Retail3");
char *data_path = NULL;

    if (base_path)
    {
        data_path = SDL_strdup(base_path);
        SDL_free(base_path);
    }
    else  return;

	strcpy(fileName, data_path);
	strcat(fileName, "LettersFall-5-Retail3a-Data");
	std::ofstream file(fileName);

    file << NumberOfPlays << "\n";
    file << audio->MusicVolume << "\n";
    file << audio->EffectsVolume << "\n";
    file << visuals->FullScreenMode << "\n";
    file << logic->GameMode << "\n";
    file << logic->SelectedBackground << "\n";
    file << logic->SelectedMusicTrack << "\n";
    file << logic->MusicJukebox << "\n";
    file << input->UserDefinedKeyButtonOne << "\n";
    file << input->UserDefinedKeyButtonTwo << "\n";
    file << input->UserDefinedKeyUP << "\n";
    file << input->UserDefinedKeyRIGHT << "\n";
    file << input->UserDefinedKeyDOWN << "\n";
    file << input->UserDefinedKeyLEFT << "\n";
    file << input->UserDefinedKeyPause << "\n";

    for (int index = 0; index < 3; index++)
    {
        file << input->JoyUP[index] << "\n";
        file << input->JoyDOWN[index] << "\n";
        file << input->JoyLEFT[index] << "\n";
        file << input->JoyRIGHT[index] << "\n";
        file << input->JoyButton1[index] << "\n";
        file << input->JoyButton2[index] << "\n";
    }

    for (int gameMode = 0; gameMode < 5; gameMode++)
    {
        for (Uint8 rank = 0; rank < 10; rank++)
        {
            char textBuffer[512];
            strcpy(textBuffer, HighScoresName[gameMode][rank]);
            file << textBuffer << "\n";

            sprintf(textBuffer, "%d", HighScoresLevel[gameMode][rank]);
            file << textBuffer << "\n";

            sprintf(textBuffer, "%d", HighScoresScore[gameMode][rank]);
            file << textBuffer << "\n";
        }
    }

    file.close();
}

//-------------------------------------------------------------------------------------------------
void Data::DisplayWebsite(void)
{
    #ifdef _WIN32
        ShellExecute(NULL, "open", "http://www.fallenangelsoftware.com", NULL, NULL, SW_SHOWNORMAL);
    #else
        char buf[1024];
        snprintf(buf, sizeof(buf), "xdg-open %s", "http://www.fallenangelsoftware.com");
        system(buf);
    #endif
}
