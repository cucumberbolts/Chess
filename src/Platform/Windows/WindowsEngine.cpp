#include "WindowsEngine.h"

#include <iostream>

std::unique_ptr<Engine> Engine::Create(const std::filesystem::path& path) {
    return std::make_unique<WindowsEngine>(std::filesystem::absolute(path).string());
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

    BOOL bSuccess = CreateProcess(path.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo);

    if (!bSuccess) {
        std::cout << "CreateProcess Failed: " << GetLastError() << std::endl;
    }

    // Close handles to the child process and its primary thread.
    // Some applications might keep these handles to monitor the status
    // of the child process, for example. 

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    CloseHandle(hEngineOutputWrite);
    CloseHandle(hEngineInputRead);
}

WindowsEngine::~WindowsEngine() {
    CloseHandle(m_EngineInputWrite);
    CloseHandle(m_EngineOutputRead);
}

bool WindowsEngine::Send(const std::string& message) {
    DWORD dwWritten;

    return WriteFile(m_EngineInputWrite, message.c_str(), (DWORD)message.size(), &dwWritten, NULL);
}

bool WindowsEngine::Receive(std::string& message) {
    DWORD dwRead;
    DWORD dwBytesAvail;
    BOOL bSuccess = PeekNamedPipe(m_EngineOutputRead, NULL, 0, &dwRead, &dwBytesAvail, NULL);
    if (dwBytesAvail == 0) {
        message.clear();
        return true;
    }

    message.resize(dwBytesAvail);
    bSuccess = ReadFile(m_EngineOutputRead, message.data(), dwBytesAvail, &dwRead, NULL);
    //message[dwRead] = '\0';

    if (!bSuccess) {
        std::cout << "Failed to read file: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

uint64_t WindowsEngine::GetTime() {
    return GetTickCount64();
}
