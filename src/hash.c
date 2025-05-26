#include "hash.h"
#include <stdbool.h>

#define CAPACIDAD_MINIMA 3
#define FACTOR_CARGA_MAX 0.7

typedef enum estado {
    VACIO,
    OCUPADO,
    BORRADO
}estado_t;

typedef struct valor {
    void* dato;
    char* clave;
    estado_t estado;
}valor_t;

struct hash {
    valor_t* tabla;
    size_t (*hash_func)(const char*);
    size_t ocupado;
    size_t capacidad;
};

void fhash();

/**
 * Crea una tabla de hash de direccionamiento abierto con la capacidad inicial
 * dada. Si la capacidad es menor a 3, se utiliza 3 como capacidad inicial.
 *
 * Devuelve la tabla creada o NULL en caso de error.
 */
hash_t *hash_crear(size_t capacidad_inicial){
    return hash_crear_con_funcion(capacidad_inicial,fhash);
}

/**
 * Igual que hash_crear, pero permite al usuario proveer su propia funcion de hash.
 * La función f no puede ser nula.
 */
hash_t *hash_crear_con_funcion(size_t capacidad_inicial,
			       size_t (*f)(const char *)){
    if (capacidad_inicial<CAPACIDAD_MINIMA)
        capacidad_inicial=CAPACIDAD_MINIMA;

    hash_t* hash=malloc(sizeof(hash_t));
    if (!hash) return NULL;

    hash->tabla=calloc(capacidad_inicial,sizeof(valor_t));
    if (!hash->tabla){
        free(hash);
        return NULL;
    }

    hash->hash_func=0; //aca va la funcion proporcionada por el usuario o en este caso el que lo desarrolla.
    hash->ocupado=0;
    hash->capacidad=capacidad_inicial;

    return hash;
}

bool hash_rehash(hash_t* h){
    valor_t* tabla_anterior=h->tabla;
    size_t capacidad_anterior=h->capacidad;
    size_t capacidad_nueva=h->capacidad * 2;

    valor_t* tabla_nueva=calloc(capacidad_nueva,sizeof(valor_t));
    if(!tabla_nueva) return false;

    h->tabla=tabla_nueva;
    h->capacidad=capacidad_nueva;
    h->ocupado=0;

    for (int i=0;i<capacidad_anterior;i++){
        if(tabla_anterior[i].estado == OCUPADO){
            insertar_valor(h,tabla_anterior[i].clave,tabla_anterior[i].dato,NULL);
        }
    }

    free(tabla_anterior);
    return true;
}

bool insertar_valor(hash_t* h, const char* c, void* v, void** a){
    size_t posicion=h->hash_func(c) % h->capacidad;

    bool insertado=false;   

    while(!insertado){
        valor_t* valor_en_pos=&h->tabla[posicion];

        if(valor_en_pos->estado == VACIO || valor_en_pos->estado == BORRADO){
            valor_en_pos->clave=c;
            valor_en_pos->dato=v;
            valor_en_pos->estado=OCUPADO;
            h->ocupado+=1;
            insertado=true;
        }

        if(valor_en_pos->estado==OCUPADO && strcmp(c,valor_en_pos->clave)==0){
            if(a != NULL)
                *a=valor_en_pos->dato;
            valor_en_pos->dato=v;
            insertado=true;
        }

        if(!insertado)
            posicion=(posicion+1) % h->capacidad;
    }

    return insertado;
}

/**
 * Inserta una clave en la tabla y asocia el valor pasado por parámetro.
 *
 * Si previamente existe un valor asociado a esa clave y anterior no es nulo, se
 * guarda un puntero al valor anterior en el puntero.
 *
 * Devuelve true si pudo insertar/actualizar el valor.
 */
bool hash_insertar(hash_t *h, const char *clave, void *valor, void **anterior){
    if (!h || !clave) return false;

    size_t posicion=h->hash_func(clave) % h->capacidad;
    
    if (h->tabla[posicion].estado == OCUPADO &&
        strcmp(clave, h->tabla[posicion].clave) != 0 &&
        ((double)h->ocupado / h->capacidad) >= FACTOR_CARGA_MAX) {
        
        if (!hash_rehash(h)) return false;
    }

    return insertar_valor(h,clave,valor,anterior);
}