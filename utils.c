#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

#define DEBUG 1
// data structure for curl
// ref:https://stackoverflow.com/questions/13905774/in-c-how-do-you-use-libcurl-to-read-a-http-response-into-a-string
struct url_data {
    size_t size;
    char* data;
};

// delete unrelated char
char *remove_invalid_char(const char *data){
    int count = 0;
    char *ret;
    int p, q;
    int i;
    if (data == NULL || strlen(data) == 0)
        return "NULL";
    for (i = 0; i < strlen(data); ++ i){
        if (data[i] == '\"' && i > 0 && data[i - 1] != '\\'){
            count ++;
        }
    }
    ret = (char *) malloc(sizeof(char) * (strlen(data) + count + 1));
    memset(ret, '\0', strlen(data) + count + 1);
    p = 0;
    q = 0;
    for (; p < strlen(data); ++ p, ++ q){
        if (data[p] == '\"' && p > 0 && data[p - 1] != '\\'){
            ret[q ++] = '\\';
        }
        ret[q] = data[p];
    }
    ret[strlen(data) + count] = '\0';
    return ret;
}

// writing process
static size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;

    data->size += (size * nmemb);

#ifdef DEBUG
    fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
#endif
    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

// get info from url
char *handle_url(char* url) {
    CURL *curl;
    int i;
    struct url_data data;
    data.size = 0;
    data.data = malloc(4096 * 1024); /* reasonable size initial buffer */
    if(NULL == data.data) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0"); 
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl); 
        int flag = 0;
        if(res != CURLE_OK) {
            for (i = 0; i < 10; ++ i){
                res = curl_easy_perform(curl); 
                if (res == CURLE_OK){
                    flag = 1;
                    break;
                }
                sleep(1);
            }
            if (flag == 0)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",  
                        curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    return data.data;
}
/*
int main(int argc, char* argv[]) {
    char* data;

    if(argc < 2) {
        fprintf(stderr, "Must provide URL to fetch.\n");
        return 1;
    }
    char *url = "https://hub.docker.com/v2/repositories/library/marklogic";
    url = "https://hub.docker.com/v2/repositories/library/gcc";
    data = handle_url(url);

    if(data) {
        printf("%d\n", strlen(data));
        printf("%s\n", data);
        free(data);
    }

    return 0;
}
*/
