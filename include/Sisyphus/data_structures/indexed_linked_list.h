#pragma once

#include <stddef.h>
#include <stdlib.h>

struct SSPIndexedLinkedList {
    int *prev;
    int *next;
    int remain;
};

void ssp_init_indexed_linked_list(struct SSPIndexedLinkedList *linked_list, int count);
void ssp_remove_indexed_linked_list(struct SSPIndexedLinkedList *linked_list, int n);
void ssp_free_indexed_linked_list(struct SSPIndexedLinkedList *linked_list);
