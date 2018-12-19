#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stdlib.h>

/* ******************************************************************
 *          DEFINICION DE LOS TIPOS DE DATOS DEL ABB
 * *****************************************************************/

typedef struct abb abb_t;
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

/* ******************************************************************
 *     DEFINICION DE LOS TIPOS DE DATOS DEL ITERADOR EXTERNO
 * *****************************************************************/

typedef struct abb_iter abb_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/
 
/* Crea el arbol
*/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/* Guarda un nuevo nodo en el arbol, verificando si va a la izquierdo a la derecha de su padre.
 * En caso de que ya exista la clave en el arbol, se swappea el nuevo nodo con su padre. Devuelve true si
 * se pudo guardar, en caso contrario false.
 * Pre: El arbol fue creado.
 * Post: Se guardo el nodo en el arbol. 
 */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/* Se busca en el arbol el nodo a borrar. Se tiene en cuenta los siguientes 3 casos:
 * Si es una hoja se borra el nodo del arbol, se devuelve el dato y se disminuye la cantidad de nodos.
 * Si es un padre que tiene un hijo, el abuelo apunta al nieto y se disminuye la cantidad de nodos.
 * Si es un padre que tiene dos hijos, buscamos el reemplazante y el padre del reemplazante, swappeamos el
 * nodo a borrar con el reemplazante y eliminamos el reemplazante, disminuimos la cantidad de nodos.
 * En caso de que el nodo no existe se devuelve NULL.
 * Pre: El arbol fue creado.
 * Post: El arbol tiene un nodo menos.
 */ 
void *abb_borrar(abb_t *arbol, const char *clave);

/* Se busca en el arbol un nodo, si existe se devuelve el dato que almacena, en caso contrario se devuelve NULL.
 * Pre: El arbol fue creado.
 * Post: Se hallo el nodo en el arbol.
 */
void *abb_obtener(const abb_t *arbol, const char *clave);

/* Se busca en el arbol un nodo, si existe se devuelve true, en caso contrario se devuelve false.
 * Pre: El arbol fue creado.
 * Post: Existe el nodo en el arbol.
 */
bool abb_pertenece(const abb_t *arbol, const char *clave);

/* Devuelve la cantidad de nodos que hay en el arbol.
 * Pre: El arbol fue creado.
 * Post: La cantidad de nodos del arbol.
 */ 
size_t abb_cantidad(abb_t *arbol);

/* Se destruye los nodos del arbol, si la funcion destruir_dato es distinto de NULL se elimina el dato.
 * Se destruye la estructura del arbol.
 * Pre: El arbol fue creado.
 * Post: El arbol fue destruido.
 */ 
void abb_destruir(abb_t *arbol);

 /* *****************************************************************
 *               PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
 
/* Crea el iterador del arbol y almacena de forma inorder los nodos del arbol en la pila. 
 * En caso de error devuelve NULL.
 * Pre: El arbol fue creado.
 * Post: El iterador fue creado.
 */ 
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

/* Crea un iterador del arbol y almacena por fecha hasta el primer nodo donde es menor a desde en la pila.
 * Luego, quedan almacenados en la pila aquellos valores mayores o iguales a desde.
 * Pre: el arbol fue creado.
 * Posr: El iterador fue creado 
*/
abb_iter_t* abb_iter_in_crear_por_fecha(const abb_t* arbol, const char* desde, const char* hasta, abb_comparar_clave_t cmp);

/* Para avanzar verifica que el iterador no este al final, si lo esta retorno false.
 * Se desapila el tope de la pila, apilo el hijo derecho del desapilado y a partir de este
 * todos los hijos izquierdos.
 * Pre: El iterador fue creado.
 * Post: Avanza el iterador.
 */ 
bool abb_iter_in_avanzar(abb_iter_t *iter);

/* Veo el tope de la pila.
 * Pre: El iterador fue creado.
 * Post: Se devuelve la clave del actual.
 */
void *abb_iter_in_ver_actual(const abb_iter_t *iter);

/* Verifica que el iterador no este al final, es decir que la pila no este vacia.
 * Pre: El iterador fue creado.
 * Post: El iterador esta al final o no.
 */
bool abb_iter_in_al_final(const abb_iter_t *iter);

/* Destruye la pila y el iterador.
 * Pre: El iterador fue creado.
 * Post: El iterador fue destruido.
 */
void abb_iter_in_destruir(abb_iter_t* iter);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/
/* Se mueve dentro del arbol de forma inorder, donde la raiz se procesa entre los hijos.
 * La funcion visitar devuelve true para seguir avanzando o false para terminar.
 * Pre: El arbol fue creado.
 */
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

#endif // ABB_H
