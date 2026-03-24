// filename: xx_conf.h
#ifndef XX_CONF_H
#define XX_CONF_H

#define WIN_W 1024
#define WIN_H 768
#define TILE_SIZE 64
#define MAP_COL 16
#define MAP_ROW 12
#define CHUNK_W (MAP_COL * TILE_SIZE)   // 16*64 = 1024
#define CHUNK_H (MAP_ROW * TILE_SIZE)   // 10*64 = 640
#define TILE_SHEET_SIZE 4  
#define MAX_ENEMY 8
#define MAX_CIVILIAN 8
#define MAX_TRAFFIC (MAX_ENEMY + MAX_CIVILIAN)
#define MAX_SHOTS  15
#define SHOT_TICK  30 // how many ticks should a shot stay on the screen 
#define FIRE_TICK  10  // wait some time before between shots.  
#define MAX_EXPLOSIONS 20
#define PARTICLE_COUNT 16
#define TRAFFIC_KIND_COUNT 2
#define POWER_UP_INTERVAL 1750  // score plus POWER_UP_INTERVAL before next power of a type.
#define INITIAL_GREEN_PWRUP 250 // 250
#define INITIAL_RED_PWRUP 1250   // 500
#define DURATION_GREEN_PWRUP 1000 * 12 // milli seconds converted to seconds

// ActorManager
#define MAX_ACTOR 128

#endif
