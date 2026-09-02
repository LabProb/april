#include "system_client.h"

#include <chrono>

SystemClient::SystemClient(std::shared_ptr<grpc::Channel> channel,
                           std::chrono::milliseconds timeout)
    : stub_(telemetry::SystemService::NewStub(std::move(channel))),
      timeout_(timeout)
{
}

grpc::Status SystemClient::GetMetrics(telemetry::Metrics& response) const
{
    telemetry::Empty request;
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + timeout_);
    return stub_->GetMetrics(&context, request, &response);
}

grpc::Status SystemClient::SetMode(const std::string& mode,
                                   telemetry::ModeResponse& response) const
{
    telemetry::ModeRequest request;
    request.set_mode(mode);

    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + timeout_);
    return stub_->SetMode(&context, request, &response);
}
