#include <stdio.h>
#include <stdlib.h>


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
        printf("\n\t USER DASHBOARD \t\n");
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
    printf("[IN] Reason: ");
    scanf("%s", transactonReason);
    createUsyncTransaction(amount, transactionType, transactonReason);
    return true;
}
void viewTransaction(){
    // Complete this function add all the details of transaction
    struct USYNCED_TRANSACTION *temp;
    temp = uSyncTransactionHead;
    int counter = 1;
    while(temp != NULL){
        printf("%d. Amount : %d\n", counter, temp->amount);
        counter++;
        temp = temp -> next;
    }
}


float fetchCurrentBalance(){

    return 0.0;
}

bool syncWithServer(){
    
    return true;
}

