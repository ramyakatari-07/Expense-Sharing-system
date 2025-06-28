#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100

// User structure
typedef struct {
    char name[50];
    float balance;
} User;

// Node for linked list of transactions
typedef struct TransactionNode {
    char paid_by[50];
    char description[100];
    float amount;
    struct TransactionNode* next;
} TransactionNode;

// Heap for debt management
typedef struct {
    User* users[MAX_USERS];
    int size;
} MinHeap;


User users[MAX_USERS];
int user_count = 0;

void addUser(char* name) {
    strcpy(users[user_count].name, name);
    users[user_count].balance = 0.0;
    user_count++;
}

TransactionNode* head = NULL;

void addTransaction(char* paid_by, char* description, float amount) {
    TransactionNode* newTransaction = (TransactionNode*)malloc(sizeof(TransactionNode));
    strcpy(newTransaction->paid_by, paid_by);
    strcpy(newTransaction->description, description);
    newTransaction->amount = amount;
    newTransaction->next = head;
    head = newTransaction;
}

// Function to swap two users in the heap
void swap(User** a, User** b) {
    User* temp = *a;
    *a = *b;
    *b = temp;
}

// MinHeapify function to maintain heap property
void minHeapify(MinHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->users[left]->balance < heap->users[smallest]->balance) {
        smallest = left;
    }
    if (right < heap->size && heap->users[right]->balance < heap->users[smallest]->balance) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->users[smallest], &heap->users[index]);
        minHeapify(heap, smallest);
    }
}

// Initialize the heap
void initHeap(MinHeap* heap) {
    heap->size = 0;
}

// Insert a user into the heap
void insertHeap(MinHeap* heap, User* user) {
    heap->users[heap->size] = user;
    int i = heap->size;
    heap->size++;

    // Move up to maintain the heap property
    while (i != 0 && heap->users[(i - 1) / 2]->balance > heap->users[i]->balance) {
        swap(&heap->users[i], &heap->users[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Extract the minimum (most negative balance) user from the heap
User* extractMin(MinHeap* heap) {
    if (heap->size <= 0) return NULL;
    if (heap->size == 1) {
        heap->size--;
        return heap->users[0];
    }

    User* root = heap->users[0];
    heap->users[0] = heap->users[heap->size - 1];
    heap->size--;

    minHeapify(heap, 0);

    return root;
}

// Settle debts among users
void settleDebts() {
    MinHeap heap;
    initHeap(&heap);

    // Insert all users with non-zero balance into the heap
    for (int i = 0; i < user_count; i++) {
        if (users[i].balance != 0) {
            insertHeap(&heap, &users[i]);
        }
    }

    // Settle debts using the heap
    while (heap.size > 1) {
        User* debtor = extractMin(&heap);  // The one who owes the most
        User* creditor = extractMin(&heap); // The one who is owed

        float settlement = (debtor->balance < -creditor->balance) ? debtor->balance : -creditor->balance;

        debtor->balance -= settlement;
        creditor->balance += settlement;

        printf("%s pays %s: %.2f\n", debtor->name, creditor->name, -settlement);

        // Reinsert users into the heap if they still have outstanding balances
        if (debtor->balance != 0) insertHeap(&heap, debtor);
        if (creditor->balance != 0) insertHeap(&heap, creditor);
    }
}

int main() {
    int choice;
    char name[50];
    char description[100];
    float amount;
    int num_users, i;

    printf("Welcome to Expense Sharing System\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Add User\n");
        printf("2. Add Expense\n");
        printf("3. Show Balances\n");
        printf("4. Settle Debts\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        

        switch (choice) {
            case 1:
                printf("Enter the name of the user: ");
                scanf("%s", name);
                addUser(name);
                printf("User %s added successfully!\n", name);
                break;

            case 2:
                printf("Enter who paid: ");
                scanf("%s", name);

                printf("Enter the number of users sharing the expense: ");
                scanf("%d", &num_users);
                
                char** user_list = (char**)malloc(num_users * sizeof(char*));
                for (i = 0; i < num_users; i++) {
                user_list[i] = (char*)malloc(50 * sizeof(char));
                }
                printf("Enter the names of users sharing the expense:\n");
                for (i = 0; i < num_users; i++) {
                    printf("User %d: ", i + 1);
                    scanf("%s", user_list[i]);
                }

                printf("Enter the description of the expense: ");
                scanf(" %[^\n]%*c", description);  // Read the entire line for description
                printf("Enter the amount: ");
                scanf("%f", &amount);

                // Split the amount equally
                float split_amount = amount / num_users;
                for (i = 0; i < num_users; i++) {
                    for (int j = 0; j < user_count; j++) {
                        if (strcmp(users[j].name, user_list[i]) == 0) {
                            users[j].balance -= split_amount;
                        }
                    }
                }
                for (int j = 0; j < user_count; j++) {
                    if (strcmp(users[j].name, name) == 0) {
                        users[j].balance += amount;
                    }
                }

                // Log the transaction
                addTransaction(name, description, amount);
                printf("Expense added successfully!\n");
                break;

            case 3:
                printf("\nBalances:\n");
                for (i = 0; i < user_count; i++) {
                    printf("%s: %.2f\n", users[i].name, users[i].balance);
                }
                break;

            case 4:
                printf("Settling debts...\n");
                settleDebts();
                break;

            case 5:
                printf("Exiting...\n");
                exit(0);
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
