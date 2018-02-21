#ifndef GFX_H
#define GFX_H

#include "entity.h"

void gfx_init();
void gfx_kill(void);
void gfx_clear(void);
void gfx_render_entity(struct entity *);
void gfx_set_persp(const float *);

#endif
