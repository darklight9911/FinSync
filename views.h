#include <stdio.h>
#include <stdlib.h>

int fetchCurrentBalance();


void welcomeMessage(){
    printf("Welcome to FinSync.\n");

}
void clearScr(){
    if (OS_NAME == "windows"){
        system("cls");
    }else{
        system("clear");
    }
}
int selectOption(int maxOption) {
    int option;
    printf("[~] Select an option (1-%d): ", maxOption);
    if (scanf("%d", &option) != 1 || option < 1 || option > maxOption) {
        while (getchar() != '\n');
        clearScr();
        sysMessage("WARN", "Invalid option selected");
        return 0; 
    }
    clearScr();
    return option;
}
int homePage(){
    welcomeMessage();
    if (authCheck()){
        return 99;
    }else{
        printf("[1]\tLogin\n[2]\tRegistration\n");
        return selectOption(2);

    }

}


struct loginCred* loginView(){
    struct loginCred* userLoginInfo;
    userLoginInfo = (struct loginCred*)malloc(sizeof(struct loginCred));
    if (userLoginInfo == NULL){
        sysMessage("Error", "Memory allocation failed to store loginCredentials");
        return NULL;
    }
    printf("[+] Username: \n");
    scanf("%s", userLoginInfo -> username);

    printf("[+] Password: \n");
    scanf("%s", userLoginInfo -> password);
    // printf("%s, %s", userLoginInfo -> username, userLoginInfo -> password);
    return userLoginInfo;
}
struct newUserCred* registrationFormView(){
    struct newUserCred* newUserCredInfo;
    newUserCredInfo = (struct newUserCred*)malloc(sizeof(struct newUserCred));
        if (newUserCredInfo == NULL) {
        // printf("Memory allocation failed.\n");
        programExit(0, "Memory allocation failed to store registration data");
    }

    printf("[+] Username: ");
    scanf("%49s", newUserCredInfo->username);

    printf("[+] Password: ");
    scanf("%49s", newUserCredInfo->password);

    printf("[+] Email: ");
    scanf("%99s", newUserCredInfo->email);
    return newUserCredInfo;
}


int userDashboard(){
    clearScr();
    while(true){
        printf("\n\t USER DASHBOARD\t\n");
        printf("\tBalance: %d Taka\t\n", fetchCurrentBalance());
        // fetchCurrentBalance();
        printf("[1] Create Transaction\n");
        printf("[2] Transaction History\n");
        printf("[3] Stats\n");
        printf("[4] Logout\n");
        int option = selectOption(4);
        if (option == 0){
            continue;
        }else{
            return option;

        }
    }

}

bool createTransactionView(){
    struct USYNCED_TRANSACTION *tempTransaction;
    int amount, transactionType;
    char transactonReason[200];
    // also provide the transactionId. have to do something about it
    printf("\tCreate Transaction\t\n");
    printf("[~] Transaction Type: \n");
    printf("[1]\tOutgoing\n[2]\tIncoming\n[3]\tGiven\n[4]\tTaken\n");
    transactionType = selectOption(4);
    printf("[IN] Enter amount: ");
    scanf("%d", &amount);
    while (getchar() != '\n');
    printf("[IN] Reason: ");
    // scanf("%s", transactonReason);
    fgets(transactonReason, 200, stdin);
    size_t len = strlen(transactonReason);
    if (len > 0 && transactonReason[len - 1] == '\n') {
        transactonReason[len - 1] = '\0';
    }

    createUsyncTransaction(amount, transactionType, transactonReason);
    return true;
}
int viewTransaction() {
    if(checkConnection(BACKEND_URI)){
        downloadTransactionHistory();
        conLog("Loaded latest transactions","info");
        Stack stack;
        initStack(&stack);
        loadTransactions(&stack, "transactionHistory.csv");
        printf("Loaded Transactions:\n");
        displayTransactions(&stack);
        TransactionHistory *transaction;
        while ((transaction = pop(&stack)) != NULL) {
            free(transaction);
        }
   

    }
    else{
        conLog("Loaded previously downloaded transactions","info");
        Stack stack;
        initStack(&stack);
        loadTransactions(&stack, "transactionHistory.csv");
        printf("Loaded Transactions:\n");
        displayTransactions(&stack);
        TransactionHistory *transaction;
        while ((transaction = pop(&stack)) != NULL) {
            free(transaction);
        }
    }
    int option;
    sysMessage("IN","Enter anything to go back");
    scanf("%d", &option);
    if (option){
        return 0;
    }
}



int fetchCurrentBalance(){
    int balance;

    if (checkConnection(BACKEND_URI)){
        size_t json_size = 500;
        char *json_data = malloc(json_size);
        char *url = "https://zoogle.projectdaffodil.xyz/api/getCurrentBalance";

        sprintf(json_data, "{\"apiToken\":\"%s\"}", readApiToken());
        struct Response getResponse;
        getResponse = callServer(url, json_data);
        if (getResponse.response_code == 200){
            removeQuotes(getResponse.data);
            balance = atoi(getResponse.data);
            CURRENT_BALANCE = balance;
        }else{
            balance = atoi(getResponse.data);
            balance = CURRENT_BALANCE;
        }
    }else{
        balance = CURRENT_BALANCE;
    }


    

    return balance;
}

int statsPage(Stack *stack) {
    if (!stack || !stack->top) {
        printf("Stack is empty!\n");
        return;
    }


    TransactionHistory *current = stack->top;
    clearScr();
    printf("\t\t Statistics \t\t\n");

    int totalSpent = 0;
    int totalIncome = 0;
    int totalGiven = 0;
    int totalTaken = 0;
    int counter = 0;

    while (current) {

        if (strcmp(current->type, "Outgoing") == 0) {
            totalSpent += current->amount;
        } else if (strcmp(current->type, "Incoming") == 0) {
            totalIncome += current->amount;
        } else if (strcmp(current->type, "Taken") == 0) {
            totalTaken += current->amount;
        } else if (strcmp(current->type, "Given") == 0) {
            totalGiven += current->amount;
        }

        current = current->next;
        counter++;
    }

    char spentInfo[200], incomeInfo[200], givenInfo[200], takenInfo[200];
    sprintf(spentInfo, "Total Spent : %d Taka\n", totalSpent);
    sprintf(incomeInfo, "Total Income : %d Taka\n", totalIncome);
    sprintf(givenInfo, "Total Given : %d Taka\n", totalGiven);
    sprintf(takenInfo, "Total Taken : %d Taka\n", totalTaken);

    sysMessage("STATS", spentInfo);
    sysMessage("STATS", incomeInfo);
    sysMessage("STATS", givenInfo);
    sysMessage("STATS", takenInfo);

    sysMessage("IN","Enter anything to go back:");
    int goBack;
    scanf("%d", &goBack);
    if (goBack){
        return 0;
    }

    
}
