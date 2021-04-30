#!/bin/bash

if gcc\
     ./src/string.c\
   	 ./src/buffer.c\
   	 ./src/util.c\
   	 ./src/main.c\
   	 ./src/plx.c\
   	-lm -lz; then
	echo -e "\033[32mOk!\033[0m"
	rm log.txt
fi

