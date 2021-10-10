#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "Option.h"
#include "StringParser.h"

class Engine {
private:
    enum class State {
        Uninitialized,
        Ready,
        Running
    };
public:
    static Engine* Create(std::filesystem::path path);

    virtual ~Engine();

    bool Init();

    void Run();

    const std::vector<Option*>& GetOptions() const { return m_Options; }
    bool SetOption(const std::string& name);  // Buttons
    bool SetOption(const std::string& name, bool value);  // Checks
    bool SetOption(const std::string& name, int32_t value);  // Spins
    bool SetOption(const std::string& name, const std::string& value);  // Strings and Combos

    void PrintInfo();
private:
    std::string m_Name, m_Author;
    std::vector<Option*> m_Options;

    State m_State = State::Uninitialized;
private:
    virtual bool Send(const std::string& message) = 0;
    virtual bool Receive(std::string& message) = 0;

    virtual uint64_t GetTime() = 0;

    void HandleCommand(const std::string& text);
    void HandleOptionCommand(StringParser& sp);
    void HandleIdCommand(StringParser& sp);

    std::optional<Option*> FindOption(const std::string& name);
};
