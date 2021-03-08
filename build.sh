#!/bin/bash

if cmake -j4 . ; then
	if make ; then
		echo -e "\n\033[32m > Build completed in "$SECONDS" seconds!\033[0m"
	fi
fi

