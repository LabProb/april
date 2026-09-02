#pragma once

#include <mutex>
#include <string>

#include <grpcpp/grpcpp.h>

#include "system.grpc.pb.h"

class SystemService final : public telemetry::SystemService::Service {
public:
    grpc::Status GetMetrics(grpc::ServerContext* context,
                            const telemetry::Empty* request,
                            telemetry::Metrics* response) override;

    grpc::Status SetMode(grpc::ServerContext* context,
                         const telemetry::ModeRequest* request,
                         telemetry::ModeResponse* response) override;

private:
    std::string mode_{"eco"};
    std::mutex mutex_;
};
