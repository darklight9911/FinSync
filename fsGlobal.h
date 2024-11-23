#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <netdb.h>
#include "constants.h"
#include <curl/curl.h>
#include "structures.h"
char* getCurrentDateTime();
struct USYNCED_TRANSACTION *uSyncTransactionHead;

struct Response callServer(const char *url, char *json_data);
void sysMessage(char prefix[], char comment[]);

bool checkString(char string1[], char string2[]) {
    int checkStrInteger = strcmp(string1, string2);
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
bool createFile(char filename[]){
    FILE *dummyFile = fopen(filename, "w");
    if (dummyFile == NULL){
        return false;
    }else{
        fprintf(dummyFile, "this file created by finsync");
        fclose(dummyFile);
        return true;
    }
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
    char comment[300];
	hostname = url;
	hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL) {
        sprintf(comment," %s no connection", url);
        conLog(comment, "error");
        
        return false;
    } else {
        sprintf(comment," %s connection established", url);
        conLog(comment, "info");
        return true;
    }

}

void programExit(int exitCode, char errorMessage[]){
    printf("[Exit]Program Exited due to %s\n", errorMessage);
    exit(exitCode);
}
bool checkFileExists(char *filename){
    FILE *file;
    if ((file = fopen(filename, "r")) != NULL){
        return true;
        fclose(file);
    }else{
        return false;
    }
}
bool startupCheck(){
    conLog("Starting startupCheck", "info");
    if (checkConnection("zoogle.projectdaffodil.xyz")){ //checking connection with the backend server
        conLog("Connection established with the backend server", "success");
    }else{
        conLog("Connection failed with the backend server", "error");
        programExit(0, "Connection failed with the backend server");       
    }
    char sessionFileName[] = "sessionFile.lock"; 
    if (checkFileExists(sessionFileName)){
        conLog("Session File Found", "success");

    }else{
        conLog("Session File not Found", "error");
        char *filename = "sessionFile.lock";
        if (createFile(filename)){
            conLog("System", "Session file created by the system");
        }else{
            programExit(0, "Programfiles not found");
        
        }
    }
    

}


void sysMessage(char prefix[],char comment[]){
    if (prefix == NULL){
        prefix = "OUT"; 
    }
    printf("[~%s] %s\n", prefix, comment);
}

bool createSession(char accessToken[]){
    FILE *sessionFile = fopen("sessionFile.lock", "w");   
    if (sessionFile == NULL){
        sysMessage("Error", "sessionFile.lock creation failed");
        return false;
    }
    fprintf(sessionFile, "%s", accessToken);
    fclose(sessionFile);
    return true;
}

bool checklogin(struct loginCred* logininfo) {
    printf("username: %s | password: %s\n", logininfo->username, logininfo->password);
    const char *url = "https://zoogle.projectdaffodil.xyz/api/login";
    // char *json_data;
    size_t json_size = 430; 
    char *json_data = malloc(json_size);
    if (json_data == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    sprintf(json_data,"{\"username\":\"%s\",\"password\":\"%s\"}",logininfo->username,logininfo->password);
    struct Response getResponse = callServer(url, json_data);
    if(getResponse.response_code == 200){   
        char responseLog[100];
        sprintf(responseLog, "Response JSON: %d\n", (int) getResponse.response_code);
        conLog(responseLog, "info");
        if (checkString(getResponse.data, "\"False\"")){
            sysMessage(NULL, "Wrong username or password!");
            free(getResponse.data);
            return 0;
        }
        if (createSession(getResponse.data)){
            sysMessage(NULL, "Login Successful");
            conLog("Authentication token saved to sessionFile.lock", "success");
        }else{
            sysMessage("FAILED", "Login Unsuccessful");
            
        }
        free(getResponse.data);
    }else{
        sysMessage("Error", "Communication between server failed");
        printf("%d", (int) getResponse.response_code);
    }
    return 0;
    
}

int registerOperation(struct newUserCred* newUserCredInfo){
    sysMessage(NULL, "Registration Process Started");
    printf("username: %s | password: %s\n", newUserCredInfo->username, newUserCredInfo->password);
    const char *url = "https://zoogle.projectdaffodil.xyz/api/signup";
    // char *json_data;
    size_t json_size = 700; 
    char *json_data = malloc(json_size);
    if (json_data == NULL) {
        printf("Memory allocation failed to create registration\n");
        return 1;
    }
    sprintf(json_data,"{\"username\":\"%s\",\"password\":\"%s\",\"email\":\"%s\"}",newUserCredInfo->username,newUserCredInfo->password, newUserCredInfo->email);
    struct Response getResponse = callServer(url, json_data);
    if(getResponse.data){   
        printf("Response JSON: %s\n",getResponse.data);
        free(getResponse.data);
    }
    return 0;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + total_size + 1);
    if (ptr == NULL) {
        printf("Not enough memory to allocate!\n");
        return 0; // Out of memory
    }

        mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->memory[mem->size] = 0;

    return total_size;
}


