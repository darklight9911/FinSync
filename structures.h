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

struct USYNCED_TRANSACTION{
    char *transactionId;
    int amount;
    int transactionType;
    char *transactionReason;
    struct USYNCED_TRANSACTION *next, *prev;
    

};
struct USYNCED_NODE {
    int amount;
    int transactionType;
    char *transactionReason;
    char *transactionId;
    struct USYNCED_NODE *next;
};
struct USYNCED_QUEUE {
    struct USYNCED_NODE *front, *rear;
};
