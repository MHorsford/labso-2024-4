#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRIORITY 10

typedef struct Process {
    char name[10];
    int remaining_time;
    int priority;
} Process;

typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;
    Node *rear;
} Queue;

void init_queue(Queue *q) {
    q->front = q->rear = NULL;
}

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

int find_highest_priority(Queue *queues) {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        if (!is_empty(&queues[i])) {
            return i;
        }
    }
    return -1; 
}

int main() {
    Queue priority_queues[MAX_PRIORITY + 1];
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        init_queue(&priority_queues[i]);
    }

    int time_slice = 10;

    Process p1 = {"P1", 200, 1};
    Process p2 = {"P2", 300, 0};
    Process p3 = {"P3", 25, 0};

    enqueue(&priority_queues[p1.priority], &p1);
    enqueue(&priority_queues[p2.priority], &p2);
    enqueue(&priority_queues[p3.priority], &p3);

    int active = 1;
    while (active) {
        active = 0;
        char output[1024] = "CPU: ";
        int output_len = 5;

        for (int prio = 0; prio <= MAX_PRIORITY; prio++) {
            Queue *current_queue = &priority_queues[prio];
            while (!is_empty(current_queue)) {
                Process *p = dequeue(current_queue);
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
                    enqueue(current_queue, p);
                }

                active = 1; 
                break;
            }
            if (active) break;
        }

        if (output_len > 5) {
            output[output_len - 1] = '\0';
            printf("%s\n", output);
        }

        active = (find_highest_priority(priority_queues) != -1);
    }

    return 0;
}