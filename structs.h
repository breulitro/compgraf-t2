#ifndef _FOO_
#define _FOO_
#include <glib.h>

typedef struct {
	float x, y, z;
} val_t;

typedef struct {
	int frame;
	val_t *trans;
	val_t *scale;
	val_t *rot;
} animation_t;

typedef struct {
	int id;
	char *file;
	GSList *animations;
} actor_t;

void dump_val(val_t *v);
void dump_val_int(val_t *v);
void dump_animation(animation_t *a);
void dump_actor(actor_t *a);

#endif
