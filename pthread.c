#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


int count = 0;

struct list_node {
    int data;
    struct list_node* next;
};
struct list_node *head = NULL;


void insert(struct list_node** head, int data) {   
    struct list_node* node = (struct list_node*)malloc(sizeof(struct list_node));
    
    node->data = data;
    
    struct list_node* tmp = *head;
    struct list_node* old_node = NULL;
    
    while(tmp != NULL && tmp->data < data) {
        old_node = tmp;
        tmp = tmp->next;
    }

    node->next = tmp;

    if(old_node == NULL) {
        *head = node;
    } else {
        old_node->next = node;
    }

    count++;
}

void *insert_handler(void *pointer) { 
    char input[10];
    fgets(input, 10, stdin);
    int val = atoi(input);
    insert(&head, val);
}

void print(struct list_node* head) {  
    printf("\nThread B: "); 

    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }

    printf("\n\n");
}

void *print_handler(void *pointer) { 
    print(head);
}

int main() {   
    pthread_t thread_a;
    pthread_t thread_b;

    pthread_mutex_t mutex;

    while (count != 10) {
        pthread_mutex_lock(&mutex);
        pthread_create(&thread_a, NULL, insert_handler, NULL);
        pthread_join(thread_a, NULL);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        pthread_create(&thread_b, NULL, print_handler, NULL);
        pthread_join(thread_b, NULL);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
