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
	bool is_ipv6;
	bool is_online;
	server(): socket_fd(0), is_online(false), is_ipv6(false){
	}
};

enum err_code{
	OK,
	
	SERVER_NULLPTR,
	SERVER_ALREADY_CREATED,
	SERVER_ACCEPT_FAILED,

	SOCKET_FAILED,
	SOCKET_SET_SOCKOPT_FAILED,
	SOCKET_BIND_FAILED,
	SOCKET_LISTEN_FAILED,

	CONTEXT_NULLPTR,
};

const std::vector<std::string> err_code_desc = {
				"OK",
				"SERVER_NULLPTR", "SERVER_ALREADY_CREATED", "SERVER ACCEPT FAILED", 
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

void print_errno(int code){
	printf("Socket error code: '%i', desc: '%s'\n", code, strerror(code));
}

int async_listen_ipv4(server* serv){
	for(;;){
		printf("Listening!\n");	
		
		struct sockaddr address;
		int address_len;
		int cli_socket = accept(serv->socket_fd, &address, (socklen_t*)&address_len);	

		if(cli_socket == -1){
			print_errno(errno);
			return err_code::SERVER_ACCEPT_FAILED;
		} 

		struct sockaddr_in *cli_addr = reinterpret_cast<struct sockaddr_in*>(&address);
		char* ip = inet_ntoa(cli_addr->sin_addr);
		printf("Got connection form: '%s'\n", ip);
		
	}

	return 0;
}


int async_listen_ipv6(server *serv){
	for(;;){
		printf("Listening!\n");
		
		int address_len;
		struct sockaddr_in6 address;
		int cli_socket = accept(serv->socket_fd, (struct sockaddr*)&address, (socklen_t*)&address_len);

		if(cli_socket == -1){
			print_errno(errno);
			return err_code::SERVER_ACCEPT_FAILED;
		} 

		char ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &(address.sin6_addr), ip, sizeof(ip));
		printf("Got connection form: '%s'\n", ip);
		
	}

	return 0;
}

err_code server_start_to_listen(context* ctx, server* serv){
	if(serv == nullptr)
		return err_code::SERVER_NULLPTR;

	if(ctx == nullptr)
		return err_code::CONTEXT_NULLPTR;

	if(serv->is_ipv6)
		ctx->listener = std::async(async_listen_ipv6, serv);
	else
		ctx->listener = std::async(async_listen_ipv4, serv);

	return err_code::OK;
}

err_code create_server(server* serv, std::string ip, int port, bool ipv6 = false){
	if(serv == nullptr)
		return err_code::SERVER_NULLPTR;

	if(serv->is_online)
		return err_code::SERVER_ALREADY_CREATED;


	int domain = AF_INET;
	if(ipv6){
		serv->is_ipv6 = ipv6;
		domain = AF_INET6;
	}

	if((serv->socket_fd = socket(domain, SOCK_STREAM, 0)) == -1){
		print_errno(errno);
		return err_code::SOCKET_FAILED;
	}
	
	int opt = 1;
	if(setsockopt(serv->socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1){
		print_errno(errno);
		return err_code::SOCKET_SET_SOCKOPT_FAILED;
	}

	if(ipv6){
		struct sockaddr_in6 address;
		memset(&address, 0, sizeof(struct sockaddr_in6));
	
		address.sin6_family = AF_INET6;
		inet_pton(AF_INET6, ip.c_str(), &address.sin6_addr);
		address.sin6_port = htons(port);

		if(bind(serv->socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
			print_errno(errno);
			return err_code::SOCKET_BIND_FAILED;
		}
	}
	else{
		struct sockaddr_in address;
		memset(&address, 0, sizeof(struct sockaddr_in));
	
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(ip.c_str());
		address.sin_port = htons(port);

		if(bind(serv->socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
			print_errno(errno);
			return err_code::SOCKET_BIND_FAILED;
		}
	}


	if(listen(serv->socket_fd, BACKLOG) == -1){
		print_errno(errno);
		return err_code::SOCKET_LISTEN_FAILED;
	}

	printf("Server created at: '%s' ip and '%i' port.\n", ip.c_str(), port);

	return err_code::OK;
}


 err_code create_client(std::string ipv4_dest, short port_dest){
	

	return err_code::OK;
}





}






















