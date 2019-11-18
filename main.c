#include<cspider/spider.h>
#include<cspider/cJSON.h>
#include <locale.h>

void save2DB(cJSON *data){

}

void p(cspider_t *cspider, char *d, char *url, void *user_data) {
    printf("%s crawled\n", url);
    cJSON *json = cJSON_Parse(d);
    cJSON *items = cJSON_GetObjectItem(json, "summaries");
    int datasum = cJSON_GetArraySize(items);
    for (int i = 0; i < datasum; i ++){
      cJSON *item = cJSON_GetArrayItem(items, i);
      printf("%s\n", cJSON_Print(item));
      save2DB(item);
    }
}

void s(void *str, void *user_data) {
    char *get = (char*)str;
    FILE *file = (FILE*)user_data;
    printf("[check point2]hahahah, s running\n");
    printf("get:%s\n", get);
    fprintf(file, "%s\n", get);
    return;
}

int main() {
    cspider_t *spider = init_cspider();
    char *agent = "Mozilla/5.0 (X11; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0";
    char *root_page = "https://hub.docker.com/api/content/v1/products/search?image_filter=official&page_size=160&q=&type=image";

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

    return cs_run(spider);
}
