#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <thread>
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
    static std::unique_ptr<Engine> Create(const std::filesystem::path& path);

    virtual ~Engine();

    bool Init();
    void Run();
    void Stop();

    const std::vector<Option*>& GetOptions() const { return m_Options; }
    bool SetButton(const std::string& name);
    bool SetCheck(const std::string& name, bool value);
    bool SetSpin(const std::string& name, int32_t value);
    bool SetString(const std::string& name, const std::string& value);
    bool SetCombo(const std::string& name, const std::string& value);
    bool SetCombo(const std::string& name, size_t valueIndex);

    void PrintInfo();
private:
    std::string m_Name, m_Author;
    std::vector<Option*> m_Options;

    //std::atomic<State> m_State = State::Uninitialized;
    State m_State = State::Uninitialized;

    static constexpr uint32_t waitTime = 50;

    std::thread m_Thread;
private:
    virtual bool Send(const std::string& message) = 0;
    virtual bool Receive(std::string& message) = 0;

    virtual uint64_t GetTime() = 0;
    
    void RunLoop();

    void HandleCommand(const std::string& text);
    void HandleOptionCommand(StringParser& sp);
    void HandleIdCommand(StringParser& sp);

    std::optional<Option*> FindOption(const std::string& name, Option::OptionType type);
};
