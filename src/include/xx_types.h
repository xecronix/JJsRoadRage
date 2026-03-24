// filename: xx_types.h
#ifndef XX_TYPES_H
#define XX_TYPES_H
#include "xx_conf.h"

typedef enum {
    KC_NULL            = 0,        // Key: NULL, used for no key pressed
    // Alphanumeric keys
    KC_APOSTROPHE      = 39,       // Key: '
    KC_COMMA           = 44,       // Key: ,
    KC_MINUS           = 45,       // Key: -
    KC_PERIOD          = 46,       // Key: .
    KC_SLASH           = 47,       // Key: /
    KC_ZERO            = 48,       // Key: 0
    KC_ONE             = 49,       // Key: 1
    KC_TWO             = 50,       // Key: 2
    KC_THREE           = 51,       // Key: 3
    KC_FOUR            = 52,       // Key: 4
    KC_FIVE            = 53,       // Key: 5
    KC_SIX             = 54,       // Key: 6
    KC_SEVEN           = 55,       // Key: 7
    KC_EIGHT           = 56,       // Key: 8
    KC_NINE            = 57,       // Key: 9
    KC_SEMICOLON       = 59,       // Key: ;
    KC_EQUAL           = 61,       // Key: =
    KC_A               = 65,       // Key: A | a
    KC_B               = 66,       // Key: B | b
    KC_C               = 67,       // Key: C | c
    KC_D               = 68,       // Key: D | d
    KC_E               = 69,       // Key: E | e
    KC_F               = 70,       // Key: F | f
    KC_G               = 71,       // Key: G | g
    KC_H               = 72,       // Key: H | h
    KC_I               = 73,       // Key: I | i
    KC_J               = 74,       // Key: J | j
    KC_K               = 75,       // Key: K | k
    KC_L               = 76,       // Key: L | l
    KC_M               = 77,       // Key: M | m
    KC_N               = 78,       // Key: N | n
    KC_O               = 79,       // Key: O | o
    KC_P               = 80,       // Key: P | p
    KC_Q               = 81,       // Key: Q | q
    KC_R               = 82,       // Key: R | r
    KC_S               = 83,       // Key: S | s
    KC_T               = 84,       // Key: T | t
    KC_U               = 85,       // Key: U | u
    KC_V               = 86,       // Key: V | v
    KC_W               = 87,       // Key: W | w
    KC_X               = 88,       // Key: X | x
    KC_Y               = 89,       // Key: Y | y
    KC_Z               = 90,       // Key: Z | z
    KC_LEFT_BRACKET    = 91,       // Key: [
    KC_BACKSLASH       = 92,       // Key: '\'
    KC_RIGHT_BRACKET   = 93,       // Key: ]
    KC_GRAVE           = 96,       // Key: `
    // Function keys
    KC_SPACE           = 32,       // Key: Space
    KC_ESCAPE          = 256,      // Key: Esc
    KC_ENTER           = 257,      // Key: Enter
    KC_TAB             = 258,      // Key: Tab
    KC_BACKSPACE       = 259,      // Key: Backspace
    KC_INSERT          = 260,      // Key: Ins
    KC_DELETE          = 261,      // Key: Del
    KC_RIGHT           = 262,      // Key: Cursor right
    KC_LEFT            = 263,      // Key: Cursor left
    KC_DOWN            = 264,      // Key: Cursor down
    KC_UP              = 265,      // Key: Cursor up
    KC_PAGE_UP         = 266,      // Key: Page up
    KC_PAGE_DOWN       = 267,      // Key: Page down
    KC_HOME            = 268,      // Key: Home
    KC_END             = 269,      // Key: End
    KC_CAPS_LOCK       = 280,      // Key: Caps lock
    KC_SCROLL_LOCK     = 281,      // Key: Scroll down
    KC_NUM_LOCK        = 282,      // Key: Num lock
    KC_PRINT_SCREEN    = 283,      // Key: Print screen
    KC_PAUSE           = 284,      // Key: Pause
    KC_F1              = 290,      // Key: F1
    KC_F2              = 291,      // Key: F2
    KC_F3              = 292,      // Key: F3
    KC_F4              = 293,      // Key: F4
    KC_F5              = 294,      // Key: F5
    KC_F6              = 295,      // Key: F6
    KC_F7              = 296,      // Key: F7
    KC_F8              = 297,      // Key: F8
    KC_F9              = 298,      // Key: F9
    KC_F10             = 299,      // Key: F10
    KC_F11             = 300,      // Key: F11
    KC_F12             = 301,      // Key: F12
    KC_LEFT_SHIFT      = 340,      // Key: Shift left
    KC_LEFT_CONTROL    = 341,      // Key: Control left
    KC_LEFT_ALT        = 342,      // Key: Alt left
    KC_LEFT_SUPER      = 343,      // Key: Super left
    KC_RIGHT_SHIFT     = 344,      // Key: Shift right
    KC_RIGHT_CONTROL   = 345,      // Key: Control right
    KC_RIGHT_ALT       = 346,      // Key: Alt right
    KC_RIGHT_SUPER     = 347,      // Key: Super right
    KC_KB_MENU         = 348,      // Key: KB menu
    // Keypad keys
    KC_KP_0            = 320,      // Key: Keypad 0
    KC_KP_1            = 321,      // Key: Keypad 1
    KC_KP_2            = 322,      // Key: Keypad 2
    KC_KP_3            = 323,      // Key: Keypad 3
    KC_KP_4            = 324,      // Key: Keypad 4
    KC_KP_5            = 325,      // Key: Keypad 5
    KC_KP_6            = 326,      // Key: Keypad 6
    KC_KP_7            = 327,      // Key: Keypad 7
    KC_KP_8            = 328,      // Key: Keypad 8
    KC_KP_9            = 329,      // Key: Keypad 9
    KC_KP_DECIMAL      = 330,      // Key: Keypad .
    KC_KP_DIVIDE       = 331,      // Key: Keypad /
    KC_KP_MULTIPLY     = 332,      // Key: Keypad *
    KC_KP_SUBTRACT     = 333,      // Key: Keypad -
    KC_KP_ADD          = 334,      // Key: Keypad +
    KC_KP_ENTER        = 335,      // Key: Keypad Enter
    KC_KP_EQUAL        = 336,      // Key: Keypad =
    // Android key buttons
    KC_BACK            = 4,        // Key: Android back button
    KC_MENU            = 5,        // Key: Android menu button
    KC_VOLUME_UP       = 24,       // Key: Android volume up button
    KC_VOLUME_DOWN     = 25        // Key: Android volume down button
} KBKey;

