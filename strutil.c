#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include <stdlib.h>
#include <string.h>

size_t strutil_contar_longitud_cadenas(char** strv){
	size_t tamanio = 0, i = 0;
	while(strv[i] != NULL){
		tamanio += strlen(strv[i]);
		i++;
	}
	return tamanio+i;
}

void strutil_copiar_cadena_join(char* cadena_origen, char* cadena_destino, size_t pos_inicio, size_t pos_fin){
	for(size_t i = pos_inicio; i < pos_fin; i++){
		cadena_destino[i] = cadena_origen[i-pos_inicio];
	}
}

char* join(char** strv, char sep){
	if(!strv) return NULL;
	size_t tamanio_cadena_resultante = strutil_contar_longitud_cadenas(strv);
	char* cadena_resultante = calloc(tamanio_cadena_resultante+1, sizeof(char));
	if(!cadena_resultante) return NULL;

	char* separador_dup = malloc(sizeof(char));
	if(!separador_dup){
		free(cadena_resultante);
		return NULL;
	}
	*separador_dup = sep;
	size_t pos_inicio = 0, pos_fin = 0, i = 0;
	while(strv[i] != NULL){
		if(i > 0){
			cadena_resultante[pos_fin] = *separador_dup;
		}
		pos_fin = pos_inicio + strlen(strv[i]);
		strutil_copiar_cadena_join(strv[i], cadena_resultante, pos_inicio, pos_fin);
		pos_inicio = pos_fin+1;
		i++;
	}
	free(separador_dup);
	return cadena_resultante;
}

size_t strutil_contar_cadenas(const char* str, char sep){
	size_t cantidad = 0;
	for(size_t i = 0; str[i] != '\0'; i++){
		if(str[i] == sep) cantidad++;
	}
	return cantidad+1;
}

char* strutil_obtener_cadena(const char* cadena_origen, size_t inicio, size_t fin){
	size_t tamanio = fin-inicio;
	if(tamanio == 0) return strdup("\0");
	char* cadena_resultante = malloc(sizeof(char)*(tamanio+1));
	if(!cadena_resultante) return NULL;

	for(size_t i = 0; i < tamanio; i++){
		cadena_resultante[i] = cadena_origen[i+inicio];
	}
	cadena_resultante[tamanio] = '\0';

	return cadena_resultante;
}

char** split(const char* str, char sep){
	if(sep == '\0'){
		char** arreglo_de_cadenas = calloc(2,sizeof(char*));
		arreglo_de_cadenas[0] = strdup(str);
		arreglo_de_cadenas[1] = NULL;

		return arreglo_de_cadenas;
	}
	size_t cantidad_cadenas = strutil_contar_cadenas(str, sep);
	char** arreglo_de_cadenas = calloc(cantidad_cadenas+1,sizeof(char*));
	if(!arreglo_de_cadenas) return NULL;

	size_t inicio = 0, pos_cadena = 0, tamanio_cadena = strlen(str)+1;
	for(size_t i = 0; i < tamanio_cadena; i++){
		if(str[i] == sep || str[i] == '\0'){
			arreglo_de_cadenas[pos_cadena] = strutil_obtener_cadena(str, inicio, i);
			if(!arreglo_de_cadenas[pos_cadena]){
				free_strv(arreglo_de_cadenas);
				return NULL;
			}
			inicio = i+1;
			pos_cadena++;
		}
	}
	arreglo_de_cadenas[cantidad_cadenas] = NULL;
	return arreglo_de_cadenas;
}

void free_strv(char* strv[]){
	size_t i = 0;
	while(strv[i] != NULL){
		free(strv[i]);
		i++;
	}
	free(strv);
}