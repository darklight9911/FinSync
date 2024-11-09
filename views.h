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
int selectOption(int maxOption){
    int selectedOption;
    printf("[+]Select a option[1~%d]:", maxOption);
    scanf("%d", &selectedOption);
    clearScr();
    if (!(selectedOption>= 1 && selectedOption << 10)){
        return 0;
    }

    if (selectedOption >= 1 && selectedOption <= maxOption){
        return selectedOption;
    }else{
        return 0;
    }
}
int homePage(){
    welcomeMessage();
    printf("[1]\tLogin\n[2]\tRegistration\n");
    return selectOption(2);

}


struct loginCred* loginView(){
    struct loginCred* userLoginInfo;
    userLoginInfo = (struct loginCred*)malloc(sizeof(struct loginCred));
    if (userLoginInfo == NULL){
        sysMessage("Error", "Memory allocation failed to store loginCredentials");
        return NULL;
    }
    printf("Provide Username and password:\n");
    scanf("%s %s", userLoginInfo -> username, userLoginInfo -> password);
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

    printf("Username: ");
    scanf("%49s", newUserCredInfo->username);

    printf("Password: ");
    scanf("%49s", newUserCredInfo->password);

    printf("Email: ");
    scanf("%99s", newUserCredInfo->email);
    return newUserCredInfo;
}

