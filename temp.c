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

valor_t *buscar_pos_insercion(hash_t *h, const char *c)
{
	size_t pos = h->hash_func(c) % h->capacidad;
	size_t pos_borrado = h->capacidad;

	valor_t *actual = NULL;
	bool encontrado = false;

	while (h->tabla[pos].estado != VACIO && !encontrado) {
		actual = &h->tabla[pos];
		if (actual->estado == OCUPADO && strcmp(actual->clave, c) == 0)
			encontrado = true;

		if (actual->estado == BORRADO && pos_borrado == h->capacidad)
			pos_borrado = pos;

		if (!encontrado)
			pos = (pos + 1) % h->capacidad;
	}
	if (encontrado)
		return actual;

	if (pos_borrado != h->capacidad)
		return &h->tabla[pos_borrado];

	return &h->tabla[pos];
}