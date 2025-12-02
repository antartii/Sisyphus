#include "data_structures/indexed_linked_list.h"

#include <stdio.h>

void ssp_init_indexed_linked_list(struct SSPIndexedLinkedList *linked_list, int count)
{
    linked_list->next = malloc(sizeof(int) * count);
    linked_list->prev = malloc(sizeof(int) * count);
    linked_list->remain = count;

    for (int i = 0; i < count; ++i) {
        linked_list->prev[i] = (i + count - 1) % count;;
        linked_list->next[i] = (i + 1) % count;
    }
}

void ssp_remove_indexed_linked_list(struct SSPIndexedLinkedList *linked_list, int n)
{
    if (linked_list->remain <= 0)
        return;

    int prev = linked_list->prev[n];
    int next = linked_list->next[n];

    linked_list->prev[next] = prev;
    linked_list->next[prev] = next;
    --linked_list->remain;
}

void ssp_free_indexed_linked_list(struct SSPIndexedLinkedList *linked_list)
{
    free(linked_list->next);
    free(linked_list->prev);

    linked_list->remain = 0;
}
