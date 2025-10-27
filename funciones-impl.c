#include "funciones.h"



/*================================== FUNCION PARA RESOLVER ECUACION ======================================*/

float resolverEcuacion(char *ecuacion, size_t longitud){

    Variables vars;
    detectarVariables(ecuacion, longitud, &vars);
    pedirValoresVariables(&vars); //ya tiene la info de que variables pedir
    TokenList tokenList = tokenizarString(ecuacion,longitud, MAXTAM, &vars);
    asignarValoresVariables(&tokenList, &vars);
    float resultado = shuntingYard(&tokenList);

    return resultado;
}


/*================================== FUNCIONES PARA VARIABLES ======================================*/




void pedirValoresVariables(Variables *vars) {
    if (vars->tieneX) {
        printf("Ingrese valor para x: ");
        scanf("%f", &vars->valorX);
    }
    if (vars->tieneY) {
        printf("Ingrese valor para y: ");
        scanf("%f", &vars->valorY);
    }
}


void detectarVariables(char *ecuacion, size_t longitud, Variables *vars) {
    vars->tieneX = vars->tieneY = 0;
    for (size_t i = 0; i < longitud; i++) {
        if (*(ecuacion+i) == 'x' || *(ecuacion+i) == 'X')
            vars->tieneX = 1;
        if (*(ecuacion+i) == 'y' || *(ecuacion+i) == 'Y')
            vars->tieneY = 1;
    }
}


void asignarValoresVariables(TokenList *t, Variables *vars) {
    for (int i = 0; i < t->size; i++) {
        if (strcmp(*(t->items+i), "x") == 0 || strcmp(*(t->items+i), "X") == 0) {
            free(*(t->items+i));
            *(t->items+i) = floatToString(vars->valorX);
        }
        else if (strcmp(*(t->items+i), "y") == 0 || strcmp(*(t->items+i), "Y") == 0) {
            free(*(t->items+i));
            *(t->items+i) = floatToString(vars->valorY);
        }
    }
}




/*======================= ALGORITMO DE SHUNTING YARD (Algortimo para pasar de notacion INFIJA a POSFIJA) y resuelve la ecuacion en posfija a traves de una cola de flotantes ================================*/



//La ecuacion ya nos llega tokenizada
float shuntingYard(TokenList *t){

    //Inicializamos la Cola y pila
    Stack pila;
    Queue salida;
    initStack(&pila, MAXTAM);
    initQueue(&salida, MAXTAM);

    char **punteroIndice = t->items;
    char **memFinal = t->items + t->size;

    //Empezamos a recorrer:
    while(punteroIndice < memFinal){

        char *token = *punteroIndice;


        // --- Operando (número) ---
        if (isdigit(*token)) {
            enqueue(&salida, token);
        }
        //Si es operador
        else if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
                 strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
                 strcmp(token, "^") == 0 || strcmp(token, "r") == 0) {

            while (!isEmpty(&pila) &&
                   (precedencia(peek(&pila)) > precedencia(token) ||
                    (precedencia(peek(&pila)) == precedencia(token) && !esAsociativoDerecha(token))) &&
                   strcmp(peek(&pila), "(") != 0) {
                enqueue(&salida, pop(&pila));
            }
            push(&pila, token);
        }

        // --- Paréntesis izquierdo ---
        else if (strcmp(token, "(") == 0) {
            push(&pila, token);
        }
        // --- Paréntesis derecho ---
        else if (strcmp(token, ")") == 0) {
            while (!isEmpty(&pila) && strcmp(peek(&pila), "(") != 0) {
                enqueue(&salida, pop(&pila));
            }
            if (!isEmpty(&pila) && strcmp(peek(&pila), "(") == 0) {
                pop(&pila); // descarta '('
            }
        }

        punteroIndice++; // avanzamos puntero al siguiente token

    }

    // Vaciar pila al final
    while (!isEmpty(&pila)) {
        enqueue(&salida, pop(&pila));
    }


    //Aca comienza el flujo de Resolucion de la ecuacion
    printf("Notacion postfija: ");
    printQueue(&salida);

    Stack_float pilaEcuacion;
    pilaEcuacion.items = malloc( 100 * sizeof(float));
    pilaEcuacion.top = -1;
    pilaEcuacion.tamMax = 100;

    if(!pilaEcuacion.items){
        printf("Error al reservar memoria para la Pila float.\n");
        exit(1);
    }

    while(!isQueueEmpty(&salida)){
            char *token = dequeue(&salida);
            if(isdigit(*token)){
                float num = atof(token);
                pushFloat(&pilaEcuacion, num);
            }else{
                float b = popFloat(&pilaEcuacion);
                float a = popFloat(&pilaEcuacion);
                float resultado;
                if (strcmp(token, "+") == 0) resultado = a + b;
                else if (strcmp(token, "-") == 0) resultado = a - b;
                else if (strcmp(token, "*") == 0) resultado = a * b;
                else if (strcmp(token, "/") == 0) resultado = a / b;
                else if (strcmp(token, "^") == 0) resultado = pow(a, b);
                else if (strcmp(token, "r") == 0) resultado = pow(b, 1.0 / a);

                pushFloat(&pilaEcuacion, resultado);
            }
    }

    float resultadoFinal = popFloat(&pilaEcuacion);
    free(pilaEcuacion.items);
    freeQueue(&salida);
    freeStack(&pila);

    return resultadoFinal;
}



