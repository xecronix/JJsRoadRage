#include "raylib.h"
#include "xx_types.h"
#include "xx_input.h"
#include "xx_conf.h"
#include "game_sim.h"
#include "gfx_manager.h"
#include "sfx_manager.h"
#include "settings_sim.h"
#include "start_sim.h"
#include "pause_sim.h"
#include "high_score_sim.h"
#include "dashboard_sim.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static const char* _level1MapFile = "assets/level1/level.map";
static char s_currentScreenView = SCRVIEW_FULLSCREEN;
static Position2D s_viewPortPosition = {0,0};

int SetGameStateTime(GameState *gs)
{
    struct timespec ts;
    
    timespec_get(&ts, TIME_UTC);
    gs->now_ms = (long long)ts.tv_sec * 1000LL + (long long)ts.tv_nsec / 1000000LL;
    return 1;
}

int SetScreenView(GameState *gs)
{
    int m  = GetCurrentMonitor();
    int mw = GetMonitorWidth(m);
    int mh = GetMonitorHeight(m);
    if (gs->screen_view == SCRVIEW_FULLSCREEN)
    {
        SetWindowState(FLAG_WINDOW_UNDECORATED);
        SetWindowSize(mw, mh);
        SetWindowPosition(0, 0);
        s_viewPortPosition.x = (mw - WIN_W) / 2;
        s_viewPortPosition.y = 0;
        s_currentScreenView = SCRVIEW_FULLSCREEN; 
    }
    else 
    {
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
        SetWindowSize(WIN_W, WIN_H);
        SetWindowPosition((mw - WIN_W) / 2, (mh - WIN_H) / 2);
        s_currentScreenView = SCRVIEW_WINDOW; 
        s_viewPortPosition.x = 0;
        s_viewPortPosition.y = 0;
    }
    return 1;
}

int main(void) 
{
	SetConfigFlags(FLAG_VSYNC_HINT); // Raylib
    InitWindow(WIN_W, WIN_H, "JJ's Road Rage: The Musical"); // Raylib
    Gfx_InitGameRT(WIN_W, WIN_H);
    //SetTargetFPS(60); // Raylib
	LoadMap(_level1MapFile);
	LoadMapTiles();
	//PrintMap();
	LoadPlayer();
	LoadCivilian();
	LoadEnemy();
	LoadShots();
	LoadExplosions();
    LoadPowerUps();
	GameState gs;
	gs.screen_id = SCREEN_SCORES;
	gs.difficulty = DIFF_EASY;
	gs.run_mode = RUNMODE_DEMO;
	gs.run_stats.score = 0;
    gs.screen_view = SCRVIEW_FULLSCREEN;
    SetScreenView(&gs);
    SetExitKey(KEY_NULL); // so the esc button doesn't close the window.  Raylib
    SetScreenView(&gs);

	if (!Sfx_Init()) return 1;
	if (!Sfx_LoadAll()) return 1;
    
    while (!WindowShouldClose()) // Raylib
	{
        SetGameStateTime(&gs);
        
        // high level input
        if (IsKBPressed(KC_P) || IsKBPressed(KC_ESCAPE))  
        {
            gs.run_mode = RUNMODE_PAUSED;
            gs.screen_id = SCREEN_RESUME;
        }

		Sfx_Update();
        
        BeginDrawing(); // Raylib
        
        // I think this is a waste.  But it seems to be needed when transitioning from
        // not full screen to full screen.
        ClearBackground(BLACK); // Raylib 
        
        // this creates an image/texture to draw on instead of the drawing on the screen
        Gfx_BeginGameRT(XX_BLACK); 
        
        // start the sims
        GameSim(&gs); // This one must be first. The rest of the sims draw over this one.
		StartSim(&gs);
        PauseSim(&gs);
		SettingsSim(&gs);
		HighScoreSim(&gs);
        
        // stop drawing on the image/texture we set up above --  Gfx_BeginGameRT(XX_BLACK);  
        Gfx_EndGameRT();
        
        // send the image/texture to the screen (probably a backbuffer)
        Gfx_PresentGameRT(s_viewPortPosition.x, s_viewPortPosition.y, WIN_W, WIN_H);
        
        DashboardSim(&gs, s_viewPortPosition);
        EndDrawing(); // Raylib
        
        // This is when we handle the event to toggle fullscreen y/n
        if (gs.screen_view != s_currentScreenView) {SetScreenView(&gs);}
        
        // Handle a quit message
        if (gs.run_mode == RUNMODE_QUIT) {break;}
    }

    FreeAssets();
	Sfx_Shutdown();
    CloseWindow(); // Raylib
    return 0;
}
