#ifndef _FOO_
#define _FOO_
#include <glib.h>

#include "load_obj.h"

int playing;
int playloop;
int velocidade;
int frame_atual;
int visual_debug;
int maxFrame;

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
	model_t *obj;
} actor_t;

void dump_val(val_t *v);
void dump_val_int(val_t *v);
void dump_animation(animation_t *a);
void dump_actor(actor_t *a);

val_t *add_val_t(val_t *a, val_t *b);
void free_val_t(val_t *v);

#endif
