#include "system_client.h"

#include <grpcpp/grpcpp.h>
#include <iostream>

int main()
{
    SystemClient client{
        grpc::CreateChannel("localhost:50051",
                            grpc::InsecureChannelCredentials())};

    telemetry::Metrics metrics;
    if (!client.GetMetrics(metrics).ok()) {
        std::cerr << "Could not contact server\n";
        return 1;
    }

    std::cout << "Current mode: " << metrics.mode() << '\n';

    telemetry::ModeResponse response;
    client.SetMode("performance", response);

    std::cout << "Updated: " << response.success() << '\n'
              << "New mode: " << response.current_mode() << '\n';
}
