#include <stdio.h>
#include "db_entity.h"

int main(int argc, char *argv[]){
    MYSQL *conn = mysql_init(NULL);
    build_connection(conn, "localhost", "root", "daiqing123", "dockerhub_info", 3306, NULL, 0);
    printf("db connected\n");

    MYSQL_RES *res = get_query_result(conn, "show tables");
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != NULL)
    printf("%s \n", row[0]);
        
    mysql_close(conn);
    return 0;
}
