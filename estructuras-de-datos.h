#ifndef ESTRUCTURAS-DE-DATOS_H_INCLUDED
#define ESTRUCTURAS-DE-DATOS_H_INCLUDED
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#define MAXTAM 100

typedef struct{
    char **items;
    int top;
    int tamMax;
}Stack;

void initStack(Stack *s, int tamMax);
int isEmpty(Stack *s);
int isFull(Stack *s);
void resizeStack(Stack *s);
void push(Stack *s, char *value);
char* pop(Stack *s);
char* peek(Stack *s);
void freeStack(Stack *s);
void printStack(Stack *s);

typedef struct{
    float *items;
    int top;
    int tamMax;
}Stack_float;

void pushFloat(Stack_float *sf, float value);
float popFloat(Stack_float *sf);


typedef struct {
    char **items;
    int front;
    int rear;
    int size;
    int tamMax;
} Queue;

void initQueue(Queue *q, int tamMax);
void freeQueue(Queue *q);
int isQueueEmpty(Queue *q);
int isQueueFull(Queue *q);
void resizeQueue(Queue *q);
void printQueue(Queue *q);
void enqueue(Queue *q, char *value); //Encolar   -> Meter un elemento al final de la cola
char* dequeue(Queue *q); //Desencolar -> Traer y sacar el primer elemento de la cola
char* peekQueue(Queue *q);   //Traer el primer elemento de la cola


#endif // ESTRUCTURAS-DE-DATOS_H_INCLUDED
