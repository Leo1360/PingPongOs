#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "ppos_data.h"
#include "ppos.h"


// #define DEBUG

#define STACKSIZE 64*1024 // Definindo tamanho da pilha
task_t mainTask;
int taskId = 1;
task_t *currentTask = &mainTask;

void ppos_init (){
    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;
    //adicionar descrito da tarefa mainTask
    mainTask.id = 0;
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
    
    task_switch(&mainTask);
}

int task_id (){
    return currentTask->id;
}