CFLAGS = -I /usr/include
LDFLAGS = -lglut -lGL -lGLU
GLIB_FLAGS = `pkg-config --libs glib-2.0 --cflags glib-2.0`

all:
	gcc main.c read_script.c -g ${CFLAGS} ${LDFLAGS} ${GLIB_FLAGS} -o compgraf-t2

clean:
	rm a.out