int precedencia(char *token) {
    if (strcmp(token, "^") == 0 || strcmp(token, "r") == 0) return 3;
    if (strcmp(token, "*") == 0 || strcmp(token, "/") == 0) return 2;
    if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0) return 1;
    return 0;
}

int esAsociativoDerecha(char *token) {
    return strcmp(token, "^") == 0; // solo potencia es derecha
}




TokenList tokenizarString(char *ecuacion, size_t longitud , int tamMax, Variables *vars) {
    TokenList tokenList; initTokenList(&tokenList, tamMax);
    for (int i = 0; i < longitud; i++) {
            if (isspace(*(ecuacion + i))) continue;     // ignora espacios

            char actual = *(ecuacion + i);
            char siguiente = (i + 1 < longitud) ? *(ecuacion + i + 1) : '\0';

            // --- NUMERO ---
            if (isdigit(actual)) {
                int start = i;
                while (i < longitud && isdigit(*(ecuacion + i))) i++;

                int len = i - start;
                char *numero = malloc(len + 1);
                memcpy(numero, ecuacion + start, len);
                numero[len] = '\0';
                addTokenList(&tokenList, numero, 1);
                char next = (i < longitud) ? *(ecuacion + i) : '\0';

                if (next == 'x' || next == 'y' || next == 'X' || next == 'Y' || next == '(') {
                        addTokenList(&tokenList, "*", 0); } i--;
                        continue;
                }

            // --- VARIABLE ---
            if (actual == 'x' || actual == 'y' || actual == 'X' || actual == 'Y') {
                addTokenList(&tokenList, charToString(actual), 1);
                if (isdigit(siguiente) || siguiente == '(') {
                    addTokenList(&tokenList, "*", 0);
                }
                continue;
            }

            // --- PARENTESIS ---
            if (actual == '(' || actual == ')') {
                addTokenList(&tokenList, charToString(actual), 1);
                if (actual == ')' && (siguiente == 'x' || siguiente == 'y' || siguiente == 'X' || siguiente == 'Y' || siguiente == '('|| isdigit(siguiente))) {
                    addTokenList(&tokenList, "*", 0);
                }
                continue;
            }

            // --- RAIZ ---
            if (actual == 'r' || actual == 'R') {

                // si no hay un número antes, por defecto es raiz cuadrada
                if (i == 0 || !isdigit(*(ecuacion + i - 1))) {
                    addTokenList(&tokenList, "2", 0); // índice 2
                }

                // saltamos la 'r' y esperamos un '('
                i++;
                if (*(ecuacion + i) != '(') {
                    printf("Error: se esperaba '(' después de 'r'\n");
                    exit(1);
                }

                // extraemos el contenido interno entre parentesis
                int contParentesis = 1;
                int start = i + 1; // después del '('
                int j = start;

                while (j < longitud && contParentesis > 0) {
                    if (*(ecuacion + j) == '(') contParentesis++;
                    else if (*(ecuacion + j) == ')') contParentesis--;
                    j++;
                }

                if (contParentesis != 0) {
                    printf("Error: paréntesis no balanceados en raíz.\n");
                    exit(1);
                }

                int lenInterna = j - start - 1; // sin los parentesis para reservar la memoria exacta que va a usar
                char *ecuacionInterna = malloc(lenInterna + 1);
                if (!ecuacionInterna) {
                    printf("Error al reservar memoria para ecuacionInterna.\n");
                    exit(1);
                }

                memcpy(ecuacionInterna, ecuacion + start, lenInterna);
                ecuacionInterna[lenInterna] = '\0';

                // ahora evaluamos la subexpresión (breakpoint)
                printf(" Subexpresion enviada a tokenizar: '%s'\n", ecuacionInterna);

                TokenList tokenListInterna = tokenizarString(ecuacionInterna, lenInterna, 50, vars);

                // breakpoint
                printf("Ecuacion tokenizada: ");
                char **ptr = tokenListInterna.items;
                char **fin = tokenListInterna.items + tokenListInterna.size;
                while (ptr < fin) {
                    printf("%s ", *ptr);
                    ptr++;
                }
                printf("\n");
                //

                //Asignamos valores a esta expresion Tokenizada internamente para que el shunting yard la resuelva correctamente
                asignarValoresVariables(&tokenListInterna, vars);

                float resultadoBase = shuntingYard(&tokenListInterna);

                // pasamos resultado a string
                char *resultadoBaseStr = floatToString(resultadoBase);

                addTokenList(&tokenList, resultadoBaseStr, 1); // base numérica
                addTokenList(&tokenList, "r", 0);              // operador raíz

                free(ecuacionInterna);
                freeTokenList(&tokenListInterna);

                // adelantamos índice al final del bloque procesado
                i = j - 1;
                continue;
            }


            // --- OTROS (operadores, etc.) ---
            addTokenList(&tokenList, charToString(actual), 1);

        }

        //breakpoint
        printf("Ecuacion tokenizada:\n");
        char **ptr = tokenList.items;
        char **fin = tokenList.items + tokenList.size;
        while (ptr < fin) {
            printf("%s ", *ptr);
            ptr++;
        }
        printf("\n");

        return tokenList;
}

