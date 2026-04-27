#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

#include "simple_log.h"

class Calculator {
public:
    int Add(int a, int b) {
        std::cout << "Add called: " << a << " + " << b << std::endl;
        return a + b;
    }
};

int main() {
    const char* serviceName = "com.example.Calculator";
    const char* objectPath = "/com/example/Calculator";

    auto connection = sdbus::createSessionBusConnection();
    connection->requestName(serviceName);

    auto object = sdbus::createObject(*connection, objectPath);

    Calculator calc;

    object->registerMethod("Add")
        .onInterface("com.example.Calculator")
        .implementedAs([&calc](int a, int b) {
            return calc.Add(a, b);
        });

    object->finishRegistration();

    auto lg = getLogger("server");
    lg(INFO)<<"Server running...";

    connection->enterEventLoop();
}
