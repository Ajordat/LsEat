#include <stdlib.h>
#include "MinHeap.h"


MinHeap HEAP_init() {
	MinHeap heap;

	heap.length = 0;
	heap.nodes = malloc(sizeof(Node));

	return heap;
}

void HEAP_print(MinHeap heap) {
	int j;
	if (!heap.length) return;
	for (j = 0; j < heap.length - 1; j++) printf("%d - ", heap.nodes[j].value);
	printf("%d\n", heap.nodes[j].value);
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
		node.value = -1;
		return node.e;
	}

	node = heap->nodes[0];
	heap->length--;

	heap->nodes[0] = heap->nodes[heap->length];

	pushDown(heap, 0);

	heap->nodes = (Node *) realloc(heap->nodes, sizeof(Node) * (heap->length));

	return node.e;
}

void HEAP_push(MinHeap *heap, Enterprise e) {
	Node node;

	node.e = e;
	node.value = e.users;

	int act = (heap->length - 1) / 2;
	int last = heap->length;

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

char HEAP_remove(MinHeap *heap, int index) {

	if (index < 0 || index >= heap->length) {
		return 0;
	}

	heap->length--;
	heap->nodes[index] = heap->nodes[heap->length];

	pushDown(heap, index);

	heap->nodes = (Node *) realloc(heap->nodes, sizeof(Node) * (heap->length));
	return 1;
}

void HEAP_close(MinHeap *heap) {
	free(heap->nodes);
}
