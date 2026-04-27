cmake_minimum_required(VERSION 3.5)
project(April LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(simple_logger REQUIRED)
find_package(Threads REQUIRED)
find_package(sdbus-c++ REQUIRED)

add_executable(April src/main.cpp)
target_link_libraries(April PRIVATE simple_logger::simple_logger sdbus-c++ Threads::Threads)

set(CMAKE_BUILD_RPATH "$ENV{HOME}/hdd/storage/Logger/Logger/install/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_custom_target(run
    COMMAND April
    DEPENDS April
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_subdirectory(external/googletest)

enable_testing()

add_executable(AprilTests test/test_main.cpp)

target_link_libraries(AprilTests
    PRIVATE
    gtest_main
)

add_test(NAME AprilTests COMMAND AprilTests)

add_executable(server  src/dbus/server.cpp)
target_link_libraries(server PRIVATE simple_logger::simple_logger sdbus-c++)

add_executable(client src/dbus/client.cpp)
target_link_libraries(client PRIVATE simple_logger::simple_logger sdbus-c++)

add_executable(counter_server src/dbus/counter-server.cpp)
target_link_libraries(counter_server PRIVATE simple_logger::simple_logger sdbus-c++)

add_executable(counter_client src/dbus/counter-client.cpp)
target_link_libraries(counter_client PRIVATE simple_logger::simple_logger sdbus-c++)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/counter-client-glue.h
        ${CMAKE_CURRENT_BINARY_DIR}/counter-server-glue.h
    COMMAND sdbus-c++-xml2cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dbus/xml/counter.xml
        --proxy=${CMAKE_CURRENT_BINARY_DIR}/counter-client-glue.h
        --adaptor=${CMAKE_CURRENT_BINARY_DIR}/counter-server-glue.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dbus/xml/counter.xml
)

add_custom_target(dbus_codegen
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/counter-client-glue.h
        ${CMAKE_CURRENT_BINARY_DIR}/counter-server-glue.h
)

add_dependencies(counter_server dbus_codegen)
add_dependencies(counter_client dbus_codegen)

target_include_directories(counter_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(counter_client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# =========================
# gRPC BLOCK
# =========================

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

find_program(PROTOC protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/system.proto)
set(GENERATED_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

set(PROTO_SRC
    ${GENERATED_SRC_DIR}/system.pb.cc
    ${GENERATED_SRC_DIR}/system.grpc.pb.cc
)

add_custom_command(
    OUTPUT ${PROTO_SRC}
    COMMAND ${PROTOC}
    ARGS --grpc_out=${GENERATED_SRC_DIR}
         --cpp_out=${GENERATED_SRC_DIR}
         -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

add_library(system_grpc ${PROTO_SRC})

target_include_directories(system_grpc PUBLIC
    ${GENERATED_SRC_DIR}
)

target_link_libraries(system_grpc
    PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

add_executable(system_server src/grpc/grpc_server.cpp)
target_link_libraries(system_server PRIVATE system_grpc Threads::Threads)

add_executable(system_client src/grpc/grpc_client.cpp)
target_link_libraries(system_client PRIVATE system_grpc Threads::Threads)