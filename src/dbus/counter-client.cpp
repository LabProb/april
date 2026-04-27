#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

#include "counter-client-glue.h"
#include "simple_log.h"

class CounterProxy : public com::example::Counter_proxy
{
public:
    CounterProxy(sdbus::IProxy& proxy)
        : com::example::Counter_proxy(proxy)
    {}
};

int main()
{
    const char* serviceName = "com.example.Counter";
    const char* objectPath = "/com/example/Counter";

    auto connection = sdbus::createSessionBusConnection();

    auto proxy = sdbus::createProxy(*connection, serviceName, objectPath);

    CounterProxy counter(*proxy);

    int result = counter.Add(10, 20);

    auto lg = getLogger("counter-client");
    lg(INFO) << "Result: " << result;
}