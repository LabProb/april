#pragma once

#include <chrono>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "system.grpc.pb.h"

class SystemClient {
public:
    explicit SystemClient(std::shared_ptr<grpc::Channel> channel,
                          std::chrono::milliseconds timeout = std::chrono::seconds(5));

    grpc::Status GetMetrics(telemetry::Metrics& response) const;
    grpc::Status SetMode(const std::string& mode, telemetry::ModeResponse& response) const;

private:
    std::unique_ptr<telemetry::SystemService::Stub> stub_;
    std::chrono::milliseconds timeout_;
};
