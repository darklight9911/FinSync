
char* getCurrentDateTime() {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
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
void initializeQueue(TransactionQueue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

void enqueue(TransactionQueue* queue, const char* transactionId, int amount, int transactionType, const char* transactionReason) {
    Transaction* newTransaction = (Transaction*)malloc(sizeof(Transaction));
    if (!newTransaction) {
        perror("Failed to allocate memory for new transaction");
        return;
    }

    strcpy(newTransaction->transactionId, transactionId);
    newTransaction->amount = amount;
    newTransaction->transactionType = transactionType;
    strcpy(newTransaction->transactionReason, transactionReason);
    newTransaction->next = NULL;

    if (queue->rear == NULL) {
        queue->front = newTransaction;
        queue->rear = newTransaction;
    } else {
        queue->rear->next = newTransaction;
        queue->rear = newTransaction;
    }
}

Transaction* dequeue(TransactionQueue* queue) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }

    Transaction* dequeuedTransaction = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    return dequeuedTransaction;
}

void loadTransactionsFromCSV(TransactionQueue* queue) {
    char* filename = "transactionStorage.csv";
    FILE* file = fopen(filename, "r");
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

        char* transactionId = strtok(line, ",");
        char* amountStr = strtok(NULL, ",");
        char* transactionTypeStr = strtok(NULL, ",");
        char* transactionReason = strtok(NULL, "\n");

        if (!transactionId || !amountStr || !transactionTypeStr || !transactionReason) {
            fprintf(stderr, "Invalid CSV format on line %d\n", lineNum + 1);
            continue;
        }

        int amount = atoi(amountStr);
        int transactionType = atoi(transactionTypeStr);
        enqueue(queue, transactionId, amount, transactionType, transactionReason);
        lineNum++;
    }

    fclose(file);
    conLog("Transactions loaded and enqueued successfully","success");
}

void printQueue(TransactionQueue* queue) {
    Transaction* current = queue->front;
    while (current != NULL) {
        printf("Transaction: %s | Amount: %d | Type: %d | Reason: %s\n",
               current->transactionId, current->amount, current->transactionType, current->transactionReason);
        current = current->next;
    }
}
void saveQueueToCSV(TransactionQueue* queue, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    fprintf(file, "TransactionId,Amount,TransactionType,TransactionReason\n");

    Transaction* current = queue->front;
    while (current != NULL) {
        fprintf(file, "%s,%d,%d,%s\n",
                current->transactionId,
                current->amount,
                current->transactionType,
                current->transactionReason);
        current = current->next;
    }

    fclose(file);
    conLog("Transaction have been saved to transactionStorage.csv", "success");
}


