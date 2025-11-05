#ifndef RESOLUCION-ECUACION_H_INCLUDED
#define RESOLUCION-ECUACION_H_INCLUDED
#include "estructuras-de-datos.h"
#include <ctype.h> // para isdigit

typedef struct {
    char **items;  // lista de tokens
    int size;      // cantidad de tokens actuales
    int capacity;  // capacidad actual
} TokenList;


typedef struct {
    int tieneX;
    int tieneY;
    float valorX;
    float valorY;
} Variables;


float resolverEcuacion(char *ecuacion, size_t longitud); // Arma DetectaVariables -> Pide valoresVariables -> (Ya con los valores) arma tokenList con TokenizarString (Devuelve TokenList) -> (Ya con la tokenList) shunting yard (Devuelve valor) lo retornamos
void asignarValoresVariables(TokenList *t, Variables *vars);
void detectarVariables(char *ecuacion, size_t longitud, Variables *vars);
void pedirValoresVariables(Variables *vars);


float shuntingYard(TokenList *t, Variables *vars);
int precedencia(const char *token);
int esAsociativoDerecha(const char *token);
int esNumeroToken(const char*token);


//Proceso de Tokenizar
TokenList tokenizarString(char *ecuacion, size_t longitud , int tamMax, Variables *vars, int *tieneError);
void initTokenList(TokenList *t, int tamMax);
void addTokenList(TokenList *t, const char *value, int debeLiberar);
void freeTokenList(TokenList *t);
char* charToString(char c);
char* floatToString(float num);

#endif // RESOLUCION-ECUACION_H_INCLUDED
