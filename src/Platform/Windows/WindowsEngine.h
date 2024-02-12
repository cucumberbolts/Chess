#pragma once

#include "Engine/Engine.h"

#include <Windows.h>

class WindowsEngine : public Engine {
public:
    WindowsEngine(const std::string& path);

    ~WindowsEngine() override;

    void Send(const std::string& message) override;
    bool Receive(std::string& message) override;
private:
    HANDLE m_EngineInputWrite = NULL;
    HANDLE m_EngineOutputRead = NULL;
    HANDLE m_ProcessHandle = NULL;
};
