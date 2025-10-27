#include "estructuras-de-datos.h"

/*============================================= STACK DE STRING (PILA) ===============================================*/
// Inicialización
void initStack(Stack *s, int tamMax) {
    s->items = malloc(tamMax * sizeof(char*));
    if (!s->items) {
        perror("Error al reservar memoria para pila");
        exit(1);
    }
    s->top = -1;
    s->tamMax = tamMax;
}

int isEmpty(Stack *s) {
    return s->top == -1;
}

int isFull(Stack *s) {
    return s->top + 1 == s->tamMax;
}

void resizeStack(Stack *s) {
    s->tamMax *= 2;
    s->items = realloc(s->items, s->tamMax * sizeof(char*));
    if (!s->items) {
        perror("Error al redimensionar pila");
        exit(1);
    }
}

void push(Stack *s, char *value) {
    if (isFull(s)) resizeStack(s);
    s->items[++s->top] = value;
}

char* pop(Stack *s) {
    if (isEmpty(s)) {
        printf("Error: la pila está vacía.\n");
        exit(1);
    }
    return s->items[s->top--];
}

char* peek(Stack *s) {
    if (isEmpty(s)) {
        printf("Error: la pila está vacía.\n");
        exit(1);
    }
    return s->items[s->top];
}

void freeStack(Stack *s) {
    free(s->items);
}

void printStack(Stack *s) {
    for (int i = 0; i <= s->top; i++) {
        printf("%s ", s->items[i]);
    }
    printf("\n");
}


/*=============================================== STACK FLOAT (PILA) =======================================================================================*/


void pushFloat(Stack_float *sf, float value){
    if(sf->top + 1 == sf->tamMax){
        int newMaxTam = sf->tamMax * 2;
        float *itemsTemp = realloc( sf->items, newMaxTam * sizeof(float) );
        if(!itemsTemp){
            printf("Error al redimensionar la pila de Float.\n");
            exit(1);
        }
        sf->tamMax = newMaxTam;
        sf->items = itemsTemp;
    }
    sf->top++;
    *(sf->items+sf->top) = value;

}

float popFloat(Stack_float *sf){
    if(sf->top == -1){
        printf("Error, la pila esta vacia.\n");
        exit(1);
    }
    return *(sf->items + (sf->top--));
}

/*=============================================== COLA (QUEUE) =======================================================================================*/



// Inicializacion
void initQueue(Queue *q, int tamMax) {
    q->items = malloc(tamMax * sizeof(char*));
    if (!q->items) {
        perror("Error al inicializar cola");
        exit(1);
    }
    q->tamMax = tamMax;
    q->size = 0;
    q->front = 0;
    q->rear = -1;
}

int isQueueEmpty(Queue *q) {
    return q->size == 0;
}

int isQueueFull(Queue *q) {
    return q->size == q->tamMax;
}

void resizeQueue(Queue *q) {
    int newTam = q->tamMax * 2;
    char **newItems = malloc(newTam * sizeof(char*));
    if (!newItems) {
        perror("Error al redimensionar cola");
        exit(1);
    }
    // Copiamos en orden
    for (int i = 0; i < q->size; i++) {
        newItems[i] = q->items[(q->front + i) % q->tamMax];
    }
    free(q->items);
    q->items = newItems;
    q->tamMax = newTam;
    q->front = 0;
    q->rear = q->size - 1;
}

// Encolar
void enqueue(Queue *q, char *value) {
    if (isQueueFull(q)) resizeQueue(q);
    q->rear = (q->rear + 1) % q->tamMax;
    q->items[q->rear] = value;
    q->size++;
}

// Desencolar
char* dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Error: la cola está vacía.\n");
        exit(1);
    }
    char *value = q->items[q->front];
    q->front = (q->front + 1) % q->tamMax;
    q->size--;
    return value;
}

// Peek
char* peekQueue(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Error: la cola está vacía.\n");
        exit(1);
    }
    return q->items[q->front];
}

// Print
void printQueue(Queue *q) {
    for (int i = 0; i < q->size; i++) {
        printf("%s ", q->items[(q->front + i) % q->tamMax]);
    }
    printf("\n");
}

void freeQueue(Queue *q) {
    free(q->items);
}
