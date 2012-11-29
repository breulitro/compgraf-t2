#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <glib.h>
#include "structs.h"
#include "read_script.h"
#include "load_obj.h"

void plot_obj(model_t *obj, animation_t *anim) {
	face_t *face;
	val_t *v;
	GSList *aux;
	int i;

	if (anim == NULL || obj == NULL)
		return;
	dump_animation(anim);

	v = anim->trans;
	if (v != NULL) {
		glTranslatef(v->x, v->y, v->z);
	}

	v = anim->scale;
	if (v != NULL) {
		glScalef(v->x, v->y, v->z);
	}

	v = anim->rot;
	if (v != NULL) {
		glRotatef(v->y, 0, 1, 0);
	}

	aux = obj->face_list;
	glBegin(GL_TRIANGLES);
	while ((aux = g_slist_next(aux)) != NULL) {

		face = (face_t *)aux->data;
		for (i = 0; i < face->fvertex_size; i++) {
			v = get_vertex(face->fvertex[i], obj);
			glVertex3f(v->x, v->y, v->z);
		}
		for (i = 0; i < face->ftexture_size; i++) {
			v = get_texture(face->ftexture[i], obj);
			glTexCoord3f(v->x, v->y, v->z);
		}
		for (i = 0; i < face->fnormal_size; i++) {
			v = get_normal(face->fnormal[i], obj);
			glNormal3f(v->x, v->y, v->z);
		}
	}
	glEnd();
}

void plot_actor(actor_t *a, int *frame_atual) {
	plot_obj(a->obj, g_slist_nth_data(a->animations, *frame_atual));
}

GSList *actors_list = NULL;
GSList *animation_list_linear = NULL;

/**
 * trans: val_t a ser dividido
 * dividor: por quanto vai ser dividido
 * n: qual momento tu quer a transformacao
 */

val_t *divide_val(val_t *val, int divisor, int n) {
	val_t *v;

	if (val == NULL)
		return NULL;

	v = malloc(sizeof(val_t));
	//uma hora n == divisor
	if (n == 0) {
		v->x = val->x;
		v->y = val->y;
		v->z = val->z;
	} else {
		v->x = val->x / ((float)n / divisor);
		v->y = val->y / ((float)n / divisor);
		v->z = val->z / ((float)n / divisor);
	}

	return v;
}

void delta_func(animation_t *a, animation_t **i) {
	int c, delta;
	animation_t *anim, *aux;
	val_t *val;

	if (*i != NULL) {
		printf("INICIAL:");
		dump_animation(*i);
		printf("FINAL:");
		dump_animation(a);
		printf("=========\n");

		aux = *i;
		delta = a->frame - aux->frame;
		//'c' nunca vai começar em 1
#ifdef DBG
		printf("Iterando de %d a %d\n", aux->frame, a->frame);
#endif
		for (c = aux->frame; c <= a->frame; c++) {
#ifdef DBG
			printf("Iteracao: %d\n", c);
#endif
			anim = malloc(sizeof(animation_t));
			anim->frame = c;
			anim->trans = divide_val(a->trans, delta, (c - aux->frame));
			anim->scale = divide_val(a->scale, delta, (c - aux->frame));
			anim->rot = divide_val(a->rot, delta, (c - aux->frame));

			animation_list_linear = g_slist_append(animation_list_linear, anim);
		}
	} else {
		//Essa animação começa só no frame a->frame, então bota nulo nos frames
		//iniciais
		for (c = 1; c <= a->frame; c++)
			animation_list_linear = g_slist_append(animation_list_linear, NULL);
	}

	*i = a;
}

void free_val_t(val_t *v) {
	if (v != NULL)
		free(v);
}

void load_obj(actor_t *a) {
	animation_t *inicial = NULL;

	printf("Loading %s\n", a->file);
	a->obj = load_new_obj(a->file);

	animation_list_linear = NULL;
	g_slist_foreach(a->animations, (GFunc)delta_func, &inicial);

	g_slist_foreach(a->animations, (GFunc)free_val_t, NULL);
	g_slist_free(a->animations);

	g_slist_foreach(animation_list_linear, (GFunc)dump_animation, NULL);
	a->animations = animation_list_linear;
}
