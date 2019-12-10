#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_entity.h"
#include "utils.h"

const char *SPECIFIC_CTN_ADDR = "https://hub.docker.com/v2/repositories/library/";
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

char* get_tag(cJSON *array_data){
    int size = cJSON_GetArraySize(array_data);
    int i;
    char* ret = (char *)malloc(sizeof(char) * 1024);
    memset(ret, '\0', 1024);
    for (i = 0; i < size; ++ i){
        cJSON *tmp_data = cJSON_GetObjectItem(cJSON_GetArrayItem(array_data, i), "name");
        strcat(ret, tmp_data->valuestring);
        strcat(ret, " ");
        tmp_data = cJSON_GetObjectItem(cJSON_GetArrayItem(array_data, i), "label");
        strcat(ret, tmp_data->valuestring);
        strcat(ret, " ");
    }
    return ret;
}

/**
 * get desc and add to ctn_data
 */
void fulfill_desp(container_info *ctn_data){
    if (strcmp(ctn_data->source, "library")){
        ctn_data->desc = "NULL";
        return;
    }
    // init space
    char* tmp_space;
    if (ctn_data->slug != NULL && strlen(ctn_data->slug) > 0){
        tmp_space = (char *) malloc(sizeof(char) * (strlen(SPECIFIC_CTN_ADDR) + strlen(ctn_data->slug) + 1));
    memset(tmp_space, '\0', strlen(SPECIFIC_CTN_ADDR) + strlen(ctn_data->slug) + 1);
    }else if (ctn_data->name != NULL && strlen(ctn_data->name) > 0){
        tmp_space = (char *) malloc(sizeof(char) * (strlen(SPECIFIC_CTN_ADDR) + strlen(ctn_data->name) + 1));
        memset(tmp_space, '\0', strlen(SPECIFIC_CTN_ADDR) + strlen(ctn_data->name) + 1);
    }else{
        ctn_data->desc = "NULL";
        return;
    }
    int i, j;
    strcat(tmp_space, SPECIFIC_CTN_ADDR);
    if (ctn_data->slug != NULL && strlen(ctn_data->slug) > 0){
        strcat(tmp_space, ctn_data->slug);
    }else{
        j = strlen(tmp_space);
        for (i = 0;i < strlen(ctn_data->name); ++ i){
            if (ctn_data->name[i] > 'A' && ctn_data->name[i] < 'Z'){
                tmp_space[j ++] = 32 + ctn_data->name[i];
            }else if (ctn_data->name[i] == ' '){
                tmp_space[j ++] = '-';
            }else{
                tmp_space[j ++] = ctn_data->name[i];
            }
        }
    }

    char *tmp_desc = handle_url(tmp_space);

    cJSON *desc = NULL;
    if (cJSON_HasObjectItem(cJSON_Parse(tmp_desc), "full_description")){
        fprintf(stdout, "[START DESP]URI: %s\n", tmp_space);
        cJSON *desc = cJSON_GetObjectItem(cJSON_Parse(tmp_desc), "full_description");
        ctn_data->desc = remove_invalid_char(desc->valuestring);
        free(tmp_desc);
        fprintf(stdout, "[END.. DESP]URI: %s\n", tmp_space);
    }

    free(tmp_space);
}

/*
 * convert json to container_info
 */
void json2ctn(container_info *ctn_data, cJSON *data){
    char *tmp_space = NULL;
    cJSON *tmp_json = NULL;

    tmp_json = cJSON_GetObjectItem(data, "id");
    ctn_data->id = remove_invalid_char(tmp_json->valuestring);

    tmp_json = cJSON_GetObjectItem(data, "name");
    ctn_data->name = remove_invalid_char(tmp_json->valuestring);

    tmp_json = cJSON_GetObjectItem(data, "short_description");
    ctn_data->short_desc = remove_invalid_char(tmp_json->valuestring);

    tmp_json = cJSON_GetObjectItem(data, "source");
    ctn_data->source = remove_invalid_char(tmp_json->valuestring);

    fulfill_desp(ctn_data);
    ctn_data->original_tags = "";

    tmp_json = cJSON_GetObjectItem(data, "slug");
    ctn_data->slug = remove_invalid_char(tmp_json->valuestring);

    tmp_json = cJSON_GetObjectItem(data, "type");
    ctn_data->type = remove_invalid_char(tmp_json->valuestring);

    tmp_json = cJSON_GetObjectItem(data, "publisher");
    ctn_data->publisher_id = remove_invalid_char(cJSON_GetObjectItem(tmp_json, "id")->valuestring);

    tmp_json = cJSON_GetObjectItem(data, "categories");
    tmp_space = get_tag(tmp_json);
    ctn_data->categories = remove_invalid_char(tmp_space);
    free(tmp_space);

    tmp_json = cJSON_GetObjectItem(data, "popularity");
    ctn_data->popularity = tmp_json->valueint;

    tmp_json = cJSON_GetObjectItem(data, "architectures");
    tmp_space = get_tag(tmp_json);
    ctn_data->architectures = remove_invalid_char(tmp_space);
    free(tmp_space);

    tmp_json = cJSON_GetObjectItem(data, "star_count");
    ctn_data->star_count = tmp_json->valueint;

    tmp_json = cJSON_GetObjectItem(data, "operating_systems");
    tmp_space = get_tag(tmp_json);
    ctn_data->operating_system = remove_invalid_char(tmp_space);
    free(tmp_space);

    tmp_json = cJSON_GetObjectItem(data, "certification_status");
    ctn_data->certification_status = remove_invalid_char(tmp_json->valuestring);
}

