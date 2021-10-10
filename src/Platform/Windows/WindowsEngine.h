#pragma once

#include <Chess/Engine.h>

#include <Windows.h>

class WindowsEngine : public Engine {
public:
    WindowsEngine(const std::string& path);

    ~WindowsEngine() override;

    bool Send(const std::string& message) override;
    bool Receive(std::string& message) override;

    uint64_t GetTime() override;
private:
    HANDLE m_EngineInputWrite;
    HANDLE m_EngineOutputRead;
};
