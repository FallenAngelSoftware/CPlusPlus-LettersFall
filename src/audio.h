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
#ifndef AUDIO
#define AUDIO

class Audio
{
public:

    Audio(void);
    virtual ~Audio(void);

    bool AudioWorking;

    char Mix_Init_Error[256];

    #define MusicTotal      10
    Mix_Music *MusicTrack[MusicTotal];
    int MusicVolume;
    Uint8 CurrentlySelectedMusicTrack;
    #define EffectsTotal      11
    Mix_Chunk *SoundFX[EffectsTotal];
    int EffectsVolume;

    void GetDataFromFile(const char* fileName, std::stringstream& ss);
    void SetupAudio(void);
    void PlayMusic(Uint8 musicIndex, int loop);
    void PlayDigitalSoundFX(Uint8 soundIndex, int loop);
};

#endif
