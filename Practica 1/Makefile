all: creation_bin dataProgram ui

# Creacion del binario a partir del TSV y generacion de la tabla hash
creation_bin: Crear_bin.c Hash.c Conversion_Bin.c
	gcc Crear_bin.c Hash.c Conversion_Bin.c -o setup

# Programa que carga la tabla hash en memoria compartida y espera por consultas
dataProgram: Hash.c Conversion_Bin.c Busqueda.c p1-dataProgram.c
	gcc Hash.c Conversion_Bin.c Busqueda.c p1-dataProgram.c -o dataProgram -lpthread

# Programa que actua como interfaz de usuario, permitiendo ingresar filtros y mostrando resultados
ui: ui.c
	gcc ui.c -o ui -lpthread

clean:
	rm -f dataProgram ui setup
