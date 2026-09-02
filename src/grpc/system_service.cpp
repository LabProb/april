#include "system_service.h"

grpc::Status SystemService::GetMetrics(grpc::ServerContext*,
                                       const telemetry::Empty*,
                                       telemetry::Metrics* response)
{
    std::lock_guard<std::mutex> lock(mutex_);

    response->set_cpu_usage(42.0F);
    response->set_memory_usage(65.0F);
    response->set_temperature(55.5F);
    response->set_mode(mode_);
    return grpc::Status::OK;
}

grpc::Status SystemService::SetMode(grpc::ServerContext*,
                                    const telemetry::ModeRequest* request,
                                    telemetry::ModeResponse* response)
{
    std::lock_guard<std::mutex> lock(mutex_);

    const std::string& new_mode = request->mode();
    if (new_mode != "eco" && new_mode != "performance") {
        response->set_success(false);
        response->set_current_mode(mode_);
        response->set_message("Invalid mode");
        return grpc::Status::OK;
    }

    mode_ = new_mode;
    response->set_success(true);
    response->set_current_mode(mode_);
    response->set_message("Mode updated");
    return grpc::Status::OK;
}
