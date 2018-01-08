#ifndef _MINHEAP_H_
#define _MINHEAP_H_


#include <stdio.h>
#include <time.h>
#include "utils.h"

#define NO_FREE 		0
#define FREE			1
#define OFFSET_TIME		2

typedef struct {
	char *name;
	char *ip;
	int port;
	int users;
	int time;
	time_t last;
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

Enterprise HEAP_consulta(MinHeap *);

void HEAP_push(MinHeap *, Enterprise);

char HEAP_remove(MinHeap *, int index, int ffree);

void HEAP_close(MinHeap *);

Enterprise HEAP_update(MinHeap *heap, Enterprise);

int HEAP_find(MinHeap heap, Enterprise);

char HEAP_disconnect(MinHeap *heap, Enterprise);


#endif
