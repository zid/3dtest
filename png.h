#include <stdint.h>

struct png {
	uint32_t w, h;
	int format;
	void *pixels;
};

void png_kill(struct png *);
struct png load_png(const char *);