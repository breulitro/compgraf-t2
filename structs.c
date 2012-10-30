#include <stdio.h>
#include <glib.h>

#include "structs.h"

void dump_val(val_t *v) {
  printf("<%.2f,%.2f,%.2f>", v->x, v->y, v->z);
}

void dump_val_int(val_t *v) {
  printf("<v[%d],v[%d],v[%d]>", (int)v->x, (int)v->y, (int)v->z);
}

void dump_animation(animation_t *a) {
  printf("Frame{%d", a->frame);
	if (a->trans != NULL) {
		printf(", trans=>");
		dump_val(a->trans);
	}
	if (a->scale != NULL) {
		printf(", scale=>");
		dump_val(a->scale);
	}
	if (a->rot != NULL) {
		printf(", rot=>");
		dump_val(a->rot);
	}
  printf("}\n");
}

void dump_actor(actor_t *a) {
	printf("Actor{id=>%d, file=>%s}\n", a->id, a->file);
	g_slist_foreach(a->animations, (GFunc)dump_animation, NULL);
}
