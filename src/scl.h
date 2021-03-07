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

namespace scl{
	const int BACKLOG = 8;


	struct server{
		std::future<int> listener;
		std::vector<std::future<int>> connections;

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

	};

	const std::vector<std::string> err_code_desc = {
					"OK",
					"SERVER_NULLPTR", "SERVER_ALREADY_CREATED", "SERVER ACCEPT FAILED",
					"SOCKET_FAILED", "SOCKET_SET_SOCKOPT_FAILED", "SOCKET_BIND_FAILED", "SOCKET_LISTEN_FAILED"};


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


	int async_listen_ipv4(server* serv);
	int async_listen_ipv6(server *serv);

	extern err_code server_create(server* serv, std::string ip, int port, bool ipv6 = false);
	extern err_code client_create(std::string ipv4_dest, short port_dest);

	extern err_code server_start_to_listen(server* serv);



}
