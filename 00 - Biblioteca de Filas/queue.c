#include <stdio.h>
#include "queue.h"

int queue_size (queue_t *queue){
    if(queue == NULL){
        return 0;
    }

    queue_t *current = queue->next;
    int size = 1;

    while(current != queue){
        current = current->next;
        size++;
    }
    return size;
}

int queue_append (queue_t **queue, queue_t *elem){
    if(queue == NULL || elem->next != NULL || elem == NULL){
        printf("###Erro: tentou adicionar elemento em fila vazia ou tentou adiiconar elemente que ja existe em outra fila");
        return -1;
    }

    if(*queue == NULL){
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
    }else{
        queue_t *ultimo = (*queue)->prev;
        ultimo->next = elem;
        elem->prev = ultimo;

        (*queue)->prev = elem;
        elem->next = (*queue);
    }


    return 0;
}

int queue_remove (queue_t **queue, queue_t *elem){
    if(queue == NULL || elem == NULL){
        printf("### Erro: fila e/ou elemento não existem");
        return -1;
    }

    queue_t *current = *queue;
    while(current != (*queue)->prev && current != elem){ // walking the list until find the begining of the list or the element
        current = current->next;
    }

    if(current != elem){ // didnt found the element 
        printf("### Erro: Tentou remover um item que nao esta na fila\n");
        return -1;
    }
    //elemnt found, excluding element
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    
    if(*queue == elem){ // element is the first on the list, excluding and changing first pointer
        if((*queue)->next != *queue){ // if last element removed, the fist pointer is nulled
            *queue = (*queue)->next;
        }else{
            *queue = NULL;
        }
    }

    elem->next = NULL;
    elem->prev = NULL;
    return 0;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ){
    printf("%s :",name);
    if(queue == NULL){
        printf("[]\n");
        return;
    }
    queue_t *current = queue;
    printf("[");
    do{
        print_elem(current);
        current = current->next;
        printf(" ");
    }while(current != queue);
    printf("]\n");
}


// int queue_remove_BKP (queue_t **queue, queue_t *elem){
//     if(queue == NULL || elem == NULL){
//         printf("### Erro: fila e/ou elemento não existem");
//         return -1;
//     }

//     if(*queue == elem){ // element is the firsr on the list, excluding and changing first pointer
//         elem->prev->next = elem->next;
//         elem->next->prev = elem->prev;
//         if((*queue)->next == *queue){ // if last element removed, the fist pointer is nulled
//             *queue = NULL;
//         }else{
//             *queue = (*queue)->next;

//         }
//         elem->next = NULL;
//         elem->prev = NULL;
//         return 0;
//     }

//     queue_t *current = (*queue)->next;
//     while(current != *queue && current != elem){ // walking the list until find the begining of the list or the element
//         current = current->next;
//     }

//     if(current == *queue){ // didnt found the element 
//         printf("### Erro: Tentou remover um item que nao esta na fila\n");
//         return -1;
//     }
//     //elemnt found, excluding element
//     elem->prev->next = elem->next;
//     elem->next->prev = elem->prev;
//     elem->next = NULL;
//     elem->prev = NULL;

// }