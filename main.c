#include <stdio.h>
#include <string.h>

// --- Definiciones Globales ---
#define MAX_ECUACIONES 10
#define MAX_LARGO 101
#define NOMBRE_ARCHIVO "ecuaciones.dat"

typedef struct {
    char texto[MAX_LARGO]; 
} tEcuacion; 

// --- Almacenamiento Principal ---
tEcuacion vectorEcuaciones[MAX_ECUACIONES];
int contadorEcuaciones = 0;

// --- Prototipos de Funciones ---
void mostrarMenu(void);
void escribirEcuacion(void);    // [A]
void mostrarEcuaciones(void);   // [B]
void guardarEcuaciones(void);   // [C]
void leerEcuaciones(void);      // [D]
void borrarEcuaciones(void);    // [E]
void limpiarBufferEntrada(void); 
int esEcuacionValida(char *ecuacion);

/**
 * Funcion Principal
 */
int main() {
    int opcion = 0;
    int resultadoScan = 0;

    do {
        mostrarMenu();
        resultadoScan = scanf("%d", &opcion);
        
        if (resultadoScan != 1) {
            printf("Error: Debes ingresar un numero.\n");
            limpiarBufferEntrada();
            continue;
        }
        
        // Limpia el \n de scanf ANTES de llamar a fgets
        limpiarBufferEntrada(); 

        switch (opcion) {
            case 1:
                escribirEcuacion();
                break;
            case 2:
                mostrarEcuaciones();
                break;
            case 3:
                guardarEcuaciones();
                break;
            case 4:
                leerEcuaciones();
                break;
            case 5:
                borrarEcuaciones();
                break;
            case 6:
                printf("Saliendo de la calculadora... (cambios no guardados)\n");
                break;
            default:
                printf("Opcion no valida. Intentalo de nuevo.\n");
        }
    } while (opcion != 6);

    return 0;
}

/**
 * Muestra el menu de opciones
 */
void mostrarMenu(void) {
    printf("\n--- Calculadora Cientifica (Punteros + Libs) ---\n");
    printf("Ecuaciones guardadas: %d / %d\n", contadorEcuaciones, MAX_ECUACIONES);
    printf("---------------------------------------\n");
    printf("1. [A] Escribir ecuacion\n");
    printf("2. [B] Ver ecuaciones de esta sesion\n");
    printf("3. [C] Guardar y reiniciar sesion\n");
    printf("4. [D] Leer ecuaciones guardadas\n");
    printf("5. [E] Borrar ecuaciones guardadas\n");
    printf("6. [X] Salir\n");
    printf("Elige una opcion: ");
}

/**
 * [A] - Escribir ecuacion
 */
void escribirEcuacion(void) {
    char bufferTemporal[MAX_LARGO];
    int indiceReemplazo = -1;
    int resultadoScan = 0;
    tEcuacion *punteroEcuacion; 
    int ecuacionValida = 0; 
    size_t longitud; 

    printf("\n--- [A] Escribir Ecuacion ---\n");
    printf("Usa x, y, 0-9, +, -, *, /, ^, r(), () y espacios.\n");

    // --- PASO 1: Obtener una ecuacion valida ---
    while (ecuacionValida == 0) {
        printf("\nIntroduce la ecuacion (o 'cancelar' para salir):\n> ");
        
        // fgets() para leer la linea
        fgets(bufferTemporal, MAX_LARGO, stdin);
        
        // Limpia el '\n' que fgets() deja al final 
        // Usa strcspn para encontrar el '\n' y lo reemplaza
        longitud = strcspn(bufferTemporal, "\n");
        *(bufferTemporal + longitud) = '\0'; 
        
        // Usa strcmp() para comparar
        if (strcmp(bufferTemporal, "cancelar") == 0) {
            printf("Operacion cancelada.\n");
            return; 
        }
        
        if (esEcuacionValida(bufferTemporal) == 1) {
            ecuacionValida = 1; 
        } else {
            printf("Por favor, intenta de nuevo.\n");
        }
    }

    // --- PASO 2: Guardar la ecuacion (que ya sabemos que es valida) ---
    if (contadorEcuaciones < MAX_ECUACIONES) {
        // --- Caso 1: Hay espacio ---s
        punteroEcuacion = vectorEcuaciones + contadorEcuaciones;
        
        // Usa strcpy() para copiar
        strcpy(punteroEcuacion->texto, bufferTemporal);
        
        contadorEcuaciones++; 
        printf("Ecuacion guardada en la posicion %d!\n", contadorEcuaciones - 1);

    } else {
        // --- Caso 2: Lleno ---
        printf("\nAlmacenamiento lleno! La ecuacion '%s' reemplazara a otra.\n", bufferTemporal);
        mostrarEcuaciones();
        
        while (indiceReemplazo < 0 || indiceReemplazo >= MAX_ECUACIONES) {
            printf("\nQue ecuacion deseas reemplazar? (Ingresa 0 a %d): ", MAX_ECUACIONES - 1);
            resultadoScan = scanf("%d", &indiceReemplazo);
            
            if (resultadoScan != 1) {
                printf("Error: Debes ingresar un numero.\n");
                limpiarBufferEntrada();
                indiceReemplazo = -1; 
            } else if (indiceReemplazo < 0 || indiceReemplazo >= MAX_ECUACIONES) {
                printf("Indice fuera de rango. Intentalo de nuevo.\n");
            }
        }
        limpiarBufferEntrada(); // Limpia el \n de scanf

        punteroEcuacion = vectorEcuaciones + indiceReemplazo;

        // Usa strcpy() para copiar
        strcpy(punteroEcuacion->texto, bufferTemporal);
        
        printf("Ecuacion %d reemplazada exitosamente!\n", indiceReemplazo);
    }
}


