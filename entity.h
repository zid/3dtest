#ifndef ENTITY_H
#define ENTITY_H

struct entity;
struct entity *entity_from_file(const char *);
void entity_set_scale(struct entity *, float);
void entity_set_trans(struct entity *, const float *);
void entity_set_rot(struct entity *, const float *);
void entity_kill(struct entity *);

#endif