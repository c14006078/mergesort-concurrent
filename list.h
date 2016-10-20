#ifndef LLIST_H_
#define LLIST_H_

#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>

typedef intptr_t val_t;

typedef struct node {
    val_t data;
    struct node *next;
} node_t;

typedef struct llist {
    node_t *head;
    uint32_t size;
} llist_t;

/**
 * new node with assignment and let it be the header of this list
 */
node_t *new_node(val_t val, node_t *next);

/**
 * init list without assignmend
 */
llist_t *list_new();

/**
 * list_add inserts a new node with the given value val in the list
 * (if the value was absent) or does nothing (if the value is already present).
 */
int list_add(llist_t *the_list, val_t val);

/**
 * print list node context
 */
void list_print(llist_t *the_list);

/**
 * get the node specify by index
 * if the index is out of range, it will return NULL
 */
node_t *list_get(llist_t *the_list, uint32_t index);

#endif
