#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "fsGlobal.h"
#include "constants.h"
#include "views.h"

void sleep();
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

        }else if(appWindow == 99){
            int x;
            

            while(true){
                x = userDashboard();
                // conLog("You are logged in", "info");
                if (x == 1){
                    sysMessage("OUT","You selected to create transaction.\n");
                    createTransactionView();
                    sleep(2);
                }else if(x == 2){
                    printf("Transaction history");
                }else if(x == 3){
                    printf("Stats");
                }else if (x == 4){
                    printf("logout");
                }else{
                    sysMessage("WARN", "You have selected unknown option\n");
                    sleep(1);
                    continue;
                }
            }
            continue;
            
        }
        
        return 0;
    }

}