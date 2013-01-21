#include <stdio.h>
#include "list.h"

void list_init(list_t *l, 
		int (*compare)(const void *key, const void *with),
		void (*datum_delete)(void *datum)) {
}

void list_visit_items(list_t *l, void (*visitor)(void *v)) {
}

void list_insert_tail(list_t *l, void *v) {
}

void list_insert_sorted(list_t *l, void *v) {
}

void list_remove_head(list_t *l) {
}

int char_compar(const void *key, const void *with) {
	char *pleft = (char) key;
	char *pright = (char) with;

	if (atoi(*pleft) < atoi(*pright) {
		return -1;
	}
	else if (atoi(*pleft) == atoi(*pright)) {
		return 0;
	}
	else {
		return 1;
	}
}

int main() {
	return 0;
}


