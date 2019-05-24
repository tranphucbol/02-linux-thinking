#include "queue.h"

struct QNode *newNode(int k)
{
    struct QNode *temp = (struct QNode *)malloc(sizeof(struct QNode));
    temp->key = k;
    temp->next = NULL;
    return temp;
}

struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

void enQueue(struct Queue *q, int k)
{

    struct QNode *temp = newNode(k);

    pthread_mutex_lock(&q->lock);
    
    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        pthread_mutex_unlock(&q->lock);
        return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
    pthread_mutex_unlock(&q->lock);
}

struct QNode *deQueue(struct Queue *q)
{
    pthread_mutex_lock(&q->lock);

    if (q->front == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    struct QNode *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;
    pthread_mutex_unlock(&q->lock);
    return temp;
}

void clearQueue(struct Queue *q)
{
    while (q->front != NULL)
    {
        free(deQueue(q));
    }
}