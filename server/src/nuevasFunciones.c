#include"utils.h"


PCB* deserializarPCB(int socket_emisor){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	unPCB -> id = deserializarInt32(socket_emisor);
	unPCB -> tamanio = deserializarInt32(socket_emisor);
	unPCB -> program_counter = deserializarInt32(socket_emisor);
	unPCB -> tabla_paginas = deserializarInt32(socket_emisor);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_emisor);
	unPCB -> instrucciones = list_create();
	uint32_t cantidadInstrucciones = deserializarInt32(socket_emisor);
	for (int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* unaInstruccion = asignarMemoria(sizeof(t_instruccion));
		unaInstruccion -> identificador = deserializarString(socket_emisor);
		unaInstruccion -> parametros = queue_create();
		unaInstruccion -> parametros -> elements = deserializarListaInt32(socket_emisor);
		list_add(unPCB -> instrucciones, unaInstruccion);
	}

	//unPCB -> instrucciones = list_create();
	//t_list* recibirInstrucciones = list_create();

	//recibirInstrucciones = deserializarListaInstrucciones(socket_emisor);
	//list_add_all(unPCB -> instrucciones, recibirInstrucciones);
	return unPCB;
}

double deserializarDouble(int emisor){
	double mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(double));
	return mensaje;
}

char* deserializarString(int emisor){
	uint32_t tamanioMensaje = deserializarInt32(emisor);
	char* mensaje = asignarMemoria(tamanioMensaje + 1);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	mensaje[tamanioMensaje - 1] = '\0';
	return mensaje;
}

uint32_t deserializarInt32(int emisor){
	uint32_t mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(uint32_t));
	return mensaje;
}

t_list* deserializarListaInstrucciones(int emisor){
	uint32_t elementosDeLalista = deserializarInt32(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		//t_instruccion* inst = deserializarInst(emisor);
		list_add(respuesta, deserializarInstruccion(emisor));
	}
	return respuesta;

}

t_instruccion* deserializarInstruccion(int socket_emisor){
	t_instruccion* unaInstruccion = asignarMemoria(sizeof(t_instruccion));
	unaInstruccion -> identificador = deserializarString(socket_emisor);
	unaInstruccion -> parametros = queue_create();
	unaInstruccion -> parametros -> elements = deserializarListaInt32(socket_emisor);
	return unaInstruccion;
}

t_list* deserializarListaInt32(int emisor){
	uint32_t elementosDeLalista = deserializarInt32(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		list_add(respuesta, deserializarInt32(emisor));
	}
	return respuesta;
}

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}

int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos){
	int bytesRecibidos = recv(socketEmisor, buffer, bytesMaximos, MSG_WAITALL);
	if(bytesRecibidos<=0){
	// error o conexión cerrada por el cliente
	if (bytesRecibidos == 0) {
		// conexión cerrada
		printf("conexion cerrada\n");
		exit(1);
	} else {
		perror("error en el recv");
	}
	}
	return bytesRecibidos;
}

/****************************************************************/
void enviarPCB(int socket_receptor, PCB unPCB, t_log* logger){
	int tamanioInstruccionesTotal = tamanio_listaInst(unPCB.instrucciones);
	uint32_t cantidadInstrucciones = list_size(unPCB.instrucciones);
	int tamanioBuffer = sizeof(uint32_t)*4 + sizeof(double) + sizeof(uint32_t) + tamanioInstruccionesTotal;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, unPCB . id);
	concatenarInt32(buffer, &desplazamiento, unPCB . tamanio);
	concatenarInt32(buffer, &desplazamiento, unPCB . program_counter);
	concatenarInt32(buffer, &desplazamiento, unPCB . tabla_paginas);
	concatenarDouble(buffer, &desplazamiento, unPCB . estimacion_rafaga);
	concatenarInt32(buffer, &desplazamiento, cantidadInstrucciones);
	log_info(logger, "********CANT INSTRUCCIONES A ENVIAR %i********", cantidadInstrucciones);
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* unaInstruccion = list_get(unPCB . instrucciones, i);
		concatenarString(buffer, &desplazamiento, unaInstruccion -> identificador);
		concatenarListaInt32(buffer, &desplazamiento, unaInstruccion -> parametros -> elements);
	}

	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);
//	log_info(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
	log_debug(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_error(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_trace(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_warning(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
}

uint32_t tamanio_listaInst(t_list* listaInst){
	uint32_t respuesta = sizeof(uint32_t);
	for(int i = 0; i < listaInst -> elements_count; i++ ){
		t_instruccion* inst = list_get(listaInst, i);
		int cantidadParametros = inst -> parametros -> elements -> elements_count;
		respuesta += sizeof(uint32_t) +  strlen(inst -> identificador) + 1 + sizeof(uint32_t)*cantidadParametros;
	}
	return respuesta;
}

void enviarMensaje(int socket, void* mensaje, int tamanio){
	send(socket, mensaje, tamanio, 0);
}

void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(uint32_t));
	*desplazamiento = *desplazamiento + sizeof(uint32_t);
}

void concatenarDouble(void* buffer, int* desplazamiento, double numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(double));
	*desplazamiento = *desplazamiento + sizeof(double);
}

void concatenarString(void* buffer, int* desplazamiento, char* mensaje){
	concatenarInt32(buffer, desplazamiento, strlen(mensaje) + 1);
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

void concatenarListaInt32(void* buffer, int* desplazamiento, t_list* listaArchivos){
	concatenarInt32(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		concatenarInt32(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

void concatenarInstruccion(void* buffer, int* desplazamiento, t_instruccion* unaInstruccion){
	concatenarString(buffer, desplazamiento, unaInstruccion -> identificador);
	concatenarListaInt32(buffer, desplazamiento, unaInstruccion -> parametros -> elements);
}

