#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_entity.h"

/*
 * check client info
 */
int check_connection(){
    if (NULL == mysql_get_client_info()){
        return 1;
    }
    return 0;
}

/*
 * get conn with mysql
 */
MYSQL * build_connection(MYSQL *con, const char *host,
        const char *user, const char *passwd, const char *db,
        unsigned int port, const char *unix_socket, unsigned long flags){
   MYSQL *ret = mysql_real_connect(con, host, user, passwd, db, port, unix_socket, flags);
   if (!ret){
    fprintf(stderr, "%s\n", mysql_error(con));
    return NULL;
   }
   return ret;
}

/*
 * get result from db by query
 */ 
MYSQL_RES * get_query_result(MYSQL *con, const char *query){
    MYSQL_RES *res;
    if (mysql_query(con, query)){
        fprintf(stderr, "%s\n", mysql_error(con));
        return NULL;
    }
   res = mysql_use_result(con);
   return res;
}

/**
 * insert container info to db
 */
int insert_container_info(MYSQL* con, container_info *ctn_info){
    const int buffer_size = 2048;
    char *query = (char *)malloc(sizeof(char) * buffer_size);
    memset(query, '\0', buffer_size);
    sprintf(query,
            "INSERT INTO `dockerhub_info`.`container_info`\
            (`id`,`name`,`short_desc`,`desc`,`original_tags`,`slug`,\
            `type`,`publisher_id`,`categories`,`popularity`,`architectures`,\
            `star_count`,`operating_system`,`source`,`certification_status`)\
            VALUES\
            (\"%s\",\"%s\",\"%s\",\"%s\",\
             \"%s\",\"%s\",\"%s\",\"%s\",\
             ,\"%s\",\"%d\",\"%s\",\"%d\"\
             ,\"%s\",\"%s\",\"%s\")",
            ctn_info->id, ctn_info->name, ctn_info->short_desc,
            ctn_info->desc, ctn_info->original_tags, ctn_info->slug,
            ctn_info->type, ctn_info->publisher_id, ctn_info->categories,
            ctn_info->popularity, ctn_info->architectures,
            ctn_info->star_count, ctn_info->operating_system,
            ctn_info->source, ctn_info->certification_status);
    if (mysql_query(con, query)){
        fprintf(stderr, "db_entity.c: data insert error\n");
        return 0;
    }
    return 1;
}
