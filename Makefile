ifeq ($(shell uname),Linux)
CFLAGS = -Wall -I/usr/local/include -I/usr/include -std=gnu99
else
CFLAGS = -Wall -I/System/Library/Frameworks/GLUT.framework/Versions/A/Headers/ -I/usr/local/include -I/usr/include -std=gnu99
endif

ifeq ($(shell uname),Linux)
LDFLAGS = -lglut -lGL -lGLU
else
LDFLAGS = -framework GLUT -framework OpenGL
endif

GLIB_FLAGS = `pkg-config --libs glib-2.0 --cflags glib-2.0`

all:
	gcc main.c read_script.c structs.c load_obj.c help.c -g ${CFLAGS} ${LDFLAGS} ${GLIB_FLAGS} -o compgraf-t2

clean:
	rm -f compgraf-t2

exemplo:
	gcc glutobj.c `pkg-config --libs glib-2.0 --cflags glib-2.0` -I/usr/local/include -I/usr/include -lglut -lGL -lGLU -lm -lglm -lpng
