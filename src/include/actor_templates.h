/* Filename: actor_templates.h
   Purpose: Envelope/template spawn system (no string parsing)
*/

#ifndef ACTOR_TEMPLATES_H
#define ACTOR_TEMPLATES_H

#include <string.h> /* memcpy */
#include "actor.h"  /* Actor */
#include "xx_types.h"


void Actor_InitFromTemplate(Actor *dst, const Actor *tpl);

/* Example templates (fill these in one .c file, not in a header) */
extern const Actor TPL_HERO;
extern const Actor TPL_HEROSHOT;
extern const Actor TPL_ENEMY;
extern const Actor TPL_ENEMYSHOT;
extern const Actor TPL_CIVILIAN;
extern const Actor TPL_ROADSIDE;
extern const Actor TPL_POWER_UP;

#endif
