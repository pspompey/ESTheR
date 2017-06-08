#include "Kernel.h"
#include "server.h"

void init_server(socket_t mem_fd, socket_t fs_fd) {

    fdset_t read_fds, all_fds = socket_set_create();
    socket_set_add(mem_fd, &all_fds);
//  socket_set_add(fs_fd, &all_fds);
    socket_t sv_sock = socket_init(NULL, config->puerto_prog);
    socket_set_add(sv_sock, &all_fds);

    while(true) {
        read_fds = all_fds;
        fdcheck(select(read_fds.max + 1, &read_fds.set, NULL, NULL, NULL));

        for(socket_t i = 0; i <= all_fds.max; i++) {
            if(!FD_ISSET(i, &read_fds.set)) continue;
            if(i == sv_sock) {
                socket_t cli_sock = socket_accept(sv_sock);
                header_t header = protocol_handshake_receive(cli_sock);
                process_t cli_process = header.syspid;
                if(cli_process == CONSOLE) {
                    socket_set_add(cli_sock, &all_fds);
                    t_client* cliente = alloc(sizeof(t_client));
                    cliente->clientID = cli_sock;
                    cliente->process= CONSOLE;
                    cliente->pids = list_create();
                    list_add (consolas_conectadas, cliente);
                    log_inform("Received handshake from %s", get_process_name(cli_process));
                }
                else if(cli_process == CPU){
                    socket_set_add(cli_sock, &all_fds);
                    t_client* cliente = alloc(sizeof(t_client));
                    cliente->clientID = cli_sock;
                    cliente->process= CPU;
                    list_add (cpu_conectadas, cliente);
                    log_inform("Received handshake from %s", get_process_name(cli_process));

                    log_inform("Envio stack size (%i) y tamaño de pagina (%i) a CPU", config->stack_size, config->page_size);
                    unsigned char buff[BUFFER_CAPACITY];
                    header_t header = protocol_header(OP_KE_SEND_STACK_SIZE);
                    header.msgsize = serial_pack(buff, "hh", config->stack_size, config->page_size);
                    packet_t packet = protocol_packet(header, buff);
                    protocol_packet_send(packet, cli_sock);
                }
                else {
                    socket_close(cli_sock);
                }
            }
            else {
            	t_client* process = buscar_proceso (i);
            		if (process->process == CONSOLE){
            			packet_t program = protocol_packet_receive(process->clientID);
                		if(program.header.opcode == OP_UNDEFINED) {
                			socket_close(i);
                			FD_CLR(i, &all_fds.set);
                			continue;
                		}else{
                			if(program.header.opcode == OP_NEW_PROGRAM) {
                				pthread_mutex_lock(&mutex_planificacion);
                				gestion_datos_newPcb(program, process);
                				pthread_mutex_unlock(&mutex_planificacion);
                			}
                		}
            		}

                	if (process->process == CPU){
                		pthread_mutex_lock(&mutex_planificacion);
            			packet_t cpu_call = protocol_packet_receive(process->clientID);
            			gestion_syscall(cpu_call, process, mem_fd);
            			pthread_mutex_unlock(&mutex_planificacion);
                	}
            }
        }
        pthread_mutex_lock(&mutex_planificacion);
        planificacion(mem_fd);
        pthread_mutex_unlock(&mutex_planificacion);
    }
}

t_client* buscar_proceso (socket_t client){
	t_client* aux;

	for (int i = 0; i < cpu_conectadas->elements_count; i++){
		aux = list_get (cpu_conectadas, i);
		if (aux->clientID == client) return aux;
	}

	for (int i = 0; i < cpu_executing->elements_count; i++){
	 	aux = list_get (cpu_executing, i);
	 	if (aux->clientID == client) return aux;
	}

	for (int i = 0; i < consolas_conectadas->elements_count; i++){
		aux = list_get (consolas_conectadas, i);
		if (aux->clientID == client) return aux;
	}

	return aux;
}