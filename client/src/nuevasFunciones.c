#include "utils.h"

PCB crearPCB(int idPCB, t_proceso* proceso){
	PCB unPCB;
	unPCB.id = idPCB;
	unPCB.tamanio = proceso -> tamanio_proceso;
	unPCB.instrucciones = list_create();
	unPCB.program_counter = 45;
	unPCB.tabla_paginas = 86;
	unPCB.estimacion_rafaga = 10000;
	list_add_all(unPCB . instrucciones, proceso -> instrucciones -> elements);

	return unPCB;
}

void agregarEstadoNew(PCB* unPCB ){
	// MUTEX READY
	list_add(procesosNew, unPCB);
	// MUTEX READY
}

void generarEstructuraPCB(int idPCB, t_proceso* proceso){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	*unPCB = crearPCB(idPCB, proceso);
	agregarEstadoNew(unPCB);
}

t_list* deserializarListaInstrucciones(int socket_emisor){
	uint32_t cantidadInstrucciones = deserializarInt32(socket_emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* instruccion = asignarMemoria(sizeof(t_instruccion));
		instruccion -> identificador = deserializarString(socket_emisor);
		instruccion -> parametros = queue_create();
		instruccion -> parametros -> elements = deserializarListaInt32(socket_emisor);
		list_add(respuesta, instruccion);
		}
	return respuesta;
	}

void enviarInstrucciones(int socket_receptor, t_list* lista, t_log* logger){
	uint32_t cantidadInstrucciones = list_size(lista);
	int tamanioBuffer = sizeof(uint32_t) + sizeof(uint32_t)*tamanioTotalListaInst(lista);
	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;
	concatenarInt32(buffer, &desplazamiento, cantidadInstrucciones);
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* instruccion = list_get(lista, i);
		log_debug(logger, "FOR %s", instruccion -> identificador);
		concatenarString(buffer, &desplazamiento, instruccion -> identificador);
		int parametros = numIdentificador(*instruccion);
		log_error(logger, "%s %i", instruccion -> identificador, parametros);
		switch(parametros){
			case 1:
				log_warning(logger, "PRIMER CASO");
				uint32_t unParametro = list_get(instruccion -> parametros -> elements, 0);
				concatenarInt32(buffer, &desplazamiento, unParametro);
				break;
			case 2:
				log_warning(logger, "SEGUNDO CASO");
				uint32_t param1 = list_get(instruccion -> parametros -> elements, 0);
				uint32_t param2 = list_get(instruccion -> parametros -> elements, 1);
				concatenarInt32(buffer, &desplazamiento, param1);
				concatenarInt32(buffer, &desplazamiento, param2);
				break;
			default:
				log_warning(logger, "CASO EXIT");
				break;

		}
		//concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements, 0));
		log_debug(logger, "CONCATENE INST");
	}

	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);

}

int numIdentificador(t_instruccion inst){
	int n = 0;

	if(string_equals_ignore_case(inst . identificador, "EXIT")){
		n = 99;
		return n;
	}

	if(string_equals_ignore_case(inst . identificador, "NO_OP")){
		n = 1;
		return n;
	}
	if(string_equals_ignore_case(inst . identificador, "I/O")){
		n = 1;
		return n;
	}
	if(string_equals_ignore_case(inst . identificador, "READ")){
		n = 1;
		return n;
	}
	if(string_equals_ignore_case(inst . identificador, "COPY")){
		n = 2;
		return n;
	}
	if(string_equals_ignore_case(inst . identificador, "WRITE")){
		n = 2;
		return n;
	}

}


uint32_t tamanioIdentificadores(t_list* lista){
	uint32_t cantidadInstrucciones = list_size(lista);
	uint32_t tamanio = sizeof(uint32_t);
	for(int i = 0; i<cantidadInstrucciones; i++){
		t_instruccion* instruccion = list_get(lista, i);
		tamanio += (strlen(instruccion -> identificador) + 1);
	}
	return tamanio;
}


uint32_t tamanioParametros(t_list* lista){
	uint32_t cantidadInstrucciones = list_size(lista);
	uint32_t tamanio = sizeof(uint32_t);
	for(int i = 0; i<cantidadInstrucciones; i++){
		t_instruccion* instruccion = list_get(lista, i);
		tamanio += (list_size(instruccion -> parametros -> elements) * sizeof(uint32_t));
	}
	return tamanio;
}

uint32_t cantidadParametros(t_list* lista){
	uint32_t cantidadInstrucciones = list_size(lista);
	uint32_t tamanio = 0;
	for(int i = 0; i<cantidadInstrucciones; i++){
		t_instruccion* instruccion = list_get(lista, i);
		tamanio += list_size(instruccion -> parametros -> elements) * sizeof(uint32_t);
	}
	return tamanio;
}

uint32_t tamanioTotalListaInst(t_list* lista){
	uint32_t respuesta = tamanioParametros(lista) + cantidadParametros(lista) + tamanioIdentificadores(lista);
	return respuesta;
}

t_instruccion* deserializarUnaInstruccion(int emisor){
	t_instruccion* inst = asignarMemoria(sizeof(t_instruccion));
	inst -> identificador = deserializarString(emisor);
	inst -> parametros = queue_create();
	inst -> parametros -> elements = deserializarListaInt32(emisor);
	return inst;
}

void enviarInstruccion(int socket_receptor, t_instruccion instruccion){
	uint32_t cantParametros = list_size(instruccion . parametros -> elements);
	uint32_t tamanioIdentificador = strlen(instruccion . identificador)+1;

	int tamanioBuffer = tamanioIdentificador + cantParametros*sizeof(uint32_t) + sizeof(uint32_t);
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	concatenarString(buffer, &desplazamiento, instruccion . identificador);
	concatenarInt32(buffer, &desplazamiento, list_get(instruccion . parametros -> elements, 0));
/*	for(int i = 0; i < cantParametros ; i++){
		uint32_t parametro = list_get(instruccion . parametros -> elements, i);
		concatenarInt32(buffer, &desplazamiento, parametro);
	}*/
//	concatenarListaInt32(buffer, &desplazamiento, instruccion . parametros -> elements);

	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);
}

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

PCB* deserializarPCB(int socket_emisor, t_log* logger){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	unPCB -> id = deserializarInt32(socket_emisor);
	unPCB -> tamanio = deserializarInt32(socket_emisor);
	unPCB -> program_counter = deserializarInt32(socket_emisor);
	unPCB -> tabla_paginas = deserializarInt32(socket_emisor);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_emisor);
	unPCB -> instrucciones = list_create();
	uint32_t cantidadInstrucciones = deserializarInt32(socket_emisor);
	log_info(logger, "CANTIDAD DE INSTRUCCIONES RECIBIDAS %i", cantidadInstrucciones);
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
/*
t_list* deserializarListaInstrucciones(int emisor){
	uint32_t elementosDeLalista = deserializarInt32(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		//t_instruccion* inst = deserializarInst(emisor);
		list_add(respuesta, deserializarInstruccion(emisor));
	}
	return respuesta;

}*/

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
