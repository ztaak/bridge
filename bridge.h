#pragma once

#include <stdio.h> // printf
#include <cstdint> // uint_t
#include <vector> // std::vector
#include <string>
#include <cstring> // memset

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <errno.h>

#include <future>


namespace bridge{

const int BACKLOG = 8;

struct context{
	std::future<int> listener;
};

struct server{
	
	int socket_fd;
	struct sockaddr_in address;
	bool is_online;
	
	server(): socket_fd(0), is_online(false){
		memset(&address, 0, sizeof(struct sockaddr_in));
	}
};

enum err_code{
	OK,
	
	SERVER_NULLPTR,
	SERVER_ALREADY_CREATED,

	SOCKET_FAILED,
	SOCKET_SET_SOCKOPT_FAILED,
	SOCKET_BIND_FAILED,
	SOCKET_LISTEN_FAILED,

	CONTEXT_NULLPTR,
};

const std::vector<std::string> err_code_desc = {
				"OK",
				"SERVER_NULLPTR", "SERVER_ALREADY_CREATED", 
				"SOCKET_FAILED", "SOCKET_SET_SOCKOPT_FAILED", "SOCKET_BIND_FAILED", "SOCKET_LISTEN_FAILED",
				"CONTEXT_NULLPTR"};


struct header{
	uint8_t command;
	uint16_t size;	
};

struct pack{
	header head;
	std::vector<uint32_t> data;
};

inline void print_errno(int code){
	printf("Socket error code: '%i', desc: '%s'\n", code, strerror(code));
}

inline int async_listen(){
	for(;;){
		printf("Listening!\n");
	}
	return 0;
}

inline err_code server_start_to_listen(context* ctx, server* serv){
	if(serv == nullptr)
		return err_code::SERVER_NULLPTR;

	if(ctx == nullptr)
		return err_code::CONTEXT_NULLPTR;

	ctx->listener = std::async(async_listen);

	return err_code::OK;
}

inline err_code create_server(server* serv, std::string ipv4, short port){
	if(serv == nullptr)
		return err_code::SERVER_NULLPTR;

	if(serv->is_online)
		return err_code::SERVER_ALREADY_CREATED;

	if((serv->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		print_errno(errno);
		return err_code::SOCKET_FAILED;
	}
	
	int opt = 1;
	if(setsockopt(serv->socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1){
		print_errno(errno);
		return err_code::SOCKET_SET_SOCKOPT_FAILED;
	}

	serv->address.sin_family = AF_INET;
	serv->address.sin_addr.s_addr = inet_addr(ipv4.c_str());
	serv->address.sin_port = htons(port);

	if(bind(serv->socket_fd, (struct sockaddr *)&serv->address, sizeof(serv->address)) < 0){
		print_errno(errno);
		return err_code::SOCKET_BIND_FAILED;
	}

	if(listen(serv->socket_fd, BACKLOG) == -1){
		print_errno(errno);
		return err_code::SOCKET_LISTEN_FAILED;
	}

	return err_code::OK;
}


inline err_code create_client(std::string ipv4_dest, short port_dest){
	

	return err_code::OK;
}





}






















