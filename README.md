<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA HASH

## Repositorio de (Nombre Apellido) - (Padrón) - (Mail)

- Para compilar:

```bash
línea de compilación
```

- Para ejecutar:

```bash
línea de ejecución
```

- Para ejecutar con valgrind:
```bash
línea con valgrind
```
---
##  Funcionamiento

Explicación de cómo funcionan las estructuras desarrolladas en el TP y el funcionamiento general del mismo.

Aclarar en esta parte todas las decisiones que se tomaron al realizar el TP, cosas que no se aclaren en el enunciado, fragmentos de código que necesiten explicación extra, etc.

Incluir **EN TODOS LOS TPS** los diagramas relevantes al problema (mayormente diagramas de memoria para explicar las estructuras, pero se pueden utilizar otros diagramas si es necesario).

### Por ejemplo:

El programa funciona abriendo el archivo pasado como parámetro y leyendolo línea por línea. Por cada línea crea un registro e intenta agregarlo al vector. La función de lectura intenta leer todo el archivo o hasta encontrar el primer error. Devuelve un vector con todos los registros creados.

<div align="center">
<img width="70%" src="img/diagrama1.svg">
</div>

En el archivo `sarasa.c` la función `funcion1` utiliza `realloc` para agrandar la zona de memoria utilizada para conquistar el mundo. El resultado de `realloc` lo guardo en una variable auxiliar para no perder el puntero original en caso de error:

```c
int *vector = realloc(vector_original, (n+1)*sizeof(int));

if(vector == NULL)
    return -1;
vector_original = vector;
```


<div align="center">
<img width="70%" src="img/diagrama2.svg">
</div>

---

## Respuestas a las preguntas teóricas
   - Explicar por qué es importante el tamaño de la tabla (tanto para tablas abiertas como cerradas)
     - Dado que en una tabla abierta se pueden encadenar colisiones sin importar el tamaño de la tabla, ¿Realmente importa el tamaño?

El tamaño de una tabla de hash es importante tanto como en el desarrollo de un hash cerrado tanto como en el de un hash abierto. 

Por un lado, en el caso de una hash cerrado, el tamaño de la tabla define cuantas direcciones posibles hay. De esta forma, con una tabla de mayor tamaño es probable que se tengan menos colisiones o que la insercion de un elemento no se asemeje a O(n) debido a que aumentaria la posibilidad de encontrar un lugar vacio.

Por otro lado, en el caso de un hash abierto, el tamaño de la tabla es decisivo para la complejidad de las primitivas del mismo. Si dejamos fijo el tamaño de la tabla, en dicho caso las colisiones no serian un problema debido a que simplemente se encadenarian una tras otra en el TDA propuesto del indice de la tabla. Aun asi, el problema no surge por las colisiones mismas, sino por el aumento de complejidad de operaciones como la busqueda de elementos.

Por ejemplo, en el caso de optar por implementar un hash abierto con una lista, si dejamos el tamaño de la tabla fijo, las colisiones se encadenarian en cada lista respecto a cada indice de la tabla, logrando que en un determinado punto la lista sobrepase excesivamente el tamaño de la tabla complejizando aun mas la busqueda de elementos hasta llevarla a un O(n) (complejidad de buscar en una lista).

De esta forma, si aumentamos el tamaño de la tabla de forma medida cuando se sobrepase la cantidad de elementos de uno de los indices, lograremos acercar la complejidad de las primitivas a un O(1) ya que la longitud de las listas propias no sera superior al largo de la tabla, y la funcion de hash nos proporcionaria un acercamiento al valor a encontrar.


