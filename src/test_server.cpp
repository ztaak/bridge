#include "scl.h"

int main(){
	scl::server serv;

	auto ec = scl::server_create(&serv, "::1", 50000, true);
	if(ec != scl::err_code::OK)
		printf("Cannot create server: '%s'\n", scl::err_code_desc[ec].c_str());


	ec = scl::server_start_to_listen(&serv);
	if(ec != scl::err_code::OK)
		printf("Cannot listen to server: '%s'\n", scl::err_code_desc[ec].c_str());

	printf("Hello server!\n");

	return 0;
}