// Geometry
enum Direction{
    NORTH,
    SOUTH,
    EAST,
    WEST
};

typedef struct{
	int x;
	int y;	
} Point2D;

typedef struct{
	int row;
	int col;	
} Grid2D;

typedef struct{
	int w;
	int h;	
} Size2D;

typedef struct{
	int x;
	int y;	
} Position2D;

typedef struct {
	Point2D particles[PARTICLE_COUNT];
	Point2D origin;
	int frame;
	int active;
}ExplosionParticles;

// GFX
typedef struct {
    int id;               /* index into _images[] */
	int ready;            /* set to 1 when an image is loaded*/ 
    Position2D src;       /* texture-space top-left */
    Position2D dest;      /* screen-space top-left */
    Size2D imgSize;       /* width/height */
    Point2D rotationPoint;/* origin for rotation (relative to dest rect) */
    float rotationAngle;  /* degrees */
} ImageAsset;

#define XXRGBA(r,g,b,a) ((XXColor){ (unsigned char)(r), (unsigned char)(g), (unsigned char)(b), (unsigned char)(a) })
#define XX_WHITE     XXRGBA(255,255,255,255)
#define XX_BLACK     XXRGBA(0,0,0,255)
#define XX_PURPLE    XXRGBA(128,0,128,255)
#define XX_GREEN     XXRGBA(0,  190, 0,255)
#define XX_RED       XXRGBA(230,41,55,255)
#define XX_YELLOW    XXRGBA(253,249,0,255)
#define XX_ORANGE    XXRGBA(255,161,0,255)
#define XX_GRAY      XXRGBA(130,130,130,255)
#define XX_LIGHTGRAY XXRGBA(200,200,200,255)

typedef struct XXColor {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} XXColor;

// Game state 
typedef enum RunMode
{
    RUNMODE_RUNNING = 0,
    RUNMODE_PAUSED,
    RUNMODE_DEMO,
    RUNMODE_QUIT
} RunMode;

typedef enum ScreenId
{
    SCREEN_START = 0,
    SCREEN_SETTINGS,
    SCREEN_PLAY,
    SCREEN_SCORES,
    SCREEN_RESUME,
    SCREEN_DEBUG
} ScreenId;

typedef enum ScreenView
{
    SCRVIEW_WINDOW     = 'W',
    SCRVIEW_FULLSCREEN = 'F'
} ScreenView;

typedef enum Difficulty
{
    DIFF_WIMP = 1,
    DIFF_EASY,
    DIFF_HARD,
    DIFF_BOSS
} Difficulty;

typedef struct RunStats
{
    int score;
    int shots_fired;
    int enemies_hit;
    int civilians_hit;
} RunStats;

typedef struct GameState
{
    RunMode run_mode;
    ScreenId screen_id;
    ScreenView screen_view;
    RunStats run_stats;
    Difficulty difficulty;
    long long now_ms;
} GameState;

// Actor
typedef enum ActorSlide{
	SLIDE_RIGHT =  1, 	
	SLIDE_LEFT  = -1, 
	SLIDE_UP    = -2, 
	SLIDE_DOWN  =  2, 
	SLIDE_NONE  =  0, 
}ActorSlide;

typedef enum ActorKind{
    ACT_KIND_HERO,        // By default, assigned value 0
    ACT_KIND_HEROSHOT,    // Assigned value 1
    ACT_KIND_ENEMY,       // Assigned value 2
    ACT_KIND_ENEMYSHOT,   // Assigned value 3
    ACT_KIND_CIVILIAN,    // Assigned value 4
    ACT_KIND_POWER_UP     // Assigned value 5
}ActorKind;

typedef enum ActorState{
	ACT_ST_ACTIVE,
	ACT_ST_INACTIVE,
	ACT_ST_VISIBLE,
	ACT_ST_HIDDEN
}ActorState;

typedef struct {
    int kind;            // which kind or type of actor
    int atk;             // how much damage the actor can inflict
    int def;             // how much damage the actor can sustain
    int state;           // visible, dying, etc
	int preferredX;      // Added for enemy and civilian stay on road logic.
    Position2D position; // x,y
    Size2D size;         // w,h
    ImageAsset imgAsset; // sprite/texture handle
    int speed;           // pixels per move step (or per tick)
	int slideDirection;  // if the actor should be sliding this is the direction
	int slideTick;       // how long should the actor slide.  
    int recyclable;      // if the actor manager can reuse this actor. 1 actor is in the pool.  0 not in pool.
} Actor;

// Related to World and World Maps
typedef struct  {
    ImageAsset imgAsset;
	char mapkey;
    int x;
    int y;
} TileRec;

typedef struct {
	int firstRow;
	int lastRow;
	int firstCol;
	int lastCol;
}FrameView;
#endif
