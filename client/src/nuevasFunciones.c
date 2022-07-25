#include "utils.h"

t_list* deserializarInstrucciones1Parametro(int emisor){
	t_list* respuesta = list_create();
	uint32_t cantidadInstrucciones = deserializarInt32(emisor);
	//printf("LLEGARON Instrucciones %i", cantidadInstrucciones);
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* unaInstruccion = deserializarUnaInstruccion(emisor);
		list_add(respuesta, unaInstruccion);
	}
	return respuesta;
}

t_instruccion* deserializarUnaInstruccion(int emisor){
	t_instruccion* inst = asignarMemoria(sizeof(t_instruccion));
	inst -> identificador = deserializarInt32(emisor);
	inst -> parametros = queue_create();
	if(string_equals_ignore_case(inst -> identificador, "I/O") ||
			string_equals_ignore_case(inst -> identificador, "NO_OP") ||
			string_equals_ignore_case(inst -> identificador, "READ")){
		uint32_t parametro = deserializarInt32(emisor);
		list_add(inst -> parametros -> elements, parametro);
	} else if(string_equals_ignore_case(inst -> identificador, "COPY") ||
			string_equals_ignore_case(inst -> identificador, "WRITE")){
		uint32_t parametro1 = deserializarInt32(emisor);
		uint32_t parametro2 = deserializarInt32(emisor);
		list_add(inst -> parametros -> elements, parametro1);
		list_add(inst -> parametros -> elements, parametro2);
	}

	return inst;
}

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
/*
t_list* deserializarListaInstrucciones(int socket_emisor){
	uint32_t cantidadInstrucciones = deserializarInt32(socket_emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* instruccion = asignarMemoria(sizeof(t_instruccion));
		instruccion -> identificador = deserializarInt32(socket_emisor);
		instruccion -> parametros = queue_create();
		instruccion -> parametros -> elements = deserializarListaInt32(socket_emisor);
		list_add(respuesta, instruccion);
		}
	return respuesta;
	}*/

void enviarInstrucciones(int socket_receptor, t_list* lista, t_log* logger){
	uint32_t cantidadInstrucciones = list_size(lista);
	int tamanioBuffer = sizeof(uint32_t)*2 + tamanioTotalListaInst(lista);
	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;
	concatenarInt32(buffer, &desplazamiento, cantidadInstrucciones);
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* instruccion = list_get(lista, i);
		//concatenarString(buffer, &desplazamiento, instruccion -> identificador);
		int parametros = numIdentificador(*instruccion);
		switch(parametros){
			case 1:;
				uint32_t unParametro = list_get(instruccion -> parametros -> elements, 0);
				concatenarInt32(buffer, &desplazamiento, unParametro);
				break;
			case 2:;
				uint32_t param1 = list_get(instruccion -> parametros -> elements, 0);
				uint32_t param2 = list_get(instruccion -> parametros -> elements, 1);
				concatenarInt32(buffer, &desplazamiento, param1);
				concatenarInt32(buffer, &desplazamiento, param2);
				break;
			default:;
				break;

		}
		//concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements, 0));
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
	//	tamanio += (strlen(instruccion -> identificador) + 1);
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
	uint32_t respuesta = tamanioParametros(lista) + cantidadParametros(lista) + sizeof(uint32_t);
	return respuesta;
}


void enviarInstruccion(int socket_receptor, t_instruccion instruccion){
	uint32_t cantParametros = list_size(instruccion . parametros -> elements);
	//uint32_t tamanioIdentificador = sizeof(int);

	int tamanioBuffer = cantParametros*sizeof(uint32_t) + sizeof(uint32_t);
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;

	//concatenarString(buffer, &desplazamiento, instruccion . identificador);
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
	uint32_t cantidadInstrucciones = list_size(unPCB . instrucciones);
	int tamanioBuffer = sizeof(uint32_t)*4 + sizeof(double) + tamanioParametros(unPCB . instrucciones) + cantidadInstrucciones*sizeof(ID_INSTRUCCION);

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, unPCB . id);
	concatenarInt32(buffer, &desplazamiento, unPCB . tamanio);
	concatenarInt32(buffer, &desplazamiento, unPCB . program_counter);
	concatenarInt32(buffer, &desplazamiento, unPCB . tabla_paginas);
	concatenarDouble(buffer, &desplazamiento, unPCB . estimacion_rafaga);

	// Concatenar Instrucciones
	concatenarInt32(buffer, &desplazamiento, cantidadInstrucciones);

	for(int k = 0; k < cantidadInstrucciones; k++){
		t_instruccion* instruccion = list_get(unPCB . instrucciones, k);
		int parametros = cantidad_de_parametros(instruccion -> identificador);
		concatenarInt32(buffer, &desplazamiento, (uint32_t) instruccion -> identificador);
		switch(parametros){
			case 1:
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,0));
				break;
			case 2:
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,0));
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,1));
				break;
			case 0:
				break;
		}
	}


	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);
	log_debug(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
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

void concatenarInt(void* buffer, int* desplazamiento, int numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(int));
	*desplazamiento = *desplazamiento + sizeof(int);
}