struct Response callServer(const char *url, char *json_data) {
    CURL *curl;
    CURLcode res;
    struct Response server_response = {NULL, 0};
    struct curl_slist *headers = NULL;
    long http_code = 0;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &server_response.data);

        res = curl_easy_perform(curl);
        
        if(res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            server_response.response_code = http_code;
        } else {
            printf("Curl failed: %s\n", curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return server_response;
}

int getLength(char* givenStr){
    return strlen(givenStr);
}

struct Response recallServer(const char *url, char *json_data) {
    CURL *curl;
    CURLcode res;
    struct Response server_response = {NULL, 0};
    struct curl_slist *headers = NULL;
    long http_code = 0;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &server_response.data);

        res = curl_easy_perform(curl);
        
        if(res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            server_response.response_code = http_code;
        } else {
            printf("Curl failed: %s\n", curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return server_response;
}
void removeQuotes(char *str) {
    int i, j = 0;
    int len = strlen(str);

    for (i = 0; i < len; i++) {
        if (str[i] != '\"') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0'; 
}
bool authCheck(){
    FILE *authFile;
    char line[256];
    char *content;
    long fileSize;
    authFile = fopen("sessionFile.lock", "r");

    if (authFile == NULL){
        sysMessage("[READ ERROR]", "Failed to read auth file");
        return false;
    }
    fseek(authFile, 0, SEEK_END);
    fileSize = ftell(authFile);
    rewind(authFile);
    content = (char *)malloc(fileSize + 1);
    if (content == NULL) {
        perror("Error allocating memory");
        fclose(authFile);
        return 1;
    }

    fread(content, 1, fileSize, authFile);
    content[fileSize] = '\0'; 
    removeQuotes(content);
    const char *url = "https://zoogle.projectdaffodil.xyz/api/checkSession";
    // char *json_data;
    size_t json_size = 700; 
    char *json_data = malloc(json_size);
    if (json_data == NULL) {
        printf("Memory allocation failed to create registration\n");
        return 1;
    }
    sprintf(json_data,"{\"accessToken\":\"%s\"}", content);
    struct Response getResponse = callServer(url, json_data);
    free(content);
    fclose(authFile);
    
    if (checkString(getResponse.data, "\"True\"")){
        conLog("Session Matched", "info");
        return true;
    }else {
        conLog("Session not matched", "warning");
        return false;
    }
}
    
void delay(int seconds) {
    int milli_seconds = seconds * 1000;
    clock_t start_time = clock();
    
    while (clock() < start_time + milli_seconds);
}

struct USYNCED_TRANSACTION* createUsyncTransaction(int amount, int transactionType) {
    if (uSyncTransactionHead == NULL) {
        uSyncTransactionHead = (struct USYNCED_TRANSACTION*)malloc(sizeof(struct USYNCED_TRANSACTION));
        if (uSyncTransactionHead == NULL) {
            sysMessage("[ERROR]", "Failed to allocate memory for the first transaction");
            programExit(0, "Memory allocation failed");
        }
        uSyncTransactionHead->prev = uSyncTransactionHead->next = NULL;
        uSyncTransactionHead->amount = amount;
        uSyncTransactionHead->transactionType = transactionType;

        conLog("Transaction Created Locally. It was the first transaction", "success");
        return uSyncTransactionHead;
    } else {
        struct USYNCED_TRANSACTION *newNode, *temp;
        newNode = (struct USYNCED_TRANSACTION*)malloc(sizeof(struct USYNCED_TRANSACTION));
        if (newNode == NULL) {
            sysMessage("[ERROR]", "Failed to allocate memory for new transaction");
            programExit(0, "Memory allocation failed");
        }

        newNode->next = newNode->prev = NULL;
        newNode->amount = amount;
        newNode->transactionType = transactionType;

        temp = uSyncTransactionHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = newNode;
        newNode->prev = temp;

        return newNode;
    }
}


