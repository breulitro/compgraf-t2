#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"

char *trim(char *str) {
	size_t len = 0;
	char *frontp = str - 1;
	char *endp = NULL;

	if (str == NULL)
		return NULL;

	if (str[0] == '\0')
		return str;

	len = strlen(str);
	endp = str + len;

	while (isspace(*(++frontp)));
	while (isspace(*(--endp)) && endp != frontp);

	if (str + len - 1 != endp)
		*(endp + 1) = '\0';
	else if (frontp != str && endp == frontp)
		*str = '\0';

	endp = str;
	if (frontp != str) {
		while (*frontp)
			*endp++ = *frontp++;
		*endp = '\0';
	}

	return str;
}

GSList *actors = NULL;
actor_t *current = NULL;

void parse_header(char *token) {
	int i;

	sscanf(token, "actors %d", &i);
	// printf("%s: %d\n", __func__, i);
	//	current = NULL;
}

//FIXME: normalizar a string lida do arquivo de script
void parse_actor(char *token) {
	char *obj;
	int i;

	printf("IM IN!\n");

	/*if (current != NULL) {
	  actors = g_slist_append(actors, current);
	  current = NULL;
	  }*/

	obj = g_new0(char, strlen(token));
	sscanf(token, "id_actor %d %s", &i, obj);
	//printf("%s: %d - %s\n", __func__, i, obj);

	current = g_new0(actor_t, 1);
	current->id = i;
	current->file = strdup(obj);
	g_free(obj);

	actors = g_slist_append(actors, current);
}

val_t *parse_val(char *token) {
	float x, y, z;
	val_t *ret = g_new(val_t, 1);

	sscanf(token, "<%f,%f,%f>", &x, &y, &z);
	//printf("%s: x = %.2f y = %.2f, z = %.2f\n", __func__, x, y, z);
	ret->x = x;
	ret->y = y;
	ret->z = z;

	return ret;
}

void parse_animation(char *token) {
	animation_t *a = NULL;
	char *tok, *taux;
	char *key, *val;
	val_t *v;

	if ((a = g_new(animation_t, 1)) == NULL) {
		perror("g_new()");
		exit(1);
	}
	memset(a, 0, sizeof(animation_t));

	if ((tok = strtok_r(token, " ", &taux)) == NULL)
		return;

	key = strtok_r(tok, "=", &val);
	a->frame = atoi(val);
	do {
		key = strtok_r(tok, "=", &val);
		//  printf("%s: %s -> %s\n", __func__, key, val);
		//    printf("%s: x = %.2f y = %.2f, z = %.2f\n", __func__, v->x, v->y, v->z);
		/*if (strcmp(key, "Frame") == 0) {
		  v = parse_val(val);
		  a->frame = ((int)v->x) == 0 ? atoi(val) : v->x;
		//printf("FRAME: %d -> #%s#\n", a->frame, val);
		} else */if (strcmp(key, "trans") == 0) {
			v = parse_val(val);
			a->trans = malloc(sizeof(val_t));
			memcpy(a->trans, v, sizeof(val_t));
		} else if (strcmp(key, "scale") == 0) {
			v = parse_val(val);
			a->scale = malloc(sizeof(val_t));
			memcpy(a->scale, v, sizeof(val_t));
		} else if (strcmp(key, "rot") == 0) {
			v = parse_val(val);
			a->rot = malloc(sizeof(val_t));
			memcpy(a->rot, v, sizeof(val_t));
		}
	//  free(v);
	} while ((tok = strtok_r(taux, " ", &taux)) != NULL);
	//dump_animation(a);
	current->animations = g_slist_append(current->animations, a);

	//dump_animation(a);
}

void parser(char *token) {
	char *tok;
	// header
	if ((tok = strstr(token, "actors")) != NULL)
		parse_header(tok);

	// actor
	if ((tok = strstr(token, "id_actor")) != NULL) {
		parse_actor(tok);
	}
	// animation
	if ((tok = strstr(token, "Frame")) != NULL) {
		parse_animation(tok);
	}
}

void clean_animation(animation_t *a) {
	if (a->trans != NULL)
		free(a->trans);
	if (a->scale != NULL)
		free(a->scale);
	if (a->rot != NULL)
		free(a->rot);
}

void clean_actor(actor_t *a) {
	if (a->file != NULL)
		free(a->file);
	g_slist_foreach(a->animations, (GFunc)clean_animation, NULL);
	g_slist_free(a->animations);
}

GSList *read_script(char *file) {
	FILE *f;
	char buf[80], *script;
	size_t rb;
	int i;

	actors = NULL;
	if (file == NULL) {
		return NULL;
	}

	i = 0;
	script = malloc(1);
	f = fopen(file, "r");
	do {
		memset(buf, 0, 80);
		rb = fread(buf, 80, 1, f);
		if (!rb && ferror(f)) {
			perror("error reading file\n");
			break;
		} else if (rb < 80)
			script = realloc(script, strlen(buf) + strlen(script));
		else
			script = realloc(script, strlen(script) + 80);

		strncpy(&script[i], buf, (rb < 80) ? strlen(buf) : 80);

		i += (rb < 80) ? strlen(buf) : 80;
		script[i] = '\0';
	} while (!feof(f));
	fclose(f);

	char *tok;
	//scene_t s;
	//actor_t *a;
	//struct transf_t *t;

	tok = strtok(script, "\n");
	for (i = 0; tok != NULL; i++) {
		// printf("%s {\n", tok);
		parser(tok);
		//printf("}\n");
		tok = strtok(NULL, "\n");
	}
	//	printf("#############################################\n");
	//	printf("actors.length = %d\n", g_slist_length(actors));
	//	g_slist_foreach(actors, (GFunc)dump_actor, NULL);

	if (script != NULL)
		free(script);

	return actors;
}

void cleanup_actors() {
	g_slist_foreach(actors, (GFunc)clean_actor, NULL);
	g_slist_free(actors);
}

void dump_actors() {
	printf("actors.length = %d\n", g_slist_length(actors));
	g_slist_foreach(actors, (GFunc)dump_actor, NULL);
}

#if 0
break_space

GSList *actors;

parser_actor(char *token) {
	actor_t actor;
}

parser_animation(char *token) {
	animation_t anim;
}
#endif
