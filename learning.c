#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct USYNCED_NODE* LOAD_USYNCED_NODE(int amount, int transactionType, char *transactionReason, char *transactionId) {
    struct USYNCED_NODE *newNode = (struct USYNCED_NODE*)malloc(sizeof(struct USYNCED_NODE));
    newNode->amount = amount;
    newNode->transactionType = transactionType;

    newNode->transactionReason = (char*)malloc(strlen(transactionReason) + 1);
    strcpy(newNode->transactionReason, transactionReason);

    newNode->transactionId = (char*)malloc(strlen(transactionId) + 1);
    strcpy(newNode->transactionId, transactionId);

    newNode->next = NULL;
    return newNode;
}


struct USYNCED_QUEUE* createQueue() {
    struct USYNCED_QUEUE* q = (struct USYNCED_QUEUE*)malloc(sizeof(struct USYNCED_QUEUE));
    q->front = q->rear = NULL;
    return q;
}

int isEmpty(struct USYNCED_QUEUE *q) {
    return (q->front == NULL);
}

void enQueue(struct USYNCED_QUEUE *q, int amount, int transactionType, char *transactionReason, char *transactionId) {
    struct USYNCED_NODE *newNode = LOAD_USYNCED_NODE(amount, transactionType, transactionReason, transactionId);
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

void deQueue(struct USYNCED_QUEUE *q) {
    if (isEmpty(q)) {
        printf("Queue Underflow\n");
        return;
    }

    struct USYNCED_NODE* temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    // Free allocated memory for strings
    free(temp->transactionReason);
    free(temp->transactionId);
    free(temp);
}

char* getFront(struct USYNCED_QUEUE* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return NULL;
    }
    return q->front->transactionId;
}

char* getRear(struct USYNCED_QUEUE* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return NULL;
    }
    return q->rear->transactionId;
}

int main() {
    struct USYNCED_QUEUE* q = createQueue();
    int amount = 10;
    int transactionType = 1;
    char transactionId[] = "hellomello";
    char transactionReason[] = "learning queue";

    // Enqueue elements into the queue
    enQueue(q, amount, transactionType, transactionReason, transactionId);
    enQueue(q, amount, transactionType, "test", "test2");

    printf("Queue Front: %s\n", getFront(q));
    printf("Queue Rear: %s\n", getRear(q));

    // Cleanup
    deQueue(q);

    return 0;
}
