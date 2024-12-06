#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <netdb.h>
#include "constants.h"
#include <curl/curl.h>
#include "structures.h"
char* getCurrentDateTime();
char* generateStrToken(int length);
struct USYNCED_TRANSACTION *uSyncTransactionHead = NULL;
char* BACKEND_URI = "zoogle.projectdaffodil.xyz";
char *USYNCED_TRANSACTIONS[MAX_SIZE];
void programExit(int exitCode, char errorMessage[]);
struct Response callServer(const char *url, char *json_data);
void sysMessage(char prefix[], char comment[]);
bool pushTransactionToServer(struct USYNCED_TRANSACTION *newTransactionInfo);
bool createTransactionView();
void removeQuotes(char *str);
bool saveTransactionToCSV(struct USYNCED_TRANSACTION *head);
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
char* readApiToken(){
    FILE *sessionFile = fopen("sessionFile.lock", "r");
    char *apiToken;
    char line[256];
    long fileSize;
    if (sessionFile == NULL){
        conLog("Failed to read apiToken", "error");
        
        return "null";
    }
    fseek(sessionFile, 0, SEEK_END);
    fileSize = ftell(sessionFile);
    rewind(sessionFile);
    apiToken = (char *)malloc(fileSize + 1);
    if (apiToken == NULL) {
        perror("Error allocating memory");
        fclose(sessionFile);
        return "null";
    }

    fread(apiToken, 1, fileSize, sessionFile);
    apiToken[fileSize] = '\0'; 
    removeQuotes(apiToken);
    return apiToken;

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
        size_t json_size = 430; 
        char *json_data = malloc(json_size);
        if (json_data == NULL) {
            conLog("Memory allocation failed to send packet to backend server to checkConnect\n", "error");
            programExit(0, "Memory allocation failed in checkConnection function");
        }
        json_data = "{\"clientId\":\"testing\"}";
        const char* url = "https://zoogle.projectdaffodil.xyz/api/checkServer";
        struct Response getResponse = callServer(url, json_data);

        if (getResponse.response_code == 200){
            if (checkString("\"True\"", getResponse.data)){
                conLog("Backend working properly", "success");

                return true;                
            }else{
                conLog("Backend Server deny to response", "warning");
                return false;

            }
        }else if(getResponse.response_code == 502){
            conLog("Backend Server Under Maintainance", "error");
            return false;
        }else{
            conLog("Backend Server Currently Offline", "error");
            return false;
        }
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
    // Neccessary files name
    char sessionFileName[] = "sessionFile.lock";
    char transactionStorageFilename[] = "transactionStorage.csv";

    if (checkConnection("zoogle.projectdaffodil.xyz")){ //checking connection with the backend server
        conLog("Connection established with the backend server", "success");
    }else{
        conLog("Connection failed with the backend server", "error");
        // programExit(0, "Connection failed with the backend server");
        while (true){
            sysMessage("INFO", "You are not connected with the internet switching to the offline mode");
            
            createTransactionView();
            if (checkConnection(BACKEND_URI)){
                conLog("DEBUG - Connection came back", "success");
                break;

            }else{
                conLog("DEBUG - Connection failed again", "error");
                continue;;
            }
        }

    }
    if (checkFileExists(sessionFileName)){
        conLog("Session File Found", "success");

    }else{
        conLog("Session File not Found", "error");
        char *filename = "sessionFile.lock";
        if (createFile(filename)){
            conLog("System", "Session file created by the system");
        }else{
            programExit(0, "Programfiles not found 1");
        
        }
    }
    if (checkFileExists(transactionStorageFilename)){
        conLog("Transaction storage files found", "success");
    }else{
        conLog("Transaction storage file not found. Trying to fix by own", "success");
        
        if (createFile(transactionStorageFilename)){
            conLog("System", "Transaction storage file created by the system");

        }else{
            programExit(0, "Programfiles not found 2");
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
            return false;
        }
        if (createSession(getResponse.data)){
            sysMessage(NULL, "Login Successful");
            conLog("Authentication token saved to sessionFile.lock", "success");
            return true;
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
        return 0; 
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
            char *curlError;
            sprintf(curlError, "Curl failed to interact [%s] with the endpoint %s", curl_easy_strerror(res), url);
            // printf("Curl failed: %s\n", curl_easy_strerror(res));
            conLog(curlError, "error");
            server_response.data = "null";
            server_response.response_code = 502;
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
            char *curlError;
            sprintf(curlError, "Curl failed to interact [%s] with the endpoint %s", curl_easy_strerror(res), url);
            // printf("Curl failed: %s\n", curl_easy_strerror(res));
            conLog(curlError, "error");
            server_response.data = "null";
            server_response.response_code = 502;
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

struct USYNCED_TRANSACTION* createUsyncTransaction(int amount, int transactionType, const char transactionReason[]) {
    struct USYNCED_TRANSACTION *newNode;

    newNode = (struct USYNCED_TRANSACTION*)malloc(sizeof(struct USYNCED_TRANSACTION));
    if (newNode == NULL) {
        sysMessage("[ERROR]", "Failed to allocate memory for a new transaction");
        programExit(0, "Memory allocation failed");
    }

    newNode->amount = amount;
    newNode->transactionType = transactionType;

    char *newTransactionId = generateStrToken(6);
    if (newTransactionId == NULL) {
        free(newNode);
        sysMessage("[ERROR]", "Failed to generate transaction ID");
        programExit(0, "Transaction ID generation failed");
    }
    newNode->transactionId = newTransactionId;

    newNode->transactionReason = (char *)malloc(strlen(transactionReason) + 1);
    if (newNode->transactionReason == NULL) {
        free(newNode->transactionId);
        free(newNode);
        sysMessage("[ERROR]", "Failed to allocate memory for transaction reason");
        programExit(0, "Memory allocation failed");
    }
    strcpy(newNode->transactionReason, transactionReason);

    newNode->prev = newNode->next = NULL;

    int pushAttemptCounter = 0;

    if (checkConnection(BACKEND_URI)) {
        conLog("Attempting to send transaction to the server", "info");

        if (pushTransactionToServer(newNode)) {
            sysMessage("SUCCESS", "Transaction synced with the server");
            free(newNode->transactionId);
            free(newNode->transactionReason);
            free(newNode);
            return NULL;
        } else {
            conLog("Transaction could not sent to the server", "error");
            programExit(0, "Transaction could not sent to the server also could not saved locally");
        }
    }

    conLog("Transaction failed to sync. Attempting to save locally", "info");
    if (uSyncTransactionHead == NULL) {
        uSyncTransactionHead = newNode;
        conLog("Transaction created locally as the first transaction", "success");
    } else {
        struct USYNCED_TRANSACTION *temp = uSyncTransactionHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
        conLog("Transaction created locally and appended to the list", "success");
    }

    if (!saveTransactionToCSV(uSyncTransactionHead)) {
        conLog("Failed to save transaction locally to CSV", "error");
        sysMessage("ERROR", "Failed to save transaction locally");
    }

    return newNode;
}

bool logoutOperation(){
    // @assigned to darklight    
    FILE *file;
    file = fopen("sessionFile.lock", "w");
    fprintf(file, "logged out!");
    fclose(file);
    return true;
    
}


char* getLocalSession(){
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
        return "None";
    }

    fread(content, 1, fileSize, authFile);
    content[fileSize] = '\0'; 
    removeQuotes(content);
    return content;
}

bool saveTransactionToCSV(struct USYNCED_TRANSACTION *head) {
    char *filename = "transactionStorage.csv";
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open the transactionStorageFile");
        conLog("Failed to open the transactionStorageFile", "error");
        return false;
    }

    if (head == NULL) {
        conLog("The transaction list is empty", "warning");
        fclose(file);
        return false;
    }

    fprintf(file, "TransactionId,Amount,TransactionType,TransactionReason\n");

    struct USYNCED_TRANSACTION *current = head;
    while (current != NULL) {
        if (current->transactionId == NULL || current->transactionReason == NULL) {
            conLog("Null fields found in the transaction node", "error");
            fclose(file);
            return false;
        }

        fprintf(file, "%s,%d,%d,%s\n",
                current->transactionId,
                current->amount,
                current->transactionType,
                current->transactionReason);

        current = current->next;
    }

    fclose(file);
    conLog("Transactions saved successfully to transactionStorage.csv", "success");
    return true;
}

char* generateStrToken(int length){
    if (length <= 0){
        return NULL;
    }
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char *randomString = (char *)malloc(length + 1);
    if (randomString == NULL){
        conLog("Memory allocation failed to generate randomString", "error");
        programExit(0,"Memory allocation failed to generate randomString");

    }
    srand(time(NULL));
    for (int i = 0; i < length; i++){
        int index = rand() % (sizeof(charset - 1));
        randomString[i] = charset[index];
    }
    randomString[length] = '\0';
    return randomString;    
}

bool pushTransactionToServer(struct USYNCED_TRANSACTION *newTransactionInfo){
    if (checkConnection(BACKEND_URI)){
        conLog("Trying to push tranasction to the server\n", "info");
        size_t json_size = 2000; 
        char *json_data = malloc(json_size);
        if (json_data == NULL) {
            conLog("Memory allocation failed to send packet to backend server to checkConnect\n", "error");
            programExit(0, "Memory allocation failed in checkConnection function");
        }
        sprintf(json_data, "{\"transactionId\": \"%s\",\"transactionReason\":\"%s\",\"amount\":%d,\"transactionType\":%d,\"apiToken\":\"%s\"}", 
        newTransactionInfo->transactionId, 
        newTransactionInfo->transactionReason,
        newTransactionInfo-> amount,
        newTransactionInfo -> transactionType,
        readApiToken());
        // printf("[DEBUG] %s \n", json_data);
        conLog(json_data, "info");
        const char* url = "https://zoogle.projectdaffodil.xyz/api/createTransaction";
        struct Response getResponse = callServer(url, json_data);
        printf("%ld\n", getResponse.response_code);
        conLog(getResponse.data, "info");
        
        return true;
    }else{
        conLog("Failed to push created transaction to the server", "warning");
        sysMessage("MANAGER", "Trying to save the transaction in offline\n");


        return false;
    }



}





struct USYNCED_NODE* LOAD_USYNCED_NODE(int amount, int transactionType, char *transactionReason, char *transactionId) {
    struct USYNCED_NODE *newNode = (struct USYNCED_NODE*)malloc(sizeof(struct USYNCED_NODE));
    newNode->amount = amount;
    newNode->transactionType = transactionType;

    newNode->transactionReason = (char*)malloc(strlen(transactionReason) + 1);
    strcpy(newNode->transactionReason, transactionReason);

    newNode->transactionId = (char*)malloc(strlen(transactionId) + 1);
    strcpy(newNode->transactionId, transactionId);

    newNode->next = NULL;
    return newNode;
}


struct USYNCED_QUEUE* createQueue() {
    struct USYNCED_QUEUE* q = (struct USYNCED_QUEUE*)malloc(sizeof(struct USYNCED_QUEUE));
    q->front = q->rear = NULL;
    return q;
}

int isEmpty(struct USYNCED_QUEUE *q) {
    return (q->front == NULL);
}

void enQueue(struct USYNCED_QUEUE *q, int amount, int transactionType, char *transactionReason, char *transactionId) {
    struct USYNCED_NODE *newNode = LOAD_USYNCED_NODE(amount, transactionType, transactionReason, transactionId);
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

void deQueue(struct USYNCED_QUEUE *q) {
    if (isEmpty(q)) {
        printf("Queue Underflow\n");
        return;
    }

    struct USYNCED_NODE* temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    free(temp->transactionReason);
    free(temp->transactionId);
    free(temp);
}

char* getFront(struct USYNCED_QUEUE* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return NULL;
    }
    return q->front->transactionId;
}

char* getRear(struct USYNCED_QUEUE* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return NULL;
    }
    return q->rear->transactionId;
}
void displayQueue(struct USYNCED_QUEUE *head){
    if (isEmpty(head)) {
        printf("Queue is empty\n");
        return;
    }

    struct USYNCED_NODE* temp = head->front;
    printf("Queue elements: ");
    while (temp != NULL) {
        printf("%s ", temp->transactionId);
        temp = temp->next;
    }
    printf("\n");
}
void storeQueueToCSV(struct USYNCED_QUEUE *head) {
    if (isEmpty(head)) {
        printf("Queue is empty\n");
        return;
    }

    FILE *file = fopen("transactionStorage.csv", "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "TransactionId,Amount,TransactionType,TransactionReason\n");

    struct USYNCED_NODE *temp = head->front;
    while (temp != NULL) {
        if (checkConnection(BACKEND_URI)){
            struct USYNCED_TRANSACTION *pushTransaction;
            if (pushTransaction == NULL){
                conLog("Memory allocation failed for pushTransaction", "error");
                programExit(0, "Memory allocation failed");
            }
            pushTransaction -> amount = temp ->amount;
            strcpy(pushTransaction -> transactionId, temp -> transactionId);
            strcpy(pushTransaction -> transactionReason, temp -> transactionReason);
            pushTransaction -> transactionType = temp -> transactionType;
            if (pushTransactionToServer(pushTransaction)){
                conLog("Unsynced transaction pushed successfully","success");
            }else{
                conLog("Unsynced transaction failed to be pushed", "error");
                fprintf(file, "%s,%d,%d,%s\n", temp->transactionId, temp->amount, temp->transactionType, temp->transactionReason);
            }


        }else{
            fprintf(file, "%s,%d,%d,%s\n", temp->transactionId, temp->amount, temp->transactionType, temp->transactionReason);
        }
        temp = temp->next;

    }

    fclose(file);
    printf("Queue data has been written to queue_data.csv\n");
}


void loadTransactionsFromCSV() {
    char *filename = "transactionStorage.csv";
    FILE *file = fopen(filename, "r");
    struct USYNCED_QUEUE* notSyncedTransactions = createQueue();
    if (!file) {
        perror("Failed to open CSV file");
        return;
    }

    char line[256];
    int lineNum = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (lineNum == 0) {
            lineNum++;
            continue;
        }

        char *transactionId = strtok(line, ",");
        char *amountStr = strtok(NULL, ",");
        char *transactionTypeStr = strtok(NULL, ",");
        char *transactionReason = strtok(NULL, "\n");

        if (transactionId && amountStr && transactionTypeStr && transactionReason) {
            int amount = atoi(amountStr);           
            int transactionType = atoi(transactionTypeStr); 

            printf("TransactionId: %s, Amount: %d, Type: %d, Reason: %s\n",
                   transactionId, amount, transactionType, transactionReason);
            enQueue(notSyncedTransactions, amount, transactionType, transactionReason, transactionId);

            
        } else {
            fprintf(stderr, "Invalid data on line %d: %s", lineNum + 1, line);
        }

        lineNum++;
    }

    fclose(file);
    printf("Transactions loaded successfully from %s\n", filename);
    
    storeQueueToCSV(notSyncedTransactions);
    displayQueue(notSyncedTransactions);
}

bool postInternetConnection(){
    loadTransactionsFromCSV();
}