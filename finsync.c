#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fsGlobal.h"
#include "constants.h"
#include "views.h"
#include <time.h>


int main()
{
    startupCheck();

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
            struct loginCred* loginInfoVAR;
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