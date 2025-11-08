cmake_minimum_required(VERSION 3.10)
project(bmw_voice_control)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#----------------------------------------------------------
# 1. find the libraries you need
#----------------------------------------------------------
set(CMAKE_PREFIX_PATH "$ENV{HOME}/riva_deps")   # your custom install
find_package(Protobuf REQUIRED CONFIG)
find_package(gRPC   REQUIRED CONFIG)
find_package(ALSA   REQUIRED)                   # if you use it

#----------------------------------------------------------
# 2. pick up the generated protobuf / gRPC sources
#----------------------------------------------------------
file(GLOB PROTO_SRCS
     "${CMAKE_CURRENT_SOURCE_DIR}/riva/proto/*.pb.cc"
     "${CMAKE_CURRENT_SOURCE_DIR}/riva/proto/*.grpc.pb.cc")

#----------------------------------------------------------
# 3. create the executable from *your* code + generated code
#----------------------------------------------------------
add_executable(bmw_voice_control
               bmw_voice_control.cpp
               ${PROTO_SRCS})

#----------------------------------------------------------
# 4. include directories
#----------------------------------------------------------
target_include_directories(bmw_voice_control PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${PROTOBUF_INCLUDE_DIRS}
                           ${ALSA_INCLUDE_DIRS})

#----------------------------------------------------------
# 5. link libraries
#----------------------------------------------------------
target_link_libraries(bmw_voice_control
                      gRPC::grpc++
                      protobuf::libprotobuf
                      ${ALSA_LIBRARIES}
                      pthread)
