#include "hash.h"
#include <string.h>
#include <stdio.h>

#define CAPACIDAD_MINIMA 3
#define FACTOR_CARGA_MAX 0.7

typedef enum estado { VACIO, OCUPADO, BORRADO } estado_t;

typedef struct valor {
	void *dato;
	char *clave;
	estado_t estado;
} valor_t;

struct hash {
	valor_t *tabla;
	size_t (*hash_func)(const char *);
	size_t ocupado;
	size_t capacidad;
};

char *my_strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *dup = malloc(len);
	if (dup)
		memcpy(dup, s, len);
	return dup;
}

size_t djb2_hash(const char *str)
{
	size_t hash = 5381;
	unsigned char c;

	while ((c = (unsigned char)*str++)) {
		hash = hash * 33 + c;
	}

	return hash;
}

/*
    Pre: El parametro "h" debe estar previamente inicializado con "hash_crear" o "hash_crear_con_funcion"
    Post: Libera toda la memoria de los pares clave valor de la tabla y la tabla propia.
*/
void destruir_tabla(valor_t *tabla, size_t capacidad,
		    void (*destructor)(void *))
{
	for (size_t i = 0; i < capacidad; i++) {
		if (tabla[i].estado == OCUPADO) {
			free(tabla[i].clave);
			if (destructor)
				destructor(tabla[i].dato);
		}
	}

	free(tabla);
}

/**
 * Crea una tabla de hash de direccionamiento abierto con la capacidad inicial
 * dada. Si la capacidad es menor a 3, se utiliza 3 como capacidad inicial.
 *
 * Devuelve la tabla creada o NULL en caso de error.
 */
hash_t *hash_crear(size_t capacidad_inicial)
{
	return hash_crear_con_funcion(capacidad_inicial, djb2_hash);
}

/**
 * Igual que hash_crear, pero permite al usuario proveer su propia funcion de hash.
 * La función f no puede ser nula.
 */
hash_t *hash_crear_con_funcion(size_t capacidad_inicial,
			       size_t (*f)(const char *))
{
	if (capacidad_inicial < CAPACIDAD_MINIMA)
		capacidad_inicial = CAPACIDAD_MINIMA;

	hash_t *hash = malloc(sizeof(hash_t));
	if (!hash)
		return NULL;

	hash->tabla = calloc(capacidad_inicial, sizeof(valor_t));
	if (!hash->tabla) {
		free(hash);
		return NULL;
	}

	hash->hash_func = f;
	hash->ocupado = 0;
	hash->capacidad = capacidad_inicial;

	return hash;
}

valor_t *buscar_pos_insercion(hash_t *h, const char *c)
{
	size_t pos = h->hash_func(c) % h->capacidad;
	size_t pos_borrado = h->capacidad;

	valor_t *actual = NULL;
	bool encontrado = false;
	size_t inicio = pos;

	while (h->tabla[pos].estado != VACIO && !encontrado) {
		actual = &h->tabla[pos];
		if (actual->estado == OCUPADO && strcmp(actual->clave, c) == 0)
			encontrado = true;

		if (actual->estado == BORRADO && pos_borrado == h->capacidad)
			pos_borrado = pos;

		if (!encontrado) {
			pos = (pos + 1) % h->capacidad;
			if (pos == inicio)
				break;
		}
	}
	if (encontrado)
		return actual;

	if (pos_borrado != h->capacidad)
		return &h->tabla[pos_borrado];

	return &h->tabla[pos];
}

bool inicializar_pos(valor_t *pos_a_insertar, const char *clave, void *valor)
{
	pos_a_insertar->clave = my_strdup(clave);
	if (!pos_a_insertar->clave)
		return false;

	pos_a_insertar->dato = valor;
	pos_a_insertar->estado = OCUPADO;
	return true;
}

bool insertar_valor(hash_t *h, const char *c, void *v, void **a)
{
	valor_t *pos_insercion = buscar_pos_insercion(h, c);

	if (pos_insercion->estado == OCUPADO &&
	    strcmp(pos_insercion->clave, c) == 0) {
		if (a != NULL)
			*a = pos_insercion->dato;
		pos_insercion->dato = v;
	} else {
		if (!inicializar_pos(pos_insercion, c, v))
			return false;
		h->ocupado++;
		if (a != NULL)
			*a = NULL;
	}

	return true;
}