/**
 * [B] - Ver ecuaciones de esta sesion
 */
void mostrarEcuaciones(void) {
    int i;
    tEcuacion *punteroEcuacion; 

    if (contadorEcuaciones == 0) {
        printf("\nNo hay ecuaciones guardadas.\n");
        return;
    }
    printf("\n--- Ecuaciones Guardadas (%d/%d) ---\n", contadorEcuaciones, MAX_ECUACIONES);
    
    for (i = 0; i < contadorEcuaciones; i++) {
        punteroEcuacion = vectorEcuaciones + i; 
        printf("[%d]: %s\n", i, punteroEcuacion->texto);
    }
}

void guardarEcuaciones(void) {
    FILE *archivo;
    archivo = fopen(NOMBRE_ARCHIVO, "wb");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s para guardar.\n", NOMBRE_ARCHIVO);
        return;
    }
    fwrite(vectorEcuaciones, sizeof(tEcuacion), contadorEcuaciones, archivo);
    fclose(archivo);
    contadorEcuaciones = 0;
    printf("Ecuaciones guardadas exitosamente en %s!\n", NOMBRE_ARCHIVO);
    printf("La sesion actual ha sido reiniciada.\n");
}

void leerEcuaciones(void) {
    FILE *archivo;
    long numeroLeidos = 0;
    archivo = fopen(NOMBRE_ARCHIVO, "rb");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s.\n", NOMBRE_ARCHIVO);
        return;
    }
    contadorEcuaciones = 0;
    numeroLeidos = fread(vectorEcuaciones, sizeof(tEcuacion), MAX_ECUACIONES, archivo);
    fclose(archivo);
    contadorEcuaciones = (int)numeroLeidos;
    printf("Se cargaron %d ecuaciones desde %s!\n", contadorEcuaciones, NOMBRE_ARCHIVO);
    mostrarEcuaciones();
}

void borrarEcuaciones(void) {
    if (remove(NOMBRE_ARCHIVO) == 0) {
        printf("Archivo %s borrado exitosamente!\n", NOMBRE_ARCHIVO);
    } else {
        printf("Error: No se pudo borrar el archivo %s.\n", NOMBRE_ARCHIVO);
    }
}


// --- FUNCIONES AUXILIARES ---
/**
/**
 * Validador de la cadena de ecuacion (lexico y sintactico).
 * Devuelve 1 (verdadero) si es valida, 0 (falso) si no.
 */
