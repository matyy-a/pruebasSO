#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<string.h>
#include<assert.h>

#define IP "127.0.0.1"
#define PUERTO "4444"

typedef enum{
	NO_OP,
	IO,
	READ,
	WRITE,
	COPY,
	EXIT,
} ID_INSTRUCCION;

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct{
	uint32_t id;
	uint32_t tamanio;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	double estimacion_rafaga;
}PCB;

typedef struct {
	ID_INSTRUCCION identificador ;
	t_queue* parametros;
}t_instruccion;

typedef struct {
    int tamanio_proceso;
    t_queue* instrucciones;
}t_proceso;

t_log* logger;

t_list* deserializarListaInstruccionesK(int emisor);
void envioListaInstrucciones(int receptor, t_list* lista);
int cantidad_de_parametros(ID_INSTRUCCION identificador);
int deserializarInt(int emisor);
void concatenarInt(void* buffer, int* desplazamiento, int numero);

void enviarPCB(int socket_receptor, PCB unPCB, t_log* logger);
void enviarInstruccion(int socket_receptor, t_instruccion instruccion);
t_instruccion* deserializarUnaInstruccion(int emisor);
t_list* deserializarInstrucciones1Parametro(int emisor);
int numIdentificador(t_instruccion inst);
uint32_t tamanioTotalListaInst(t_list* lista);
uint32_t cantidadParametros(t_list* lista);
uint32_t tamanioParametros(t_list* lista);
uint32_t tamanioIdentificadores(t_list* lista);
t_list* deserializarListaInstrucciones(int socket_emisor);
void enviarInstrucciones(int socket_receptor, t_list* lista, t_log* logger);

PCB* deserializarPCB(int socket_emisor);
uint32_t deserializarInt32(int emisor);
t_list* deserializarListaInt32(int emisor);
t_list* deserializarListaInstrucciones(int emisor);
t_instruccion* deserializarInstruccion(int socket_emisor);
double deserializarDouble(int emisor);
char* deserializarString(int emisor);
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);
uint32_t tamanio_listaInst(t_list* listaInst);
void enviarMensaje(int socket, void* mensaje, int tamanio);
void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero);
void concatenarDouble(void* buffer, int* desplazamiento, double numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
void concatenarListaInt32(void* buffer, int* desplazamiento, t_list* listaArchivos);
void concatenarInstruccion(void* buffer, int* desplazamiento, t_instruccion* unaInstruccion);



void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif /* UTILS_H_ */

