#include <glib.h>

#include "structs.h"

typedef struct {
  GSList *vertex_list;
  GSList *face_list;
  GSList *texture_list;
  GSList *normal_list;
} model_t;

// OBJ
typedef struct {
	int *fvertex;
  int fvertex_size;

  int *ftexture;
  int ftexture_size;

  int *fnormal;
  int fnormal_size;
} face_t;



#if 0
typedef struct _ObjModel {
	int NumVertex;
	int NumNormal;
	int NumTexCoord;
	int NumTriangle;

	ObjVertex *VertexArray;
	ObjNormal *NormalArray;
	ObjTexCoord *TexCoordArray;

	ObjTriangle *TriangleArray;
} ObjModel;
#endif


model_t *load_new_obj(char *file);
val_t *get_vertex(int index, model_t *obj);
val_t *get_texture(int index, model_t *obj);
val_t *get_normal(int index, model_t *obj);

