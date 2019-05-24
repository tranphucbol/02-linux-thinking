#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct QNode 
{ 
    int key; 
    struct QNode *next; 
}; 

struct Queue 
{ 
    struct QNode *front, *rear;
    pthread_mutex_t lock;
}; 

struct QNode* newNode(int k);
struct Queue *createQueue();
void enQueue(struct Queue *q, int k);
struct QNode *deQueue(struct Queue *q);
void clearQueue(struct Queue *q);

#endif //QUEUE_H
