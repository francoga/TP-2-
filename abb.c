#include "abb.h"
#include "pila.h"
#include <stdlib.h>
#include <string.h>

/* Definicion del struct abb_nodo*/
typedef struct abb_nodo{
	void* dato;
	char* clave;
	struct abb_nodo* izq;
	struct abb_nodo* der;
}abb_nodo_t;

/* Definicion del struct abb*/
struct abb{
	abb_nodo_t* raiz;
	size_t cantidad;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
};

/* Definicion del struct abb_iter*/
struct abb_iter{
	const abb_t* arbol;
	pila_t *pila;
	const char* desde;
	const char* hasta;
	abb_comparar_clave_t cmp;
};

/* ******************************************************************
 *                     FUNCIONES AUXILIARES
 * *****************************************************************/
char *duplicar_clave(const char *clave){
	size_t largo= strlen(clave) + 1;
	char *dup_clave = malloc(sizeof(char)*largo);
	if(dup_clave == NULL){
		return NULL;
	}
	strcpy(dup_clave, clave);
	return dup_clave;
}

abb_nodo_t* abb_nodo_crear(const char* clave, void* dato){
	abb_nodo_t* nuevo_nodo = malloc(sizeof(abb_nodo_t));

	if(!nuevo_nodo) return NULL;
	nuevo_nodo->clave = duplicar_clave(clave);
	
	if(!nuevo_nodo->clave) return NULL;
	
	nuevo_nodo->dato = dato;
	nuevo_nodo->izq = NULL;
	nuevo_nodo->der = NULL;
	return nuevo_nodo;
}

abb_nodo_t* abb_buscar_padre(abb_nodo_t* padre, const char* clave, abb_comparar_clave_t cmp){
	if(!padre) return NULL;

	if(cmp(padre->clave, clave) < 0){
		if(!padre->der) {
			return padre;
		}else if(cmp(padre->der->clave, clave) == 0) {
			return padre;
		}
		return abb_buscar_padre(padre->der, clave, cmp);
	}else if(cmp(padre->clave, clave) > 0){
		if(!padre->izq) {
			return padre;
		}else if(cmp(padre->izq->clave, clave) == 0) {
			return padre;
		}
		return abb_buscar_padre(padre->izq, clave, cmp);
	}
	return padre;
}

abb_nodo_t* abb_buscar_nodo(abb_nodo_t* padre, const char* clave, abb_comparar_clave_t cmp){
	if(!padre) return NULL;

	if(cmp(padre->clave, clave) == 0){
		return padre;
	}else if(cmp(padre->clave, clave) < 0){
		return abb_buscar_nodo(padre->der, clave, cmp);
	}
	return abb_buscar_nodo(padre->izq, clave, cmp);
}

void abb_swap_nodos(abb_nodo_t* padre, abb_nodo_t* hijo){
	char* clave_aux= padre->clave;
	void* dato_aux= padre->dato;
	padre->clave= hijo->clave;
	padre->dato= hijo->dato;
	hijo->clave= clave_aux;
	hijo->dato= dato_aux;
}

void* abb_borrar_nodo(abb_nodo_t* nodo_borrar) {
	void* dato= nodo_borrar->dato;
	free(nodo_borrar->clave);
	free(nodo_borrar);
	return dato;
}

void* abb_borrar_hoja(abb_t* arbol, abb_nodo_t* padre ,abb_nodo_t* hijo){
	if(hijo == arbol->raiz) {
		arbol->raiz= NULL;
	}
	if(padre->der == hijo){
		padre->der = NULL;
	}else{
		padre->izq = NULL;
	}
	return abb_borrar_nodo(hijo);
}

void abb_actualizar_nodo(abb_nodo_t* padre, abb_nodo_t* hijo,  abb_destruir_dato_t destruir_dato){
	abb_swap_nodos(padre, hijo);
	void* dato = abb_borrar_nodo(hijo);

	if(destruir_dato != NULL) destruir_dato(dato);
}

abb_nodo_t* abb_buscar_hijo(abb_nodo_t* padre) {
	if(padre->izq != NULL) return padre->izq;
	return padre->der;
}

bool abb_es_nodo_hoja(abb_nodo_t* nodo_actual) {
	if(nodo_actual->izq == NULL && nodo_actual->der == NULL) {
		return true;
	}
	return false;
}

