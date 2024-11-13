struct loginCred {
    char username[200];
    char password[200];
};
struct newUserCred{
    char username[200];
    char password[200];
    char email[200];
};
struct Response {
    char *data;
    long response_code;
};
