#!/bin/bash

if cmake .; then
	if make; then
		if [[ $1 == "test" ]]; then
			./edi
		fi
	fi
fi


