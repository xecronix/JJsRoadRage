/* file: sfx_manager.c */
#include "raylib.h"
#include "sfx_manager.h"
#include <stdio.h>

#define SONG_COUNT 6

static Sound s_shot;
static Sound s_hit;
static Sound s_gameOver;
static Sound s_menuChange;
static Sound s_menuAccept;

static Music s_mp3;

static int s_ready = 0;
static int s_musicReady = 0;
static char *s_songs[6];
static int s_currentSong = -1;
static float s_musicVolume = .5;
static const char *s_songTitles[6] =
{
    "Red Light Revolution",
    "Brake Check Banger",
    "Asphalt Anthem",
    "Horn Section Havoc",
    "Neon Tire Fire",
    "Highway Hallelujah"
};

int Sfx_Init(void)
{
    InitAudioDevice();
    if (!IsAudioDeviceReady())
    {
        printf("ERROR: audio device not ready\n");
        return 0;
    }

    s_ready = 1;
    return 1;
}

int Sfx_LoadAll(void)
{
    if (!s_ready) return 0;

    s_shot         = LoadSound("assets/shot.wav");
    s_hit          = LoadSound("assets/hit.wav");
	s_gameOver     = LoadSound("assets/explode.wav");
    s_menuChange   = LoadSound("assets/click.wav");
    s_menuAccept   = LoadSound("assets/click_heavy.wav");
    
    s_songs[0] = "assets/a1-astronaut12-ultimate-gaming-soundtrack-for-legends_astronaut-272122.mp3";
    s_songs[1] = "assets/a2-astronaut12-energy-gaming-electro-trap-301124.mp3";
    s_songs[2] = "assets/a3-astronaut12-victory-awaits-in-the-gaming-universe_astronaut-265184.mp3";
    s_songs[3] = "assets/a4-fassounds-level-up-energetic-gaming-rock-music-251284.mp3";
    s_songs[4] = "assets/a5-hitslab-retro-retro-synthwave-gaming-music-270173.mp3";
    s_songs[5] = "assets/a6-mroneilovealot-neon-overdrive-cyberpunk-gaming-edm-415723.mp3";

    if (!IsSoundValid(s_shot))       return 0;
    if (!IsSoundValid(s_hit))      return 0;
	if (!IsSoundValid(s_gameOver))      return 0;
    if (!IsSoundValid(s_menuChange))   return 0;
    if (!IsSoundValid(s_menuAccept)) return 0;

    SetSoundVolume(s_shot, 0.50f);
    SetSoundVolume(s_hit, 0.70f);
	SetSoundVolume(s_gameOver, 0.70f);
    SetSoundVolume(s_menuChange, 0.35f);
    SetSoundVolume(s_menuAccept, 0.45f);

    Sfx_SetSong(0);

    return 1;
}

int Sfx_PlayShot(void)
{
    if (!s_ready) return 0;

    /* Use PlaySoundMulti if you want overlapping shots */
    PlaySound(s_shot);
    return 1;
}

int Sfx_PlayHit(void)
{
    if (!s_ready) return 0;
    PlaySound(s_hit);
    return 1;
}

int Sfx_PlayMenuChange(void)
{
    if (!s_ready) return 0;
    PlaySound(s_menuChange);
    return 1;
}

int Sfx_PlayMenuAccept(void)
{
    if (!s_ready) return 0;
    PlaySound(s_menuAccept);
    return 1;
}

int Sfx_PlayGameOver(void)
{
    if (!s_ready) return 0;
    PlaySound(s_gameOver);
    return 1;
}

int Sfx_Shutdown(void)
{
    if (!s_ready) return 1;

	if (s_musicReady)
    {
        StopMusicStream(s_mp3);
        UnloadMusicStream(s_mp3);
        s_musicReady = 0;
    }

    UnloadSound(s_shot);
    UnloadSound(s_hit);
    UnloadSound(s_menuChange);
    UnloadSound(s_menuAccept);
	UnloadSound(s_gameOver);

    CloseAudioDevice();
    s_ready = 0;
    return 1;
}

int Sfx_Update(void)
{
    if (!s_musicReady) return 0;

    /* Music streams must be updated every frame */
    UpdateMusicStream(s_mp3);
    if (!IsMusicStreamPlaying(s_mp3) || s_musicReady == 0)
	{
        int nextSong = s_currentSong + 1;
        if (nextSong > 5) { nextSong = 0; }
 
        Sfx_SetSong(nextSong);
	}

    return 1;
}

const char *Sfx_CurrentSongPlaying(void)
{
    
    return s_songTitles[s_currentSong];
}

int Sfx_CurrentSongIndex(void)
{
    return s_currentSong;
}

int Sfx_MusicVolumeDown(void)
{
    s_musicVolume -= .1f;
    if (s_musicVolume < 0.0) {s_musicVolume = 0.0;}
    SetMusicVolume(s_mp3, s_musicVolume);
    return 1;
}

int Sfx_MusicVolumeUp(void)
{
    s_musicVolume += .1f;
    if (s_musicVolume > 1.0) {s_musicVolume = 1.0;}
    SetMusicVolume(s_mp3, s_musicVolume);
    return 1;
}

int Sfx_SetSong(int songIdx)
{
    Music nextSong;
    
    if (songIdx == s_currentSong) {return 1;} //no op no err

    if (songIdx < 0 || songIdx >= SONG_COUNT) return 0;
    if (s_songs[songIdx] == NULL) return 0;

    nextSong = LoadMusicStream(s_songs[songIdx]);
    if (nextSong.stream.buffer == 0)
    {
        printf("ERROR: failed to load %s\n", s_songs[songIdx]);
        return 0;
    }

    if (s_musicReady)
    {
        StopMusicStream(s_mp3);
        UnloadMusicStream(s_mp3);
    }
    
    nextSong.looping = false;
    SetMusicVolume(nextSong, s_musicVolume);
    s_currentSong = songIdx;
    s_musicReady = 1;
    s_mp3 = nextSong;
    PlayMusicStream(s_mp3);
    return 1;
}

int Sfx_CurrentMusicVolume(void)
{
    return (int)(s_musicVolume * 10);
}
