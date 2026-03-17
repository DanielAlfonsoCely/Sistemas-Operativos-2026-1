## Especificaciones del Dataset

### Descripción de los campos

El dataset utilizado es `title.basics.tsv` de IMDB, que contiene información
básica de títulos audiovisuales. Cada registro tiene los siguientes campos:

- **titleType**: tipo de título (movie, short, tvEpisode, tvMiniSeries, tvMovie,
  tvPilot, tvSeries, tvShort, tvSpecial, video, videoGame)
- **primaryTitle**: título principal usado en materiales promocionales
- **originalTitle**: título en el idioma original de producción
- **isAdult**: indica si el contenido es para adultos (0 = no, 1 = sí)
- **startYear**: año de estreno o inicio de la serie
- **runtimeMinutes**: duración en minutos
- **genres**: géneros asociados, separados por coma (máximo 3)

### Justificación de los criterios de búsqueda

Se implementaron dos criterios de búsqueda basados en los campos más relevantes
para identificar un título:

1. **Búsqueda por título**: el campo `primaryTitle` es el identificador principal
   de la tabla hash, lo que permite búsquedas en tiempo casi constante O(1).

2. **Búsqueda por título + filtros**: permite refinar resultados cuando varios
   títulos comparten el mismo nombre. Los filtros de tipo, año y género cubren
   los casos de uso más comunes — por ejemplo, distinguir entre una película y
   una serie del mismo nombre, o encontrar una versión específica por año.

### Rangos de valores válidos

| Campo | Valores válidos |
|---|---|
| primaryTitle | String no vacío, máximo 128 caracteres |
| titleType | movie, short, tvEpisode, tvMiniSeries, tvMovie, tvPilot, tvSeries, tvShort, tvSpecial, video, videoGame |
| startYear | Entero positivo |
| genres | String no vacío, máximo 64 caracteres (ej: Action,Drama,Thriller) |
| runtimeMinutes | Entero positivo, Enter si no aplica |
| isAdult | 0 o 1 |

### Ejemplos de uso

**Búsqueda simple por título:**
```
Opcion: 1
Ingrese el titulo que desea buscar: Inception

╔══════════ Resultado ══════════╗
  Tipo:      movie
  Titulo:    Inception
  Original:  Inception
  Adultos:   No
  Anio:      2010
  Duracion:  148 min
  Generos:   Action,Adventure,Sci-Fi
╚═══════════════════════════════╝
  Tiempo de busqueda: 0.42 ms
```

**Búsqueda con filtros:**
```
Opcion: 2
Ingrese el titulo: The Office
Tipo: tvSeries
Anio de inicio: 2005
Genero: Enter (omitir)

╔══════════ Resultado ══════════╗
  Tipo:      tvSeries
  Titulo:    The Office
  Original:  The Office
  Adultos:   No
  Anio:      2005
  Duracion:  N/A
  Generos:   Comedy
╚═══════════════════════════════╝
  Tiempo de busqueda: 0.38 ms
```

**Título no encontrado:**
```
Opcion: 1
Ingrese el titulo que desea buscar: Mi Pelicula

NA - Pelicula no encontrada.
Desea agregar esta pelicula? (S/N): S
```
## Adaptaciones realizadas

### Estructura de indexación

Inicialmente se planteó implementar la tabla hash con listas enlazadas en memoria RAM,
donde cada nodo almacenaba el offset y el título. Esta aproximación fue descartada porque
con 12 millones de registros cada nodo ocupaba ~144 bytes, resultando en aproximadamente
1.7 GB en RAM — lo cual viola el límite de 10 MB establecido en las especificaciones.

La solución adoptada fue eliminar completamente las listas enlazadas en memoria y trasladar
el encadenamiento de colisiones al archivo binario. La tabla hash en RAM es simplemente un
arreglo de 999 `long` (≈8 KB) donde cada posición almacena el offset del primer registro
de ese bucket. Las colisiones se resuelven mediante el campo `next_offset` dentro de cada
struct `Movie`, que apunta al siguiente registro con el mismo hash directamente en
`peliculas.bin`.

### Archivos generados

El sistema genera dos archivos binarios a partir del TSV original:

- `peliculas.bin`: contiene todos los registros `Movie` serializados en binario. Las
  colisiones del hash se encadenan dentro de este archivo mediante `next_offset`.
- `hash.bin`: contiene el arreglo de 999 offsets serializado. Se carga a RAM al iniciar
  `dataProgram` y se actualiza cada vez que se inserta una nueva película.

El TSV original no es necesario después de la conversión.

### Uso de memoria dinámica

Durante la conversión del TSV, cada registro se aloja dinámicamente con `malloc`, se
procesa y se libera con `free` inmediatamente, manteniendo el uso de RAM mínimo durante
la conversión.