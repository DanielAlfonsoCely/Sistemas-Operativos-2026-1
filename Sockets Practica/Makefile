all: creation_bin p2-server p2-client

# Genera peliculas.bin y hash.bin desde el TSV
creation_bin: Crear_bin.c Hash.c Conversion_Bin.c
	gcc Crear_bin.c Hash.c Conversion_Bin.c -o creation_bin

# Servidor — maneja sockets, búsquedas y log
p2-server: p2-server.c Hash.c Conversion_Bin.c Busqueda.c
	gcc p2-server.c Hash.c Conversion_Bin.c Busqueda.c -o p2-server -lpthread

# Cliente — interfaz de usuario con sockets
p2-client: p2-client.c
	gcc p2-client.c -o p2-client

clean:
	rm -f creation_bin p2-server p2-client
