
CFLAGS = -I /usr/include
LDFLAGS = -lglut -lGL -lGLU

all:
	gcc main.c ${CFLAGS} ${LDFLAGS}

clean:
	rm a.out
