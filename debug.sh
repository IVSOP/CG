#!/bin/bash

DIR="debug"

if cmake -DCMAKE_BUILD_TYPE=Debug -S . -B $DIR; then
	printf "\n"
	if make -j$(nproc) -C $DIR; then
		printf "\n"
		gdb -ex=r --args $@
	else
		printf ">> build failed\n"
	fi
else
	printf ">> configure failed\n"
fi
