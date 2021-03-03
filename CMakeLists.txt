cmake_minimum_required(VERSION 3.10)

# set the project name
project(ABFT VERSION 1.0)

set(CMAKE_CXX_FLAGES "${CMAKE_CXX_FLAGES}" -std=gnu99)
set(CMAKE_C_COMPILER "/home/Research/SimGrid-3.12/bin/smpicc")
set(CMAKE_CXX_COMPILER "/home/Research/SimGrid-3.12/bin/smpicc")

set(SimGrid_LIBRARY "/home/Research/SimGrid-3.12/lib/")

# add the executable
#set(SIMULATOR_SOURCES ${PROJECT_SOURCE_DIR}/src/HammingAbft/HCAbft.c ${PROJECT_SOURCE_DIR}/src/Common/ber.c ${PROJECT_SOURCE_DIR}/src/Common/hamming_cs.c ${PROJECT_SOURCE_DIR}/src/Common/matrix_op.c ${PROJECT_SOURCE_DIR}/src/Common/func.h)
set(SIMULATOR_SOURCES ${PROJECT_SOURCE_DIR}/src/HammingAbft/HCAbft.c ${PROJECT_SOURCE_DIR}/src/Common/func.h ${PROJECT_SOURCE_DIR}/src/Common/matrix_op.c ${PROJECT_SOURCE_DIR}/src/Common/hamming_cs.c ${PROJECT_SOURCE_DIR}/src/Common/ber.c)
add_executable(HCAbft ${SIMULATOR_SOURCES})
target_link_libraries(HCAbft m)

set(MATRIX_MULTIPLICATION ${PROJECT_SOURCE_DIR}/src/MM/MM.c ${PROJECT_SOURCE_DIR}/src/Common/func.h ${PROJECT_SOURCE_DIR}/src/Common/matrix_op.c ${PROJECT_SOURCE_DIR}/src/Common/hamming_cs.c ${PROJECT_SOURCE_DIR}/src/Common/ber.c)
add_executable(MM ${MATRIX_MULTIPLICATION})
target_link_libraries(MM m)

set(HELLO ${PROJECT_SOURCE_DIR}/src/HammingAbft/test.c ${PROJECT_SOURCE_DIR}/src/Common/func.h ${PROJECT_SOURCE_DIR}/src/Common/hamming_cs.c)
add_executable(Hello ${HELLO})
