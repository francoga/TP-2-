#include "pila.h"
#include <stdlib.h>

const int TAMANIO_INICIAL= 20;
const int CONSTANTE_AUMENTO= 2;
const int CONSTANTE_REDUCCION= 2;
const int VERIFICACION_DE_REDUCCION= 4;

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

bool redimensionar_pila(pila_t* pila, size_t tam_nuevo){
	void** aux= realloc(pila->datos, tam_nuevo * sizeof(void*));
	
	if(aux == NULL) {
		return false;
	}
	
	pila->datos= aux;
	pila->capacidad= tam_nuevo;
	return true;
}


/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...

pila_t* pila_crear(void) {
	pila_t* pila= malloc(sizeof(pila_t));
	
	if(pila == NULL) {
		return NULL;
	}
	
	pila->datos= malloc(TAMANIO_INICIAL*sizeof(void*));
	
	if(pila->datos == NULL) {
		free(pila);
		return NULL;
	}
	
	pila->capacidad= TAMANIO_INICIAL;
	pila->cantidad= 0;
	return pila;
}

void pila_destruir(pila_t *pila) {
	free(pila->datos);
	free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
	return pila->cantidad == 0;
}

bool pila_apilar(pila_t *pila, void* valor) {
	if(pila->capacidad == pila->cantidad) {
		if(!redimensionar_pila(pila, pila->capacidad * CONSTANTE_AUMENTO)) {
			return false;
		}
	}
	pila->datos[pila->cantidad]= valor;
	pila->cantidad += 1;	
	return true;
}

void* pila_ver_tope(const pila_t *pila) {
	if(pila_esta_vacia(pila)) {
		return NULL;
	}
	return pila->datos[pila->cantidad-1];
}

void* pila_desapilar(pila_t *pila) {
	if(pila_esta_vacia(pila)) {
		return NULL;
	}
	
	if(pila->cantidad == (pila->capacidad / VERIFICACION_DE_REDUCCION) && TAMANIO_INICIAL < pila->capacidad / VERIFICACION_DE_REDUCCION) {
		if(!redimensionar_pila(pila, pila->capacidad / CONSTANTE_REDUCCION)) {
			return NULL;
		}
	}
	
	void* tope= pila_ver_tope(pila);
	pila->cantidad -= 1;
	return tope;
}
