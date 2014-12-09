#include <stdio.h>
#include "rtc.h"
#include <curl/curl.h>
#include <stdlib.h>
extern int client(void);

/* Since this program depends on libcurl, you may need to 
 * install libcurl4-gnutls-dev.
 *
 * On Linux Mint, this can be done by running:
 * sudo apt-get install libcurl4-gnutls-dev
 */
struct MemoryStruct{
char *memory;
size_t size;
}; 


void HTTP_GET(const char* url){
       struct MemoryStruct chunk;
       chunk.memory = malloc(1);
       chunk.size = 0 ;  
	CURL *curl;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void *)&chunk);	
		curl_easy_perform(curl);
	 
         	curl_easy_cleanup(curl);
		if(chunk.memory)
                  free(chunk.memory);  
	}
}

int client(void){
char buf[200]; // ="http://192.168.0.10:3126/update?id=1&password=password&name=microex&data=123&status=OK&timestamp=20141116-09:12:34";



	snprintf(buf, 200, "http://%s:%d/update?id=%d&password=%s&name=%s&data=%d&status=%s&timestamp=%s",
			http.host,
			http.port,
			http.id,
			http.password,
			http.name,
			http.adcval,
			http.status,
            		timestamp);
	HTTP_GET(buf);
	return 0;
}
