#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

#include "counter-server-glue.h"
#include "simple_log.h"

class Counter : public com::example::Counter_adaptor
{
public:
    Counter(sdbus::IObject& object)
        : com::example::Counter_adaptor(object)
    {}

private:
    int32_t Add(const int32_t& a, const int32_t& b) override
    {
        auto lg = getLogger("counter add");
        lg << "Counter Add: " << a << " + " << b;
        return a + b;
    }
};

int main()
{
    const char* serviceName = "com.example.Counter";
    const char* objectPath = "/com/example/Counter";

    auto connection = sdbus::createSessionBusConnection();
    connection->requestName(serviceName);

    auto object = sdbus::createObject(*connection, objectPath);

    Counter counter(*object);

    auto lg = getLogger("counter-server");
    lg(INFO) << "Counter server running...";
    
    object->finishRegistration();  
    connection->enterEventLoop();
}