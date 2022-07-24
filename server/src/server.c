#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

//	t_list* lista = deserializarListaInstrucciones(cliente_fd)

/*	t_list* lista = deserializarInstrucciones1Parametro(cliente_fd);
	//log_error(logger, "LLEGO LA LISTA %i", list_size(lista) );
	t_instruccion* inst = list_remove(lista, 0);
	log_error(logger, "%s", inst -> identificador );
	log_error(logger, "%i", list_get(inst -> parametros -> elements,0));

	inst = list_remove(lista, 0);
	log_error(logger, "%s", inst -> identificador );
	log_error(logger, "%i", list_get(inst -> parametros -> elements,0));
	log_error(logger, "%i", list_get(inst -> parametros -> elements,1));

	inst = list_remove(lista, 0);
	log_error(logger, "%s", inst -> identificador );
	log_error(logger, "%i", list_get(inst -> parametros -> elements,0));

	inst = list_remove(lista, 0);
	log_error(logger, "%s", inst -> identificador );

	//t_instruccion* inst = deserializarUnaInstruccion(cliente_fd);
	//log_error(logger, "%s", inst -> identificador);
	//log_error(logger, "%i", list_get(inst -> parametros -> elements, 0));*/

	PCB* unPCB = deserializarPCB(cliente_fd);

	log_error(logger, "RECIBI INSTRUCCIONES DE KERNEL 1) ");
	//mostrarDatosPCB(*unPCB, logger);
	unPCB -> program_counter = 9999;

	enviarPCB(cliente_fd, *unPCB, logger);
	/*

	unPCB = deserializarPCB(cliente_fd);
	log_error(logger, "RECIBI INSTRUCCIONES DE KERNEL 2) ");
	mostrarDatosPCB(*unPCB, logger);

	unPCB -> id = 532;
	enviarPCB(cliente_fd, *unPCB, logger);*/
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
void mostrarDatosPCB(PCB unPCB, t_log* logger){
	log_info(logger, "ID PCB: %i", unPCB . id);
	log_info(logger, "TAMANIO PCB: %i", unPCB . tamanio);
	log_info(logger, "PC PCB: %i", unPCB . program_counter);
	log_info(logger, "TP PCB: %i", unPCB . tabla_paginas);
	log_info(logger, "ESTIMACION PCB: %f", unPCB . estimacion_rafaga);
	log_info(logger, "CANTIDAD INST: %i", list_size(unPCB . instrucciones));

	t_instruccion* inst = list_remove(unPCB . instrucciones, 0);
	log_error(logger, "%s", inst -> identificador );
	log_error(logger, "%i", list_get(inst -> parametros -> elements,0));

	inst = list_remove(unPCB . instrucciones, 0);
	log_error(logger, "%s", inst -> identificador );
	log_error(logger, "%i", list_get(inst -> parametros -> elements,0));
	log_error(logger, "%i", list_get(inst -> parametros -> elements,1));

	inst = list_remove(unPCB . instrucciones, 0);
	log_error(logger, "%s", inst -> identificador );
	log_error(logger, "%i", list_get(inst -> parametros -> elements,0));

	inst = list_remove(unPCB . instrucciones, 0);
	log_error(logger, "%s", inst -> identificador );

}
