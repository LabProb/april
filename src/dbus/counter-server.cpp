#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

#include "counter-server-glue.h"
class Counter : public com::example::Counter_adaptor
{
public:
    Counter(sdbus::IObject& object)
        : com::example::Counter_adaptor(object)
    {}

private:
    int32_t Add(const int32_t& a, const int32_t& b) override
    {
        std::cout << "Counter Add: " << a << " + " << b << std::endl;
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

    std::cout << "Counter server running..." << std::endl;
    
    object->finishRegistration();  
    connection->enterEventLoop();
}
