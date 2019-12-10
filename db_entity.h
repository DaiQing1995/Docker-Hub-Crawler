#include <mysql/mysql.h>
#include <cspider/cJSON.h>

typedef struct container_info{
    char* id;
    char* name;
    char* short_desc;
    char* desc;
    char* original_tags;
    char* slug;
    char* type;
    char* publisher_id;
    char* categories;
    int popularity;
    char* architectures;
    int star_count;
    char* operating_system;
    char* source;
    char* certification_status;
}container_info;

int check_connection();

MYSQL * build_connection(MYSQL *con, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long flags);

MYSQL_RES * get_query_result(MYSQL *con, const char *query);

int insert_container_info(MYSQL* con, container_info *ctn_info);

void init_ctn_data(container_info *ctn_data);

void free_ctn_data(container_info *ctn_data);

void json2ctn(container_info *ctn_data, cJSON *data);
