#!/usr/bin/bash

./build/Generator plane 5 2 plane_5_2.3d
./build/Generator plane 2 3 plane_2_3.3d
./build/Generator box 2 3 box_2_3.3d
./build/Generator cone 1 2 4 3 cone_1_2_4_3.3d
./build/Generator cone 2 4 20 5 cone_2_4_20_5.3d
./build/Generator cone 2 4 20 10 cone_2_4_20_10.3d
./build/Generator cylinder 2 4 10 5 cylinder_2_4_10_5.3d
./build/Generator torus 2 4 30 30 torus_2_4_30_30.3d
./build/Generator sphere 2 20 20 sphere_2_20_20.3d
./build/Generator sphere 1 10 10 sphere_1_10_10.3d
./build/Generator sphere 1 20 20 sphere_1_20_20.3d
./build/Generator sphere 1 8 8 sphere_1_8_8.3d
# ./build/Generator patch teapot.patch 10 bezier_10.3d