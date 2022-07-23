#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;

	PCB* unPCB = deserializarPCB(cliente_fd);

	log_error(logger, "RECIBI INSTRUCCIONES DE KERNEL 1) ");
	mostrarDatosPCB(*unPCB, logger);
	unPCB -> program_counter = 9999;

	enviarPCB(cliente_fd, *unPCB, logger);


	unPCB = deserializarPCB(cliente_fd);
	log_error(logger, "RECIBI INSTRUCCIONES DE KERNEL 2) ");
	mostrarDatosPCB(*unPCB, logger);

	unPCB -> id = 532;
	enviarPCB(cliente_fd, *unPCB, logger);
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
void mostrarDatosPCB(PCB unPCB, t_log* log){
	log_info(log, "ID PCB: %i", unPCB . id);
	log_info(log, "TAMANIO PCB: %i", unPCB . tamanio);
	log_info(log, "PC PCB: %i", unPCB . program_counter);
	log_info(log, "TP PCB: %i", unPCB . tabla_paginas);
	log_info(log, "ESTIMACION PCB: %f", unPCB . estimacion_rafaga);
	log_info(log, "CANTIDAD INST: %i", list_size(unPCB . instrucciones));

}
