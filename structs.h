typedef struct {
	float x, y, z;
} val_t;

typedef struct frame_t {
	struct frame_t *next;
	int id;
	val_t trans;
	val_t scale;
	val_t rot;
} transf_t;

typedef struct {
	int id;
	char *file;
	transf_t *frames;
} actor_t;

typedef struct {
	int nactors;
	actor_t *actors;
} scene_t;
