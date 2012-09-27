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

static GSList *vertex_list = NULL;
static GSList *face_list = NULL;

int cnt_texture = 0;
int cnt_vertex = 0;
int cnt_normal = 0;


// FIXME: para faces temos que ter params infinitos
static int *parse_new_int(char *str, int params) {
	char *saveptr, *tok, *ptr;
	int i;
	int *val;

	if ((str == NULL) || (params < 1))
		return NULL;

	ptr = str;
	val = g_new(int, params);

	i = 0;
	while (((tok = strtok_r(ptr, " ", &saveptr)) != NULL) && i < params) {
		val[i] = atoi(tok);
		ptr = saveptr;
		i++;
	}

	return val;
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

static face_t *parse_new_face(char *str_face) {
	face_t *face;
	int *ival_list;

	ival_list = parse_new_int(str_face, 3);

	if (ival_list == NULL)
		return NULL;

	face = g_new(face_t, 1);
	face->x = ival_list[0] - 1;
	face->y = ival_list[1] - 1;
	face->z = ival_list[2] - 1;
	g_free(ival_list);

	return face;
}

static void parse_line(char *line) {
	char *tok, *params;
	val_t *vertex, *vtexture;
	face_t *face;

	// TODO: remover espaco no inicio da linha
	tok = strtok_r(line, " ", &params);
//	printf("TOK: '%s'\n", tok);

	if ((g_strcmp0(tok, "v")) == 0) {
		// VERTEX
		vertex = parse_new_vertex(params);
		vertex_list = g_slist_append(vertex_list, vertex);
		cnt_vertex++;
	} else if ((g_strcmp0(tok, "f")) == 0) {
		// FACES
		face = parse_new_face(params);
		face_list = g_slist_append(face_list, face);
	} else if ((g_strcmp0(tok, "vt")) == 0) {
		// VERTEX TEXTURE
		vtexture = parse_new_vtexture(params);
	}
}

static int breakdown(char *buffer) {
	char *saveptr, *tok, *ptr;

	if (buffer == NULL)
		return 0;

	ptr = buffer;

	while ((tok = strtok_r(ptr, "\n", &saveptr))!= NULL) {
		parse_line(tok);
		ptr = saveptr;
	}

	return 1;
}

static void cleanup_faces() {
	int i;
	face_t *face;

	for (i = 0; i < g_slist_length(face_list); i++) {
		face = g_slist_nth_data(face_list, i);
		g_free(face);
	}
	g_slist_free(face_list);
	face_list = NULL;
}

static void cleanup_vertex() {
	int i;
	val_t *vertex;

	for (i = 0; i < g_slist_length(vertex_list); i++) {
		vertex = g_slist_nth_data(vertex_list, i);
		g_free(vertex);
	}
	g_slist_free(vertex_list);
	vertex_list = NULL;
}

int load_obj(char *file) {
	FILE *fp;
	long size;
	char *file_buffer;

	if (file == NULL)
		return 0;

	fp = fopen(file, "r");

	if (!fp) {
		perror("fopen");
		return 0;
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	file_buffer = malloc(sizeof(char) * size);
	memset(file_buffer, 0, size);
	fread(file_buffer, size, 1, fp);
	breakdown(file_buffer);
	g_free(file_buffer);
	fclose(fp);

	return 1;
}

int main() {
	int i;
	val_t *v;
	face_t *f;

	load_obj("yoda.obj");

	for (i = 0; i < g_slist_length(vertex_list); i++) {
		v = g_slist_nth_data(vertex_list, i);
		printf("%f %f %f\n", v->x, v->y, v->z);
	}

	for (i = 0; i < g_slist_length(face_list); i++) {
		f = g_slist_nth_data(face_list, i);
		printf("%d %d %d\n", f->x, f->y, f->z);
	}

	cleanup_vertex();
	cleanup_faces();

	return 0;
}
