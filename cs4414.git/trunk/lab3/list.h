#ifndef LIST_H
#define LIST_H

typedef struct list_item {
    struct list_item *pred, *next;
    void *datum;
} list_item_t;

typedef struct list {
    list_item_t *head, *tail;
    unsigned length;
    int (*compare)(const void *key, const void *with);
    void (*datum_delete)(void *);
} list_t;

/* pass a list, compare function with two list_item_t, delete function with list_item_t */
void list_init(list_t *l,
               int (*compare)(const void *key, const void *with),
               void (*datum_delete)(void *datum));

/* pass a list, visitor function with list_item_t */
void list_visit_items(list_t *l, void (*visitor)(void *v));

/* pass a list, list_item_t */
void list_insert_tail(list_t *l, void *v);

void list_insert_sorted(list_t *l, void *v);

void list_remove_head(list_t *l);

#endif
