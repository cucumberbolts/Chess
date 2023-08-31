#include "Platform/Unix/UnixEngine.h"

#include <iostream>
#include <string_view>

#include <sys/types.h>
#include <sys/wait.h>

std::unique_ptr<Engine> Engine::Create(const std::filesystem::path &path) {
    return std::make_unique<UnixEngine>(path);
}

UnixEngine::UnixEngine(const std::filesystem::path& path) {
    if (!(pipe(m_OutputFilePipes) == 0 && pipe(m_InputFilePipes) == 0)) {
        throw EnginePipeError("Failed to create pipes!");
    }

    int m_EnginePID = fork();
    if (m_EnginePID == -1) {
        throw EngineCreationFailure("Could not create engine!");
    }

    if (m_EnginePID == 0) {
        // Child process
        close(0);
        dup(m_OutputFilePipes[0]);
        close(m_OutputFilePipes[0]);
        close(m_OutputFilePipes[1]);

        close(1);
        dup(m_InputFilePipes[1]);
        close(m_InputFilePipes[1]);
        close(m_InputFilePipes[0]);

        execl(path.c_str(), path.filename().c_str(), (char*)0);

        // If execl() returns, the child process could not start
        throw EngineCreationFailure("Unable to start child process!");
    } else {
        // Parent process

        // Close unneeded pipes
        close(m_OutputFilePipes[0]);
        close(m_InputFilePipes[1]);
    }
}

UnixEngine::~UnixEngine() {
    Stop();

    try {
        Send("quit\n");
    } catch (EnginePipeError&) {
        // TODO: Force termination
    }

    int stat_val;
    pid_t child_pid = waitpid(m_EnginePID, &stat_val, 0);

    if (WIFEXITED(stat_val))
        std::cout << "Child exited with code " << WEXITSTATUS(stat_val) << " \n";
    else
        std::cout << "Child terminated abnormally" << std::endl;

    close(m_InputFilePipes[0]);
    close(m_OutputFilePipes[1]);
}

void UnixEngine::Send(const std::string& message) {
    int bytes_written = write(m_OutputFilePipes[1], message.c_str(), message.size());
    if (bytes_written == -1)
        throw EnginePipeError("Failed to write to pipe!");
}

bool UnixEngine::Receive(std::string& message) {
    constexpr size_t MAX_READ_SIZE = 512;

    size_t offset = 0;

    while (true) {
        // Use select() to see if there is data in the pipe
        fd_set file_descriptor_set;
        FD_ZERO(&file_descriptor_set);
        FD_SET(m_InputFilePipes[0], &file_descriptor_set);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        select(m_InputFilePipes[0] + 1, &file_descriptor_set, NULL, NULL, &tv);

        // If there is no more data in the pipe, return
        int bytes_avail = FD_ISSET(m_InputFilePipes[0], &file_descriptor_set);
        if (!bytes_avail) {
            // Return false if no there was no data in the pipe
            if (offset == 0) {
                message.clear();
                return false;
            }
            return true;
        }

        message.resize(message.size() + MAX_READ_SIZE);

        int bytes_read = read(m_InputFilePipes[0], message.data() + offset, MAX_READ_SIZE);

        if (bytes_read == -1)
            throw EnginePipeError("Failed to read pipe!");

        offset += bytes_read;
    }
}
