#include "client.h"

int main(void)
{
	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;
	procesosNew = list_create();

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	ip = config_get_string_value(config,"IP");
	puerto = config_get_string_value(config,"PUERTO");
	valor = config_get_string_value(config,"CLAVE");

	log_info(logger, "IP: %s PUERTO: %s CLAVE: %s", ip, puerto, valor);

	/* ---------------- LEER DE CONSOLA ---------------- */


	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	//log_info(logger, "SIZE OF LIST NEW: %i", list_size(procesosNew));
	t_instruccion* instrucc1 = asignarMemoria(sizeof(t_instruccion));
	instrucc1 -> identificador = "I/O";
	instrucc1 -> parametros = queue_create();

	list_add(instrucc1 -> parametros -> elements, 3000);

	t_proceso* proceso = asignarMemoria(sizeof(t_proceso));
	proceso -> tamanio_proceso = 15;
	proceso -> instrucciones = queue_create();
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc1);

	t_list* lista = list_create();
	list_add(lista, instrucc1);
	list_add(lista, instrucc1);
	list_add(lista, instrucc1);

	//enviarInstrucciones(lista, logger);
	//uint32_t t = tamanioTotalListaInst(lista);
	enviarInstruccion(conexion, *instrucc1);
	log_error(logger, "ENVIE INSTRUCCION");


/*
	generarEstructuraPCB(23, proceso);

	PCB* unPCB = list_get(procesosNew, 0);

	log_warning(logger, "MIS DATOS");
	mostrarDatosPCB(*unPCB, logger);

	enviarPCB(conexion, *unPCB, logger);

	unPCB = deserializarPCB(conexion, logger);

	log_warning(logger, "RECIBI DATOS DE CPU 1)");
	mostrarDatosPCB(*unPCB, logger);

	unPCB -> estimacion_rafaga = 8884.25;
	enviarPCB(conexion, *unPCB, logger);

	unPCB = deserializarPCB(conexion, logger);
	log_warning(logger, "RECIBI DATOS DE CPU 2)");
	mostrarDatosPCB(*unPCB, logger);
	//log_info(logger, "SIZE OF LIST NEW: %i", list_size(procesosNew));
*/
	terminar_programa(conexion, logger, config);

}

void mostrarDatosPCB(PCB unPCB, t_log* log){
	log_info(log, "ID PCB: %i", unPCB . id);
	log_info(log, "TAMANIO PCB: %i", unPCB . tamanio);
	log_info(log, "PC PCB: %i", unPCB . program_counter);
	log_info(log, "TP PCB: %i", unPCB . tabla_paginas);
	log_info(log, "ESTIMACION PCB: %f", unPCB . estimacion_rafaga);
	log_info(log, "CANTIDAD INST: %i", list_size(unPCB . instrucciones));

}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "TP0", 1, LOG_LEVEL_DEBUG);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	leido = readline("> ");

	while(*leido != '\0')
	{
		log_info(logger, leido);
		free(leido);
		leido = readline("> ");
	}
	free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido = readline(">");;
	t_paquete* paquete = crear_super_paquete();

	// Leemos y esta vez agregamos las lineas al paquete

	while(leido != '\0')
	{
		agregar_a_paquete(paquete, leido, sizeof(paquete));
		free(leido);
		leido = readline(">");
	}
	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
