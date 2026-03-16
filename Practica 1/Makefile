all: creation_bin dataProgram ui

creation_bin: Crear_bin.c Hash.c Conversion_Bin.c
	gcc Crear_bin.c Hash.c Conversion_Bin.c -o setup //- Creación del binario a partir del TSV y generación de la tabla hash

dataProgram: Hash.c Conversion_Bin.c Busqueda.c dataProgram.c
	gcc Hash.c Conversion_Bin.c Busqueda.c dataProgram.c -o dataProgram -lpthread 	// - Programa que carga la tabla hash en memoria compartida y espera por consultas

ui: ui.c
	gcc ui.c -o ui -lpthread // - Programa que actúa como interfaz de usuario, permitiendo ingresar filtros y mostrando resultados

clean:
	rm -f dataProgram ui setup