#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "Chess/Board.h"

#include "Option.h"

#include "Utility/StringParser.h"

class Engine {
public:
    static std::unique_ptr<Engine> Create(const std::filesystem::path& path);

    virtual ~Engine();

    bool Init();
    void Run();  // Sends the "go" command
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

    struct {
        std::vector<AlgebraicMove> Continuation;
        LongAlgebraicMove PonderMove;
        int32_t Depth = 0;
        int32_t Score = 0;  // Could be centipawns or mate
        bool Mate = false;  // If the score is mate or centipawns
    } m_BestContinuation;

    Board m_Board;

    std::thread m_Thread;
protected:
    enum class State {
        Uninitialized,
        Ready,
        Running
    };

    State m_State = State::Uninitialized;
private:
    virtual bool Send(const std::string& message) = 0;
    virtual bool Receive(std::string& message) = 0;

    void RunLoop();

    void HandleCommand(const std::string& text);
    void HandleOptionCommand(StringParser& sp);
    void HandleIdCommand(StringParser& sp);
    void HandleInfoCommand(StringParser& sp);

    std::optional<Option*> FindOption(const std::string& name, Option::OptionType type);
};
