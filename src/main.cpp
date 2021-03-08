#include <cstdio>
#include "edi.hpp"

int main(int argc, char** argv) {
	Edi edi((argc == 2) ? argv[1] : "");
	if(edi.seems_fine()) {
		edi.start();
	}
	else {
		return -1;
	}
	return 0;
}

