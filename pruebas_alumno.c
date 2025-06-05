#include "pa2m.h"
#include "src/hash.h"

#define CANTIDAD_CLAVES 50000
#define CADA_CUANTAS_BORRAR 100

void prueba_insercion_y_eliminacion_mezclada()
{
	hash_t *hash = hash_crear(100);
	pa2m_afirmar(hash != NULL, "Se crea el hash correctamente.");

	bool exito = true;
	size_t insertadas = 0;
	size_t eliminadas = 0;

	for (size_t i = 0; i < CANTIDAD_CLAVES && exito; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);

		if (hash_insertar(hash, clave, (void *)(size_t)i, NULL)) {
			insertadas++;
		} else {
			printf("Error al insertar clave %zu\n", i);
			exito = false;
		}

		if (i > 0 && i % CADA_CUANTAS_BORRAR == 0) {
			size_t clave_a_borrar = i - CADA_CUANTAS_BORRAR / 2;
			sprintf(clave, "clave%zu", clave_a_borrar);
			void *dato = hash_sacar(hash, clave);
			if (dato != NULL) {
				eliminadas++;
			} else {
				printf("Error al eliminar clave %zu\n",
				       clave_a_borrar);
			}
		}
	}

	printf("Insertadas: %zu\n", insertadas);
	printf("Eliminadas: %zu\n", eliminadas);
	pa2m_afirmar(insertadas > 0, "Se insertaron claves.");
	pa2m_afirmar(eliminadas > 0, "Se eliminaron claves intercaladamente.");
	pa2m_afirmar(hash_tamanio(hash) > 0, "La tabla tiene claves al final.");

	hash_destruir(hash);
}

int main()
{
	pa2m_nuevo_grupo(
		"============== Pruebas de Inserción y Eliminación Masiva ==============");
	prueba_insercion_y_eliminacion_mezclada();

	return pa2m_mostrar_reporte();
}
