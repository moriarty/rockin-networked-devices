cmake_minimum_required (VERSION 2.8.3) 
project(conveyor_belt)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

find_package(Boost REQUIRED COMPONENTS system)

include_directories(
  include/
  ${Boost_INCLUDE_DIRS}
) 

add_library(conveyor_belt_kf_d44 SHARED
  src/conveyor_belt_kf_d44.cpp
)
target_link_libraries(conveyor_belt_kf_d44
  modbus
)

add_executable(simple_function_test
  apps/simple_function_test
)
target_link_libraries(simple_function_test
  conveyor_belt_kf_d44
)