
#include "el_list.h"

el_list* el_create_list(void){
    el_list *this;
    int i;
    
    this = (el_list*)malloc(sizeof(el_list));

    this->head = NULL;
    this->tail = NULL;
    this->length = 0;
    this->loop = -1;

    for(i=0;i<EL_LIST_LOOP_NEST;i++){
        this->loop_node[i] = NULL;
        this->loop_next[i] = NULL;
    }
    
    return this;
}

void el_delete_list(el_list*this){
    el_list_node *node;

    node = this->head;
    while(node!=NULL){
        this->head = node->next;
        free(node);
        node = this->head;
    }
    
    free(this);
}

void el_list_append_item(el_list*this,void*p){
    el_list_node *node;
    
    node = (el_list_node*)malloc(sizeof(el_list_node));
    
    node->next = NULL;
    node->payload = p;

    if(this->head==NULL){
        this->head = node;
        this->tail = node;
    }else{
        this->tail->next = node;
        this->tail = node;
    }
    
    this->length++;
}

void el_list_loop_begin(el_list*this){
    this->loop++;
    this->loop_node[this->loop] = NULL;
    this->loop_next[this->loop] = this->head;
    this->loop_prev = NULL;
}

void el_list_loop_end(el_list*this){
    this->loop--;
}

void el_list_loop_step(el_list*this){
    int l = this->loop;
    if(l==0){
        this->loop_prev = this->loop_node[l];
    }
    this->loop_node[l] = this->loop_next[l];
    this->loop_next[l] = this->loop_node[l]->next;
}

int el_list_loop_has_next(el_list*this){
    int l = this->loop;
    return (this->loop_next[l] != NULL);
}

void* el_list_loop_get_item(el_list*this){
    int l = this->loop;
    return this->loop_node[l]->payload;
}

void el_list_loop_remove_item(el_list*this){
    int l = this->loop;
    el_list_node *prev = this->loop_prev;
    el_list_node *node = this->loop_node[l];
    
    if(prev==NULL){
        
        this->head = node->next;
        
    }else{
        
        prev->next = node->next;
        
        if(node==this->tail){
            this->tail = this->loop_prev;
        }
        
    }
    
    free(node);
    
    this->length--;
}

unsigned int el_list_get_length(el_list*this){
    return this->length;
}






#ifdef EL_LIST_DEBUG

#include <stdio.h>

int main(int argc,char*argv[]){
    el_list *list;
    int var[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int *p,*q;
    
    list = el_create_list();
    
    el_list_append_item(list,var + 0);
    el_list_append_item(list,var + 1);
    el_list_append_item(list,var + 2);
    el_list_append_item(list,var + 3);
    el_list_append_item(list,var + 4);
    el_list_append_item(list,var + 5);
    el_list_append_item(list,var + 6);
    el_list_append_item(list,var + 7);
    el_list_append_item(list,var + 8);

    printf("length: %d\n",el_list_get_length(list));
    
    el_list_loop_begin(list);
    while(el_list_loop_has_next(list)){
        el_list_loop_step(list);
        
        p = (int*)el_list_loop_get_item(list);
        
        printf("%d\n",*p);
    }
    el_list_loop_end(list);
    
    printf("nested loop:\n");
    
    el_list_loop_begin(list);
    while(el_list_loop_has_next(list)){
        el_list_loop_step(list);
        
        p = (int*)el_list_loop_get_item(list);
            
        el_list_loop_begin(list);
        while(el_list_loop_has_next(list)){
            el_list_loop_step(list);
            
            q = (int*)el_list_loop_get_item(list);
            
            printf("%d\t",(*p)*(*q));
            
        }
        el_list_loop_end(list);
        
        printf("\n");
        
    }
    el_list_loop_end(list);
    
    printf("remove items:\n");
    
    el_list_loop_begin(list);
    while(el_list_loop_has_next(list)){
        el_list_loop_step(list);
        
        p = (int*)el_list_loop_get_item(list);
        
        if((*p)%2 != 0){
            el_list_loop_remove_item(list);
        }
        
    }
    el_list_loop_end(list);
    
    el_list_loop_begin(list);
    while(el_list_loop_has_next(list)){
        el_list_loop_step(list);
        
        p = (int*)el_list_loop_get_item(list);
        
        printf("%d\n",*p);
        
    }
    el_list_loop_end(list);
    
    printf("\n");
    
    printf("length: %d\n",el_list_get_length(list));
        
    el_delete_list(list);
    
    system("pause");
    
    return 0;
}

#endif
