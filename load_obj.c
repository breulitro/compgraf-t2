/* Especificação do formato .obj para objetos poligonais:
 *
 * # - commentário
 * v x y z - vértice
 * vn x y z - normal
 * vt x y z - texcoord
 * f a1 a2 ... - face com apenas índices de vértices
 * f a1/b1/c1 a2/b2/c2 ... - face com índices de vértices, normais e texcoords
 * f a1//c1 a2//c2 ... - face com índices de vértices e normais (sem texcoords)
 *
 * Campos ignorados:
 * g nomegrupo1 nomegrupo2... - especifica que o objeto é parte de um ou mais grupos
 * s numerogrupo ou off - especifica um grupo de suavização ("smoothing")
 * o nomeobjeto: define um nome para o objeto
 *
 * Biblioteca de materiais e textura:
 * mtllib - especifica o arquivo contendo a biblioteca de materiais
 * usemtl - seleciona um material da biblioteca
 * usemat - especifica o arquivo contendo a textura a ser mapeada nas próximas faces
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "structs.h"
#include "load_obj.h"

// FIXME: remover \r do arquivo?
static face_t *parse_new_face(char *str) {
	char *saveptr, *tok, *ptr;
	char *saveptr1, *tok1, *ptr1;
	int i;
	int *fvertex;
	int *fnormal;
	int *ftexture;
  face_t *f;
  int vals[3];

	if (str == NULL)
		return NULL;

	ptr = str;
	i = 0;

  f = g_new(face_t, 1);
  memset(f, 0, sizeof(face_t));

	while (((tok = strtok_r(ptr, " ", &saveptr)) != NULL)) {
    if ((strstr(tok, "/") != NULL)) {
      ptr1 = tok;
      while (((tok1 = strtok_r(ptr1, "/", &saveptr1)) != NULL)) {
        vals[i] = atoi(tok1);
        ptr1 = saveptr1;
        i++;
      }
      if (i > 0) {
        // Primeiro param == vertice
        f->fvertex_size++;
        f->fvertex = g_realloc(f->fvertex, (f->fvertex_size) * sizeof(int));
        f->fvertex[f->fvertex_size - 1] = vals[0];
      }
      if (i > 1) {
        // Segundo param == texture
        f->ftexture_size++;
        f->ftexture = g_realloc(f->ftexture, (f->ftexture_size) * sizeof(int));
        f->ftexture[f->ftexture_size - 1] = vals[1];
      }
      if (i > 2) {
        // Terceiro param == normal
        f->fnormal_size++;
        f->fnormal = g_realloc(f->fnormal, (f->fnormal_size) * sizeof(int));
        f->fnormal[f->fnormal_size - 1] = vals[2];
      }
      i = 0;
    } else {
      // Se nao temos /, só temos vertices na face.
      f->fvertex_size++;
      f->fvertex = g_realloc(f->fvertex, (f->fvertex_size) * sizeof(int));
      f->fvertex[f->fvertex_size - 1] = atoi(tok);
    }
		ptr = saveptr;
	}

	return f;
}

static float *parse_new_float(char *str, int params) {
	char *saveptr, *tok, *ptr;
	int i;
	float *val;

	if ((str == NULL) || (params < 1))
		return NULL;

	ptr = str;
	val = g_new(float, params);

	i = 0;
	while (((tok = strtok_r(ptr, " ", &saveptr)) != NULL) && i < params) {
		val[i] = atof(tok);
		ptr = saveptr;
		i++;
	}

	return val;
}

static val_t *parse_new_vertex(char *str_vertex) {
	val_t *vertex;
	float *fval_list;

	fval_list = parse_new_float(str_vertex, 3);

	if (fval_list == NULL)
		return NULL;

	vertex = g_new(val_t, 1);
	vertex->x = fval_list[0];
	vertex->y = fval_list[1];
	vertex->z = fval_list[2];
	g_free(fval_list);

	return vertex;
}

static val_t *parse_new_vtexture(char *str) {
	val_t *vtexture;
	float *fval_list;

	fval_list = parse_new_float(str, 3);

	if (fval_list == NULL)
		return NULL;

	vtexture = g_new(val_t, 1);
	vtexture->x = fval_list[0];
	vtexture->y = fval_list[1];
	vtexture->z = fval_list[2];
	g_free(fval_list);

	return vtexture;
}

static void parse_line(char *line, model_t *obj) {
	char *tok, *params;
	val_t *vertex, *vtexture;
	face_t *face;

	// TODO: remover espaco no inicio da linha
	tok = strtok_r(line, " ", &params);

	if ((g_strcmp0(tok, "v")) == 0) {
		// VERTEX
		vertex = parse_new_vertex(params);
		obj->vertex_list = g_slist_append(obj->vertex_list, vertex);
	} else if ((g_strcmp0(tok, "f")) == 0) {
		// FACES
		face = parse_new_face(params);
		obj->face_list = g_slist_append(obj->face_list, face);
	} else if ((g_strcmp0(tok, "vt")) == 0) {
		// VERTEX TEXTURE
		vtexture = parse_new_vtexture(params);
	}
}

static int breakdown(char *buffer, model_t *obj) {
	char *saveptr, *tok, *ptr;

	if (buffer == NULL)
		return 0;

	ptr = buffer;

	while ((tok = strtok_r(ptr, "\n", &saveptr))!= NULL) {
		parse_line(tok, obj);
		ptr = saveptr;
	}

	return 1;
}

#if 0
static void cleanup_faces(model_t *obj) {
	int i;
	face_t *face;

	for (i = 0; i < g_slist_length(obj->face_list); i++) {
		face = g_slist_nth_data(obj->face_list, i);
		g_free(face);
	}
	g_slist_free(obj->face_list);
	obj->face_list = NULL;
}
#endif
static void cleanup_vertex(model_t *obj) {
	int i;
	val_t *vertex;

	for (i = 0; i < g_slist_length(obj->vertex_list); i++) {
		vertex = g_slist_nth_data(obj->vertex_list, i);
		g_free(vertex);
	}
	g_slist_free(obj->vertex_list);
	obj->vertex_list = NULL;
}

model_t *load_new_obj(char *file) {
	FILE *fp;
	long size;
	char *file_buffer;
  model_t *obj;

	if (file == NULL)
		return NULL;

  obj = NULL;

	fp = fopen(file, "r");

	if (!fp) {
		perror("fopen");
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	file_buffer = malloc(sizeof(char) * size);
	memset(file_buffer, 0, size);
	fread(file_buffer, size, 1, fp);
  obj = g_new(model_t, 1);
  obj->vertex_list = NULL;
  obj->face_list = NULL;
	breakdown(file_buffer, obj);
	g_free(file_buffer);
	fclose(fp);

	return obj;
}

val_t *get_vertex(int index, model_t *obj) {
  // index - 1 já que faces indexa começando em 1 e nao em 0
	return g_slist_nth_data(obj->vertex_list, index - 1);
}

void release_obj(model_t *obj) {
	cleanup_vertex(obj);
//	cleanup_faces(obj);

  g_free(obj);
}

#if 0
int main() {
	int i;
	val_t *v;
	face_t *f;
  model_t *obj;
  GSList *aux;

	//obj = load_new_obj("Clementine.obj");
	obj = load_new_obj("cube.obj");

#if 0
	for (i = 0; i < g_slist_length(obj->vertex_list); i++) {
		v = g_slist_nth_data(obj->vertex_list, i);
		printf("%f %f %f\n", v->x, v->y, v->z);
	}
#endif

  aux = obj->face_list;
  while ((aux = g_slist_next(aux)) != NULL) {
    f = (face_t *)aux->data;
    printf("%d/%d/%d\n", f->fvertex_size, f->ftexture_size, f->fnormal_size);
    for (i = 0; i < f->fvertex_size; i++) {
      v = get_vertex(f->fvertex[i], obj);
      printf("(%f %f %f) ", v->x, v->y, v->z);
    }
    printf("\n");
    //if (v == NULL)
    //  printf("SHIT! %d, %d\n", i, f->faces[i]);
    //printf("%f %f %f\n", v->x, v->y, v->z);
    //printf("\n");
  }

  release_obj(obj);

	return 0;
}
#endif
