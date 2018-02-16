#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "entity-priv.h"
#include "model.h"

void nlog(const char *, ...);

struct entity *entity_from_file(const char *path)
{
	struct entity *e;
	struct png *p;
	GLuint vbo[2];
	unsigned int i, n;

	e = malloc(sizeof(struct entity));
	if(!e)
	{
		nlog("entity_from_file: Out of memory");
		exit(1);
	}

	e->m = load_model(path);
	if(!e->m)
	{
		nlog("load_model (%s) failed, exploding.", path);
		exit(1);
	}
	n = e->m->nmeshes;

	e->tex = malloc(sizeof (GLuint[n]));
	e->vao = malloc(sizeof (GLuint[n]));
	glGenTextures(n, e->tex);
	glGenVertexArrays(n, e->vao);

	for(i = 0; i < n; i++)
	{
		GLint format;
		GLsizeiptr data_size;

		p = &e->m->meshes[i]->p;
		glBindTexture(GL_TEXTURE_2D, e->tex[i]);
		format = p->format == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, p->w, p->h, 0, format, GL_UNSIGNED_BYTE, p->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindVertexArray(e->vao[i]);

		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float[8]) * e->m->meshes[i]->nverts, e->m->meshes[i]->verts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat[8]), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat[8]), (GLvoid *)(sizeof(GLfloat[3])));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat[8]), (GLvoid *)(sizeof(GLfloat[6])));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		data_size = e->m->meshes[i]->ele_size * e->m->meshes[i]->nelements * 3;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, m2ptr(e->m->meshes[i]), GL_STATIC_DRAW);

		png_kill(p);
	}

	e->trans = gl_get_uniform("trans");
	e->rot   = gl_get_uniform("rot");
	e->scale = gl_get_uniform("scale");

	return e;
}

void entity_set_scale(struct entity *e, const float s)
{
	e->scale1f = s;
}

void entity_set_trans(struct entity *e, const float *s)
{
	memcpy(e->transm4, s, sizeof (float[16]));
}

void entity_set_rot(struct entity *e, const float *s)
{
	memcpy(e->rotm4, s, sizeof (float[16]));
}

void entity_kill(struct entity *e)
{
	if(e->tex)
		free(e->tex);
	if(e->vao)
		free(e->vao);
	model_kill(e->m);
	free(e);
}
