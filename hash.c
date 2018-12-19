#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>

#define TAMANIO_INICIAL 20
#define FACTOR_DE_CARGA_AUMENTO 0.72
#define FACTOR_DE_CARGA_REDUCCION 0.5
#define HASH_CONSTANTE_DE_AUMENTO 3
#define HASH_CONSTANTE_DE_REDUCCION 2

/* DEFINICION ESTRUCTURAS */
struct hash{
	lista_t** tabla;
	size_t cantidad;
	size_t capacidad;
	hash_destruir_dato_t destruir_dato;
};

typedef struct nodo_hash{
	 const char* clave;
	 void* dato;
}nodo_hash_t;
 
 /* Definición del struct hash iterador.*/
struct hash_iter{
	 size_t pos_actual;
	 const hash_t* hash;
	 lista_iter_t* iter_actual;
 };
 
 /* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/
size_t hash_iter_buscar_siguiente(lista_t** tabla, size_t pos_inicio, size_t capacidad){
	size_t nueva_posicion = capacidad;
	bool encontro_siguiente = false;
	for (size_t i = pos_inicio; i < capacidad && !encontro_siguiente; i++){
		encontro_siguiente = (tabla[i] != NULL);
		if(encontro_siguiente) nueva_posicion = i;
	}
	if(!encontro_siguiente && pos_inicio == 0) nueva_posicion = 0;

	return nueva_posicion;
}

char* hash_duplicar_clave(const char* clave){
	size_t largo = strlen(clave) + 1;
	char *dup_clave = malloc(sizeof(char)*largo);
	if(!dup_clave) return NULL;

	strcpy(dup_clave, clave);
	return dup_clave;
}

nodo_hash_t* hash_nodo_crear(const char* clave, void* dato){
	nodo_hash_t* nuevo_nodo = malloc(sizeof(nodo_hash_t));
	if(!nuevo_nodo) return NULL;

	nuevo_nodo->clave = hash_duplicar_clave(clave);
	nuevo_nodo->dato = dato;
	return nuevo_nodo;
}

//FUNCION DE HASHING
size_t hashing(const char* clave, size_t tamanio_tabla) {
	//unsigned char *p = clave;
	unsigned long number = 5381;
	int c;
	while ((c = *clave++)){
		number = ((number << 5) + number) + c;
	}
	return (number % tamanio_tabla);
}

 void eliminar_listas_del_hash(hash_t* hash) {
	 lista_t** tabla_vieja = hash->tabla;

	for(int i = 0; i < hash->capacidad; i++){
		if(tabla_vieja[i] != NULL){
			lista_destruir(tabla_vieja[i], NULL);
		}
	}
	free(tabla_vieja);
 }
 
/* Funcion que redimensiona el hash, aumentando el tamanio de la tabla o 
 * disminuyendo sin superar el tamanio original*/ 
bool hash_redimensionar(hash_t* hash, size_t tamanio_nuevo){
	
	lista_t** tabla_nueva= calloc(tamanio_nuevo,sizeof(lista_t*));
	/* Si malloc devuelve NULL para la pedida de memoria de la nueva tabla
	 * o si el nuevo tamanio para redimensionar es menor al inicial 
	 * retornamos false*/
	if(tabla_nueva == NULL || tamanio_nuevo < TAMANIO_INICIAL){
		free(tabla_nueva);
		return false;
	}

	for(int i= 0; i < hash->capacidad; i++){
		if(hash->tabla[i] != NULL){
			lista_iter_t* iterador= lista_iter_crear(hash->tabla[i]);
			nodo_hash_t* nodo_actual= NULL;
			
			if(iterador == NULL) return false;
			
			while(!lista_iter_al_final(iterador)){
				nodo_actual= lista_iter_ver_actual(iterador);
				size_t nueva_posicion= hashing(nodo_actual->clave, tamanio_nuevo);
				/* Si en nueva posicion no tiene una lista la creo*/
				if(!tabla_nueva[nueva_posicion]){
					tabla_nueva[nueva_posicion]= lista_crear();
				}

				if(!lista_insertar_ultimo(tabla_nueva[nueva_posicion], nodo_actual)){
					lista_iter_destruir(iterador);
					return false;
				}
				lista_iter_avanzar(iterador);
			}
			lista_iter_destruir(iterador);
		}
	}
	/* Borro las listas vacias del hash viejo*/
	eliminar_listas_del_hash(hash);
	
	hash->tabla = tabla_nueva;
	hash->capacidad = tamanio_nuevo;
	return true;
}

