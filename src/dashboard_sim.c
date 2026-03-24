#include "dashboard_sim.h"
#include "gfx_manager.h"
#include "sfx_manager.h"
#include "xx_types.h"
#include "xx_input.h"
#include <string.h>
#include <stdio.h>

#define CHAR_SLOTS 27
#define SZ_SCROLL_TEXT 512
#define SZ_LAST_SONG (SZ_SCROLL_TEXT - (CHAR_SLOTS * 2))
#define RADIO_TEXT_X   430
#define RADIO_TEXT_Y   80
#define RADIO_VOLUME_X 425
#define RADIO_VOLUME_Y 55
#define RADIO_SONG_IND_X 435
#define RADIO_SONG_IND_Y 115


ImageAsset dashboard;
static int s_doOnce = 0;
static int s_scroll_idx = 0;
static char s_last_song[SZ_LAST_SONG];
static char s_scroll_text[SZ_SCROLL_TEXT];
static long long s_next_update = -1;
static GameState *gs;

void DoOnce(void)
{
    if (s_doOnce == 1) {return;}
    
    memset(s_last_song, '\0', SZ_LAST_SONG);
    memset(s_scroll_text, '\0', SZ_SCROLL_TEXT);
    LoadAssetFromFile(&dashboard, "assets/level1/stereo-complete-in-dash.png");
    dashboard.src.x = 0;
    dashboard.src.y = 0;
    dashboard.imgSize.w = 1024;
    dashboard.imgSize.h = 235;
    dashboard.rotationPoint.x = 0;
    dashboard.rotationPoint.y = 0;
    dashboard.rotationAngle = 0.0f;
    s_doOnce = 1;
}

int IfTimer(void)
{
    if (s_next_update < 0)
    {
        s_next_update = gs->now_ms + 175;
        return 0;
    }

    if (gs->now_ms >= s_next_update)
    {
        s_next_update += 175;
        return 1;
    }

    return 0;
}

int SongNameStr(char *out, int outSize)
{
    const char *song = Sfx_CurrentSongPlaying();
    int visible = outSize - 1;
    int len;
    int maxIndex;

    if (out == NULL || outSize <= 1) return 0;
    if (song == NULL) song = "";

    if (strcmp(song, s_last_song) != 0)
    {
        char spaces[CHAR_SLOTS + 1];
        memset(spaces, ' ', CHAR_SLOTS);
        spaces[CHAR_SLOTS] = '\0';
        snprintf(s_last_song, sizeof(s_last_song), "%s", song);
        snprintf(s_scroll_text, sizeof(s_scroll_text),
            "%s%s%s",
            spaces, song, spaces);
        s_scroll_idx = 0;
    }

    len = (int)strlen(s_scroll_text);

    if (len <= visible)
    {
        snprintf(out, outSize, "%s", s_scroll_text);
        return 1;
    }

    maxIndex = len - visible;

    if (IfTimer())
    {
        s_scroll_idx++;
        if (s_scroll_idx > maxIndex)
            s_scroll_idx = 0;
    }

    memcpy(out, s_scroll_text + s_scroll_idx, visible);
    out[visible] = '\0';
    return 1;
}

int DrawVolumeIndicator(void)
{
    int x = RADIO_VOLUME_X + dashboard.dest.x;
    int y = RADIO_VOLUME_Y + dashboard.dest.y;
    float r = 7.0f; 
    int spacing = (int)(r * 2) + 15;    
    
    for (int i = 0; i < Sfx_CurrentMusicVolume(); i++)    
    {
        XXDrawCircle(x, y, r, XX_ORANGE);
        x += spacing;
    }
    return 1;
}

int DrawSongIndicator(void)
{
    int x = RADIO_SONG_IND_X + dashboard.dest.x;
    int y = RADIO_SONG_IND_Y + dashboard.dest.y;
    float r = 7.0f; 
    
    x += Sfx_CurrentSongIndex() * 51;
    XXDrawCircle(x, y, r, XX_GRAY);
    return 1;
}

int DashboardSim(GameState *gmst, Position2D gamePosition)
{
    gs = gmst;  
    DoOnce();
    
    if (IsKBPressed(KC_KP_ADD) || IsKBPressed(KC_EQUAL)){Sfx_MusicVolumeUp();}
    if (IsKBPressed(KC_MINUS) || IsKBPressed(KC_KP_SUBTRACT)) {Sfx_MusicVolumeDown();}
    
    if (gs->screen_id != SCREEN_SETTINGS) // <<-- This screen uses 1 - 4 to adjust play level
    {
        if (IsKBPressed(KC_ONE)) {Sfx_SetSong(0);}
        if (IsKBPressed(KC_TWO)) {Sfx_SetSong(1);}
        if (IsKBPressed(KC_THREE)) {Sfx_SetSong(2);}
        if (IsKBPressed(KC_FOUR)) {Sfx_SetSong(3);}
        if (IsKBPressed(KC_FIVE)) {Sfx_SetSong(4);}
        if (IsKBPressed(KC_SIX)) {Sfx_SetSong(5);}
    }

    
    // if we're not in fullscreen mode return 0
    if (gs->screen_view != SCRVIEW_FULLSCREEN) {return 0;}
    
     
    // We're in fullscreen mode and we have some space under the playfield.
    // We'll put the dashboard a car stero in that spot.
    
    // Since the dashboard image is the same width as WIN_W this formula could 
    // have been s_viewPortPosition.x / 2.  But I'll leave it like it is.
    dashboard.dest.x = gamePosition.x + (WIN_W - dashboard.imgSize.w) / 2;
    dashboard.dest.y = WIN_H; 
    DrawAsset(&dashboard);
    
    char songTitle[CHAR_SLOTS + 1];
    SongNameStr(songTitle, sizeof(songTitle));
    XXDrawText(songTitle, dashboard.dest.x + RADIO_TEXT_X, dashboard.dest.y + RADIO_TEXT_Y, 20, XX_RED);
    DrawVolumeIndicator();
    DrawSongIndicator();

    return 1;
}
