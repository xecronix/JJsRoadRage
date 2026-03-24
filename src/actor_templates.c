/* Filename: actor_templates.c */
#include <string.h> /* memcpy */
#include "actor_templates.h"
#include "actor.h"
#include "xx_types.h"

void Actor_InitFromTemplate(Actor *dst, const Actor *tpl) {
    // Full overwrite reset (FB-style "actorReset") 
    memcpy(dst, tpl, sizeof(Actor));
}

/* Example templates (fill these in one .c file, not in a header) */
extern const Actor TPL_HERO;
extern const Actor TPL_HEROSHOT;
extern const Actor TPL_ENEMY;
extern const Actor TPL_ENEMYSHOT;
extern const Actor TPL_CIVILIAN;
extern const Actor TPL_ROADSIDE;
extern const Actor TPL_POWER_UP;

const Actor TPL_HERO = {
    .kind = ACT_KIND_HERO,
    .atk = 1,
    .def = 10,
    .state = ACT_ST_ACTIVE,
    .position = {600,400},
    .size = {64, 64},
    .imgAsset = {0},   /* safe zero init */
    .speed = 3,
    .recyclable = 0
};

const Actor TPL_HEROSHOT = {
    .kind = ACT_KIND_HEROSHOT,
    .atk = 1,
    .def = 10,
    .state = ACT_ST_ACTIVE,
    .position = {0,0},
    .size = {64, 64},
    .imgAsset = {0},   
    .speed = 3,
    .recyclable = 0
};

const Actor TPL_ENEMY = {
    .kind = ACT_KIND_ENEMY,
    .atk = 1,
    .def = 10,
    .state = ACT_ST_ACTIVE,
    .position = {0, 0},
    .size = {64, 64},
    .imgAsset = {0},
    .speed = 3,
    .recyclable = 0
};

const Actor TPL_CIVILIAN = {
    .kind = ACT_KIND_CIVILIAN,
    .atk = 1,
    .def = 10,
    .state = ACT_ST_ACTIVE,
    .position = {0, 0},
    .size = {64, 64},
    .imgAsset = {0},
    .speed = 3,
    .recyclable = 0
};

const Actor TPL_POWER_UP = {
    .kind = ACT_KIND_POWER_UP,
    .atk = 1,
    .def = 10,
    .state = ACT_ST_HIDDEN,
    .position = {0, -64},
    .size = {64, 64},
    .imgAsset = {0},
    .speed = 3,
    .recyclable = 0
};

