#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void list_init(list_t *l,
               int (*compare)(const void *key, const void *with),
               void (*datum_delete)(void *datum)) {
    l->head = NULL;
    l->tail = NULL;
    l->length = 0;
    l->compare = compare;
    l->datum_delete = datum_delete;
}

void list_visit_items(list_t *l, void (*visitor)(void *v)) {
    list_item_t *node = l->head;
    while (node->next != NULL){
        visitor((void*) node);
        node = node->next;
    }
    visitor((void*) node);
}

void list_insert_tail(list_t *l, void *v) {
    // create node
    list_item_t* node = (list_item_t*) malloc(sizeof(list_item_t));
    node->datum = v;
    l->length++;

    // grab existing tail, set up pointers
    list_item_t* tail = l->tail;

    if (tail != NULL) {
        tail->next = node;
        node->next = NULL;
        node->pred = tail;
        l->tail = node;
    }
    else {
        l->head = node;
        l->tail = node;
        node->next = NULL;
        node->pred = NULL;
    }
}

void list_insert_sorted(list_t *l, void *v) {
    /* 4 cases for insert:
       1. insert into empty list
       2. insert minimal element
       3. insert maximal element
       4. insert in middle of list
    */

    // create node
    list_item_t* node = (list_item_t*) malloc(sizeof(list_item_t));
    node->datum = v;
    l->length++;

    // grab existing head, set up pointers
    list_item_t* head = l->head;
    if (head != NULL) {
        // case 2
        if (l->compare(node, head) <= 0) {
            l->head = node;
            node->next = head;
            node->pred = NULL;
            head->pred = node;
        }
        else {
            while (head->next != NULL && l->compare(node, head) > 0) {
                head = head->next;
            }
            // case 3
            if (head->next == NULL && l->compare(node, head) > 0) {
                l->tail->next = node;
                node->next = NULL;
                node->pred = l->tail;
                l->tail = node;
            }
            // case 4
            else if (l->compare(node, head) <= 0) {
                node->pred = head->pred;
                head->pred->next = node;
                head->pred = node;
                node->next = head;
            }
        }
    }
    // case 1
    else {
        l->head = node;
        l->tail = node;
        node->next = NULL;
        node->pred = NULL;
    }
}

void list_remove_head(list_t *l) {
    list_item_t* head = (list_item_t*) l->head;
    list_item_t* nextHead = head->next;
    l->head = nextHead;
    l->datum_delete(head);
    l->length = l->length - 1;
}
