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
#include "structs.h"
#include <string.h>
#include <stdio.h>
#include <glib.h>

GHashTable *read_obj(char *file) {
	FILE *f;
	size_t rb;
	char buf[80];
	char *objfile;

	val_t *v;

	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
	GSList *vertices = NULL;
	GSList *faces = NULL;
	
	f = fopen(file, "r");
	if (!f) {
		perror("fopen");
		exit(1);
	}
	int i = 0;
	objfile = malloc(1);
	do {
		memset(buf, 0, 80);
		rb = fread(buf, 80, 1, f);
		if (!rb && ferror(f)) {
			perror("error reading file\n");
			break;
		} else if (rb < 80)
			objfile = realloc(objfile, strlen(buf) + strlen(objfile));
		else
			objfile = realloc(objfile, strlen(objfile) + 80);

		strncpy(&objfile[i], buf, (rb < 80) ? strlen(buf) : 80);

		i += (rb < 80) ? strlen(buf) : 80;
		objfile[i] = '\0';
	} while (!feof(f));
	fclose(f);

	char *tok = strtok(objfile, "\n");
	char trash;
	while (tok != NULL) {
		if (tok[0] == 'v') {
			v = g_new(val_t, 1);
			sscanf("%c %f %f %f", &trash, &v->x, &v->y, &v->z);
			vertices = g_slist_append(vertices, v);
		}
		if (tok[0] == 'f') {
			v = g_new(val_t, 1);
			sscanf("%c %f %f %f", &trash, &v->x, &v->y, &v->z);
			faces = g_slist_append(faces, v);
		}

		tok = strtok(NULL, "\n");
	}

	g_hash_table_insert(hash, "vertices", vertices);
	g_hash_table_insert(hash, "faces", faces);
	
	g_free(objfile);
	return hash;
}

void dump_vertice(val_t *v) {
	printf("vertice ");
	dump_val(v);
	puts("");
}

void dump_face(val_t *v) {
	printf("face ");
	dump_val(v);
	puts("");
}

void dump_hash(GHashTable *t) {
	GSList *l;

	l = g_hash_table_lookup(t, "vertices");
	g_slist_foreach(l, (GFunc)dump_vertice, NULL);
	l = g_hash_table_lookup(t, "faces");
	g_slist_foreach(l, (GFunc)dump_face, NULL);
}
