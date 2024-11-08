#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fsGlobal.h"
#include "constants.h"
#include "views.h"
#include <time.h>
// #include "structures.h"
struct loginCred* loginInfoVAR;

int main()
{
    startupCheck();
    const char *url = "http://localhost:8000/createAccount";
    const char *json_data = "{\"username\":\"value\",\"password\":\"test\",\"email\":\"test\"}";

    int response_code = send_post_request(url, json_data);
    printf("HTTP Response Code: %d\n", response_code);

    return 0;
    // callServer();
    // int counter = 0;
    int appWindow = homePage();
    while (true){
          
        // printf("%d", appWindow);
        if (appWindow == 0){
            system("clear");
            sysMessage("INVALID", "No valid input shown");
            int appWindow = homePage();

        }else if(appWindow == 1){
            // printf("You selected to login");

            loginInfoVAR = loginView();
            checklogin(loginInfoVAR);
            
        }else if(appWindow == 2){
            struct newUserCred* newUserCredInfo;
            newUserCredInfo = registrationFormView();
            registerOperation(newUserCredInfo);

        }
        return 0;
    }

}