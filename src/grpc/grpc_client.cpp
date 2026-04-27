#include <grpcpp/grpcpp.h>
#include "system.grpc.pb.h"

#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class SystemClient {
public:
    SystemClient(std::shared_ptr<Channel> channel)
        : stub_(telemetry::SystemService::NewStub(channel)) {}

    void GetMetrics() {
        telemetry::Empty request;
        telemetry::Metrics response;
        ClientContext context;

        Status status = stub_->GetMetrics(&context, request, &response);

        if (status.ok()) {
            std::cout << "CPU: " << response.cpu_usage() << "%\n";
            std::cout << "MEM: " << response.memory_usage() << "%\n";
            std::cout << "TEMP: " << response.temperature() << "\n";
            std::cout << "MODE: " << response.mode() << "\n";
        }
    }

    void SetMode(const std::string& mode) {
        telemetry::ModeRequest request;
        request.set_mode(mode);

        telemetry::ModeResponse response;
        ClientContext context;

        Status status = stub_->SetMode(&context, request, &response);

        if (status.ok()) {
            std::cout << "Success: " << response.success() << "\n";
            std::cout << "Mode: " << response.current_mode() << "\n";
            std::cout << "Msg: " << response.message() << "\n";
        }
    }

private:
    std::unique_ptr<telemetry::SystemService::Stub> stub_;
};

int main() {
    SystemClient client(
        grpc::CreateChannel("localhost:50051",
                            grpc::InsecureChannelCredentials()));

    client.GetMetrics();
    client.SetMode("performance");
    client.GetMetrics();
}