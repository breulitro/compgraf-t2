#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"

GSList *actors = NULL;
actor_t *current = NULL;

static void parse_header(char *token) {
	int i;

	sscanf(token, "#actors %d", &i);
//	printf("%s: %d\n", __func__, i);
	//	current = NULL;
}

static void parse_actor(char *token) {
	gchar **tokens;

	if (token == NULL)
		return;

	tokens = g_strsplit(token, " ", -1);
	if (g_strv_length(tokens) < 3)
		return;

	current = g_new0(actor_t, 1);
	current->id = atoi(tokens[1]);
	current->file = g_strdup(tokens[2]);

	g_strfreev(tokens);

	printf("%s: %d - %s\n", __func__, current->id, current->file);

	actors = g_slist_append(actors, current);
}

/* XXX: Esta função retorna memoria alocada internamente que deve ser liberada pelo
 * utilizador da mesma */
static val_t *parse_val(char *token) {
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

static void parse_animation(char *token) {
	animation_t *a = NULL;
	char *key, *val;
	val_t *v;
	gchar **tokens;
	int i;

	if (token == NULL)
		return;

	if ((a = g_new0(animation_t, 1)) == NULL) {
		perror("g_new()");
		exit(1);
	}

	tokens = g_strsplit(token, " ", -1);

	for (i = 0; i < g_strv_length(tokens); i++) {
		key = strtok_r(tokens[i], "=", &val);

		if (key != NULL && val != NULL) {
			if (g_ascii_strcasecmp(key, "Frame") == 0) {
				a->frame = atoi(val);
			} else if (g_ascii_strcasecmp(key, "trans") == 0) {
				v = parse_val(val);
				if (v != NULL) {
					a->trans = g_new0(val_t, 1);
					memcpy(a->trans, v, sizeof(val_t));
					g_free(v);
					v = NULL;
				}
			} else if (g_ascii_strcasecmp(key, "scale") == 0) {
				v = parse_val(val);
				if (v != NULL) {
					a->scale = g_new0(val_t, 1);
					memcpy(a->scale, v, sizeof(val_t));
					g_free(v);
					v = NULL;
				}
			} else if (g_ascii_strcasecmp(key, "rot") == 0) {
				v = parse_val(val);
				if (v != NULL) {
					a->rot = g_new0(val_t, 1);
					memcpy(a->rot, v, sizeof(val_t));
					g_free(v);
					v = NULL;
				}
			}
		}
	}

	g_strfreev(tokens);

	//dump_animation(a);
	current->animations = g_slist_append(current->animations, a);

	//dump_animation(a);
}

static void parser(char *token) {
	gchar **tokens;

	if (token == NULL)
		return;

	g_strchug(token);
	tokens = g_strsplit(token, " ", -1);

	// header
	if ((g_ascii_strcasecmp(tokens[0], "#actors")) == 0)
		parse_header(token);
	// actor
	else if ((g_ascii_strcasecmp(tokens[0], "#id_actor")) == 0)
		parse_actor(token);
	// animation
	else if ((g_ascii_strncasecmp(tokens[0], "Frame", 5)) == 0)
		parse_animation(token);

	g_strfreev(tokens);
}

static void clean_animation(animation_t *a) {
	if (a == NULL)
		return;

	if (a->trans != NULL)
		g_free(a->trans);
	if (a->scale != NULL)
		g_free(a->scale);
	if (a->rot != NULL)
		g_free(a->rot);
}

static void clean_actor(actor_t *a) {
	if (a == NULL)
		return;

	if (a->file != NULL)
		g_free(a->file);

	g_slist_foreach(a->animations, (GFunc)clean_animation, NULL);
	g_slist_free(a->animations);
}

static void cleanup_actors() {
	g_slist_foreach(actors, (GFunc)clean_actor, NULL);
	g_slist_free(actors);
}

static void dump_actors() {
	printf("actors.length = %d\n", g_slist_length(actors));
	g_slist_foreach(actors, (GFunc)dump_actor, NULL);
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


