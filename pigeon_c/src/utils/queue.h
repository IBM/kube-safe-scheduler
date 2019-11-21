/*
 * queue.h
 *
 *  Created on: Jan 30, 2018
 *      Author: tantawi
 */

#ifndef UTILS_QUEUE_H_
#define UTILS_QUEUE_H_

#include <stdbool.h>

struct s_queue;

/**
 * A queue data structure
 */
typedef struct s_queue queue;

/**
 * create a queue
 */
extern queue* queue_create();

/**
 * destroy the queue
 */
extern void queue_destroy(queue* _self);

/**
 * add an element to the rear of the queue
 */
extern void queue_add(queue* _self, void* _element);

/**
 * remove an element from the front of the queue (null if empty)
 */
extern void* queue_remove(queue* _self);

/**
 * inspect the element at the front of the queue (null if empty)
 */
extern void* queue_peek(queue* _self);

/**
 * the number of elements in the queue
 */
extern int queue_size(queue* _self);

/**
 * is the queue empty
 */
extern bool queue_isempty(queue* _self);

/**
 * toString()
 * provide function for printing an element object
 */
extern void queue_print(queue* _self, void (*element_print)(void *));


#endif /* UTILS_QUEUE_H_ */
