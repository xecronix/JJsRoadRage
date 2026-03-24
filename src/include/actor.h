// filename: actor.h
#ifndef ACTOR_H
#define ACTOR_H

#include "xx_types.h"   // defines Position2D, Size2D
#include "gfx_manager.h"

//int ActorInit   (Actor *a, const Actor *tpl);
int ActorClone (Actor *dest, const Actor *src);

#endif
