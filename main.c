#include "registro_vuelos.h"

// Programa principal 
int main(int argc, char* argv[]) {
	char* linea= NULL;
	size_t capacidad= 0;

	abb_t* arbol = abb_crear(strcmp, NULL);
	hash_t* dicc= hash_crear((hash_destruir_dato_t)registro_destruir);
	while(getline(&linea, &capacidad, stdin) >= 0){
		char *puntero_salto_linea = strchr(linea, SALTO_LINEA);
		if(puntero_salto_linea != NULL){
			*puntero_salto_linea = '\0';
		}
		char** parametros= split(linea, ' ');
		int cantidad_parametros = contar_parametros(parametros);
		if(strcmp(parametros[0],COMANDO_AGREGAR) == 0){
			const char* nombre_archivo_entrada= parametros[1];
			FILE* archivo_entrada = fopen(nombre_archivo_entrada, "rt");
			if(!archivo_entrada || cantidad_parametros != 2){
				fprintf(stderr, "Error en comando %s\n", COMANDO_AGREGAR);
			}else {
				procesar_archivo(archivo_entrada, arbol, dicc);
				fprintf(stdout, "OK\n");
			}
		}else if(strcmp(parametros[0],COMANDO_VER_TABLERO) == 0) {
			if(cantidad_parametros != 5 || !verificar_parametros_ver_tablero(parametros)) {
				fprintf(stderr, "Error en comando %s\n", COMANDO_VER_TABLERO);
			}else {
				tablero_t* nuevo_tablero = crear_tablero(parametros);
				procesar_arbol_para_ver_tablero(arbol, nuevo_tablero);
				imprimir_ver_tablero(nuevo_tablero);
				free(nuevo_tablero);
				fprintf(stdout, "OK\n");
			}
		}else if(strcmp(parametros[0],COMANDO_INFO_VUELO) == 0) {
			char* numero_vuelo = parametros[1];
			if(cantidad_parametros != 2 || !hash_pertenece(dicc, numero_vuelo)) {
				fprintf(stderr, "Error en comando %s\n", COMANDO_INFO_VUELO);
			}else {
				imprimir_info_vuelo(dicc, numero_vuelo);
				fprintf(stdout, "OK\n");
			}
		}else if(strcmp(parametros[0],COMANDO_PRIORIDAD_VUELOS) == 0) {
			int cantidad_prio = atoi(parametros[1]);
			if(cantidad_parametros != 2 || cantidad_prio <= 0) {
				fprintf(stderr, "Error en comando %s\n", COMANDO_PRIORIDAD_VUELOS);
			}else { 
				heap_t* heap_prioridad= heap_crear(funcion_comparacion);
				hash_iter_t* hash_iterador= hash_iter_crear(dicc);
				pila_t* pila_vuelos = pila_crear();
				prioridad_vuelos(dicc, hash_iterador, heap_prioridad, cantidad_prio);
				ordenar_vuelos(heap_prioridad, pila_vuelos);
				imprimir_prioridad(pila_vuelos);
				heap_destruir(heap_prioridad, NULL);
				fprintf(stdout, "OK\n");
			} 
		}else if(strcmp(parametros[0],COMANDO_BORRAR) == 0) {
			if(cantidad_parametros != 3 || !verificar_parametros_borrar(parametros)) {
				fprintf(stderr, "Error en comando %s\n", COMANDO_BORRAR);
			}else {
				lista_t* lista_borrar= lista_crear();
				procesar_arbol_para_borrar(arbol, lista_borrar, parametros[1], parametros[2]);
				// Imprimo por pantalla
				imprimir_elementos_borrados(lista_borrar, arbol, dicc);
				fprintf(stdout, "OK\n");	
			}
		}
		free_strv(parametros);
	}
	free(linea);
	hash_destruir(dicc);
	abb_destruir(arbol);
	return 0;
}
