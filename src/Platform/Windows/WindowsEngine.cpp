#include "WindowsEngine.h"

#include <iostream>

Engine* Engine::Create(const std::string& path) {
    return new WindowsEngine(path);
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

    bool success = CreateProcess(path.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo);

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

void WindowsEngine::Send(const std::string& message) {
    DWORD dwWritten;

    WriteFile(m_EngineInputWrite, message.c_str(), (DWORD)message.size(), &dwWritten, NULL);
}

void WindowsEngine::Receive(std::string& message) {
    DWORD dwRead;
    BOOL bSuccess = FALSE;

    CHAR cData[2048];

    // You have to sleep() before using PeekNamedPipe()
    // DWORD dwBytesAvail;
    //bSuccess = PeekNamedPipe(m_EngineOutputRead, NULL, 0, &dwRead, &dwBytesAvail, NULL);
    //if (dwBytesAvail == 0)
    //    return;

    bSuccess = ReadFile(m_EngineOutputRead, cData, 2048, &dwRead, NULL);
    if (!bSuccess)
        DWORD error = GetLastError();

    message.assign(cData, dwRead);
}
