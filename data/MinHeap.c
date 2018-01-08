#include <stdlib.h>
#include "MinHeap.h"


MinHeap HEAP_init() {
	MinHeap heap;

	heap.length = 0;
	heap.nodes = malloc(sizeof(Node));

	return heap;
}

int HEAP_length(MinHeap heap) {
	return heap.length;
}

void HEAP_print(MinHeap heap) {
	int j;
	char aux[LENGTH];

	if (!heap.length) return;

	for (j = 0; j < heap.length - 1; j++) {
		sprintf(aux, "[%s-%s-%d-%d-%d]-", heap.nodes[j].e.name, heap.nodes[j].e.ip, heap.nodes[j].e.port,
				heap.nodes[j].e.users, heap.nodes[j].e.time);
		debug(aux);
	}
	if (heap.length) {
		sprintf(aux, "[%s-%s-%d-%d-%d]\n", heap.nodes[j].e.name, heap.nodes[j].e.ip, heap.nodes[j].e.port,
				heap.nodes[j].e.users, heap.nodes[j].e.time);
		debug(aux);
	} else {
		debug("[Empty Heap]\n");
	}
}

void pushDown(MinHeap *heap, int pos) {
	int next = pos * 2 + 1;

	while (pos * 2 + 1 < heap->length) {
		if (heap->length > pos * 2 + 2)
			next += heap->nodes[pos * 2 + 1].value > heap->nodes[pos * 2 + 2].value;

		if (heap->nodes[next].value < heap->nodes[pos].value) {
			Node aux = heap->nodes[pos];
			heap->nodes[pos] = heap->nodes[next];
			heap->nodes[next] = aux;
		} else break;
		pos = next;
		next = pos * 2 + 1;
	}
}

Enterprise HEAP_pop(MinHeap *heap) {
	Node node;

	if (!heap->length) {
		node.e.port = -1;
		return node.e;
	}

	node = heap->nodes[0];
	heap->length--;

	heap->nodes[0] = heap->nodes[heap->length];

	pushDown(heap, 0);

	heap->nodes = (Node *) realloc(heap->nodes, sizeof(Node) * (heap->length));

	return node.e;
}

Enterprise HEAP_consulta(MinHeap *heap) {
	Enterprise e;
	double diff;

	if (!heap->length) {
		e.port = -1;
		e.name = e.ip = NULL;
		e.users = -1;
		e.time = -1;
		return e;
	}

	diff = difftime(time(NULL), heap->nodes[0].e.last);
	if (diff > heap->nodes[0].e.time + OFFSET_TIME) {
		HEAP_pop(heap);
		return HEAP_consulta(heap);
	}

	return heap->nodes[0].e;
}

void HEAP_push(MinHeap *heap, Enterprise e) {
	Node node;

	int act = (heap->length - 1) / 2;
	int last = heap->length;

	e.last = time(NULL);
	node.e = e;
	node.value = e.users;

	heap->nodes = realloc(heap->nodes, sizeof(Node) * (heap->length + 1));
	heap->nodes[last] = node;
	while (node.value < heap->nodes[act].value && act != last) {
		heap->nodes[last] = heap->nodes[act];
		heap->nodes[act] = node;
		last = act;
		act = (act - 1) / 2;
	}
	heap->length++;
}

char HEAP_remove(MinHeap *heap, int index, int ffree) {

	if (index < 0 || index >= heap->length) {
		return 0;
	}

	heap->length--;
	if (ffree) {
		free(heap->nodes[index].e.ip);
		free(heap->nodes[index].e.name);
	}

	if (heap->length > 0) {
		heap->nodes[index] = heap->nodes[heap->length];

		pushDown(heap, index);

		heap->nodes = realloc(heap->nodes, sizeof(Node) * (heap->length));
	} else {
		free(heap->nodes);
		heap->nodes = malloc(sizeof(Node));
	}

	return 1;
}

Enterprise HEAP_update(MinHeap *heap, Enterprise ent) {
	int i = 0;
	Enterprise e;

	e.ip = e.name = NULL;
	e.users = e.port = -1;

	for (; i < heap->length; i++) {
		if (heap->nodes[i].e.port == ent.port && !strcmp(heap->nodes[i].e.ip, ent.ip)) {
			heap->nodes[i].e.last = time(NULL);
			e = heap->nodes[i].e;
			if (e.users != ent.users) {
				HEAP_remove(heap, i, NO_FREE);
				e.users = ent.users;
				HEAP_push(heap, e);
			}
			break;
		}
	}
	return e;
}

int HEAP_find(MinHeap heap, Enterprise e) {
	int i = 0;

	for (; i < heap.length; i++)
		if (heap.nodes[i].e.port == e.port && !strcmp(heap.nodes[i].e.ip, e.ip))
			return i;

	return -1;
}

char HEAP_disconnect(MinHeap *heap, Enterprise e) {
	int i = 0;

	for (; i < heap->length; i++) {
		if (heap->nodes[i].e.port == e.port && !strcmp(heap->nodes[i].e.ip, e.ip)) {
			HEAP_remove(heap, i, FREE);
			return 1;
		}
	}
	return 0;
}

void HEAP_close(MinHeap *heap) {
	int i = 0;

	for (; i < heap->length; i++) {
		free(heap->nodes[i].e.ip);
		free(heap->nodes[i].e.name);
	}
	free(heap->nodes);
}
