#include<stdio.h>
#include<string.h>
#include<stdbool.h>

bool logininfo (const char *username, const char *password){

   if (strcmp(username, "test")== 0 && strcmp(password,"test")== 0){
    return true;
   }
   return false;
}
int main(){
    const char *username = "test";
    const char *password = "test";

    if (logininfo(username, password)){
        printf("Login successful.\n");
    }  else {
        printf("Login failed.\n");
    }
    return 0;
}