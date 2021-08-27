#pragma once

#include <string>
#include <vector>

#include "Option.h"
#include "StringParser.h"

class Engine {
public:
    static Engine* Create(const std::string& path);

    virtual ~Engine();

    virtual void Send(const std::string& message) = 0;
    virtual void Receive(std::string& message) = 0;

    bool Init();

    void PrintInfo();
protected:
    std::string m_Name, m_Author;
    std::vector<Option*> m_Options;
protected:
    void HandleOptionCommand(StringParser& sp);
    void HandleIdCommand(StringParser& sp);
};