/*====================================== FUNCIONES TOKENLIST UTILS  ========================*/




void initTokenList(TokenList *t, int tamMax){
    t->items = malloc( tamMax* sizeof(char*) );
    if(!t->items){
        printf("Error al iniciar la Lista de Tokens.\n");
        exit(1);
    }
    t->size = 0 ;
    t->capacity = tamMax;
}

void addTokenList(TokenList *t, const char *value, int debeLiberar) {
    if (t->size == t->capacity) {
        int newTam = t->capacity * 2;
        char **tokenListTemporal = realloc(t->items, newTam * sizeof(char*));
        if (!tokenListTemporal) {
            printf("Error al agrandar la lista de Tokens.\n");
            exit(1);
        }
        t->items = tokenListTemporal;
        t->capacity = newTam;
    }

    *(t->items + t->size) = strdup(value);
    t->size++;

    if (debeLiberar)
        free((void*)value);
}


char* charToString(char c) {
    char *string = malloc(2 * sizeof(char)); // 1 char + '\0'
    if (!string) {
        perror("Error al convertir de char a string.\n");
        exit(1);
    }
    *(string) = c;
    *(string+1) = '\0';
    return string;
}

char* floatToString(float num) {
    char *buffer = malloc(32);
    if (!buffer) {
        printf("Error al reservar memoria para floatToString.\n");
        exit(1);
    }
    snprintf(buffer, 32, "%.6f", num); // Funcion estandar de C queescribe en un buffer char una cadena formateada
    return buffer;
}


void freeTokenList(TokenList *t) {
    for (int i = 0; i < t->size; i++) {
        free(t->items[i]);
    }
    free(t->items);
    t->items = NULL;
    t->size = 0;
    t->capacity = 0;
}



