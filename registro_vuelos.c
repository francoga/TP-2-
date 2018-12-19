#include "registro_vuelos.h"

#define LONGITUD_STRING_FECHA 19

/* Definición del struct tablero.
 */ 
struct tablero {
	char* fecha_inicio;
	char* fecha_fin;
	char* modo;
	lista_t* lista_de_vuelos;
	size_t cantidad_vuelos;
};

 /* Definición del struct registro.
 */ 
struct registro {
	char* flight_number;
	char* priority;
	char* date;
	char* vuelo_completo;
};

int strcmp_fecha(const char* clave, const char* fecha_a_comparar) {
	return strncmp(clave, fecha_a_comparar, LONGITUD_STRING_FECHA);
}

char *duplicar_linea_registro(const char *linea){
	size_t largo = strlen(linea)+1;
	char *dup_linea = calloc(largo, sizeof(char));
	if(!dup_linea) return NULL;
	strcpy(dup_linea, linea);
	return dup_linea;
}

registro_t* registro_crear(char* linea){
	registro_t* nuevo_registro = malloc(sizeof(registro_t));
	if(!nuevo_registro) return NULL;
	
	//saco el salto de linea
	linea[strlen(linea)-1]= '\0';

	char** linea_procesada = split(linea, ',');
	nuevo_registro->flight_number = duplicar_linea_registro(linea_procesada[0]);
	nuevo_registro->priority = duplicar_linea_registro(linea_procesada[5]);
	nuevo_registro->date = duplicar_linea_registro(linea_procesada[6]);
	nuevo_registro->vuelo_completo= join(linea_procesada, ' ');
	free_strv(linea_procesada);
	return nuevo_registro;
}

int funcion_comparacion(const void* registro1,const void* registro2) {
	registro_t* registro_heap= (registro_t*)registro1;
	registro_t* registro_hash= (registro_t*)registro2;
	
	if(strcmp(registro_hash->priority, registro_heap->priority) == 0) {
		 return strcmp(registro_heap->flight_number, registro_hash->flight_number);
	}
	return strcmp(registro_hash->priority, registro_heap->priority);
}

void registro_destruir(registro_t* registro) { 
	free(registro->flight_number);
	free(registro->date);
	free(registro->priority);
	free(registro->vuelo_completo);
	free(registro);
}

// Creo la clave para el abb
char* crear_clave_abb(char* numero_vuelo, char* fecha) {
	size_t largo_total= strlen(numero_vuelo) + strlen(fecha) + 1;
	
	char* clave= calloc(largo_total, sizeof(char));
	
	if(!clave) return NULL;
	strcpy(clave, fecha);
	clave= strcat(clave, numero_vuelo);
	return clave;
}

bool verificar_parametros_ver_tablero(char** parametros) {
	int cantidad = atoi(parametros[1]);
	
	if(cantidad <= 0) return false;

	if(!(strcmp(parametros[2], "asc") == 0 || strcmp(parametros[2], "desc") == 0)) return false;

	if(strcmp(parametros[3], parametros[4]) > 0) return false;

	return true;
}

bool verificar_parametros_borrar(char** parametros) {
	if(strcmp(parametros[1], parametros[2]) > 0) return false;
	return true;
}

tablero_t* crear_tablero(char** parametros) {
	tablero_t* nuevo_tablero = malloc(sizeof(tablero_t));
	
	if(!nuevo_tablero) return NULL;
	nuevo_tablero->lista_de_vuelos = lista_crear();
	nuevo_tablero->fecha_inicio = parametros[3];
	nuevo_tablero->fecha_fin = parametros[4];
	nuevo_tablero->modo = parametros[2];
	nuevo_tablero->cantidad_vuelos = atoi(parametros[1]);
	return nuevo_tablero;
}

void prioridad_vuelos(hash_t* dicc, hash_iter_t* iterador, heap_t* heap_prioridad, size_t cant_vuelos) {
	while(!hash_iter_al_final(iterador)) {
		char* clave= (char*)hash_iter_ver_actual(iterador);
		registro_t* registro_actual= hash_obtener(dicc, clave);
	
		if(heap_cantidad(heap_prioridad) >= cant_vuelos) { 
			registro_t* registro_heap= heap_ver_max(heap_prioridad);

			if(funcion_comparacion(registro_heap, registro_actual) > 0) {
				// desencolo el maximo del heap
				heap_desencolar(heap_prioridad);
				//encolo un nuevo elemento al heap
				heap_encolar(heap_prioridad, registro_actual);
			}
		}else {// encolo k elementos al heap
			heap_encolar(heap_prioridad, registro_actual);
		}
		hash_iter_avanzar(iterador);
	}
	hash_iter_destruir(iterador);
}

bool ordenar_vuelos(heap_t* heap_prioridad, pila_t* pila_vuelos) {
	if(heap_esta_vacio(heap_prioridad)) return true;

	registro_t* registro = heap_desencolar(heap_prioridad);

	pila_apilar(pila_vuelos, registro);

	return ordenar_vuelos(heap_prioridad, pila_vuelos);
}

