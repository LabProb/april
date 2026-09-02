#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "system_client.h"
#include "system_service.h"

namespace {

class SystemRpcTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        grpc::ServerBuilder builder;
        builder.AddListeningPort("127.0.0.1:0", grpc::InsecureServerCredentials(), &port_);
        builder.RegisterService(&service_);
        server_ = builder.BuildAndStart();
        ASSERT_NE(server_, nullptr);
        ASSERT_GT(port_, 0);
    }

    void TearDown() override
    {
        if (server_) {
            server_->Shutdown();
        }
    }

    SystemClient makeClient(std::chrono::milliseconds timeout = std::chrono::seconds(1)) const
    {
        return SystemClient{grpc::CreateChannel("127.0.0.1:" + std::to_string(port_),
                                                grpc::InsecureChannelCredentials()),
                            timeout};
    }

    SystemService service_;
    std::unique_ptr<grpc::Server> server_;
    int port_{};
};

TEST_F(SystemRpcTest, CommunicatesMetricsAndMode)
{
    auto client = makeClient();
    telemetry::Metrics initial_metrics;
    ASSERT_TRUE(client.GetMetrics(initial_metrics).ok());
    EXPECT_EQ(initial_metrics.mode(), "eco");

    telemetry::ModeResponse mode_response;
    ASSERT_TRUE(client.SetMode("performance", mode_response).ok());
    EXPECT_TRUE(mode_response.success());
    EXPECT_EQ(mode_response.current_mode(), "performance");

    telemetry::Metrics updated_metrics;
    ASSERT_TRUE(client.GetMetrics(updated_metrics).ok());
    EXPECT_EQ(updated_metrics.mode(), "performance");
}

TEST_F(SystemRpcTest, InvalidModePropagatesApplicationErrorWithoutChangingState)
{
    auto client = makeClient();
    telemetry::ModeResponse mode_response;

    ASSERT_TRUE(client.SetMode("turbo", mode_response).ok());
    EXPECT_FALSE(mode_response.success());
    EXPECT_EQ(mode_response.current_mode(), "eco");
    EXPECT_EQ(mode_response.message(), "Invalid mode");

    telemetry::Metrics metrics;
    ASSERT_TRUE(client.GetMetrics(metrics).ok());
    EXPECT_EQ(metrics.mode(), "eco");
}

TEST_F(SystemRpcTest, ConcurrentCallsKeepServiceStateConsistent)
{
    constexpr int request_count = 16;
    std::vector<std::thread> threads;
    threads.reserve(request_count);

    for (int index = 0; index < request_count; ++index) {
        threads.emplace_back([this, index] {
            auto client = makeClient();
            telemetry::ModeResponse response;
            EXPECT_TRUE(client.SetMode(index % 2 == 0 ? "eco" : "performance", response).ok());
            EXPECT_TRUE(response.success());
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    auto client = makeClient();
    telemetry::ModeResponse response;
    ASSERT_TRUE(client.SetMode("performance", response).ok());
    EXPECT_EQ(response.current_mode(), "performance");
}

TEST_F(SystemRpcTest, DeadlineBoundsCallsAfterShutdown)
{
    server_->Shutdown();
    auto client = makeClient(std::chrono::milliseconds(100));
    telemetry::Metrics metrics;

    const auto start = std::chrono::steady_clock::now();
    const grpc::Status status = client.GetMetrics(metrics);
    const auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_FALSE(status.ok());
    EXPECT_LT(elapsed, std::chrono::seconds(2));
}

} // namespace
