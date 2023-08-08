#include "WindowsEngine.h"

std::unique_ptr<Engine> Engine::Create(const std::filesystem::path& path) {
    return std::make_unique<WindowsEngine>(path.string());
}

WindowsEngine::WindowsEngine(const std::string& path) {
    HANDLE hEngineOutputWrite, hEngineInputRead;  // Don't need to keep track of these

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    CreatePipe(&m_EngineOutputRead, &hEngineOutputWrite, &sa, 0);
    SetHandleInformation(m_EngineOutputRead, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&hEngineInputRead, &m_EngineInputWrite, &sa, 0);
    SetHandleInformation(m_EngineInputWrite, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;

    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
    startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.hStdError = hEngineOutputWrite;
    startupInfo.hStdOutput = hEngineOutputWrite;
    startupInfo.hStdInput = hEngineInputRead;
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(path.c_str(), NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo)) {
        CloseHandle(hEngineOutputWrite);
        CloseHandle(hEngineInputRead);
        CloseHandle(m_EngineInputWrite);
        CloseHandle(m_EngineOutputRead);

        throw EngineCreationFailure("CreateProcess() failed: " + std::to_string(GetLastError()));
    }

    // Close handles to the child process and its primary thread.
    // Some applications might keep these handles to monitor the status
    // of the child process, for example. 

    m_ProcessHandle = processInfo.hProcess;

    CloseHandle(processInfo.hThread);

    CloseHandle(hEngineOutputWrite);
    CloseHandle(hEngineInputRead);
}

WindowsEngine::~WindowsEngine() {
    Stop();

    try {
        Send("quit\n");
    } catch (EnginePipeError&) {
        TerminateProcess(m_ProcessHandle, 1);  // Force termination
    }

    WaitForSingleObject(m_ProcessHandle, INFINITE);

    CloseHandle(m_EngineInputWrite);
    CloseHandle(m_EngineOutputRead);
    CloseHandle(m_ProcessHandle);
}

void WindowsEngine::Send(const std::string& message) {
    DWORD dwWritten;
    if (!WriteFile(m_EngineInputWrite, message.c_str(), (DWORD)message.size(), &dwWritten, NULL))
        throw EnginePipeError("Failed to write to pipe! Win32 error code: " + std::to_string(GetLastError()));
}

bool WindowsEngine::Receive(std::string& message) {
    DWORD dwRead;
    DWORD dwBytesAvail;
    if (!PeekNamedPipe(m_EngineOutputRead, NULL, 0, &dwRead, &dwBytesAvail, NULL))
        throw EnginePipeError("Failed to peek pipe! Win32 error code: " + std::to_string(GetLastError()));

    if (dwBytesAvail == 0) {
        message.clear();
        return false;
    }

    message.resize(dwBytesAvail);

    if (!ReadFile(m_EngineOutputRead, message.data(), dwBytesAvail, &dwRead, NULL))
        throw EnginePipeError("Failed to read pipe! Win32 error code: " + std::to_string(GetLastError()));

    return true;
}
