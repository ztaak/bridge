#include "bridge.h"


int main(int argc, char** argv){
	printf("Hello!'\n");
	bridge::header ph;
	ph.command = 12;

	bridge::create_client("127.0.0.1", 50000);

	return 0;
}










