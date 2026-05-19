# LEEME — Práctica 2: Sistema Cliente-Servidor IMDb

**Materia:** Sistemas Operativos  
**Universidad Nacional de Colombia — Ingeniería de Sistemas**

---

## Integrantes

- Daniel Alfonso Cely Infante
- Maria Catalina Rodriguez
- Yerlan (Persona 3)

---

## Descripción

Sistema cliente-servidor que permite buscar y agregar películas y series de la
base de datos IMDb. El servidor gestiona el archivo binario en disco y atiende
hasta 32 clientes simultáneos mediante hilos (pthreads). El cliente presenta un
menú interactivo al usuario y se comunica con el servidor a través de sockets TCP.

---

## Archivos fuente

| Archivo | Descripción |
|---|---|
| `p2-server.c` | Servidor: sockets, hilos, búsqueda, log |
| `p2-client.c` | Cliente: menú interactivo, envío y recepción de queries |
| `imdb.h` | Estructuras compartidas: `Movie`, `Query`, `Response`, `Criteria` |
| `Hash.c` | Tabla hash en disco: inicializar, calcular, guardar, cargar |
| `Busqueda.c` | Búsqueda por nombre y por filtros sobre `peliculas.bin` |
| `Conversion_Bin.c` | Conversión del TSV a binario e inserción de nuevas películas |
| `Crear_bin.c` | Programa auxiliar para generar `peliculas.bin` y `hash.bin` |
| `Makefile` | Compilación de todos los ejecutables |

---

## Requisitos

- Sistema operativo Linux (probado en Debian)
- GCC
- Dataset `title.basics.tsv` de IMDb en la misma carpeta (solo para la conversión inicial)

---

## Compilación

```bash
make
```

Esto genera tres ejecutables: `creation_bin`, `p2-server` y `p2-client`.

---

## Ejecución

### Paso 1 — Generar los archivos binarios (solo la primera vez)

```bash
./creation_bin
```

Esto lee `title.basics.tsv` y genera:
- `peliculas.bin` — registros de películas serializados en binario
- `hash.bin` — tabla hash de offsets para búsqueda rápida

Este paso puede tardar varios minutos dependiendo del tamaño del dataset (~12 millones de registros).

### Paso 2 — Iniciar el servidor (Terminal 1)

```bash
./p2-server
```

El servidor no muestra ningún menú. Queda escuchando en el puerto **3535** y registra
todas las operaciones en `server.log`.

### Paso 3 — Iniciar el cliente (Terminal 2)

```bash
./p2-client
```

El cliente se conecta automáticamente al servidor en `127.0.0.1:3535` y muestra el menú.

---

## Uso del cliente

Al iniciar, el cliente muestra el siguiente menú:

```
╔════════════════════════════════════╗
║         IMDB Movie Search          ║
╚════════════════════════════════════╝

  1. Buscar por titulo
  2. Buscar por titulo + filtros
  3. Salir
```

**Opción 1 — Buscar por título:**  
Ingrese el título exacto de la película o serie. El servidor responde con el
primer registro que coincida.

**Opción 2 — Buscar por título + filtros:**  
Ingrese el título y opcionalmente filtre por tipo, año de inicio y género.
Presione Enter para omitir cualquier filtro.

**Opción 3 — Salir:**  
Cierra la conexión con el servidor y termina el programa.

Después de cada operación se solicita presionar cualquier tecla para volver al menú.

---

## Formato del log

El servidor registra cada operación en `server.log` con el siguiente formato:

```
[20260509T143022] Cliente [127.0.0.1] [SEARCH - Inception]
[20260509T143022] Cliente [127.0.0.1] [SEARCH_FILTER - The Office - tvSeries/2005]
[20260509T143022] Cliente [127.0.0.1] [ADD_MOVIE - Mi Pelicula]
```

---

## Protocolo de comunicación

El cliente envía una estructura `Response` con la `Query` rellena al servidor.
El servidor procesa la solicitud, completa los campos `movie`, `found` y
`search_time_ms` de la misma estructura y la devuelve al cliente.

| Campo `searchCriteria` | Acción |
|---|---|
| `SEARCH` | Búsqueda por nombre o por filtros |
| `ADD_MOVIE` | Inserción de nueva película |
| `EXIT` | Cierre de conexión |

---

## Limpieza

```bash
make clean
```

Elimina los ejecutables compilados. Los archivos `.bin` y `server.log` no se eliminan.