bool abb_es_nodo_un_hijo(abb_nodo_t* nodo_actual) {
	if(nodo_actual->izq != NULL && nodo_actual->der == NULL) {
		return true;
	}else if(nodo_actual->der != NULL && nodo_actual->izq == NULL){
		return true;
	}
	return false;
}

void* abb_borrar_un_hijo(abb_t* arbol, abb_nodo_t* nodo_padre, abb_nodo_t* nodo_hijo){
	if(nodo_hijo == arbol->raiz) {
		arbol->raiz= abb_buscar_hijo(nodo_hijo);
	}
	
	if(arbol->cmp(nodo_padre->clave, nodo_hijo->clave) < 0) {
		nodo_padre->der= abb_buscar_hijo(nodo_hijo);
	}else if(arbol->cmp(nodo_padre->clave, nodo_hijo->clave) > 0) {
		nodo_padre->izq= abb_buscar_hijo(nodo_hijo);
	}
	return abb_borrar_nodo(nodo_hijo);
}

bool abb_es_nodo_dos_hijos(abb_nodo_t* nodo_hijo){
	if(nodo_hijo->izq != NULL && nodo_hijo->der != NULL) return true;
	return false;
}

abb_nodo_t* abb_buscar_reemplazante(abb_nodo_t* padre){
	abb_nodo_t* nodo_actual = padre->der;
	while(nodo_actual->izq != NULL){
		nodo_actual = nodo_actual->izq;
	}
	return nodo_actual;
}

void* abb_borrar_dos_hijos(abb_t* arbol, abb_nodo_t* nodo_padre, abb_nodo_t* nodo_hijo){
	abb_nodo_t* reemplazante = abb_buscar_reemplazante(nodo_hijo);
	abb_nodo_t* padre_reemplazante = abb_buscar_padre(nodo_hijo, reemplazante->clave, arbol->cmp);
	abb_swap_nodos(nodo_hijo, reemplazante);
	
	/* Si el reemplazante es el hijo derecho del nodo que tengo que borrar*/
	if(nodo_hijo->der == reemplazante){
		padre_reemplazante->der= reemplazante->der;
	}else{
		padre_reemplazante->izq= reemplazante->der;
	}
	return abb_borrar_nodo(reemplazante);
}

bool _abb_guardar(abb_t *arbol, abb_nodo_t* padre, abb_nodo_t* nuevo_nodo) {
	if(arbol->cmp(padre->clave, nuevo_nodo->clave) < 0){
		if(!padre->der) {
			padre->der = nuevo_nodo;
			arbol->cantidad++;
			return true;
		}else{ 
			return _abb_guardar(arbol, padre->der, nuevo_nodo);
		}
	}else if(arbol->cmp(padre->clave, nuevo_nodo->clave) > 0){
		if(!padre->izq) {
			padre->izq = nuevo_nodo;
			arbol->cantidad++;
			return true;
		}else {
			return _abb_guardar(arbol, padre->izq, nuevo_nodo);
		}
	}else{// si los nodos son iguales
		abb_actualizar_nodo(padre, nuevo_nodo, arbol->destruir_dato);
	}
	return true;
}

abb_nodo_t* abb_obtener_nodo(abb_nodo_t* padre, const char* desde, const char* hasta, abb_comparar_clave_t cmp) {
	if(!padre) return NULL;

	if(cmp(padre->clave, desde) >= 0 && cmp(padre->clave, hasta) <= 0){
		return padre;
	}else if(cmp(padre->clave, desde) < 0){
		return abb_obtener_nodo(padre->der, desde, hasta, cmp);
	}
	return abb_obtener_nodo(padre->izq, desde, hasta, cmp);
}

/* ******************************************************************
 *                     PRIMITIVAS
 * *****************************************************************/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* nuevo_abb = malloc(sizeof(abb_t));

	if(!nuevo_abb) return NULL;
	nuevo_abb->raiz = NULL;
	nuevo_abb->cantidad = 0;
	nuevo_abb->cmp = cmp;
	nuevo_abb->destruir_dato = destruir_dato;
	return nuevo_abb;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){

	abb_nodo_t* nuevo_nodo = abb_nodo_crear(clave, dato);
	if(!nuevo_nodo) return false;

	if(!arbol->raiz){
		arbol->raiz = nuevo_nodo;
		arbol->cantidad++;
		return true;
	}
	return _abb_guardar(arbol, arbol->raiz, nuevo_nodo);
}

