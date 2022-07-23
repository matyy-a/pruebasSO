#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct{
	uint32_t id;
	uint32_t tamanio;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	double estimacion_rafaga;
}PCB;

typedef struct {
	int tamanio_id;
	char* identificador;
	t_queue* parametros;
}t_instruccion;

typedef struct {
    int tamanio_proceso;
    t_queue* instrucciones;
}t_proceso;

t_list* procesosNew;

PCB crearPCB(int idPCB, t_proceso* proceso);
void agregarEstadoNew(PCB* unPCB );
void generarEstructuraPCB(int idPCB, t_proceso* proceso);
void enviarPCB(int socket_receptor, PCB unPCB, t_log* logger);
void mostrarDatosPCB(PCB unPCB, t_log* log);
void* asignarMemoria(int cantidad);
uint32_t tamanio_listaInst(t_list* listaInst);
void enviarMensaje(int socket, void* mensaje, int tamanio);
void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero);
void concatenarDouble(void* buffer, int* desplazamiento, double numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
void concatenarListaInt32(void* buffer, int* desplazamiento, t_list* listaArchivos);
void concatenarInstruccion(void* buffer, int* desplazamiento, t_instruccion* unaInstruccion);


PCB* deserializarPCB(int socket_emisor, t_log* logger);
uint32_t deserializarInt32(int emisor);
t_list* deserializarListaInt32(int emisor);
t_list* deserializarListaInstrucciones(int emisor);
t_instruccion* deserializarInstruccion(int socket_emisor);
double deserializarDouble(int emisor);
char* deserializarString(int emisor);
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
t_paquete* crear_super_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

#endif /* UTILS_H_ */

