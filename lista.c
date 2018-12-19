#include "lista.h"
#include <stdlib.h>

/* Definición del struct nodo
 */
 typedef struct nodo{
	 void* dato;
	 struct nodo* prox;
 }nodo_t;
  
 /* Definición del struct lista.
 */
 struct lista{
	 nodo_t* primero;
	 nodo_t* ultimo;
	 size_t cantidad;
 }; 

 /* Definición del struct iterador externo.
 */
 struct lista_iter {
	 nodo_t* actual;
	 nodo_t*anterior;
	 lista_t* lista;
};

  /* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

nodo_t* nodo_crear(void* valor) {
	nodo_t* nodo= malloc(sizeof(nodo_t));
	
	if(nodo == NULL) return NULL;
	
	nodo->dato= valor;
	nodo->prox= NULL;
	return nodo;
}

  /* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

lista_t* lista_crear(void) {
	lista_t* lista= malloc(sizeof(lista_t));
	
	if(lista == NULL) return NULL;
	
	lista->primero= NULL;
	lista->ultimo= NULL;
	lista->cantidad= 0;
	return lista;
}

bool lista_esta_vacia(const lista_t* lista) {
	return lista->cantidad == 0;
}

bool lista_insertar_primero(lista_t* lista, void* dato) {
	nodo_t* nodo_nuevo= nodo_crear(dato);
	
	if(nodo_nuevo == NULL) return false;
	
	if(lista_esta_vacia(lista)) {
		lista->ultimo= nodo_nuevo;
	}
	nodo_nuevo->prox= lista->primero;
	lista->primero= nodo_nuevo;
	lista->cantidad++;
	return true;
}

bool lista_insertar_ultimo(lista_t* lista, void* dato) {
	nodo_t* nodo_nuevo= nodo_crear(dato);
	
	if(nodo_nuevo == NULL) return false;
	
	if(lista_esta_vacia(lista)) {
		lista->primero= nodo_nuevo;
	}else {
		lista->ultimo->prox= nodo_nuevo;
	}
	lista->ultimo= nodo_nuevo;
	lista->cantidad++;
	return true;
}

void* lista_borrar_primero(lista_t* lista) {
	if(lista_esta_vacia(lista)) return NULL;
	
	nodo_t* borrar= lista->primero;
	void* dato= lista->primero->dato;
	
	/* Si el primero y el ultimo son iguales, entonces la lista tiene un solo elemento,
	 * si lo borro la lista queda vacia.*/
	if(lista->primero == lista->ultimo) {
		lista->primero= NULL;
		lista->ultimo= NULL;
	}else {
		lista->primero= borrar->prox;
	}
	lista->cantidad--;
	free(borrar);
	return dato;
}

void* lista_ver_primero(const lista_t* lista) {
	if(lista_esta_vacia(lista)) return NULL;
	
	return lista->primero->dato;
}

void* lista_ver_ultimo(const lista_t* lista) {
	if(lista_esta_vacia(lista)) return NULL;
	
	return lista->ultimo->dato;
}

size_t lista_largo(const lista_t* lista) {
	return lista->cantidad;
}

void lista_destruir(lista_t* lista, void destruir_dato(void*)) {
	while(!lista_esta_vacia(lista)) {
		void* borrar= lista_borrar_primero(lista);
		
		if(destruir_dato != NULL) {
			destruir_dato(borrar);
		}
	}
	free(lista);
}

/* ******************************************************************
 *               PRIMITIVAS DE LA ITERADOR EXTERNO
 * *****************************************************************/
 
lista_iter_t* lista_iter_crear(lista_t* lista) {
	 lista_iter_t* lista_iter= malloc(sizeof(lista_iter_t));
	 
	 if(lista_iter == NULL) return NULL;
	 
	 lista_iter->actual= lista->primero;
	 lista_iter->anterior= NULL;
	 lista_iter->lista= lista;
	 return lista_iter;
}

bool lista_iter_al_final(const lista_iter_t* iter) {
	return iter->actual == NULL;
}

bool lista_iter_avanzar(lista_iter_t* iter) {
	if(lista_iter_al_final(iter)) return false;
	
	iter->anterior= iter->actual;
	iter->actual= iter->actual->prox;
	return true;
}

void* lista_iter_ver_actual(const lista_iter_t* iter) {
	if(lista_iter_al_final(iter)) return NULL;
	return iter->actual->dato;
}

void lista_iter_destruir(lista_iter_t* iter) {
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
	/*Si la lista esta vacia o cuando actual es el primero de la lista*/
	if(iter->actual == iter->lista->primero) {
		if(!lista_insertar_primero(iter->lista, dato)) return false;
		iter->actual= iter->lista->primero;
	}else if(lista_iter_al_final(iter)) {
		if(!lista_insertar_ultimo(iter->lista, dato)) return false;
		iter->actual= iter->lista->ultimo;
	}else {
		nodo_t* nodo_insertar= nodo_crear(dato);
		
		if(nodo_insertar == NULL) return false;
		
		iter->anterior->prox= nodo_insertar;
		nodo_insertar->prox= iter->actual;
		iter->actual= nodo_insertar;
		iter->lista->cantidad++;
	}
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter) {
	if(lista_iter_al_final(iter) || lista_esta_vacia(iter->lista)) return NULL;
	
	nodo_t* borrar= iter->actual;
	void* dato= iter->actual->dato;
	/* Cuando el iterador apunta al primer nodo de la lista*/
	if(iter->anterior == NULL) {
		iter->actual= borrar->prox;
		iter->lista->primero= iter->actual;
	}else {
		if(iter->actual->prox == NULL){/* Si el iterador esta apuntando al ultimo de la lista*/
			iter->lista->ultimo= iter->anterior;
		}
		/* Cuando el iterador no esta al inicio ni al final de la lista*/
		iter->anterior->prox= borrar->prox;
		iter->actual= borrar->prox;
	}
	iter->lista->cantidad--;
	free(borrar);
	return dato;
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/
 
 void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra) {
	 if(lista_esta_vacia(lista)) return;
	 
	 nodo_t* actual= lista->primero;
	 
	 while(actual != NULL) {
		 if(!visitar(actual->dato, extra)) return;
		 actual= actual->prox;
	 }
 }
