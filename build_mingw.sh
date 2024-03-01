#!/bin/bash

DIR="buildWin"

if cmake -DCMAKE_TOOLCHAIN_FILE=TC-mingw.cmake -D-DGLFW_BUILD_DOCS=OFF -S . -B $DIR; then
	printf "\n"
	if make -j$(nproc) -C $DIR; then
		exit 0
	else
		printf ">> build failed\n"
		exit 1
	fi
else
	printf ">> configure failed\n"
	exit 1
fi
