#include <stdio.h>
#include <GL/glut.h>
#include <glib.h>
#include "structs.h"
#include "read_script.h"
#include "load_obj.h"

// all variables initialized to 1.0, meaning
// the triangle will initially be white
float red = 1.0f, blue = 1.0f, green = 1.0f;
float xangle = 0.0, yangle = 0.0, zangle = 0.0;

val_t	olho = {-20, 10, -100},
		foco = {0, 10, 0},
		normal = {0, 1, 0};
/*val_t olho = {80, 80, 80},
  foco = {9, -17, -1},
  normal = {0, 1, 0};
 */
void changeSize(int w, int h) {
	float ratio;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	ratio = 1.0 * w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	gluPerspective(45, ratio, 1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void processNormalKeys(unsigned char key, int x, int y) {
	if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
		printf("GLUT_ACTIVE_CTRL: %c\n", key);// não entendi como funciona isso
	} else {
		switch (key) {
			case 0x27:
				exit(0);
			case 'a':
				//xangle += .5;
				foco.x += 10;
				break;
			case 'd':
				//yangle -= .5;
				foco.z -= 10;
				break;
			case 's':
				//xangle -= .5;
				foco.x -= 10;
				break;
			case 'w':
				//yangle += .5;
				foco.z += 10;
				break;

		}
	}
}

void processSpecialKeys(int key, int x, int y) {
	int mod;

	switch(key) {
		case GLUT_KEY_F1 :
			mod = glutGetModifiers();
			if (mod == (GLUT_ACTIVE_CTRL | GLUT_ACTIVE_ALT)) {
				red = 1.0;
				green = 1.0;
				blue = 1.0;
			}
			red = 1.0;
			green = 0.0;
			blue = 0.0;
			break;
		case GLUT_KEY_F2 :
			red = 0.0;
			green = 1.0;
			blue = 0.0;
			break;
		case GLUT_KEY_F3 :
			red = 0.0;
			green = 0.0;
			blue = 1.0;
			break;
		case GLUT_KEY_LEFT:
			olho.z -=10;
			break;
		case GLUT_KEY_RIGHT:
			olho.x += 10;
			break;
		case GLUT_KEY_UP:
			olho.x -= 10;
			break;
		case GLUT_KEY_DOWN:
			olho.z +=10;
			break;
	}
}

GSList *interpolate(GSList *l) {
	//TODO
}

void desenhaChao() {
	float z, x;

	glColor3f(0, 0, 1);
	glLineWidth(1);
	glBegin(GL_LINES);

	for(z =- 1000; z <= 1000; z += 10) {
		glVertex3f(-1000, -0.1f, z);
		glVertex3f(1000, -0.1f, z);
	}
	for(x =- 1000; x <= 1000; x += 10) {
		glVertex3f(x, -0.1f, -1000);
		glVertex3f(x, -0.1f, 1000);
	}
	glEnd();
	glLineWidth(1);
}

void texturize() {
#if 0
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 13);
#endif
}

void plot_obj(model_t *obj, animation_t *anim) {
	face_t *face;
	val_t *v;
	GSList *aux;
	int i;

	if (anim == NULL)
		return;
	dump_animation(anim);

	v = anim->trans;
	if (v != NULL) {
		glTranslatef(v->x, v->y, v->z);
	}

	v = anim->scale;
	if (v != NULL) {
		glScalef(v->x, v->y, v->z);
	}

	v = anim->rot;
	if (v != NULL) {
		glRotatef(v->y, 0, 1, 0);
	}

	//glTranslatef(100, 0, 0);
	//  glScalef(20, 20, 20);
	aux = obj->face_list;
	glBegin(GL_TRIANGLES);
	while ((aux = g_slist_next(aux)) != NULL) {

		face = (face_t *)aux->data;
		for (i = 0; i < face->fvertex_size; i++) {
			v = get_vertex(face->fvertex[i], obj);
			glVertex3f(v->x, v->y, v->z);
		}
		for (i = 0; i < face->ftexture_size; i++) {
			v = get_texture(face->ftexture[i], obj);
			glTexCoord3f(v->x, v->y, v->z);
		}
		for (i = 0; i < face->fnormal_size; i++) {
			v = get_normal(face->fnormal[i], obj);
			glNormal3f(v->x, v->y, v->z);
		}
	}
	glEnd();
}

void plot_actor(actor_t *a, int *frame_atual) {
	plot_obj(a->obj, g_slist_nth_data(a->animations, *frame_atual));
}

GSList *actors_list = NULL;

