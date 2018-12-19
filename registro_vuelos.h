#ifndef REGISTRO_H
#define REGISTRO_H

#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include "hash.h"
#include "heap.h"
#include "lista.h"
#include "abb.h"
#include "pila.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define COMANDO_AGREGAR "agregar_archivo"
#define COMANDO_VER_TABLERO "ver_tablero"
#define COMANDO_INFO_VUELO "info_vuelo"
#define COMANDO_PRIORIDAD_VUELOS "prioridad_vuelos"
#define COMANDO_BORRAR "borrar"
#define SALTO_LINEA '\n'
#define VACIO  "\0"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
struct registro;
typedef struct registro registro_t;

struct tablero;
typedef struct tablero tablero_t;

/* ******************************************************************
 *               PRIMITIVAS DE REGISTRO VUELOS
 * *****************************************************************/
// Crea un struct registro.
// Pos: Devuelve un struct registro.
registro_t* registro_crear(char* linea);

// Casos de compararacion:
// -Segun su prioridad de vuelo.
// -Si tienen la misma prioridad se comparan los codigos de vuelos.
// Pre: Se reciben dos registros creados y cargados.
// Pos: Se devuelve mayor (1) o menor (-1).  
int funcion_comparacion(const void* registro1,const void* registro2);

// Destruyo un struct registro.
// Pre: Se recibe un struct registro.
// Pos: El registro fue eliminado.
void registro_destruir(registro_t* registro);

/* ******************************************************************
 *               		PRIMITIVA DE TABLERO
 * *****************************************************************/
// Crea un struct tablero.
// Pos: Devuelve un struct tablero.
tablero_t* crear_tablero(char** parametros);

/* ******************************************************************
 *               		FUNCIONES PRINCIPALES
 * *****************************************************************/

// Funciona como envoltorio de strncmp para la realizar las comparaciones 
// teniendo en cuenta solo la parte de la clave que corresponde con la fecha
// Pos: Devuelve menor a 0 si la "clave" es menor a la "fecha a comparar",
// en caso contrario, devuelve mayor a 0 si la "clave" es mayor a la "fecha a comparar"
// o se devuelve 0 si son iguales. 
int strcmp_fecha(const char* clave, const char* fecha_a_comparar);

// Cuenta la cantidad de parametros que se reciben por stdin.
// Pos: Se devuelve la cantidad de parametros recibidos por stdin.
int contar_parametros(char** parametros);

// Crea clave para almacenarla en el abb de busqueda
// Pos: Devuelve una clave nueva.
char* crear_clave_abb(char* numero_vuelo, char* fecha);

// Se lee el archivo recibido y se cargan en las TDAS Hash y Abb.
// Pre: Se recibe un archivo valido.
// Pos: Se cargaron los datos del archivo a los TDAS.
void procesar_archivo(FILE* archivo_entrada, abb_t* arbol, hash_t* dicc);

// Se verifica que los parametros ingresados por stdin sean los correctos para utilizar
// el comando ver tablero.
//Pre: Se reciben parametros ingresados por stdin.
//Pos: Devuelve false en caso los siguientes casos: si el numero de vuelos a mostrar es menor o igual a 0,
// si el modo a imprimir no es "asc" ni "desc" y si la fecha de inicio es menor a la de fin. En caso contrario,  
// se devuelve true.
bool verificar_parametros_ver_tablero(char** parametros);

// Funcion que utiliza el iterador externo del TDA Abb de busqueda.
// La clave recibida se utiliza para comparar si esta dentro del rango que debe imprimir "ver tablero",
// si cumple se alamacena en un TDA Lista Enlazada.
// Pre: fueron creados el arbol y tablero.
// Pos: se proceso el arbol y se cargan elementos al TDA Lista Enlazada.
bool procesar_arbol_para_ver_tablero(abb_t* arbol, tablero_t* tablero);

// Imprime los structs registros que estan dentro del rango que se mando por stdin.
// Pre: se recibe un struct tablero con una lista cargada.
// Post: se imprime de forma ascendente o descendente y segun la cantidad de lineas que se desea mostrar.
void imprimir_ver_tablero(tablero_t* nuevo_tablero);

// Se leen todos los datos del hash y se cargan a un Heap de minimos los k datos con mayor prioridad.
// Pre: Se recibe un hash con todos los datos cargados y se crea un iterador para recorrerlo completo.
// Pos: se cargaron al heap los k structs con mayor prioridad. 
void prioridad_vuelos(hash_t* dicc, hash_iter_t* iterador, heap_t* heap_prioridad, size_t cant_vuelos);

// Se desencola del heap los k registros guardados y se almacenan en una pila, para que el tope de la pila
// sea el registro de mayor prioridad.
// Pos: Se cargan los k registros a la pila.
bool ordenar_vuelos(heap_t* heap_prioridad, pila_t* pila_vuelos);

// Se recibe una pila con los registros de mayor prioridad y se imprimen por stdout.
// Pos: Se imprimieron los k registro de mayor prioridad y se destruye la pila.
void imprimir_prioridad(pila_t* pila_vuelos);

// Se verifica que la cantidad de parametros ingresados para el comando borrar sea el correcto.
// Pos: se devuelve false en caso de que la fecha de inicio sea menor a la de fin. En caso contrario
// se devuelve true.
bool verificar_parametros_borrar(char** parametros);

// Se recorrer el arbol con un iterador externo y se almacenan en una lista enlazada los vuelos que se van a borrar 
// y que esten dentro del rango de inicio y fin.
// Pre: Se recibe un arbol con datos cargados.
// Pos: Se alamacenan a la lista los vuelos a borrar. 
void procesar_arbol_para_borrar(abb_t* arbol, lista_t* lista_borrar, const char* fecha_inicio, const char* fecha_fin);

// Se imprimen los vuelos que se desean borrar y que estan dentro del rango mandado por parametro.
// Los vuelos fueron eliminados del hash y del abb.
// Pre: se recibe en una lista los vuelos que se van a borrar.
// Pos: se eliminan del hash y del abb, se imprimen por stdout.
void imprimir_elementos_borrados(lista_t* lista_borrar, abb_t* arbol, hash_t* dicc);

// Se busca en el hash el numero de vuelo y se imprime toda su informacion por stdout.
// Pre: el numero de vuelo pertence al hash.
// Pos: se imprimio informacion sobre el numero de vuelo ingresado
void imprimir_info_vuelo(hash_t* dicc, char* numero_vuelo);

#endif
