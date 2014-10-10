cmake_minimum_required (VERSION 2.8.3) 
project(conveyor_belt)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

find_package(Boost 1.46 REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)

# generate header and source for proto messages
file(GLOB ProtoFiles "${PROJECT_SOURCE_DIR}/msgs/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_MESSAGE_SOURCES PROTO_MESSAGE_HEADERS ${ProtoFiles})

include_directories(
  build/        # header and sources of .proto messages are generated here
  include/
  ${Boost_INCLUDE_DIRS}
) 

### LIBRARIES #################################################################
add_library(conveyor_belt_kf_d44 SHARED
  src/conveyor_belt_kf_d44.cpp
)
target_link_libraries(conveyor_belt_kf_d44
  modbus
)

add_library(conveyor_belt_server SHARED
  src/conveyor_belt_server.cpp
  ${PROTO_MESSAGE_SOURCES}
)
target_link_libraries(conveyor_belt_server
  conveyor_belt_kf_d44
  ${PROTOBUF_LIBRARIES} 
)

### EXECUTABLES #################################################################
add_executable(zmq_server
  apps/zmq_server.cpp
)
target_link_libraries(zmq_server
  conveyor_belt_server
  zmq
  ${Boost_LIBRARIES}
)

add_executable(zmq_client_function_test
  apps/zmq_client_function_test.cpp
  ${PROTO_MESSAGE_SOURCES}
)
target_link_libraries(zmq_client_function_test
  zmq
  ${PROTOBUF_LIBRARIES}
)

add_executable(simple_function_test
  apps/simple_function_test.cpp
)
target_link_libraries(simple_function_test
  conveyor_belt_kf_d44
)