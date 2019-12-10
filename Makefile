CC = gcc
OBJ = main.o db_entity.o utils.o
LIBS = -lcspider
CURL_LIB = -lcurl
CFLAGS = -I /usr/include/libxml2
DB_SETTINGS = `mysql_config --cflags --libs`

main: ${OBJ}
	${CC} -c -g *.c ${LIBS} ${CURL_LIB} ${CFLAGS} ${DB_SETTINGS}
	${CC} -o ${@} ${^} ${LIBS} ${CURL_LIB} ${CFLAGS} ${DB_SETTINGS}

test_db: test_db.o
	${CC} -o ${@} test_db.c db_entity.c ${DB_SETTINGS} 

utils: utils.o
	${CC} -o ${@} utils.c ${CURL_LIB} 

.PHONY = clean
clean:
	rm -f main test_db test *.o ${@} *.out *~ *.gch utils

