CC = gcc
OBJ = main.o
LIBS = -lcspider
CFLAGS = -I /usr/include/libxml2
main: ${OBJ}
	${CC} -c -g *.c ${LIBS} ${CFLAGS}
	${CC} -o ${@} ${^} ${LIBS} ${CFLAGS}

DB_SETTINGS = `mysql_config --cflags --libs`
test_db: test_db.o
	${CC} -o ${@} test_db.c db_entity.c ${DB_SETTINGS}

.PHONY = clean
clean:
	rm -f main test_db test *.o ${@} *.out *~ *.gch

