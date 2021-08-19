#pragma once

#include <string>

class Engine {
public:
    static Engine* Create(const std::string& path);

    virtual ~Engine() = default;

    virtual void Send(const std::string& message) = 0;
    virtual void Receive(std::string& message) = 0;
};
