/*
 * queue.c
 *
 *  Created on: Jan 30, 2018
 *      Author: tantawi
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "queue.h"

/**
 * An element node in the queue
 */
typedef struct s_qnode qnode;

struct s_qnode {
	/** pointer to the element object */
	void* element;
	/** next node in the queue */
	qnode* next;
};

/**
 * A queue data structure
 */
struct s_queue {
	/** pointer to the front node in the queue */
	qnode* front;
	/** pointer to the rear node in the queue */
	qnode* rear;
	/** number of elements in the queue (for convenience) */
	int size;
};

qnode* qnode_create(void* _element) {
	qnode* nd = (qnode*) malloc (sizeof(qnode));
	assert(nd);
	nd->element = _element;
	nd->next = NULL;
	return nd;
}

void qnode_destroy(qnode* _self) {
	if (_self) {
		free(_self);
	}
}

queue* queue_create() {
	queue* q = (queue*) malloc(sizeof(queue));
	assert(q);
	q->front = NULL;
	q->rear = NULL;
	q->size = 0;
	return q;
}

void queue_destroy(queue* _self) {
	if (_self) {
		while (_self->front) {
			qnode* nd = _self->front;
			_self->front = nd->next;
			qnode_destroy(nd);
		}
		free(_self);
	}
}

void queue_add(queue* _self, void* _element) {
	qnode* nd = qnode_create(_element);
	_self->size++;

	if (!_self->rear) {
		_self->rear = nd;
		_self->front = nd;
		return;
	}

	_self->rear->next = nd;
	_self->rear = nd;
}

void* queue_remove(queue* _self) {
	if (!_self->front) {
		return NULL;
	}

	_self->size--;
	qnode* nd = _self->front;
	void* e = nd->element;

	_self->front = nd->next;
	if (!_self->front) {
		_self->rear = NULL;
	}

	qnode_destroy(nd);
	return e;
}

void* queue_peek(queue* _self) {
	if (!_self->front) {
		return NULL;
	}
	return _self->front->element;
}

int queue_size(queue* _self) {
	return _self->size;
}

bool queue_isempty(queue* _self) {
	return _self->size == 0;
}

void queue_print(queue* _self, void (*element_print)(void *)) {
	if (_self == NULL || (*element_print) == NULL) {
		return;
	}
	printf("{ ");
	qnode* ptr = _self->front;
	while (ptr) {
		void* e = ptr->element;
		(*element_print)(e);
		printf(" ");
		ptr = ptr->next;
	}
	printf("}");
}