bool hash_rehash(hash_t *h)
{
	valor_t *tabla_anterior = h->tabla;
	size_t capacidad_anterior = h->capacidad;
	size_t capacidad_nueva = h->capacidad * 2;

	valor_t *tabla_nueva = calloc(capacidad_nueva, sizeof(valor_t));
	if (!tabla_nueva)
		return false;

	h->tabla = tabla_nueva;
	h->capacidad = capacidad_nueva;
	h->ocupado = 0;

	size_t indice = 0;
	bool error_insercion = false;
	while (indice < capacidad_anterior && !error_insercion) {
		if (tabla_anterior[indice].estado == OCUPADO) {
			if (!insertar_valor(h, tabla_anterior[indice].clave,
					    tabla_anterior[indice].dato,
					    NULL)) {
				error_insercion = true;
			}
		}
		indice++;
	}

	if (error_insercion) {
		destruir_tabla(h->tabla, h->capacidad, NULL);
		h->tabla = tabla_anterior;
		h->capacidad = capacidad_anterior;
	} else
		destruir_tabla(tabla_anterior, capacidad_anterior, NULL);

	return !error_insercion;
}

bool debe_rehashear(hash_t *h, const char *clave, size_t posicion)
{
	return (h->tabla[posicion].estado == OCUPADO &&
		strcmp(clave, h->tabla[posicion].clave) != 0 &&
		(((double)h->ocupado) / ((double)h->capacidad)) >=
			FACTOR_CARGA_MAX);
}

/**
 * Inserta una clave en la tabla y asocia el valor pasado por parámetro.
 *
 * Si previamente existe un valor asociado a esa clave y anterior no es nulo, se
 * guarda un puntero al valor anterior en el puntero.
 *
 * Devuelve true si pudo insertar/actualizar el valor.
 */
bool hash_insertar(hash_t *h, const char *clave, void *valor, void **anterior)
{
	if (!h || !clave)
		return false;

	size_t posicion = h->hash_func(clave) % h->capacidad;

	if (debe_rehashear(h, clave, posicion)) {
		if (!hash_rehash(h))
			return false;
	}

	return insertar_valor(h, clave, valor, anterior);
}

/*
    Pre: El hash debe estar previamente inicializado con "hash_crear" o "hash_crear_con_funcion"
         La clave pasada por pamaretro no debe ser NULL.
    Post: Devuelve el par clave-valor de la tabla acorde a la clave ingresada.
          Devuelve null en caso de no encontrar el par clave-valor.
*/
valor_t *buscar_slot_tabla(hash_t *h, const char *c)
{
	size_t pos = h->hash_func(c) % h->capacidad;

	valor_t *actual = NULL;

	bool encontrado = false;
	while (h->tabla[pos].estado != VACIO && !encontrado) {
		if (h->tabla[pos].estado == OCUPADO &&
		    strcmp(c, h->tabla[pos].clave) == 0) {
			actual = &h->tabla[pos];
			encontrado = true;
		}

		if (!encontrado)
			pos = (pos + 1) % h->capacidad;
	}

	return actual;
}

void *hash_sacar(hash_t *h, const char *clave)
{
	if (!h || !clave)
		return NULL;

	valor_t *pos_buscada = buscar_slot_tabla(h, clave);
	if (!pos_buscada)
		return NULL;

	free(pos_buscada->clave);
	void *valor = pos_buscada->dato;

	pos_buscada->estado = BORRADO;
	pos_buscada->clave = NULL;
	pos_buscada->dato = NULL;

	h->ocupado -= 1;
	return valor;
}

void *hash_buscar(hash_t *h, const char *clave)
{
	if (!h || !clave)
		return NULL;

	valor_t *pos_buscada = buscar_slot_tabla(h, clave);
	if (!pos_buscada)
		return NULL;

	return pos_buscada->dato;
}

bool hash_existe(hash_t *h, const char *clave)
{
	if (!h || !clave)
		return false;

	void *dato_buscado = hash_buscar(h, clave);
	return (dato_buscado == NULL) ? false : true;
}

size_t hash_tamanio(hash_t *h)
{
	return (h) ? h->ocupado : 0;
}

void hash_destruir(hash_t *h)
{
	if (!h)
		return;

	destruir_tabla(h->tabla, h->capacidad, NULL);
	free(h);
}

void hash_destruir_todo(hash_t *h, void (*destructor)(void *))
{
	if (!h)
		return;

	destruir_tabla(h->tabla, h->capacidad, destructor);
	free(h);
}

size_t hash_iterar_claves(hash_t *h, bool (*f)(const char *, void *), void *ctx)
{
	if (!h || !f)
		return 0;

	size_t iterados = 0;

	size_t indice = 0;
	bool terminar_iteracion = false;
	while (indice < h->capacidad && !terminar_iteracion) {
		if (h->tabla[indice].estado == OCUPADO) {
			terminar_iteracion = !f(h->tabla[indice].clave, ctx);
			iterados++;
		}
		indice++;
	}
	return iterados;
}