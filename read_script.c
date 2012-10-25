#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "structs.h"
typedef struct {
  float x, y, z;
} val_t;

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
  printf("%s: %d\n", __func__, i);
}

void parse_actor(char *token) {
  char obj[80];
  int i;

  sscanf(token, "id_actor %d %s", &i, &obj);
  printf("%s: %d - %s\n", __func__, i, obj);
}

val_t *parse_val(char *token) {
  float x, y, z;
  val_t *ret = malloc(sizeof(val_t));

  sscanf(token, "<%f,%f,%f>", &x, &y, &z);
  //printf("%s: x = %.2f y = %.2f, z = %.2f\n", __func__, x, y, z);
  ret->x = x;
  ret->y = y;
  ret->z = z;

  return ret;
}

void parse_animation(char *token) {
  char *tok, *taux;
  char *key, *val;
  val_t *v;

  if ((tok = strtok_r(token, " ", &taux)) == NULL)
    return;
  do {
    key = strtok_r(tok, "=", &val);
    printf("%s: %s -> %s\n", __func__, key, val);
    v = parse_val(val);
    printf("%s: x = %.2f y = %.2f, z = %.2f\n", __func__, v->x, v->y, v->z);
    free(v);
  } while ((tok = strtok_r(taux, " ", &taux)) != NULL);
}

void parser(char *token) {
  char *tok;
  // header
  if ((tok = strstr(token, "actors")) != NULL)
    parse_header(tok);

  // actor
  if ((tok = strstr(token, "id_actor")) != NULL) {
    printf("ACTOR!!\n");
    parse_actor(tok);
  }
  // animation
  if ((tok = strstr(token, "Frame")) != NULL) {
    printf("ANIMAE!!\n");
    parse_animation(tok);
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

parser_actor(char *token) {
  actor_t actor;
}

parser_animation(char *token) {
  animation_t anim;
}
#endif
