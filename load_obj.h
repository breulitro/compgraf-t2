#include <glib.h>

#include "structs.h"

typedef struct {
  GSList *vertex_list;
  GSList *face_list;
} model_t;

model_t *load_new_obj(char *file);
val_t *get_vertex(int index, model_t *obj);
