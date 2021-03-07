#include "scl.h"

using namespace scl;

int scl::async_listen_ipv4(server* serv){
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


int scl::async_listen_ipv6(server *serv){
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

err_code scl::server_start_to_listen(server* serv){
	if(serv == nullptr)
		return err_code::SERVER_NULLPTR;


	if(serv->is_ipv6)
		serv->listener = std::async(async_listen_ipv6, serv);
	else
		serv->listener = std::async(async_listen_ipv4, serv);

	return err_code::OK;
}

err_code scl::server_create(server* serv, std::string ip, int port, bool ipv6){
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
