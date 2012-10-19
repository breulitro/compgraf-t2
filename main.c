#include <stdio.h>
#include <GL/glut.h>
// all variables initialized to 1.0, meaning
// the triangle will initially be white
float red=1.0f, blue=1.0f, green=1.0f;
float angle = 0.0;

void changeSize(int w, int h) {

  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if(h == 0)
    h = 1;
  float ratio = 1.0* w / h;

  // Use the Projection Matrix
  glMatrixMode(GL_PROJECTION);

        // Reset Matrix
  glLoadIdentity();

  // Set the viewport to be the entire window
  glViewport(0, 0, w, h);

  // Set the correct perspective.
  gluPerspective(45,ratio,1,1000);

  // Get Back to the Modelview
  glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Reset transformations
  glLoadIdentity();
  // Set the camera
  gluLookAt(  0.0f, 0.0f, 10.0f,
      0.0f, 0.0f,  0.0f,
      0.0f, 1.0f,  0.0f);

  glRotatef(angle, 0.0f, 1.0f, 0.0f);

  glColor3f(red, green, blue);

  glBegin(GL_TRIANGLES);
    glVertex3f(-1.0f,-2.0f, 0.0f);
    glVertex3f( 2.0f, 0.0f, 0.0);
    glVertex3f( 0.0f, 2.0f, 0.0);
/*    glVertex3f(-0.5,-0.5,0.0);
    glVertex3f(0.5,0.0,0.0);
    glVertex3f(0.0,0.5,0.0);
*/
  glEnd();

//  angle += .2;

  glutSwapBuffers();
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
       if (mod == (GLUT_ACTIVE_CTRL|GLUT_ACTIVE_ALT)) {
        red = 1.0; green = 1.0; blue = 1.0;
       }
       red = 1.0; green = 0.0; blue = 0.0;
       break;
    case GLUT_KEY_F2 :
       red = 0.0;
       green = 1.0;
       blue = 0.0; break;
    case GLUT_KEY_F3 :
       red = 0.0;
       green = 0.0;
       blue = 1.0; break;
    case GLUT_KEY_LEFT:
      angle += .5;
      break;
    case GLUT_KEY_RIGHT:
      angle -= .5;
      break;
  }
}

int main(int argc, char **argv) {

  // init GLUT and create window
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
