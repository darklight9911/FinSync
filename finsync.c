#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
/*
Assignment 1:
Title: USER INTERACTION
Show 4 option. And let user to select them and then tell them what they selected.
*/
int main()
{
    while (1)
    {

        printf("[+]Select an option:\n[1]Create\n[2]View\n[3]Stats\n[4]Requests\n");
        int input;
        printf("Enter option number:\n");
        scanf("%d", &input);
        if (input == 1)
        {
            system("clear");
            printf("Entered to the Creation.\n");
        }
        else if (input == 2)
        {
            system("clear");
            printf("Entered to the view section.\n");
        }
        else if (input == 3)
        {
            system("clear");
            printf("Entered to the statics.\n");
        }
        else if (input == 4)
        {
            system("clear");
            printf("Entered to the Requests section.\n");
        }
        else
        {
            exit(0);
        }
    }
    return 0;
}