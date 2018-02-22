#include <string.h>
#include "gfx.h"
#include "gl.h"
#include "entity-priv.h"

static float perspm4[16];
static GLint persp;
extern GLint shader;

void gfx_kill(void)
{
	gl_destroy();
}

void gfx_set_persp(const float *s)
{
	memcpy(perspm4, s, sizeof(perspm4));
}

void gfx_render_entity(struct entity *e)
{
	unsigned int i;

	glUseProgram(shader);

	glUniformMatrix4fv(e->rot,   1, GL_TRUE, e->rotm4);
	glUniformMatrix4fv(e->trans, 1, GL_TRUE, e->transm4);
	glUniformMatrix4fv(persp,    1, GL_TRUE, perspm4);
	glUniform1f(e->scale, e->scale1f);

	for(i = 0; i < e->m->nmeshes; i++)
	{
		size_t ele_count, ele_size;

		glBindVertexArray(e->vao[i]);
		glBindTexture(GL_TEXTURE_2D, e->tex[i]);

		ele_count = e->m->meshes[i]->nelements * 3;
		ele_size = e->m->meshes[i]->ele_size == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		glDrawElements(GL_TRIANGLES, ele_count, ele_size, 0);
	}
}

void gfx_clear(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gfx_init()
{
	gl_init();
	persp = gl_get_uniform("persp");
}
