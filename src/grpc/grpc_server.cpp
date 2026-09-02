#include "system_service.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

int main()
{
    const std::string server_address{"0.0.0.0:50051"};
    SystemService service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server{builder.BuildAndStart()};
    if (!server) {
        std::cerr << "Unable to start server on " << server_address << '\n';
        return 1;
    }

    std::cout << "Server listening on " << server_address << '\n';
    server->Wait();
}