void concatenarListaInt32(void* buffer, int* desplazamiento, t_list* listaArchivos){
	concatenarInt32(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		concatenarInt32(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

void concatenarInstruccion(void* buffer, int* desplazamiento, t_instruccion* unaInstruccion){
//	concatenarString(buffer, desplazamiento, unaInstruccion -> identificador);
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

PCB* deserializarPCB(int socket_emisor){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	unPCB -> id = deserializarInt32(socket_emisor);
	unPCB -> tamanio = deserializarInt32(socket_emisor);
	unPCB -> program_counter = deserializarInt32(socket_emisor);
	unPCB -> tabla_paginas = deserializarInt32(socket_emisor);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_emisor);
	unPCB -> instrucciones = list_create();
	t_list* recibirInstrucciones = list_create();
	recibirInstrucciones = deserializarListaInstruccionesK(socket_emisor);
	list_add_all(unPCB -> instrucciones, recibirInstrucciones);
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

int deserializarInt(int emisor){
	int mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(int));
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
//	unaInstruccion -> identificador = deserializarString(socket_emisor);
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


// Otras funciones
void envioListaInstrucciones(int receptor, t_list* lista){
	int tamanioLista = list_size(lista);

	int desplazamiento = 0;
	int tamanioBuffer = tamanioParametros(lista) + tamanioLista*sizeof(ID_INSTRUCCION);
	void* buffer = asignarMemoria(tamanioBuffer);

	concatenarInt(buffer, &desplazamiento, tamanioLista);

	for(int k = 0; k < tamanioLista; k++){
		t_instruccion* instruccion = list_get(lista, k);
		int parametros = cantidad_de_parametros(instruccion -> identificador);
		concatenarInt32(buffer, &desplazamiento, (uint32_t) instruccion -> identificador);
		switch(parametros){
			case 1:
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,0));
				break;
			case 2:
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,0));
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,1));
				break;
			case 0:
				break;
		}
	}
	//t_proceso* proceso = deserializar_proceso(buffer, tamanioBuffer);
	//mostrar_proceso(proceso);

	enviarMensaje(receptor, buffer, tamanioBuffer);
	free(buffer);
}


t_list* deserializarListaInstruccionesK(int emisor){
	uint32_t tamanioLista = deserializarInt(emisor);
	t_list* lista = list_create();
	for(int k = 0; k < tamanioLista; k++){
		t_instruccion* instruccion = asignarMemoria(sizeof(instruccion));
		instruccion -> identificador = deserializarInt32(emisor);
		instruccion -> parametros = queue_create();
		int param = cantidad_de_parametros(instruccion -> identificador);
		switch(param){
			case 1:
				list_add(instruccion -> parametros -> elements, deserializarInt32(emisor));
				break;
			case 2:
				list_add(instruccion -> parametros -> elements, deserializarInt32(emisor));
				list_add(instruccion -> parametros -> elements, deserializarInt32(emisor));
				break;
			case 99:
				break;
		}
		list_add(lista, instruccion);
	}
	return lista;
}

void mostrar_proceso(t_proceso* proceso) {

	list_map(proceso->instrucciones->elements,(void*) mostrar_instruccion);
}

void mostrar_instruccion(t_instruccion* instruccion) {

	log_debug(logger,"ID: %i",(instruccion->identificador));
	for(int i = 0; i < cantidad_de_parametros(instruccion->identificador); i++) {
		log_debug(logger,"PARAMETRO: %i",(int)queue_peek(instruccion->parametros));
		queue_pop(instruccion->parametros);
	}
}

t_proceso* deserializar_proceso(void* contenido, int tam_max) {

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->instrucciones =  queue_create();
	int *desp = malloc(sizeof(int));
	*desp = 4;

	while(*desp < tam_max) {

		t_instruccion* instruccion = malloc(sizeof(t_instruccion));
		ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
		t_queue* parametros = queue_create();
		memcpy(id,contenido + *desp, sizeof(ID_INSTRUCCION));
		*desp += sizeof(ID_INSTRUCCION);
		for(int i = 0; i < cantidad_de_parametros(*id); i++) {
			int *parametro = malloc(sizeof(int));
			memcpy(parametro,contenido + *desp, sizeof(int));
			*desp += sizeof(int);
			queue_push(parametros,(void*) *parametro);
		}
		instruccion->identificador = *id;
		instruccion->parametros = parametros;
		queue_push(proceso->instrucciones,instruccion);
	}

	return proceso;
}

int cantidad_de_parametros(ID_INSTRUCCION identificador) {

	switch (identificador) {

		case IO:
			return 1;
			break;
		case NO_OP:
			return 1;
			break;
		case READ:
			return 1;
			break;
		case EXIT:
			return 0;
			break;
		case COPY:
			return 2;
			break;
		case WRITE:
			return 2;
		}

		return 0;
}
