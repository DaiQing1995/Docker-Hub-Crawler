#include <stdio.h>
#include <stdlib.h>
#include <cspider/spider.h>
#include <locale.h>
#include "db_entity.h"
#include "utils.h"

MYSQL *conn = NULL;

void save2DB(cJSON *data){
    container_info *ctn_data = (container_info *)malloc(sizeof(container_info));
    init_ctn_data(ctn_data);
    json2ctn(ctn_data, data);
    if (insert_container_info(conn, ctn_data) == 0){
        fprintf(stderr, "insert error\n");
    }
    free_ctn_data(ctn_data);
    free(ctn_data);
}

void handle_document(char *d){
    cJSON *json = cJSON_Parse(d);
    cJSON *items = cJSON_GetObjectItem(json, "summaries");
    int datasum = cJSON_GetArraySize(items);
    for (int i = 0; i < datasum; i ++){
      cJSON *item = cJSON_GetArrayItem(items, i);
      save2DB(item);
    }
}

void p(cspider_t *cspider, char *d, char *url, void *user_data) {
    char *server = "localhost";
    char *user = "root";
    char *password = "daiqing123";
    char *database = "dockerhub_info";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "database connection failed\n");
    }

    if (d == NULL || strlen(d) == 0){
        fprintf(stderr, "data crawled NULL\n");
        return;
    }

    printf("%s crawled\n", url);
    handle_document(d);

    mysql_close(conn);
}

void s(void *str, void *user_data) {
    char *get = (char*)str;
    FILE *file = (FILE*)user_data;
    printf("[check point2]hahahah, s running\n");
    printf("get:%s\n", get);
    fprintf(file, "%s\n", get);
    return;
}

/**
 * use search api to get more data
 */
void search_data_add() {
    char *data_recv = NULL;
    char *server = "localhost";
    char *user = "root";
    char *password = "daiqing123";
    char *database = "dockerhub_info";
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
        fprintf(stderr, "database connection failed\n");
        return ;
    }
    int i, j;
    char *url_head = "https://hub.docker.com/api/content/v1/products/search?q=";
    char *url_tail = "&page_size=50&type=image";
    char search_query[3];
    char *url_full = (char *) malloc(sizeof(char) * (strlen(url_head) + strlen(url_tail) + 3));

    for (i = 0; i < 26; ++ i){
        for (j = 0; j < 26; ++ j){
            memset(url_full, '\0', strlen(url_head) + strlen(url_tail) + 3);
            search_query[0] = 'a' + i;
            search_query[1] = 'a' + j;
            search_query[2] = '\0';
            strcat(url_full, url_head);
            strcat(url_full, search_query);
            strcat(url_full, url_tail);
            printf("URI:%s\n", url_full);
            data_recv = handle_url(url_full);
            handle_document(data_recv);   
            free(data_recv);
        }
    }
    free(url_full);

    mysql_close(conn);
}

int main(int argc, char* argv[]) {
    cspider_t *spider = init_cspider();
    char *agent = "Mozilla/5.0 (X11; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0";
    char *root_page = "https://hub.docker.com/api/content/v1/products/search?page_size=16000";

    setlocale(LC_ALL,"");
    
    cs_setopt_url(spider, root_page);

    cs_setopt_useragent(spider, agent);
    //指向自定义的解析函数，和数据持久化函数
    cs_setopt_process(spider, p, NULL);
    cs_setopt_save(spider, s, stdout);
    //设置抓取线程数量，和数据持久化的线程数量
    cs_setopt_threadnum(spider, DOWNLOAD, 1);
    cs_setopt_threadnum(spider, SAVE, 1);
    cs_setopt_logfile(spider, stdout);

    search_data_add();

    //return cs_run(spider);
    return 0;
}
