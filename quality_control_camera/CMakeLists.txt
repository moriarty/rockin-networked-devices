cmake_minimum_required (VERSION 2.8.3) 
project(quality_control_camera)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

find_package(Boost 1.46 REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)
find_package(OpenCV 2.3 REQUIRED)


# generate header and source for proto messages
file(GLOB ProtoFiles "${PROJECT_SOURCE_DIR}/msgs/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_MESSAGE_SOURCES PROTO_MESSAGE_HEADERS ${ProtoFiles})

include_directories(
  build/        # header and sources of .proto messages are generated here
  include/
  ${OpenCV_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)


### LIBRARIES #################################################################
add_library(camera SHARED
  src/camera.cpp
)
target_link_libraries(camera
  ${OpenCV_LIBS}
)

add_library(quality_control_camera_server SHARED
  src/quality_control_camera_server.cpp
  ${PROTO_MESSAGE_SOURCES}
)
target_link_libraries(quality_control_camera_server
  camera
  ${PROTOBUF_LIBRARIES} 
  ${OpenCV_LIBRARIES}
)

### EXECUTABLES #################################################################
add_executable(zmq_server
  apps/zmq_server.cpp
)
target_link_libraries(zmq_server
  quality_control_camera_server
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
