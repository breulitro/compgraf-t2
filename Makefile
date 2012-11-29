CFLAGS = -Wall -I/System/Library/Frameworks/GLUT.framework/Versions/A/Headers/ -I/usr/local/include -I/usr/include -std=gnu99
#LDFLAGS = -lglut -lGL -lGLU -framework GLUT
LDFLAGS = -framework GLUT -framework OpenGL
GLIB_FLAGS = `pkg-config --libs glib-2.0 --cflags glib-2.0`

all:
	gcc main.c read_script.c structs.c load_obj.c -g ${CFLAGS} ${LDFLAGS} ${GLIB_FLAGS} -o compgraf-t2

clean:
	rm a.out