int contar_parametros(char** parametros){
	int contador = 0, posicion = 0;
	while(parametros[posicion] != NULL){
		if (strcmp(parametros[posicion], VACIO) != 0){
			contador++;
		}
		posicion++;
	}
	return contador;
}

void imprimir_prioridad(pila_t* pila_vuelos) {
	while(!pila_esta_vacia(pila_vuelos)) {
		registro_t* registro= pila_desapilar(pila_vuelos);
		fprintf(stdout, "%s - %s\n", registro->priority, registro->flight_number);
	}
	pila_destruir(pila_vuelos);
}

void procesar_arbol_para_borrar(abb_t* arbol, lista_t* lista_borrar, const char* fecha_inicio, const char* fecha_fin) {
	abb_iter_t* iterador= abb_iter_in_crear_por_fecha(arbol, fecha_inicio, fecha_fin, strcmp_fecha);
	
	if(!iterador) return;
	
	while(!abb_iter_in_al_final(iterador)) {
		registro_t* registro_actual = abb_iter_in_ver_actual(iterador);
		if(strcmp(fecha_inicio, registro_actual->date) <= 0 && strcmp(fecha_fin, registro_actual->date) >= 0) {
			lista_insertar_ultimo(lista_borrar, registro_actual);
		}
		abb_iter_in_avanzar(iterador);
	}
	abb_iter_in_destruir(iterador);	
}

void imprimir_elementos_borrados(lista_t* lista_borrar, abb_t* arbol, hash_t* dicc) {
	while(!lista_esta_vacia(lista_borrar)) {
		registro_t* registro_actual = lista_borrar_primero(lista_borrar);
		fprintf(stdout, "%s\n", registro_actual->vuelo_completo);
		char* clave= crear_clave_abb(registro_actual->flight_number, registro_actual->date);
		abb_borrar(arbol, clave);
		hash_borrar(dicc, registro_actual->flight_number);
		registro_destruir(registro_actual);
		free(clave);
	}
	lista_destruir(lista_borrar, NULL);
}

void imprimir_info_vuelo(hash_t* dicc, char* numero_vuelo) {
	registro_t* registro_actual = hash_obtener(dicc, numero_vuelo);
	fprintf(stdout, "%s\n", registro_actual->vuelo_completo);
}

bool procesar_arbol_para_ver_tablero(abb_t* arbol, tablero_t* tablero) {
	abb_iter_t* iterador= abb_iter_in_crear_por_fecha(arbol, tablero->fecha_inicio, tablero->fecha_fin, strcmp_fecha);
	
	if(!iterador) return false;
	
	while(!abb_iter_in_al_final(iterador)) {
		registro_t* registro = abb_iter_in_ver_actual(iterador);
		if(strcmp_fecha(registro->date, tablero->fecha_inicio) >= 0 && strcmp_fecha(registro->date, tablero->fecha_fin) <= 0) {
			if(strcmp(tablero->modo, "desc") == 0) {
				lista_insertar_primero(tablero->lista_de_vuelos, registro);
			}else if (strcmp(tablero->modo, "asc") == 0) {
				lista_insertar_ultimo(tablero->lista_de_vuelos, registro);
			}
		}
	}
	return true;
}

void imprimir_ver_tablero(tablero_t* nuevo_tablero) {
	size_t i= 0;
	while(!lista_esta_vacia(nuevo_tablero->lista_de_vuelos) && i < nuevo_tablero->cantidad_vuelos) {
		registro_t* registro_actual = lista_borrar_primero(nuevo_tablero->lista_de_vuelos);
		fprintf(stdout, "%s - %s\n", registro_actual->date, registro_actual->flight_number);
		i++;
	}
	lista_destruir(nuevo_tablero->lista_de_vuelos, NULL);
}

void procesar_archivo(FILE* archivo_entrada, abb_t* arbol, hash_t* dicc) {
	char* linea= NULL;
	size_t capacidad= 0;
	while(getline(&linea, &capacidad, archivo_entrada) != -1) {
		registro_t* nuevo_registro= registro_crear(linea);
		
		char* clave= crear_clave_abb(nuevo_registro->flight_number, nuevo_registro->date);
		
		if(hash_pertenece(dicc, nuevo_registro->flight_number)) { 
			registro_t* registro_viejo= hash_obtener(dicc, nuevo_registro->flight_number);
			char* clave_vieja= crear_clave_abb(nuevo_registro->flight_number, registro_viejo->date);
			
			// Elimino la clave vieja del arbol
			abb_borrar(arbol, clave_vieja);
			free(clave_vieja);
		}
		hash_guardar(dicc, nuevo_registro->flight_number, nuevo_registro);
		abb_guardar(arbol, clave, nuevo_registro);
		free(clave);
	}
	free(linea);
	fclose(archivo_entrada);
}
