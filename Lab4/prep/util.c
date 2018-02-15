#include <string.h>
#include "util.h"
void getTempFromJson(char *json, char* buff){
	if(strstr(json, "\"temp\"") == NULL) return;
	
	char * start = strstr(json, "\"temp\"") + 7;
	int i = 0;
	while(start[i] != ','){
		buff[i] = start[i];
		i++;
	}
	buff[i + 1] = '\0';
}
