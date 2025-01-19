#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "ppos_data.h"
#include "ppos.h"
#include "queue.c"


// #define DEBUG
#define PRINT_QUEUE

task_t mainTask;
int taskId = 1;
task_t *currentTask = &mainTask;
task_t dispatcherTask;
task_t *readyList;


void dispatcher(void *arg);
task_t* scheduler();
void printTask(void *elem);

void ppos_init (){
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;
    //adicionar descrito da tarefa mainTask
    mainTask.id = 0;
    mainTask.status = TASK_STATUS_READY;

    //queue_append((queue_t**) &readyList,(queue_t*) &mainTask);

    task_init(&dispatcherTask, dispatcher, NULL);

}

int task_init (task_t *task, void (*start_routine)(void *),  void *arg) {
    #ifdef DEBUG
    printf("PPOS > task_init: criando task com id= %d \n", taskId);
    #endif

    char *stack;
    getcontext(&task->context); //Criando contexto e salvando em task.context

    stack = malloc(STACKSIZE); //allocando stack para a task
    if(stack){
        task->context.uc_stack.ss_sp = stack; //Colocando poneiro para a stack no contexto
        task->context.uc_stack.ss_size = STACKSIZE; //Salvando tamanho da stack no contexto
        task->context.uc_stack.ss_flags = 0; //Zerando flag da stack no contexto
        task->context.uc_link = 0; //Contexto n aponta para outro contexto de retorno
    }else{
        perror("[Error] PPOS > task_init: unable to allocate stack for task \n");
        return -1;
    }

    task->id = taskId; //Incrementando taskId e setando id da task
    taskId++;
    makecontext(&task->context, (void*)(start_routine), 1, arg); //Adicionando ponteiro de função no contexto da task

    task->status = TASK_STATUS_READY;

    queue_append((queue_t**) &readyList, (queue_t*)task);

    #ifdef DEBUG
    printf("PPOS > task_init: Task id= %d criada \n", taskId-1);
    #endif
    return task->id;
}

int task_switch (task_t *task){
    if(task == NULL){
        perror("[Error] PPOS > task_switch: task null or uninitialized \n");
        return -1;
    }
    // adicionar mudança de status das tasks aqui 
    #ifdef DEBUG
    printf("PPOS > task_switch: task_id= %d -> task_id= %d \n", currentTask->id, task->id);
    #endif
    ucontext_t *oldContext = &currentTask->context; // pegando contexto da task atual
    currentTask = task; // Apontando current task para a nova task

    swapcontext(oldContext, &task->context); // realizando mudanca de contexto
    return 0;
}

void task_exit (int exit_code){
    #ifdef DEBUG
    printf("PPOS > task_exit: task_id= %d \n", currentTask->id);
    #endif
    currentTask->status = TASK_STATUS_DEAD;
    task_switch(&dispatcherTask);
}

int task_id (){
    return currentTask->id;
}

void task_yield (){
    #ifdef DEBUG
    printf("PPOS > task_yield: task_id= %d\n", currentTask->id);
    #endif
    task_switch(&dispatcherTask);
}

void task_setprio (task_t *task, int prio) {
    prio = prio>20 ? 20 : prio;
    prio = prio<-20 ? -20 : prio;

    task->priorConst = prio;
    task->priorDin = prio;

}

int task_getprio (task_t *task) {
    if(task == NULL){
        return currentTask->priorConst;
    }
    return (*task).priorConst;
}


void dispatcher(void *arg){
    queue_remove((queue_t**) &readyList, (queue_t*)&dispatcherTask);
    task_t *nextTask = readyList;
    while(readyList != NULL){
        nextTask = scheduler();
        task_switch(nextTask);
        if(nextTask->status == TASK_STATUS_DEAD){
            free(nextTask->context.uc_stack.ss_sp);
            queue_remove((queue_t**) &readyList, (queue_t*)nextTask);
        }
        
    }
    dispatcherTask.status = TASK_STATUS_DEAD;
    task_switch(&mainTask);
}

task_t* scheduler(){
    task_t *nextTask = readyList;
    task_t *itterTask = readyList;
    #ifdef PRINT_QUEUE
    queue_print("scheduler: ",(queue_t*) (readyList), printTask);
    #endif

    itterTask->priorDin = itterTask->priorDin-1 < -20 ? -20: itterTask->priorDin-1;
    itterTask = itterTask->next;
    while(itterTask != readyList){
        itterTask->priorDin = itterTask->priorDin-1 < -20 ? -20: itterTask->priorDin-1;
        if(nextTask->priorDin > itterTask->priorDin){
            nextTask = itterTask;
        }
        itterTask = itterTask->next;
    }

    nextTask->priorDin = nextTask->priorConst;
    #ifdef PRINT_QUEUE
    printf("scheduler: selected task -> ");
    printTask(nextTask);
    printf("\n");
    #endif

    return nextTask;
}


void printTask(void *elem){
    if (!elem){
      return ;
    }

    task_t *task = elem;
    printf("[id=%d, pd=%d]", task->id, task->priorDin);

}