int esEcuacionValida(char *ecuacion) {
    const char *permitidos = "0123456789xy+-*/^()r \t";
    char *p_ecu = ecuacion; // Puntero para iterar la ecuacion
    
    char *digitos = "0123456789";
    char *variables = "xy";
    
    int balanceParentesis = 0; 

    // 1. Revisa ecuacion vacia
    if (*p_ecu == '\0') { 
        printf("Error: La ecuacion no puede estar vacia.\n");
        return 0; // Falso
    }

    // 2. Itera la ecuacion caracter por caracter
    while (*p_ecu != '\0') {
        char actual = *p_ecu;
        char siguiente = *(p_ecu + 1);
        char anterior = (p_ecu == ecuacion) ? '\0' : *(p_ecu - 1); 

        // --- Validacion Lexica (Paso A: Caracter permitido?) ---
        if (strchr(permitidos, actual) == NULL) {
            printf("Error: El caracter '%c' no esta permitido.\n", actual);
            return 0; // Falso
        }

        // --- Validacion Sintactica (Paso B: Parentesis) ---
        if (actual == '(') {
            // Esta regla asegura que '(' solo venga despues de 'r' o '^'
            if (anterior != 'r' && anterior != '^') {
                printf("Error sintactico: El parentesis '(' solo se permite despues de 'r' (raiz) o '^' (potencia).\n");
                return 0;
            }
            balanceParentesis++;
        }
        if (actual == ')') {
            balanceParentesis--;
        }
        if (balanceParentesis < 0) {
             printf("Error sintactico: Cierre de parentesis ')' inesperado.\n");
             return 0;
        }

        // --- Validacion Sintactica (Paso C: Errores especificos) ---
        
        // 'r' y '^' DEBEN ir seguidos de '('
        if (actual == 'r' || actual == '^') {
            if (siguiente != '(') {
                printf("Error sintactico: El caracter '%c' debe ir seguido de parentesis. Ej: %c( ... ).\n", actual, actual);
                return 0;
            }
        }
        
        // --- Validacion Sintactica (Paso D: /0, 0r, etc.) ---
        if (siguiente != '\0') {
            
            // REGLA: Bloquear "0r"
            if (actual == '0' && siguiente == 'r') {
                printf("Error sintactico: No se permite la raiz de indice '0' (0r).\n");
                return 0;
            }

            // REGLA: Bloquear "/0"
            if (actual == '/' && siguiente == '0') {
                char siguiente_del_cero = *(p_ecu + 2);
                if (siguiente_del_cero == '\0' || strchr(digitos, siguiente_del_cero) == NULL) {
                    printf("Error sintactico: Division literal por cero ('/0').\n");
                    return 0;
                }
            }

            // --- Validacion Sintactica (Paso E: Multiplicacion implicita) ---
            int actual_es_digito = (strchr(digitos, actual) != NULL);
            int actual_es_variable = (strchr(variables, actual) != NULL);
            int actual_es_cierre = (actual == ')');
            
            int siguiente_es_digito = (strchr(digitos, siguiente) != NULL);
            int siguiente_es_variable = (strchr(variables, siguiente) != NULL);
            // int siguiente_es_apertura = (siguiente == '('); // Ya no se necesita

            // REGLA: "2x"
            if (actual_es_digito && siguiente_es_variable) {
                printf("Error sintactico: Multiplicacion implicita '%c%c'. Use '%c*%c'.\n", actual, siguiente, actual, siguiente);
                return 0;
            }
            // REGLA: "x2"
            if (actual_es_variable && siguiente_es_digito) {
                printf("Error sintactico: Multiplicacion implicita '%c%c'. Use '%c*%c'.\n", actual, siguiente, actual, siguiente);
                return 0;
            }
            // REGLA: "xy"
            if (actual_es_variable && siguiente_es_variable) {
                 printf("Error sintactico: Multiplicacion implicita '%c%c'. Use '%c*%c'.\n", actual, siguiente, actual, siguiente);
                return 0;
            }
            // REGLA: ")2"
            if (actual_es_cierre && siguiente_es_digito) {
                printf("Error sintactico: Multiplicacion implicita '%c%c'. Use '%c*%c'.\n", actual, siguiente, actual, siguiente);
                return 0;
            }
            // REGLA: ")y"
            if (actual_es_cierre && siguiente_es_variable) {
                printf("Error sintactico: Multiplicacion implicita '%c%c'. Use '%c*%c'.\n", actual, siguiente, actual, siguiente);
                return 0;
            }
        }
        
        p_ecu++; // Avanzamos puntero de la ecuacion
    }

    // --- Validacion Final (Paso F: Balance de Parentesis) ---
    if (balanceParentesis != 0) {
        printf("Error sintactico: Faltan parentesis de cierre. (Quedaron %d abiertos).\n", balanceParentesis);
        return 0;
    }
    
    return 1; // Verdadero, paso todas las validaciones
}


/**
 * Limpia el buffer de entrada (stdin).
 */
void limpiarBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}