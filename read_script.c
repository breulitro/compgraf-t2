#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "structs.h"

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
#define SINAL (-1)
void parser(char *token) {
	char *tok, *taux, *base, *des;
	int ibuf;
	char sbuf[80];
	// header
	if ((tok = strstr(token, "actors")) != NULL) {
		sscanf(tok, "actors %d", &ibuf);
		printf("buf = %d\n", ibuf);
	}
	// actor
	if ((tok = strstr(token, "id_actor")) != NULL) {
		sscanf(tok, "id_actor %d %s", &ibuf, &sbuf); 
		printf("ACTOR %d file:%s\n", ibuf, sbuf);
	}
	// animation
	if ((tok = strstr(token, "Frame")) != NULL) {
		des = strdup(token);
		
		if ((tok = strtok_r(des, " ", &taux)) == NULL)
			return;
		do {
			tok = strtok_r(tok, "=", &base);
			printf("key: \"%s\" -> \"%s\"\n", tok, base);
		} while ((tok = strtok_r(taux, " ", &taux)) != NULL);

		free(des);
	}
}

int main(int argc, char **argv) {
	FILE *f;
	char buf[80], *script;
	size_t rb;
	int i;

	if (argc != 2) {
		printf("usage:%s <script file>\n", argv[0]);
		return 1;
	}

	i = 0; 
	script = malloc(1);
	f = fopen(argv[1], "r");
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
	printf("%s\n", script);

	char *tok;
	//scene_t s;
	//actor_t *a;
	//struct transf_t *t;

	tok = strtok(script, "\n");
	for (i = 0; tok != NULL; i++) {
		printf("%s {\n", tok);
		parser(tok);
		printf("}\n");
		tok = strtok(NULL, "\n");
	}

	if (script != NULL)
		free(script);
}

#if 0
break_space

GSList *actors;


typedef struct {
        float x, y, z;
} val_t;


typedef struct {
	int frame;
	val_t trans;
	val_t scale;
	val_t rot;
} animation_t;

typedef struct {
        int id;
        char *file;
	GSList *animations;
} actor_t;

parser_actor(char *token) {
	actor_t actor;
	
}

parser_animation(char *token) {
	animation_t anim;
}
#endif
