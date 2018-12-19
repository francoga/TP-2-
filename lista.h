#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct lista;
typedef struct lista lista_t;

struct lista_iter;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/
// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no nodos.
// Pre: la lista fue creada.
// Post: se verifica si la lista esta vacia
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo nodo al principio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo nodo a la lista, dato se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo nodo al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo nodo a la lista, dato se encuentra al final
// de la lista
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer nodo de la lista. Si la lista tiene nodos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer nodo anterior, la lista
// contiene un nodo menos.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor que almacena el primer nodo de la lista. Si la lista tiene
// nodos, se devuelve el valor del primero nodo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: devuelve del primer nodo de la lista el valor que almacena, siempre que la lista no este vacía.
void *lista_ver_primero(const lista_t *lista);


// Obtiene el valor que almacena el ultimo nodo de la lista. Si la lista tiene
// nodos, se devuelve el valor que almacene el ultimo nodo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: devuelve del ultimo nodo de la lista el valor que almacena, siempre que la lista no este vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista
// Pre: La lista fue creada
// Post: Devuelve el largo de la lista
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));


/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
// Crea un iterador
// Pre: la lista fue creada
// Post: devuelve un nuevo iterador que esta apuntando al primer nodo de la lista
lista_iter_t *lista_iter_crear(lista_t *lista);

// El iterador avanza al siguiente nodo de la lista, siempre y cuando no sea el final de la lista.
// A medida que el iterador avanza, el actual pasa a ser el anterior y el siguiente el nuevo actual.
// Si el iterador puede avanzar devuelve true, en caso contrario false.
// Pre: el iterador fue creado.
// Post: el iterador apunta al siguiente nodo de la lista
bool lista_iter_avanzar(lista_iter_t *iter);

// Se obtiene el valor que apunta el iterador, siempre que no sea final de la lista.
// Pre: el iterador fue creado
// Post: se devuelve el valor al que apunta el iterador,  en caso de estar al final de lista devuelve NULL
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Se verifica si el iterador llego al final de la lista. Devuelve true si actual es NULL, en caso contrario false.
// Pre: el iterador fue creado.
// Post: se verifica la posicion del iterador
bool lista_iter_al_final(const lista_iter_t *iter);

// Se elimina el iterador
// Pre: el iterador fue creado
void lista_iter_destruir(lista_iter_t *iter);

// Se inserta un nuevo nodo en la posicion actual del iterador. Devuelve true si se pudo insertar, en caso contrario false.
// Pre: el iterador fue creado.
// Post: Se inserta un nuevo nodo a la lista, la cantidad de la lista aumenta en 1.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Elimina el nodo al que apunta el iterador. Si no se llego a final de la lista, se quita el
// el nodo al que apunta el iterador, y se devuelve su valor. Si se llega a final de la lista devuelve NULL.
// Pre: la lista fue creada y no esta vacia.
// Post: se devolvió el valor del nodo al que apunta el iterador, la lista
// contiene un nodo menos, si la lista no estaba vacía.
void *lista_iter_borrar(lista_iter_t *iter);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/
//Itero toda la lista hasta llegar a NULL o si la funcion booleana visitar da false. 
//Se recibe como parametro la funcion visitar que opera sobre los datos de los nodos de la lista.
//Pre: la lista fue creada
//Post: se recorren los nodos de la lista
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/
// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);

#endif // LISTA_H