void init_ctn_data(container_info *ctn_data){
    ctn_data->id = NULL;
    ctn_data->name = NULL;
    ctn_data->short_desc = NULL;
    ctn_data->desc = NULL;
    ctn_data->original_tags = NULL;
    ctn_data->slug = NULL;
    ctn_data->type = NULL;
    ctn_data->publisher_id = NULL;
    ctn_data->categories = NULL;
    ctn_data->architectures = NULL;
    ctn_data->operating_system = NULL;
    ctn_data->source = NULL;
    ctn_data->certification_status = NULL;
    ctn_data->popularity = 0;
    ctn_data->star_count = 0;
}
/*
 *  free ctn_data
 */
void free_ctn_data(container_info *ctn_data){
    if (ctn_data->id != NULL){
        free(ctn_data->id);
        ctn_data->id = NULL;
    }
    if (ctn_data->name != NULL){
        free(ctn_data->name);
        ctn_data->name = NULL;
    }
    if (ctn_data->short_desc != NULL){
        free(ctn_data->short_desc);
        ctn_data->short_desc = NULL;
    }
    if (ctn_data->desc != NULL && strcmp(ctn_data->desc, "NULL")){
        free(ctn_data->desc);
        ctn_data->desc = NULL;
    }
    if (ctn_data->slug != NULL){
        free(ctn_data->slug);
        ctn_data->slug = NULL;
    }
    if (ctn_data->type != NULL){
        free(ctn_data->type);
        ctn_data->type = NULL;
    }
    if (ctn_data->publisher_id != NULL){
        free(ctn_data->publisher_id);
        ctn_data->publisher_id = NULL;
    }
    if (ctn_data->categories != NULL && strcmp(ctn_data->categories, "NULL")){
        free(ctn_data->categories);
        ctn_data->categories = NULL;
    }
    if (ctn_data->architectures != NULL && strcmp(ctn_data->architectures, "NULL")){
        free(ctn_data->architectures);
        ctn_data->architectures = NULL;
    }
    if (ctn_data->operating_system != NULL && strcmp(ctn_data->operating_system, "NULL")){
        free(ctn_data->operating_system);
        ctn_data->operating_system = NULL;
    }
    if (ctn_data->source != NULL){
        free(ctn_data->source);
        ctn_data->source = NULL;
    }
    if (ctn_data->certification_status != NULL){
        free(ctn_data->certification_status);
        ctn_data->certification_status = NULL;
    }
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
 * create file: id --> container desc
 */
void insert_desc2file(container_info *ctn_info){
    char data_dir[128];
    memset(data_dir, '\0', 128);
    strcat(data_dir,"data/");
    strcat(data_dir, ctn_info->id);
    FILE *fp = fopen(data_dir, "w");
    long long i;
    for (i = 0; i < strlen(ctn_info->desc); ++ i)
        fprintf(fp, "%c", ctn_info->desc[i]);
    fclose(fp);
}

/**
 * insert container info to db
 */
int insert_container_info(MYSQL* con, container_info *ctn_info){
    static int count_insert = 0;
    const int buffer_size = 2048 * 1024;
    char query[buffer_size];
    memset(query, '\0', buffer_size);
    printf("\n\n%d-th insert\n", count_insert ++);
    if (ctn_info->desc != NULL && strlen(ctn_info->desc) != 0 && strcmp(ctn_info->desc, "NULL"))
        insert_desc2file(ctn_info);
    sprintf(query, "INSERT INTO `dockerhub_info`.`container_info`(`id`,`name`,`short_desc`,`desc`,`original_tags`,`slug`,`type`,`publisher_id`,`categories`,`popularity`,`architectures`,`star_count`,`operating_system`,`source`,`certification_status`)VALUES(\"%s\",\"%s\",\"%s\",\"data/%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d,\"%s\",%d,\"%s\",\"%s\",\"%s\")", ctn_info->id, ctn_info->name, ctn_info->short_desc, ctn_info->id, ctn_info->original_tags, ctn_info->slug, ctn_info->type, ctn_info->publisher_id, ctn_info->categories, ctn_info->popularity, ctn_info->architectures, ctn_info->star_count, ctn_info->operating_system, ctn_info->source, ctn_info->certification_status);
    if (count_insert == 293){
        printf("DEBUG POINT");
    }
    int error;
    if ((error = mysql_query(con, query)) && ctn_info->id != NULL){
        fprintf(stderr, "db_entity.c:[%d] data insert error\n", error);
        printf("query executed failed: %s\n", query);
        return 0;
    }
    return 1;
}
