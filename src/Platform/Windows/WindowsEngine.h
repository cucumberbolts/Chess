#pragma once

#include <Chess/Engine.h>

#include <Windows.h>

class WindowsEngine : public Engine {
public:
    WindowsEngine(const std::string& path);

    ~WindowsEngine() override;

    void Send(const std::string& message) override;
    void Receive(std::string& message) override;
private:
    HANDLE m_EngineInputWrite;
    HANDLE m_EngineOutputRead;
};
