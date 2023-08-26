#pragma once

#include <ChessEngine/Engine.h>

class UnixEngine : public Engine {
public:
    UnixEngine(const std::string& path);

    ~UnixEngine() override;

    void Send(const std::string& message) override;
    bool Receive(std::string& message) override;
};
