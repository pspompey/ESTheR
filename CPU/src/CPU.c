#include "CPU.h"
#include <commons/config.h>
#include "utils.h"
#include "socket.h"
#include "protocol.h"
#include "log.h"
#include <commons/collections/list.h>
#include "structures.h"

#define tamanioPagina 512

void harcodeoParaProbarCPU();
t_pcb* pcbActual;

int main(int argc, char **argv) {
	guard(argc == 2, "Falta indicar ruta de archivo de configuración");
	set_current_process(CPU);

	t_cpu* cpu = malloc(sizeof(t_cpu));
	leerConfiguracionCPU(cpu, argv[1]);

	// Harcodeo de elementos que me deberian llegar por socket para poder trabajar.
	harcodeoParaProbarCPU();

	puts("Conectándose al Kernel...");
	int kernel_fd = socket_connect(cpu->ip_kernel, cpu->puerto_kernel);
	protocol_send_handshake(kernel_fd);
	puts("Conectado.");

	//------------Envio de mensajes al servidor------------
	char message[SOCKET_BUFFER_CAPACITY];

	while(socket_receive_string(message, kernel_fd) > 0) {
		printf("Recibido mensaje: \"%s\"\n", message);
	}

	free(cpu);
	socket_close(kernel_fd);
	return 0;
}

void harcodeoParaProbarCPU() {
	// El PCB me lo manda el kernel, yo solo creo el stack
	pcbActual = malloc(sizeof(t_pcb));
	pcbActual->indexStack = list_create();
	pcbActual->stackOffsetPointer = 0;
	pcbActual->stackPagePointer = 0;

	char* instruccion[6];
	instruccion[0] = "#!/usr/bin/ansisop";
	instruccion[1] = "begin";
	instruccion[2] = "variables a, b";
	instruccion[3] = "a = 3";
	instruccion[4] = "b = 5";
	instruccion[5] = "a = b + 12";
	instruccion[6] = "end";

	int i;
	for (i = 0; i < 7; ++i) {
		log_inform("La instruccion a parsear es: %s", instruccion[i]);
		analizadorLinea(instruccion[i], &funcionesAnSISOP, &funcionesKernel);
	}
}

// Primitivas AnSISOP

t_stack* t_stack_create(){
	t_stack* stack = malloc(sizeof(t_stack));
	stack->cantidad_args = 0;
	stack->args = list_create();
	stack->cantidad_vars = 0;
	stack->vars = list_create();
	stack->retPos = 0;
	stack->retVar.offset = 0;
	stack->retVar.page = 0;
	stack->retVar.size = 0;

	return stack;
}

t_puntero definirVariable(t_nombre_variable identificador_variable){

	log_inform("Definir variable");

	int var_offset = pcbActual->stackOffsetPointer;
	int var_pagina = pcbActual->stackPagePointer;

	//Agarro el ultimo stack
	t_stack* stack = list_get(pcbActual->indexStack, pcbActual->indexStack->elements_count -1);

	if(stack == NULL){ // si no hay registros, creo uno nuevo
		stack = t_stack_create();
		// Guardo el nuevo registro en el índice:
		list_add(pcbActual->indexStack, stack);
	}

	t_var* variable = malloc(sizeof(t_var));
	variable->id = identificador_variable;
	variable->mempos.offset = var_offset;
	variable->mempos.page = var_pagina;
	variable->mempos.size = sizeof(int);

	list_add(stack->vars, variable);

	int total_heap_offset = tamanioPagina + pcbActual->stackOffsetPointer;
	pcbActual->stackOffsetPointer += sizeof(int);
	pcbActual->stackPagePointer = (total_heap_offset + sizeof(int)) / tamanioPagina;

	log_inform("'%c' -> Dirección lógica definida: %i, %i, %i.", identificador_variable, var_pagina, var_offset, sizeof(int));
	log_inform("La posicion retornada: %i", total_heap_offset);

	return total_heap_offset;
}

// @return	Donde se encuentre la variable buscada
t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable){

	t_puntero posicion;

	malloc(identificador_variable);
	log_inform("Obtengo posicion de %s", &identificador_variable);

	return posicion;
}

t_valor_variable dereferenciar(t_puntero direccion_variable){

	t_valor_variable valorVariable;

	log_inform("Dereferenciar");

	return valorVariable;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor){

	log_inform("Asignar valor");
}


void llamarSinRetorno(t_nombre_etiqueta etiqueta){

	log_inform("Llamar sin retorno");

}

void finalizar(void){

	log_inform("Finalizar");

}

void leerConfiguracionCPU(t_cpu* cpu, char* path) {

	t_config* config = config_create(path);

	cpu->ip_kernel = config_get_string_value(config, "IP_KERNEL");
	cpu->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	printf("---------------Mi configuración---------------\n");
	printf("IP KERNEL: %s\n", cpu->ip_kernel);
	printf("PUERTO KERNEL: %s\n", cpu->puerto_kernel);
	printf("----------------------------------------------\n");
}

