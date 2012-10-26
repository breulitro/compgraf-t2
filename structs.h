#include <glib.h>

typedef struct {
  float x, y, z;
} val_t;

typedef struct {
  int frame;
  val_t *trans;
  val_t *scale;
  val_t *rot;
} animation_t;

typedef struct {
  int id;
  char *file;
  GSList *animations;
} actor_t;

