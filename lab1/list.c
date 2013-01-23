#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void list_init(list_t *l, 
		int (*compare)(const void *key, const void *with),
		void (*datum_delete)(void *datum)) {
}

void list_visit_items(list_t *l, void (*visitor)(void *v)) {
}

void list_insert_tail(list_t *l, void *v) {
	// create node
	list_item_t* tail = (list_item_t*) malloc(sizeof(list_item_t));
	tail->datum = v;

	// grab existing tail, set up pointers
	list_item_t* node = l->tail;
	if (node != NULL) {
		node->next = tail;
		tail->next = NULL;
		tail->pred = node;
		l->tail = tail;
	}
	else {
		l->head = tail;
		l->tail = tail;
		tail->next = NULL;
		tail->pred = NULL;
	}
}

void list_insert_sorted(list_t *l, void *v) {
}

void list_remove_head(list_t *l) {
}