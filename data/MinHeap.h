#ifndef _MINHEAP_H_
#define _MINHEAP_H_


#include <stdio.h>
#include "utils.h"

typedef struct {
	char *name;
	char *ip;
	int port;
	int users;
} Enterprise;

typedef struct {
	Enterprise e;
	int value;
} Node;

typedef struct {
	Node *nodes;
	int length;
} MinHeap;


MinHeap HEAP_init();

void HEAP_print(MinHeap);

int HEAP_length(MinHeap heap);

Enterprise HEAP_pop(MinHeap *);

void HEAP_push(MinHeap *, Enterprise);

char HEAP_remove(MinHeap *, int index);

void HEAP_close(MinHeap *);

char HEAP_update(MinHeap *heap, int port, int users);

char HEAP_disconnect(MinHeap *heap, int port);


#endif
