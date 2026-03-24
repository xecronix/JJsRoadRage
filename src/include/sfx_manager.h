/* file: sfx_manager.h */
#ifndef SFX_MANAGER_H_INCLUDED
#define SFX_MANAGER_H_INCLUDED

// sfx

int Sfx_Init(void);
int Sfx_LoadAll(void);
int Sfx_PlayShot(void);
int Sfx_PlayHit(void);
int Sfx_PlayMenuChange(void);
int Sfx_PlayMenuAccept(void);
int Sfx_PlayGameOver(void);
char *Sfx_CurrentSongPlaying(void);
int Sfx_CurrentSongIndex(void);
int Sfx_MusicVolumeUp(void);
int Sfx_MusicVolumeDown(void);
int Sfx_SetSong(int songIdx);
int Sfx_CurrentMusicVolume(void);

// Music
int Sfx_Update(void);

int Sfx_Shutdown(void);

#endif
