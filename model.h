#ifndef MODEL_H
#define MODEL_H
#include <stdint.h>
#include "png.h"

#define m2ptr(m) ((m)->ele_size == 2 ? (void *)(m)->short_elements : (void *)(m)->int_elements)
struct mesh
{
	/* Mesh info */
	uint32_t nverts, nelements;
	int ele_size;
	float *verts;
	union {
		unsigned short *short_elements;
		uint32_t *int_elements;
	};
	
	/* Texture info */
	struct png p;
};

struct model {
	unsigned int nmeshes;
	struct mesh **meshes;
};

struct model *load_model(const char *name);
void model_kill(struct model *);
#endif