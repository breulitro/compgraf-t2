#include <stdio.h>
#include <GL/glut.h>
#include <glib.h>

#include "structs.h"
#include "read_script.h"
#include "read_obj.h"

// all variables initialized to 1.0, meaning
// the triangle will initially be white
float red = 1.0f, blue = 1.0f, green = 1.0f;
float angle = 0.0;

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
	if (key == 27)
		exit(0);
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
			angle += .5;
			break;
		case GLUT_KEY_RIGHT:
			angle -= .5;
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

void plot_face(val_t *f, GSList *lv) {
  val_t *v1, *v2, *v3;
  v1 = g_slist_nth_data(lv, (int)f->x);
  v2 = g_slist_nth_data(lv, (int)f->y);
  v3 = g_slist_nth_data(lv, (int)f->z);
    glVertex3f(v1->x, v1->y, v1->z);
    glVertex3f(v2->x, v2->y, v2->z);
    glVertex3f(v3->x, v3->y, v3->z);
}

GHashTable *obj = NULL;
void plot_obj() {
  GSList *lv = g_hash_table_lookup(obj, "vertices");
  GSList *lf = g_hash_table_lookup(obj, "faces");
  g_slist_foreach(lf, (GFunc)plot_face, lv);
}

val_t olho = {80, 80, 80},
      foco = {9, -17, -1},
      normal = {0, 1, 0};

void renderScene(void) {
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

  glBegin(GL_TRIANGLES);
  plot_obj();
  glEnd();


	glutSwapBuffers();
}

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("usage: %s <script file> [glut params]\n", argv[0]);
		return 1;
	}

	GSList *actors = read_script(argv[1]);
	//FIXME: argv[0] que vai pro glutInit não é o nome do programa
	argv++;
	argc--;
	printf("script loaded\n");

	dump_actors();
	obj = read_obj("yoda.obj");
	dump_hash(obj);

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

	// enter GLUT event processing cycle
	glutMainLoop();
}
