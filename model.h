#include <stdint.h>
#include "png.h"

#define m2ptr(m) ((m)->ele_type == 0 ? (void *)(m)->short_elements : (void *)(m)->int_elements)
#define mesh_ele_size(m) ((m)->ele_type ? sizeof(int) : sizeof(short))

struct mesh
{
	/* Mesh info */
	uint32_t nverts, nelements;
	int ele_type;
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

struct model *load_bin(const char *name);
