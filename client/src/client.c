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
	instrucc1 -> identificador = "NO_OP";
	instrucc1 -> parametros = queue_create();

	list_add(instrucc1 -> parametros -> elements, 3);

	t_instruccion* instrucc4 = asignarMemoria(sizeof(t_instruccion));
	instrucc4 -> identificador = "I/O";
	instrucc4 -> parametros = queue_create();

	list_add(instrucc4 -> parametros -> elements, 5000);

	t_instruccion* instrucc2 = asignarMemoria(sizeof(t_instruccion));
	instrucc2 -> identificador = "WRITE";
	instrucc2 -> parametros = queue_create();

	list_add(instrucc2 -> parametros -> elements, 5000);
	list_add(instrucc2 -> parametros -> elements, 3544);

	t_instruccion* instrucc3 = asignarMemoria(sizeof(t_instruccion));
	instrucc3 -> identificador = "EXIT";
	instrucc3 -> parametros = queue_create();

	t_proceso* proceso = asignarMemoria(sizeof(t_proceso));
	proceso -> tamanio_proceso = 15;
	proceso -> instrucciones = queue_create();
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc2);
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc3);

	t_list* lista = list_create();
	list_add(lista, instrucc1);
	list_add(lista, instrucc2);
	list_add(lista, instrucc4);
	list_add(lista, instrucc3);
	//list_add(lista, instrucc3);

	//enviarInstrucciones(lista, logger);
	//uint32_t t = tamanioTotalListaInst(lista);
	//enviarInstruccion(conexion, *instrucc1);
//	log_warning(logger, "NUMERO %i",numIdentificador(*instrucc3));
//	enviarInstrucciones(conexion, lista, logger);
	//log_error(logger, "ENVIE INSTRUCCION");


	generarEstructuraPCB(23, proceso);

	PCB* unPCB = list_get(procesosNew, 0);

	enviarPCB(conexion, *unPCB, logger);
	log_warning(logger, "MIS DATOS");
	mostrarDatosPCB(*unPCB, logger);


	unPCB = deserializarPCB(conexion);

	log_warning(logger, "RECIBI DATOS DE CPU 1)");
	mostrarDatosPCB(*unPCB, logger);
/*
	unPCB -> estimacion_rafaga = 8884.25;
	enviarPCB(conexion, *unPCB, logger);

	unPCB = deserializarPCB(conexion, logger);
	log_warning(logger, "RECIBI DATOS DE CPU 2)");
	mostrarDatosPCB(*unPCB, logger);
	//log_info(logger, "SIZE OF LIST NEW: %i", list_size(procesosNew));
*/
	terminar_programa(conexion, logger, config);

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
