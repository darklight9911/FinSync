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

typedef struct Transaction {
    char transactionId[50];
    int amount;
    int transactionType;
    char transactionReason[200];
    struct Transaction* next; 
} Transaction;

typedef struct TransactionQueue {
    Transaction* front;
    Transaction* rear;
} TransactionQueue;


typedef struct TransactionHistory {
    char id[50];
    char time[50];
    int amount;
    char type[10];
    char reason[200];
    struct TransactionHistory *next;
} TransactionHistory;

typedef struct Stack {
    TransactionHistory *top;
} Stack;