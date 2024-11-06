#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

char* getCurrentDateTime();

bool checkString(char string1[], char string2[]) {
    int checkStrInteger = strcmp(string1, string2);
    printf("DEBUG: %s ? %s : %d\n", string1, string2, checkStrInteger);
    return checkStrInteger == 0;
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
    fclose(logs);
    return true;
}

void conOut(char string[], char level[]) {
    if (checkString(level, "warning")) {
        level = "[warning]";
    } else if (checkString(level, "error")) {
        level = "[error]";
    } else if (checkString(level, "success")) {
        level = "[success]";
    } else if (checkString(level, "critical")) {
        level = "[critical]";
    } else {
        level = "[info]";
    }
    printf("[%s] %s\n", level, string);
}

char* getCurrentDateTime() {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}
