#include "queue.h"
#include <stdlib.h>
#include <string.h>

struct queue *queue_alloc(const size_t capacity, const size_t item_size) {
    struct queue *new_queue = malloc(sizeof(struct queue));

    if (!new_queue) {
        return NULL;
    }
    if (!queue_init(new_queue, capacity, item_size)) {
        free(new_queue);
        return NULL;
    }
    return new_queue;
}

int queue_init(struct queue *q, const size_t capacity, const size_t item_size) {
    if (!q || capacity == 0 || item_size == 0) {
        return 0;
    }
    
    q->items = malloc(capacity * item_size);
    if (!q->items) {
        return 0;
    }

    q->capacity = capacity;
    q->item_size = item_size;
    q->front = 0;
    q->rear = 0;
    q->count = 0;

    return 1;
}

void queue_deinit(struct queue *q) {
    if (!q) {
        return;
    }

    q->capacity = 0;
    q->item_size = 0;
    q->front = 0;
    q->rear = 0;
    q->count = 0;

    if (q->items) {
        free(q->items);
        q->items = NULL;
    }
}

void queue_dealloc(struct queue **q) {
    if (!q || !*q) {
        return;
    }

    queue_deinit(*q);
    free(*q);
    *q = NULL;
}

int queue_enqueue(struct queue *q, const void *item) {
    if (!q || !item || q->count >= q->capacity) {
        return 0;
    }

    memcpy((char *)q->items + (q->rear * q->item_size), item, q->item_size);

    q->rear = (q->rear + 1) % q->capacity;
    q->count++;

    return 1;
}

int queue_dequeue(struct queue *q, void *item) {
    if (!queue_front(q, item)) {
        return 0;
    }
    
    q->front = (q->front + 1) % q->capacity;
    q->count--;

    return 1;
}

int queue_front(const struct queue *q, void *item) {
    if (!q || queue_item_count(q) == 0 || !item) {
        return 0;
    }

    memcpy(item, (char *)q->items + (q->front * q->item_size), q->item_size);
    
    return 1;
}

size_t queue_item_count(const struct queue *q) {
    return (q) ? q->count : 0;
}