nodo_hash_t* hash_buscar_nodo(lista_t *lista_actual, const char *clave){

	lista_iter_t* iterador = lista_iter_crear(lista_actual);
	while(!lista_iter_al_final(iterador)){
		nodo_hash_t* nodo_actual = lista_iter_ver_actual(iterador);
		if(strcmp(nodo_actual->clave, clave) == 0){
			lista_iter_destruir(iterador);
			return nodo_actual;
		}
		lista_iter_avanzar(iterador);
	}
	lista_iter_destruir(iterador);
	return NULL;
}

// Calcula el factor de carga
float hash_calcular_factor_de_carga(size_t cantidad, size_t capacidad) {
	return ((float)cantidad/(float)capacidad);
}

/* ******************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/
hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash == NULL) return NULL;

	hash->tabla = calloc(TAMANIO_INICIAL, sizeof(lista_t*));
	if(hash->tabla == NULL){
		free(hash);
		return NULL;
	}
	hash->capacidad = TAMANIO_INICIAL;
	hash->cantidad = 0;
	hash->destruir_dato = destruir_dato;
	return hash;
}

bool hash_guardar(hash_t* hash, const char *clave, void *dato){
	
	if (hash_calcular_factor_de_carga(hash->cantidad, hash->capacidad) > FACTOR_DE_CARGA_AUMENTO){
		if(!hash_redimensionar(hash, hash->capacidad * HASH_CONSTANTE_DE_AUMENTO) && hash->cantidad == hash->capacidad){
			return false;
		}
	}
	
	size_t posicion_hash = hashing(clave, hash->capacidad);
	
	if(!hash->tabla[posicion_hash]) {
		hash->tabla[posicion_hash] = lista_crear();
		if(!hash->tabla[posicion_hash]) return false;
	}

	if(hash->tabla[posicion_hash]){
		//BUSCAR Y REEMPLAZAR
		nodo_hash_t* nodo_a_actualizar = hash_buscar_nodo(hash->tabla[posicion_hash], clave);
		if(nodo_a_actualizar){
			if(hash->destruir_dato != NULL) {
				hash->destruir_dato(nodo_a_actualizar->dato);
			}
			nodo_a_actualizar->dato = dato;
			return true;
		}else {/* El nodo no esta en la lista, entonces tengo que crear un nuevo nodo e insertar a la lista*/
			nodo_hash_t* nuevo_nodo = hash_nodo_crear(clave, dato);
			if(!nuevo_nodo) return false;

			if(!lista_insertar_ultimo(hash->tabla[posicion_hash], nuevo_nodo)){
				free((char*)nuevo_nodo->clave);
				free(nuevo_nodo);
				return false;
			}
			hash->cantidad++;
			return true;
		}
	}
	return true;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
	size_t posicion_hash= hashing(clave, hash->capacidad);
	
	if(hash->tabla[posicion_hash] == NULL){
		return false;
	}
	nodo_hash_t* nodo= hash_buscar_nodo(hash->tabla[posicion_hash], clave);
	
	if(nodo != NULL) return true;
	return false;
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cantidad;
}

