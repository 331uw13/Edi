#include <stdio.h>
#include "edi.h"


int main(int argc, char** argv) {
	EDIStart((argc >= 2) ? argv[1] : NULL);
}


