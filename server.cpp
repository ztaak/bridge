#include "bridge.h"



int main(int argc, char** argv){
	bridge::server serv;
	bridge::context ctx;

	auto ec = bridge::create_server(&serv, "::1", 50000, true); 
	if(ec != bridge::err_code::OK)
		printf("Cannot create server: '%s'\n", bridge::err_code_desc[ec].c_str());


	ec = bridge::server_start_to_listen(&ctx, &serv);

	printf("Hello server!\n");

	return 0;
}














