cmake_minimum_required (VERSION 2.8.3) 
project(drilling_machine)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

find_package(Boost 1.46 REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)
find_package(OODL_YouBot REQUIRED)
find_package(Wiringpi REQUIRED)

add_definitions(-DPROJECT_DIR="${PROJECT_SOURCE_DIR}")

# generate header and source for proto messages
file(GLOB ProtoFiles "${PROJECT_SOURCE_DIR}/msgs/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_MESSAGE_SOURCES PROTO_MESSAGE_HEADERS ${ProtoFiles})

include_directories(
  build/        # header and sources of .proto messages are generated here
  include/
  ${Boost_INCLUDE_DIRS}
  ${OODL_YOUBOT_INCLUDE_DIR}
  ${WIRINGPI_INCLUDE_DIR}
)

### LIBRARIES #################################################################
add_library(drilling_machine SHARED
  src/drilling_machine.cpp
)
target_link_libraries(drilling_machine
  ${OODL_YOUBOT_LIBRARIES}
  ${WIRINGPI_LIBRARIES}
)

add_library(drilling_machine_server SHARED
  src/drilling_machine_server.cpp
  ${PROTO_MESSAGE_SOURCES}
)
target_link_libraries(drilling_machine_server
  drilling_machine
  ${PROTOBUF_LIBRARIES} 
)

### EXECUTABLES #################################################################
add_executable(zmq_server
  apps/zmq_server.cpp
)
target_link_libraries(zmq_server
  drilling_machine_server
  zmq
  ${Boost_LIBRARIES}
)
add_custom_target(setcap ALL
    COMMAND sudo setcap cap_net_raw+ep ${PROJECT_SOURCE_DIR}/bin/zmq_server
)
add_dependencies(setcap zmq_server)

add_executable(zmq_client_function_test
  apps/zmq_client_function_test.cpp
  ${PROTO_MESSAGE_SOURCES}
)
target_link_libraries(zmq_client_function_test
  zmq
  ${PROTOBUF_LIBRARIES}
)

add_executable(connection_status
  apps/connection_status.cpp
  ${PROTO_MESSAGE_SOURCES}
)
target_link_libraries(connection_status
  zmq
  ${PROTOBUF_LIBRARIES}
)
