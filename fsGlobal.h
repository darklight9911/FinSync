#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <netdb.h>
#include "constants.h"
#include <curl/curl.h>
#include "structures.h"
char* getCurrentDateTime();




bool checkString(char string1[], char string2[]) {
    int checkStrInteger = strcmp(string1, string2);
    // printf("DEBUG: %s ? %s : %d\n", string1, string2, checkStrInteger);
    return checkStrInteger == 0;
}

bool conLog(char string[], char level[]) {
    FILE *logs = fopen("debug.log", "a+");
    if (logs == NULL) {
        printf("ERROR: Log write unsuccessful.\n");
        return false;
    }

    char *currentTime = getCurrentDateTime();
    currentTime[strcspn(currentTime, "\n")] = '\0';

    fprintf(logs, "%s~[%s] %s\n", currentTime, level, string);
    if (CONLOG_OUTPUT_ENABLED){
        printf("%s~[%s] %s\n", currentTime, level, string);
    }
    fclose(logs);
    return true;
}

void conOut(char string[], char level[]){
    if (checkString(level, "warning")) {
        level = "[warning]";
    } else if(checkString(level, "error")) {
        level = "[error]";
    } else if(checkString(level, "success")) {
        level = "[success]";
    } else if(checkString(level, "critical")) {
        level = "[critical]";
    }else if(checkString(level, "debug")){
        level = "[debug]";
    }
    else {
        level = "[info]";
    }
    
    printf("[%s] %s\n", level, string);

}

char* getCurrentDateTime() {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}

bool checkConnection(char url[]){
    char *hostname;   
	struct hostent *hostinfo;

	hostname = url;
	hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL) {
        // printf("-> no connection!\n");
        return false;
    } else {
        // printf("-> connection established!\n");
        return true;
    }

}

void programExit(int exitCode, char errorMessage[]){
    conOut("[Exit]Program Exited due to %s\n", errorMessage);
    exit(exitCode);
}
bool startupCheck(){
    conLog("Starting startupCheck", "info");
    if (checkConnection("neobank.projectdaffodil.xyz")){ //checking connection with the backend server
        conLog("Connection established with the backend server", "success");
    }else{
        conLog("Connection failed with the backend server", "error");
        programExit(0, "Connection failed with the backend server");       
    }

}
void sysMessage(char prefix[],char comment[]){
    if (prefix == NULL){
        prefix = "OUT"; 
    }
    printf("[~%s] %s\n", prefix, comment);
}


bool checklogin(struct loginCred* logininfo) {
    printf("username: %s | password: %s\n", logininfo->username, logininfo->password);
    return true;
}

void registerOperation(struct newUserCred* newUserCredInfo){
    sysMessage(NULL, "Registration Process Started");
    printf("%s", newUserCredInfo -> username);
    
}

char* callServer(const char *url, const char *json_data) {
    CURL *curl;
    CURLcode res;
    long response_code = 0;

    curl_global_init(CURL_GLOBAL_ALL); 
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            printf("Response Code: %ld\n", response_code);
        }

        // Cleanup
        curl_slist_free_all(headers); 
        curl_easy_cleanup(curl);      
    }

    curl_global_cleanup();
    return response_code;
}