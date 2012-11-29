#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"

GSList *actors = NULL;
actor_t *current = NULL;

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

	if (token == NULL)
		return;

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

/* XXX: Esta função retorna memoria alocada internamente que deve ser liberada pelo
 * utilizador da mesma */
val_t *parse_val(char *token) {
	float x, y, z;
	val_t *ret;

	if (token == NULL)
		return NULL;

	ret = g_new0(val_t, 1);

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

	if (token == NULL)
		return;

	if ((a = g_new0(animation_t, 1)) == NULL) {
		perror("g_new()");

		exit(1);
	}

	if ((tok = strtok_r(token, " ", &taux)) == NULL)
		return;

	key = strtok_r(tok, "=", &val);

	if (val == NULL || key == NULL)
		return;

	a->frame = atoi(val);

	do {
		key = strtok_r(tok, "=", &val);
		v = parse_val(val);

		if (v != NULL) {
			if (g_ascii_strcasecmp(key, "trans") == 0) {
				a->trans = g_new0(val_t, 1);
				memcpy(a->trans, v, sizeof(val_t));
			} else if (g_ascii_strcasecmp(key, "scale") == 0) {
				a->scale = g_new0(val_t, 1);
				memcpy(a->scale, v, sizeof(val_t));
			} else if (g_ascii_strcasecmp(key, "rot") == 0) {
				a->rot = g_new0(val_t, 1);
				memcpy(a->rot, v, sizeof(val_t));
			}
			g_free(v);
			v = NULL;
		}
	} while ((tok = strtok_r(taux, " ", &taux)) != NULL);

	//dump_animation(a);
	current->animations = g_slist_append(current->animations, a);

	//dump_animation(a);
}

void parser(char *token) {
	char *tok;

	if (token == NULL)
		return;

	// header
	if ((tok = strstr(token, "actors")) != NULL)
		parse_header(tok);
	// actor
	if ((tok = strstr(token, "id_actor")) != NULL)
		parse_actor(tok);
	// animation
	if ((tok = strstr(token, "Frame")) != NULL)
		parse_animation(tok);
}

void clean_animation(animation_t *a) {
	if (a == NULL)
		return;

	if (a->trans != NULL)
		g_free(a->trans);
	if (a->scale != NULL)
		g_free(a->scale);
	if (a->rot != NULL)
		g_free(a->rot);
}

void clean_actor(actor_t *a) {
	if (a == NULL)
		return;

	if (a->file != NULL)
		g_free(a->file);

	g_slist_foreach(a->animations, (GFunc)clean_animation, NULL);
	g_slist_free(a->animations);
}

GSList *read_script(char *file) {
	FILE *fp;
	char *file_buffer, *tok;
	long size;

	actors = NULL;

	if (file == NULL) {
		printf("%s: Empty file\n", __func__);
		return NULL;
	}

	fp = fopen(file, "r");
	if (!fp) {
		perror("fopen");
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	file_buffer = g_new0(char, size);
	fread(file_buffer, size, 1, fp);
	fclose(fp);

	tok = strtok(file_buffer, "\n");
	while (tok != NULL) {
		parser(tok);
		tok = strtok(NULL, "\n");
	}
	//	printf("#############################################\n");
	//	printf("actors.length = %d\n", g_slist_length(actors));
	//	g_slist_foreach(actors, (GFunc)dump_actor, NULL);

	if (file_buffer != NULL)
		g_free(file_buffer);

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
