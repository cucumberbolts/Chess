#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Option.h"
#include "StringParser.h"

class Engine {
public:
    static Engine* Create(const std::string& path);

    virtual ~Engine();

    bool Init();

    // These should be case insensitive
    bool SetOption(const std::string& name);  // Buttons
    bool SetOption(const std::string& name, bool value);  // Checks
    bool SetOption(const std::string& name, int32_t value);  // Spins
    bool SetOption(const std::string& name, const std::string& value);  // Strings and Combos

    void PrintInfo();
protected:
    virtual void Send(const std::string& message) = 0;
    virtual void Receive(std::string& message) = 0;
private:
    std::string m_Name, m_Author;
    std::vector<Option*> m_Options;
private:
    void HandleOptionCommand(StringParser& sp);
    void HandleIdCommand(StringParser& sp);

    std::optional<Option*> FindOption(const std::string& name);
};
