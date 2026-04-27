#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

#include "simple_log.h"

int main() {
    const char* serviceName = "com.example.Calculator";
    const char* objectPath = "/com/example/Calculator";
    const char* interfaceName = "com.example.Calculator";

    auto connection = sdbus::createSessionBusConnection();

    auto proxy = sdbus::createProxy(*connection, serviceName, objectPath);
    proxy->finishRegistration();

    int result;

    proxy->callMethod("Add")
        .onInterface(interfaceName)
        .withArguments(5, 7)
        .storeResultsTo(result);

    auto lg = getLogger("client");
    lg(INFO)<< "Result: " << result;
}
