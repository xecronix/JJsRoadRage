// filename: actor.c
#include "xx_types.h"
#include "actor.h"
#include <string.h>

int ActorClone (Actor *dest, const Actor *src)
{
        memcpy(dest, src, sizeof(Actor));
        return 1;
}


