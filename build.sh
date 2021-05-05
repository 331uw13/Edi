#!/bin/bash

if gcc\
     ./src/string.c\
   	 ./src/buffer.c\
   	 ./src/file_io.c\
   	 ./src/input_handler.c\
   	 ./src/command_handler.c\
   	 ./src/common.c\
   	 ./src/draw.c\
	 ./src/util.c\
   	 ./src/main.c\
   	 ./src/edi.c\
   	 ./src/plx.c\
   	-lm -lz -llua; then
	echo -e "\033[32mOk!\033[0m"
fi

