# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
# 32-bit
# set(CMAKE_C_COMPILER   i686-w64-mingw32-gcc)
# set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)

# flags for only building and linking statically
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc -DGLEW_STATIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -DGLEW_STATIC")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -DGLEW_STATIC")

# where is the target environment located
# set(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc
#     /home/alex/mingw-install)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# # search headers and libraries in the target environment
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


# had to do sudo update-alternatives --config x86_64-w64-mingw32-gcc and sudo update-alternatives --config x86_64-w64-mingw32-g++ to change to posix threads implementation