void *abb_borrar(abb_t *arbol, const char *clave){
	if(!arbol->raiz || !abb_pertenece(arbol, clave)) return NULL;

	abb_nodo_t* nodo_hijo = abb_buscar_nodo(arbol->raiz, clave, arbol->cmp); //nodo a borrar
	abb_nodo_t* nodo_padre = abb_buscar_padre(arbol->raiz, clave, arbol->cmp);
	void* dato = NULL;

	if(abb_es_nodo_hoja(nodo_hijo)){
		dato = abb_borrar_hoja(arbol, nodo_padre, nodo_hijo);
	}else if(abb_es_nodo_un_hijo(nodo_hijo)){
		dato = abb_borrar_un_hijo(arbol, nodo_padre, nodo_hijo);
	}else if(abb_es_nodo_dos_hijos(nodo_hijo)){
		dato = abb_borrar_dos_hijos(arbol, nodo_padre, nodo_hijo);
	}
	arbol->cantidad--;
	return dato;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_buscar_nodo(arbol->raiz, clave, arbol->cmp);

	if(!nodo) return NULL;
	return nodo;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_buscar_nodo(arbol->raiz, clave, arbol->cmp);

	if(!nodo) return false;
	return true;
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
}

void _abb_destruir(abb_t* arbol, abb_nodo_t* nodo_actual) {
	if(nodo_actual == NULL) return;

	_abb_destruir(arbol, nodo_actual->izq);
	_abb_destruir(arbol, nodo_actual->der);
	void* dato= abb_borrar_nodo(nodo_actual);
	
	if(arbol->destruir_dato != NULL) arbol->destruir_dato(dato);
}

void abb_destruir(abb_t *arbol){
	_abb_destruir(arbol, arbol->raiz);
	free(arbol);
}

/* *****************************************************************
 *               PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(!iter) return NULL;
	iter->pila = pila_crear();
	if(!iter->pila){
		free(iter);
		return NULL;
	}
	iter->arbol = arbol;
	
	abb_nodo_t* actual = arbol->raiz;
	while(actual != NULL) {
		pila_apilar(iter->pila, actual);
		actual = actual->izq;
	}
	return iter;
}
// Funcion auxiliar que utiliza abb_iter_in_crear_por_fecha para poder avanzar.
void abb_iter_avanzar_izquierda(abb_nodo_t* nodo, abb_iter_t* iter) {
	abb_nodo_t* actual= nodo;
	bool menor_a_desde= false; 
	
	while(actual->izq != NULL && !menor_a_desde) {
		menor_a_desde= iter->cmp(actual->izq->clave, iter->desde) < 0;
		pila_apilar(iter->pila, actual->izq);
		if(menor_a_desde != 0) {
			abb_iter_in_avanzar(iter);
		}
		actual= actual->izq;
	}
}

abb_iter_t* abb_iter_in_crear_por_fecha(const abb_t* arbol, const char* desde, const char* hasta, abb_comparar_clave_t cmp) {
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(!iter) return NULL;
	iter->pila = pila_crear();
	if(!iter->pila){
		free(iter);
		return NULL;
	}
	iter->arbol = arbol;
	iter->desde= desde;
	iter->hasta= hasta;
	iter->cmp= cmp;
	
	abb_nodo_t* actual= arbol->raiz;
	pila_apilar(iter->pila, actual);
	abb_iter_avanzar_izquierda(actual, iter);
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter)) return false;

	abb_nodo_t* desapilado = pila_desapilar(iter->pila);
	if(desapilado->der != NULL) {
		pila_apilar(iter->pila, desapilado->der);
		abb_nodo_t* actual = desapilado->der;
		abb_iter_avanzar_izquierda(actual, iter);
	}
	return true;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->pila);
}

void *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if(abb_iter_in_al_final(iter)) return NULL;
	
	abb_nodo_t* tope = pila_ver_tope(iter->pila);
	return tope->dato;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/
bool abb_iterador(abb_nodo_t* nodo_actual, bool visitar(const char *, void *, void *), void* extra){
	if(!nodo_actual) return true;

	if(!abb_iterador(nodo_actual->izq, visitar, extra)) return false;
	if(!visitar(nodo_actual->clave, nodo_actual->dato, extra)) return false;
	if(!abb_iterador(nodo_actual->der, visitar, extra)) return false;

	return true;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
	abb_iterador(arbol->raiz, visitar, extra);
}
