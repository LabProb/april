#include "system_client.h"

#include <grpcpp/grpcpp.h>

#include <iostream>

namespace {

bool reportTransportFailure(const grpc::Status& status)
{
    if (status.ok()) {
        return false;
    }

    std::cerr << "RPC failed (" << status.error_code() << "): "
              << status.error_message() << '\n';
    return true;
}

} // namespace

int main()
{
    SystemClient client{grpc::CreateChannel("localhost:50051",
                                             grpc::InsecureChannelCredentials())};

    telemetry::Metrics metrics;
    if (reportTransportFailure(client.GetMetrics(metrics))) {
        return 1;
    }
    std::cout << "CPU: " << metrics.cpu_usage() << "%\n"
              << "MEM: " << metrics.memory_usage() << "%\n"
              << "TEMP: " << metrics.temperature() << "\n"
              << "MODE: " << metrics.mode() << "\n";

    telemetry::ModeResponse mode_response;
    if (reportTransportFailure(client.SetMode("performance", mode_response))) {
        return 1;
    }
    std::cout << "Success: " << mode_response.success() << "\n"
              << "Mode: " << mode_response.current_mode() << "\n"
              << "Msg: " << mode_response.message() << "\n";
    return mode_response.success() ? 0 : 1;
}
