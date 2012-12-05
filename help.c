#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

int linestart = 10;		/* start point on y axis for text lines */
int linespace = 20;		/* spac betwwen text lines */

void HelpRenderBitmapString(float x, float y, void *font, char *string)
{
  char *c;
  /*  set position to start drawing fonts */
  glRasterPos2f(x, y);
  /*  loop all the characters in the string */
  for (c = string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

void HelpDisplay(GLint ww, GLint wh)
{
  void *Help_Font = GLUT_BITMAP_8_BY_13;

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);


  /*  switch to projection mode */
  glMatrixMode(GL_PROJECTION);
  /*  save previous matrix which contains the  */
  /* settings for the perspective projection */
  glPushMatrix();
  /*  reset matrix */
  glLoadIdentity();
  /*  set a 2D orthographic projection */
  gluOrtho2D(0, ww, 0, wh);
  /*  invert the y axis, down is positive */
  glScalef(1, -1, 1);
  /*  mover the origin from the bottom left corner */
  /*  to the upper left corner */
  glTranslatef(0, -wh, 0);
  glMatrixMode(GL_MODELVIEW);


  glPushMatrix();
  glLoadIdentity();
  linestart = 10;

  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font, "Help Menu");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font, "---------");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "H/h = Toggle Help Menu");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "Esc = Exits Program");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "Left Arrow = Rewind");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "Right Arrow = Forward");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "Up Arrow = Speed up");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "Down Arrow = Speed down");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "PageUp/PageDown = Change light position");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "l = Toggle Loop");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "r, g ,b = Red, Green and Blue increase");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "R, G ,B = Red, Green and Blue decrease");
  HelpRenderBitmapString(30, linestart +=
      linespace, (void *) Help_Font,
      "Enter = Play/Pause");

  glPopMatrix();

  /*  set the current matrix to GL_PROJECTION */
  glMatrixMode(GL_PROJECTION);
  /*  restore previous settings */
  glPopMatrix();
  /*  get back to GL_MODELVIEW matrix */
  glMatrixMode(GL_MODELVIEW);

  //    if (lighting)
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
}


