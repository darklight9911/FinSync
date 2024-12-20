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
    start:
    // postInternetConnection();
    // programExit(0,"DEBUG mode ended");
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
            if (checklogin(loginInfoVAR)){
                // sleep(2);
                goto start;

            }else{
                struct loginCred* loginInfoVAR;
                loginInfoVAR = loginView();
                if (checklogin(loginInfoVAR)){
                    goto start;
                }else{
                    clearScr();
                    sysMessage("[MAX]", "Maximum number of attempt completed");
                    programExit(0, "Exited due to exceeding limits");
                }
            }
            
        }else if(appWindow == 2){
            struct newUserCred* newUserCredInfo;
            newUserCredInfo = registrationFormView();
            registerOperation(newUserCredInfo);
            goto start;
        }else if(appWindow == 99){
            int x;
            

            while(true){
                x = userDashboard();//option
                // conLog("You are logged in", "info");
                if (x == 1){
                    sysMessage("OUT","You selected to create transaction.\n");
                    createTransactionView();
                    // sleep(2);
                }else if(x == 2){
                    printf("Transaction history");
                    int tom;
                    tom = viewTransaction();
                    if (tom == 0){
                        continue;
                    }
                    // sleep(3);
                }else if(x == 3){
                        Stack stack;
                        initStack(&stack);
                        if (checkConnection(BACKEND_URI)){
                            downloadTransactionHistory();
                            loadTransactions(&stack, "transactionHistory.csv");
                            
                        }else{
                            loadTransactions(&stack, "transactionHistory.csv");
                        
                        }

                        int y = statsPage(&stack);
                        if (y == 0){
                            continue;

                        }
                }else if (x == 4){

                    sysMessage("PROCESS", "Trying to logout");
                    if (logoutOperation()){
                        clearScr();
                        sysMessage("SUCCESS", "Logout successful");
                        goto start;
                    }
                
                    break;

                }
                // else if (x == 4){
                //     sysMessage("PROC", "Trying to save the transactions in tempStorage");
                //     if (saveTransactionToCSV(uSyncTransactionHead)){
                //         conLog("Transaction saved into tempStorage", "success");
                //     }else{
                //         conLog("Transaction could not saved", "error");
                //     }
                //     // sleep(2);       
                //     }
                else{
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