void hash_destruir(hash_t *hash) {
	
	for(int i= 0; i < hash->capacidad; i++) {
		if(hash->tabla[i] != NULL) {
			lista_t* lista_actual= hash->tabla[i];
			while(!lista_esta_vacia(lista_actual)) {
				nodo_hash_t* nodo_actual= lista_borrar_primero(lista_actual);
				if(hash->destruir_dato != NULL) {
					hash->destruir_dato(nodo_actual->dato);
				}
				free((char*)nodo_actual->clave);
				free(nodo_actual);
			}
		}
		free(hash->tabla[i]);
	}
	free(hash->tabla);
	free(hash);
}

void* hash_borrar(hash_t *hash, const char *clave) {
 
	size_t posicion = hashing(clave, hash->capacidad);
	nodo_hash_t* nodo_eliminar = NULL;
	void* dato= NULL;
	
	if(hash->tabla[posicion] == NULL) return NULL;
	
	nodo_eliminar= hash_buscar_nodo(hash->tabla[posicion], clave);
	
	if(nodo_eliminar == NULL) return NULL;
	
	lista_iter_t* iterador= lista_iter_crear(hash->tabla[posicion]);
	while(!lista_iter_al_final(iterador)) {
		
		if(nodo_eliminar == (nodo_hash_t*)lista_iter_ver_actual(iterador)) {
			lista_iter_borrar(iterador);
			dato= nodo_eliminar->dato;
			hash->cantidad--;
			free((char*)nodo_eliminar->clave);
			free(nodo_eliminar);
		}
		lista_iter_avanzar(iterador);
	}
	lista_iter_destruir(iterador);

	if(hash_calcular_factor_de_carga(hash->cantidad, hash->capacidad) < FACTOR_DE_CARGA_REDUCCION){
		hash_redimensionar(hash, hash->capacidad/HASH_CONSTANTE_DE_REDUCCION);
	}
	return dato;
} 

void* hash_obtener(const hash_t *hash, const char *clave) {
	
	if(hash->cantidad == 0) return NULL;
	
	size_t posicion= hashing(clave, hash->capacidad);
	nodo_hash_t* nodo= hash_buscar_nodo(hash->tabla[posicion], clave);
		
	if(nodo == NULL) return NULL;
	return nodo->dato;
}
	
/* ******************************************************************
 *              PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iterador = malloc(sizeof(hash_iter_t));
	if(!iterador) return NULL;

	iterador->hash = hash;
	iterador->pos_actual = hash_iter_buscar_siguiente(hash->tabla, 0, hash->capacidad);
	iterador->iter_actual = NULL;
	if(iterador->hash->tabla[iterador->pos_actual] != NULL){
		iterador->iter_actual = lista_iter_crear(iterador->hash->tabla[iterador->pos_actual]);
	}
	return iterador;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	bool avanzo = false;
	if(iter->iter_actual != NULL){
		avanzo = lista_iter_avanzar(iter->iter_actual);
		if(lista_iter_al_final(iter->iter_actual)){
			lista_iter_destruir(iter->iter_actual);
			iter->pos_actual = hash_iter_buscar_siguiente(iter->hash->tabla, iter->pos_actual+1, iter->hash->capacidad);
			if(iter->pos_actual == iter->hash->capacidad){
				iter->iter_actual = NULL;
				return false;
			}
			iter->iter_actual = lista_iter_crear(iter->hash->tabla[iter->pos_actual]);
		}
	}
	return avanzo;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(!iter->iter_actual) return NULL;
	if(lista_iter_al_final(iter->iter_actual)) return NULL;

	nodo_hash_t* nodo_actual = lista_iter_ver_actual(iter->iter_actual);
	return nodo_actual->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if(!iter->iter_actual) return true;
	
	size_t pos_siguiente= hash_iter_buscar_siguiente(iter->hash->tabla, iter->pos_actual+1, iter->hash->capacidad);
	if(lista_iter_al_final(iter->iter_actual) && pos_siguiente == iter->hash->capacidad) return true;

	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->iter_actual);
	free(iter);
}
