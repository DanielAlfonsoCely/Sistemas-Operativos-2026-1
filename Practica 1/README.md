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
