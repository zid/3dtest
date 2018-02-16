#ifndef ENTITYPRIV_H
#define ENTITYPRIV_H

#include "gl.h"
#include "model.h"

struct entity
{
	/* These should be opengl types but I am not puilling that
	 * entire header just for these declarations, yet.
	 */
	GLuint *vao, *tex;
	GLint scale, rot, trans;

	float rotm4[16];
	float transm4[16];
	float scale1f;

	/* Mesh count, mesh data, texture info, etc */
	struct model *m;
};

#endif
