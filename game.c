#include <math.h>
#include "entity.h"
#include "gfx.h"

/* Generic container for a set of verticies 
 * and their rotation/translation/scale etc 
 */

/* Bobomb Battlefield */
static struct entity *bbb;

static const float persp_mat[] = {
	1.0/tan(M_PI/4)/1.333, 0.0, 0.0, 0.0,
	0.0, 1.0/tan(M_PI/4), 0.0, 0.0,
	0.0, 0.0, -(8192+0.1)/(8192-0.1), -(2.0*8192*0.1)/(8192-0.1),
	0.0, 0.0, -1.0, 0.0
};

static const float identity[16] = {
	1, 0, 0, 0, 
	0, 1, 0, 0, 
	0, 0, 1, 0, 
	0, 0, 0, 1
};

static float trans[16] = {
	1, 0, 0, 5,
	0, 1, 0, -8,
	0, 0, 1, -10,
	0, 0, 0, 1,
};
void rot(float *f, float a, float x, float y, float z)
{
	float xx, xy, xz, xw, yy, yz, yw, zz, zw;
	float len;
	
	len = sqrt(x * x + y * y + z * z + a * a);
	x/=len; y/=len; z/=len; a/=len;

	xx = x * x;
	xy = x * y;
	xz = x * z;
	xw = x * a;
	yy = y * y;
	yz = y * z;
	yw = y * a;
	zz = z * z;
	zw = z * a;
	
	f[0]  = 1 - 2 * (yy + zz);
	f[1]  =     2 * (xy - zw);
	f[2]  =     2 * (xz + yw);
	f[3]  = 0;
	f[4]  =     2 * (xy + zw);
	f[5]  = 1 - 2 * (xx + zz);
	f[6]  =     2 * (yz - xw);
	f[7]  = 0;
	f[8]  =     2 * (xz - yw);
	f[9]  =     2 * (yz + xw);
	f[10] = 1 - 2 * (xx + yy);
	f[11] = 0;
	f[12] = 0;
	f[13] = 0;
	f[14] = 0;
	f[15] = 1;
}
void game_init(void)
{
	float rotm[16];
	gfx_set_persp(persp_mat);

	bbb = entity_from_file("bomb.bin");
	entity_set_scale(bbb, 4.68);
	entity_set_trans(bbb, trans);
	rot(rotm, 1.0, -.5, 0, 0.0);
	entity_set_rot(bbb, rotm);
}

void game(void)
{
	gfx_clear();
	gfx_render_entity(bbb);
}

void game_kill(void)
{
	entity_kill(bbb);
	gfx_kill();
}