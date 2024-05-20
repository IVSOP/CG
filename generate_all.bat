@echo off

buildWin\Generator.exe plane 5 2 plane_5_2.3d
buildWin\Generator.exe plane 2 3 plane_2_3.3d
buildWin\Generator.exe box 2 3 box_2_3.3d
buildWin\Generator.exe cone 1 2 4 3 cone_1_2_4_3.3d
buildWin\Generator.exe cone 2 4 20 5 cone_2_4_20_5.3d
buildWin\Generator.exe cone 2 4 20 10 cone_2_4_20_10.3d
buildWin\Generator.exe cylinder 2 4 10 5 cylinder_2_4_10_5.3d
buildWin\Generator.exe torus 2 4 30 30 torus_2_4_30_30.3d
buildWin\Generator.exe torus 55 60 50 50 torus_55_60_50_50.3d
buildWin\Generator.exe torus 6 7 50 50 torus_6_7_50_50.3d
buildWin\Generator.exe sphere 2 20 20 sphere_2_20_20.3d
buildWin\Generator.exe sphere 1 10 10 sphere_1_10_10.3d
buildWin\Generator.exe sphere 1 20 20 sphere_1_20_20.3d
buildWin\Generator.exe sphere 1 8 8 sphere_1_8_8.3d
buildWin\Generator.exe patch teapot.patch 10 bezier_10.3d
buildWin\Generator.exe patch custom_test_files_phase_3\comet_tail.patch 5 comet_tail_5.3d
buildWin\Generator.exe patch custom_test_files_phase_3\comet.patch 5 comet_5.3d

pause
