CC = gcc
OBJ = main.o
LIBS = -lcspider
CFLAGS = -I /usr/include/libxml2
main: ${OBJ}
	${CC} -o ${@} ${^} ${LIBS} ${CFLAGS}

.PHONY = clean
clean:
	rm main *.o ${@}

