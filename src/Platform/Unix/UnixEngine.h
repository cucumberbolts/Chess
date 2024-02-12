#pragma once

#include "Engine/Engine.h"

#include <unistd.h>

#include <filesystem>

class UnixEngine : public Engine {
public:
    UnixEngine(const std::filesystem::path& path);

    ~UnixEngine() override;

    void Send(const std::string& message) override;
    bool Receive(std::string& message) override;
private:
    // m_InputFilePipes[0] reads from the child process' standard output
    // m_OutputFilePipes[1] writes to the child process' standard input
    // m_InputFilePipes[1] and m_OutputFilePipes[0] are closed (they are not used)
    int m_InputFilePipes[2];
    int m_OutputFilePipes[2];

    // The process ID of the child process (the chess engine)
    pid_t m_EnginePID;
};
