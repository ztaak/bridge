#include "bridge.h"



int main(int argc, char** argv){
	bridge::server serv;
	auto ec = bridge::create_server(&serv, "127.0.0.1", 50000); 
	if(ec != bridge::err_code::OK)
		printf("Cannot create server: '%s'\n", bridge::err_code_desc[ec].c_str());


	printf("Hello server!\n");

	return 0;
}














