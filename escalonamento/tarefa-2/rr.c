#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Process {
    char name[10];
    int remaining_time;
} Process;

typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;
    Node *rear;
} Queue;

void enqueue(Queue *q, Process *p) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->process = p;
    new_node->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

Process *dequeue(Queue *q) {
    if (q->front == NULL) return NULL;

    Node *temp = q->front;
    Process *p = temp->process;
    q->front = q->front->next;

    if (q->front == NULL) q->rear = NULL;

    free(temp);
    return p;
}

int is_empty(Queue *q) {
    return q->front == NULL;
}

int main() {
    Queue current = {NULL, NULL};
    Queue next = {NULL, NULL};
    int time_slice = 10;

    Process p1 = {"P1", 200};
    Process p2 = {"P2", 300};
    Process p3 = {"P3", 25};

    enqueue(&current, &p1);
    enqueue(&current, &p2);
    enqueue(&current, &p3);

    do {
        char output[1024] = "CPU: ";
        int output_len = 5;

        while (!is_empty(&current)) {
            Process *p = dequeue(&current);
            int executed = (p->remaining_time > time_slice) ? time_slice : p->remaining_time;
            p->remaining_time -= executed;

            char entry[50];
            if (p->remaining_time == 0) {
                sprintf(entry, "%s, %d*; ", p->name, executed);
            } else {
                sprintf(entry, "%s, %d; ", p->name, executed);
            }

            if (output_len + strlen(entry) < sizeof(output)) {
                strcat(output + output_len, entry);
                output_len += strlen(entry);
            }
            
            if (p->remaining_time > 0) {
                enqueue(&next, p);
            }
        }

        if (output_len > 5) {
            output[output_len - 1] = '\0'; 
        }

        printf("%s\n", output);

        Queue temp = current;
        current = next;
        next = temp;

        next.front = NULL;
        next.rear = NULL;

    } while (!is_empty(&current));

    return 0;
}