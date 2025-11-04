#include "funciones.h"
#include "estructuras-de-datos.h"


/*================================== FUNCION PARA RESOLVER ECUACION ======================================*/

float resolverEcuacion(char *ecuacion, size_t longitud){

    Variables vars;
    float resultado;
    int opcion;
    do{
    printf("1: Ingresar solo un par de valores\n 2: Crear una tabla a partir de un par de valores\n");
    scanf("%d", &opcion);
    }while(opcion != 1 && opcion != 2);

    if(opcion == 2){
    float resultados[11];
    float* presultados = resultados;
    Variables vecVar[11];
    Variables* pvec = vecVar;

    detectarVariables(ecuacion, longitud, &vars);
    pedirValoresVariables(&vars); //ya tiene la info de que variables pedir
    *pvec = vars;

    float i=0;
    float j=1;
    while(i<=5){
        pvec->valorX = (vars.valorX)+i;
        pvec->valorY = (vars.valorY)+i;
        pvec++;
        i++;
    }
    while(j<=5){
        pvec->valorX = (vars.valorX)-j;
        pvec->valorY = (vars.valorY)-j;
        pvec++;
        j++;
    }

    pvec=vecVar;

    for(int i=0; i<11; i++){

    vars.valorX = pvec->valorX;
    vars.valorY = pvec->valorY;
    printf("%2.f", vars.valorX);
    printf("%2.f", vars.valorY);

    TokenList tokenList = tokenizarString(ecuacion,longitud, MAXTAM, &vars);
    asignarValoresVariables(&tokenList, &vars);
    float resultado = shuntingYard(&tokenList, &vars);
    *presultados = resultado;

    pvec++;
    presultados++;
    }

    pvec=vecVar;
    presultados=resultados;
    printf("X\t|\tY\t|\tRESULTADO\n");
for(int i=0; i<11; i++){
    // Usamos los punteros base
    printf("%2.f\t|\t%2.f\t|\t%.2f\n", (vecVar+i)->valorX, (vecVar+i)->valorY, *(resultados+i));
}


    }else{
        detectarVariables(ecuacion, longitud, &vars);
        pedirValoresVariables(&vars); //ya tiene la info de que variables pedir
        TokenList tokenList = tokenizarString(ecuacion,longitud, MAXTAM, &vars);
        asignarValoresVariables(&tokenList, &vars);
        resultado = shuntingYard(&tokenList, &vars);

        printf("Resultado: %.2f\n", resultado);
    }

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
float shuntingYard(TokenList *t, Variables *vars ){

    //breakpoint despues de reemplazar valores por variables
        printf("Ecuacion tokenizada:\n");
        char **ptr = t->items;
        char **fin = t->items + t->size;
        while (ptr < fin) {
            printf("%s ", *ptr);
            ptr++;
        }
        printf("\n");

    char **punteroIndice = t->items;
    char **memFinal = t->items + t->size;

    //Inicializamos la Cola y pila
    Stack pila;
    Queue salida;
    initStack(&pila, MAXTAM);
    initQueue(&salida, MAXTAM);



    //Empezamos a recorrer:
    while(punteroIndice < memFinal){

        char *token = *punteroIndice;


        // --- Operando (número) ---
        if (esNumeroToken(token)) {
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
            if(esNumeroToken(token)){
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





int precedencia(const char *token) {
    if (strcmp(token, "^") == 0 || strcmp(token, "r") == 0) return 3;
    if (strcmp(token, "*") == 0 || strcmp(token, "/") == 0) return 2;
    if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0) return 1;
    return 0;
}

int esAsociativoDerecha(const char *token) {
    return strcmp(token, "^") == 0; // solo potencia es derecha
}




TokenList tokenizarString(char *ecuacion, size_t longitud , int tamMax, Variables *vars) {
    TokenList tokenList; initTokenList(&tokenList, tamMax);

    for (int i = 0; i < longitud; i++) {
        if (isspace(*(ecuacion + i))) continue; // ignora espacios

        char actual = *(ecuacion + i);
        char siguiente = (i + 1 < longitud) ? *(ecuacion + i + 1) : '\0';


        // ==================================================================
        // --- MODIFICACIÓN #1: LÓGICA DE NÚMEROS MEJORADA ---
        // Esta lógica ahora se ejecuta ANTES de los operadores y maneja
        // signos unarios (+/-), decimales y números enteros.
        // ==================================================================

        // Primero, determinamos si el contexto es unario para un signo +/-
        int esContextoUnario = 0;
        if (i == 0) { // Inicio de la cadena
            esContextoUnario = 1;
        } else {
            // Buscar el caracter anterior no espacial
            int prev_idx = i - 1;
            while (prev_idx >= 0 && isspace(*(ecuacion + prev_idx))) {
                prev_idx--;
            }
            // Es unario si está al inicio o viene después de un operador / paréntesis
            if (prev_idx < 0 || strchr("+-*/(^r", *(ecuacion + prev_idx))) {
                esContextoUnario = 1;
            }
        }

        // Un número puede empezar con:
        // 1. Un dígito (e.g., "3")
        // 2. Un punto decimal (e.g., ".5")
        // 3. Un signo en contexto unario (e.g., "-3" o "+.5")
        int esSignoUnarioValido = (actual == '+' || actual == '-') && esContextoUnario && (isdigit(siguiente) || siguiente == '.');

        if (isdigit(actual) || (actual == '.' && isdigit(siguiente)) || esSignoUnarioValido) {

            int start = i; // Marcamos el inicio del número (incluyendo el signo si es unario)

            if (esSignoUnarioValido) {
                i++; // Consumimos el signo, avanzamos al dígito o punto
            }

            // Leer parte entera (si la hay)
            while (i < longitud && isdigit(*(ecuacion + i))) {
                i++;
            }

            // Leer parte decimal (si la hay)
            if (i < longitud && *(ecuacion + i) == '.') {
                i++; // Consumir el '.'
                // Leer dígitos después del decimal
                while (i < longitud && isdigit(*(ecuacion + i))) {
                    i++;
                }
            }

            // Creamos el token de número completo
            int len = i - start;
            char *numero = malloc(len + 1);
            if (!numero) {
                printf("Error de memoria al crear token de numero.\n");
                exit(1);
            }
            memcpy(numero, ecuacion + start, len);
            numero[len] = '\0';

            // Añadimos el token (e.g., "-3", "3.14", ".5", "+10")
            addTokenList(&tokenList, numero, 1); // '1' para que addTokenList libere 'numero'

            // Verificamos si hay multiplicación implícita después del número
            char next = (i < longitud) ? *(ecuacion + i) : '\0';
            if (next == 'x' || next == 'y' || next == 'X' || next == 'Y' || next == '(') {
                addTokenList(&tokenList, "*", 0);
            }

            i--; // El bucle for principal incrementará i, así que retrocedemos 1
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
            // (Tu lógica de raíz existente va aquí, no la modifico)
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

            float resultadoBase = shuntingYard(&tokenListInterna, vars);

            // pasamos resultado a string
            char *resultadoBaseStr = floatToString(resultadoBase);

            addTokenList(&tokenList, resultadoBaseStr, 1); // base numérica
            addTokenList(&tokenList, "r", 0); // operador raíz

            free(ecuacionInterna);
            freeTokenList(&tokenListInterna);

            // adelantamos índice al final del bloque procesado
            i = j - 1;
            continue;
        }

        // ==================================================================
        // --- MODIFICACIÓN #2: LÓGICA DE SIGNOS "Lo newww" ---
        // Esta lógica ahora solo se ejecutará para signos BINARIOS,
        // ya que los unarios fueron capturados por la lógica de NÚMERO.
        // ==================================================================
        char signoActual;
        if(actual == '-' || actual == '+'){
            signoActual = actual;
            int j = i+1;
            char posInterna = *(ecuacion+j); //Siguiente al actual
            while(posInterna == '+' || posInterna == '-'){
                if(signoActual == '+' && posInterna == '+') signoActual = '+';
                else if(signoActual == '+' && posInterna == '-') signoActual = '-';
                else if(signoActual == '-' && posInterna == '+') signoActual = '-';
                else if(signoActual == '-' && posInterna == '-') signoActual = '+';
                j++;
                posInterna = *(ecuacion + j);
            }

            addTokenList(&tokenList, charToString(signoActual), 1);

            i = j - 1;
            continue; // no agregamos el actual otra vez
        }


        // --- OTROS (operadores * / ^ etc.) ---
        addTokenList(&tokenList, charToString(actual), 1);

    } // Fin del bucle for

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


int esNumeroToken(const char*token){
    if(!token || *token == "\0" ) return 0;

    const char *p = token;

    // signo opcional
    if (*p == '+' || *p == '-') p++;

    int digits = 0;
    while (*p && isdigit((unsigned char)*p)) { digits++; p++; }

    // parte decimal opcional
    if (*p == '.') {
        p++;
        while (*p && isdigit((unsigned char)*p)) { digits++; p++; }
    }

    // debe haber al menos un dígito en total
    if (digits == 0) return 0;

    // no debe quedar nada raro al final
    return *p == '\0';
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



