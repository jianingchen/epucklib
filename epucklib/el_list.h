
#ifndef EL_LIST_H
#define EL_LIST_H

#include <stdlib.h>

#define EL_LIST_LOOP_NEST   3

typedef struct EL_LIST_NODE{
    struct EL_LIST_NODE *next;
    void* payload;
} el_list_node;

typedef struct EL_LIST{
    el_list_node *head;
    el_list_node *tail;
    unsigned int length;
    el_list_node *loop_prev;
    el_list_node *loop_node[EL_LIST_LOOP_NEST];
    el_list_node *loop_next[EL_LIST_LOOP_NEST];
    int loop;
} el_list;

el_list* el_create_list(void);
void el_delete_list(el_list*list);
void el_list_append_item(el_list*list,void*p);
void el_list_loop_begin(el_list*list);
void el_list_loop_end(el_list*list);
void el_list_loop_step(el_list*list);
int el_list_loop_has_next(el_list*list);
void* el_list_loop_get_item(el_list*list);
void el_list_loop_remove_item(el_list*list);// only in out-most loop
unsigned int el_list_get_length(el_list*list);

#endif
