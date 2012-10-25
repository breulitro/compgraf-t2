CFLAGS = -I /usr/include
LDFLAGS = -lglut -lGL -lGLU
GLIB_FLAGS = `pkg-config --libs glib-2.0 --cflags glib-2.0`

read_script:
	gcc read_script.c ${GLIB_FLAGS} -o read_script

all:
	gcc main.c ${CFLAGS} ${LDFLAGS}

clean:
	rm a.out