void renderScene(void) {
	static int frame_atual;
	static int framerate;

	frame_atual += !(++framerate % 30);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	desenhaChao();

	// Reset transformations
	glLoadIdentity();

	// Set the camera

	//	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glColor3f(red, green, blue);

	gluLookAt(olho.x, olho.y, olho.z,
			foco.x, foco.y, foco.z,
			normal.x, normal.y, normal.z);

	//  glRotatef(xangle, 1, 0, 0);
	//  glRotatef(yangle, 0, 1, 0);
	//  glRotatef(zangle, 0, 0, 1);


	g_slist_foreach(actors_list, (GFunc)plot_actor, &frame_atual);

	glutSwapBuffers();
}

void processMousePassiveMotion(int x, int y) {
	printf("%s: x=%d y =%d\n", __func__, x, y);
}

void processMouseActiveMotion(int x, int y) {
	printf("%s: x=%d y =%d\n", __func__, x, y);
}

void processMouse(int bt, int state, int x, int y) {
	if (state == GLUT_DOWN)
		if (bt == GLUT_LEFT_BUTTON)
			printf("%s:Left click @ x=%d y =%d\n", __func__, x, y);
		else if (bt == GLUT_RIGHT_BUTTON)
			printf("%s:Right click @ x=%d y =%d\n", __func__, x, y);
		else if (bt == GLUT_MIDDLE_BUTTON)
			printf("%s:Middle click @ x=%d y =%d\n", __func__, x, y);

}

GSList *animation_list_linear = NULL;

/**
 * trans: val_t a ser dividido
 * dividor: por quanto vai ser dividido
 * n: qual momento tu quer a transformacao
 */

val_t *divide_val(val_t *val, int divisor, int n) {
	val_t *v;

	if (val == NULL)
		return NULL;

	v = malloc(sizeof(val_t));
	//uma hora n == divisor
	if (n == 0) {
		v->x = val->x;
		v->y = val->y;
		v->z = val->z;
	} else {
		v->x = val->x / ((float)n / divisor);
		v->y = val->y / ((float)n / divisor);
		v->z = val->z / ((float)n / divisor);
	}

	return v;
}

void delta_func(animation_t *a, animation_t **i) {
	int c, delta;
	animation_t *anim, *aux;
	val_t *val;

	if (*i != NULL) {
		printf("INICIAL:");
		dump_animation(*i);
		printf("FINAL:");
		dump_animation(a);
		printf("=========\n");

		aux = *i;
		delta = a->frame - aux->frame;
		//'c' nunca vai começar em 1
#ifdef DBG
		printf("Iterando de %d a %d\n", aux->frame, a->frame);
#endif
		for (c = aux->frame; c <= a->frame; c++) {
#ifdef DBG
			printf("Iteracao: %d\n", c);
#endif
			anim = malloc(sizeof(animation_t));
			anim->frame = c;
			anim->trans = divide_val(a->trans, delta, (c - aux->frame));
			anim->scale = divide_val(a->scale, delta, (c - aux->frame));
			anim->rot = divide_val(a->rot, delta, (c - aux->frame));

			animation_list_linear = g_slist_append(animation_list_linear, anim);
		}
	} else {
		//Essa animação começa só no frame a->frame, então bota nulo nos frames
		//iniciais
		for (c = 1; c <= a->frame; c++)
			animation_list_linear = g_slist_append(animation_list_linear, NULL);
	}

	*i = a;
}

void free_val_t(val_t *v) {
	if (v != NULL)
		free(v);
}

void load_obj(actor_t *a) {
	animation_t *inicial = NULL;

	printf("Loading %s\n", a->file);
	a->obj = load_new_obj(a->file);

	animation_list_linear = NULL;
	g_slist_foreach(a->animations, (GFunc)delta_func, &inicial);

	g_slist_foreach(a->animations, (GFunc)free_val_t, NULL);
	g_slist_free(a->animations);

	g_slist_foreach(animation_list_linear, (GFunc)dump_animation, NULL);
	a->animations = animation_list_linear;
}

int main(int argc, char **argv) {
	GHashTable *obj;

	if (argc < 2) {
		printf("usage: %s <script file> [glut params]\n", argv[0]);
		return 1;
	}

	actors_list = read_script(argv[1]);
	argv[1] = argv[0];
	argv++;
	argc--;
	printf("script loaded\n");

	g_slist_foreach(actors_list, (GFunc)load_obj, NULL);

	glutInit(&argc, argv);
	//-1 == default
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
	glutCreateWindow("CG T2");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	//  glutPassiveMotionFunc(processMousePassiveMotion);

	// enter GLUT event processing cycle
	glutMainLoop();
}
