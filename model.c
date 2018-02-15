#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "model.h"

void nlog(const char *fmt, ...);

/*
#ifndef HAVE_STRDUP
static char *strdup(const char *s1)
{
	size_t len = strlen(s1) + 1;
	char *s2;

	s2 = malloc(len);
	if(s2)
		memcpy(s2, s1, len);

	return s2;
}
#endif
*/
/* Returns the filename of the associated texture to load */
static char *load_mesh(FILE *f, struct mesh *m)
{
	char texname[64];
	size_t r;
	size_t len;

	fread(texname, 1, 64, f);

	r = fread(&m->nverts, 1, 4, f);
	if(r != 4)
	{
		nlog("load_bin: Error reading vertex count");
		goto out1;
	}

	r = fread(&m->nelements, 1, 4, f);
	if(r != 4)
	{
		nlog("load_bin: Error reading element count");
		goto out1;
	}

	len = sizeof(float[8]) * m->nverts;

	m->verts = malloc(len);
	if(!m->verts)
	{
		nlog("load_bin: Out of memory");
		goto out1;
	}

	fseek(f, 32-4-4, SEEK_CUR);
	r = fread(m->verts, 1, len, f);
	if(r != len)
	{
		nlog("load_bin: Error reading vertex data");
		goto out2;
	}

	if(m->nelements > 65535UL)
	{
		m->ele_size = 4;
		m->int_elements = malloc(sizeof(int) * m->nelements * 3);
		if(!m->int_elements)
		{
			nlog("load_bin: Out of memory");
			goto out3;
		}
	}
	else
	{
		m->ele_size = 2;
		m->short_elements = malloc(sizeof(short) * m->nelements * 3);
		if(!m->short_elements)
		{
			nlog("load_bin: Out of memory");
			goto out3;
		}
	}

	len = m->nelements * m->ele_size * 3;
	r = fread(m2ptr(m), 1, len, f);
	if(r != len)
	{
		nlog("load_bin: Error reading element data");
		goto out3;
	}

	return strdup(texname);

out3:
	free(m2ptr(m));
out2:
	free(m->verts);
out1:
	fclose(f);

	return 0;
}

struct model *load_bin(const char *name)
{
	FILE *f;
	struct model *model;
	unsigned int n, i;
	int r;

	f = fopen(name, "rb");
	if(!f)
	{
		nlog("load_bin: Too dumb to find %s", name);
		return 0;
	}

	r = fread(&n, 1, 4, f);
	if(r != 4)
	{
		nlog("load_bin: No object count, file empty?");
		goto out1;
	}

	fseek(f, 32, SEEK_SET);

	model = malloc(sizeof (struct model));
	if(!model)
	{
		nlog("load_bin: Out of memory");
		goto out2;
	}

	model->meshes = malloc(sizeof (struct mesh *[16]));
	if(!model->meshes)
	{
		nlog("load_bin: Out of memory");
		goto out3;
	}

	model->nmeshes = n;

	for(i = 0; i < n; i++)
	{
		char *texture_filename;
		size_t pos;
		
		model->meshes[i] = malloc(sizeof (struct mesh));
		if(!model->meshes[i])
		{
			nlog("load_bin: Out of memory");
			goto out4;
		}

		texture_filename = load_mesh(f, model->meshes[i]);
		if(!texture_filename)
			return 0;

		model->meshes[i]->p = load_png(texture_filename);
		free(texture_filename);
		if(!model->meshes[i]->p.pixels)
		{
			nlog("load_bin: png load failed");
			goto out4;
		}
		pos = ftell(f);
		pos = ((pos+31)/32)*32; /* Round up to 32 for file alignment*/
		fseek(f, pos, SEEK_SET);
	}

	return model;

out4:
	for(i = 0; i < n; n++)
		if(model->meshes[i])
			free(model->meshes[i]);
		else
			break;
out3:
	free(model->meshes[i]);
out2:
	free(model);
out1:
	fclose(f);

	return NULL;
}
