#include "Engine.h"

#include "Option.h"
#include "StringParser.h"

#include <algorithm>
#include <iostream>
#include <sstream>

Engine::~Engine() {
    Stop();

    for (auto option : m_Options)
        delete option;
}

bool Engine::Init() {
    Send("uci\n");

    //loop till "uciok" is received
    std::string data;
#if 0
    uint64_t time = GetTime();
    while (m_State != State::Ready) {
        if (GetTime() - time > waitTime) {  // Check every 50 milliseconds to avoid calling PeekNamedPipe() a lot
            Receive(data);
            HandleCommand(data);

            time = GetTime();
        }
    }
#elif 1
    while (m_State != State::Ready) {
        Receive(data);
        HandleCommand(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
        //Sleep(waitTime);
    }
#elif 1
    while (m_State != State::Ready) {
        Receive(data);
        HandleCommand(data);
    }
#endif

    std::cout << "Engine initialized!\n";

    return true;
}

void Engine::Run() {
    if (m_State != State::Ready) {
        std::cout << "Engine is not ready!\n";
        return;
    }

    m_State = State::Running;

    Send("go\n");
    
    m_Thread = std::thread(&Engine::RunLoop, this);
}

void Engine::Stop() {
    if (m_State == State::Running) {
        Send("stop\n");
        m_State = State::Ready;
        m_Thread.join();
    }
}

bool Engine::SetButton(const std::string& name) {
    if (auto option = FindOption(name, Option::OptionType::Button)) {
        std::stringstream ss;
        ss << "setoption name " << name << "\r\n";

        Send(ss.str());

        return true;
    }

    return false;
}

bool Engine::SetCheck(const std::string& name, bool value) {
    if (auto option = FindOption(name, Option::OptionType::Check)) {
        Check* check = (Check*)option.value();

        check->Value = value;

        std::stringstream ss;
        ss << "setoption name " << name << " value " << std::boolalpha << check->Value << "\r\n";

        Send(ss.str());

        return true;
    }

    return false;
}

bool Engine::SetSpin(const std::string& name, int32_t value) {
    if (auto option = FindOption(name, Option::OptionType::Spin)) {
        Spin* spin = (Spin*)option.value();

        if (value < spin->Min || value > spin->Max)
            return false;

        spin->Value = value;

        std::stringstream ss;
        ss << "setoption name " << name << " value " << spin->Value << "\r\n";

        Send(ss.str());

        return true;
    }

    return false;
}

bool Engine::SetString(const std::string& name, const std::string& value) {
    if (auto option = FindOption(name, Option::OptionType::String)) {
        String* string = (String*)option.value();

        string->Value = value;

        std::stringstream ss;
        ss << "setoption name " << name << " value " << string->Value << "\r\n";

        Send(ss.str());

        return true;
    }

    return false;
}

bool Engine::SetCombo(const std::string& name, const std::string& value) {
    if (auto option = FindOption(name, Option::OptionType::Combo)) {
        Combo* combo = (Combo*)option.value();

        // Don't know if this should be case insensitive as well
        auto valueIndex = std::find(combo->Values.begin(), combo->Values.end(), value);

        if (valueIndex == combo->Values.end())
            return false;

        combo->Value = std::distance(combo->Values.begin(), valueIndex);

        std::stringstream ss;
        ss << "setoption name " << name << " value " << combo->Values[combo->Value] << "\r\n";

        Send(ss.str());

        return true;
    }

    return false;
}

bool Engine::SetCombo(const std::string& name, size_t valueIndex) {
    if (auto option = FindOption(name, Option::OptionType::Combo)) {
        Combo* combo = (Combo*)option.value();

        if (valueIndex > combo->Values.size())
            return false;

        std::stringstream ss;
        ss << "setoption name " << name << " value " << combo->Values[combo->Value] << "\r\n";
        
        Send(ss.str());

        return true;
    }

    return false;
}

void Engine::RunLoop() {
#if 1
    std::string data;
    uint64_t time = GetTime();
    while (m_State == State::Running) {
        if (GetTime() - time > waitTime) {  // Check every 500 milliseconds to avoid calling PeekNamedPipe() a lot
            Receive(data);

            if (!data.empty())
                std::cout << "Received: " << data << "\n";

            HandleCommand(data);

            time = GetTime();
        }
    }
#else
    std::string data;
    while (m_State == State::Running) {
        Receive(data);

        if (!data.empty())
            std::cout << "Received: " << data << "\n";
    }
#endif
}

void Engine::HandleCommand(const std::string& text) {
    StringParser sp(text);

    std::string_view command;

    while (sp.Next(command, StringParser::Delimiter::Newline)) {
        // Parse command
        if (command.empty())
            continue;

        StringParser commandParser{ std::string(command) };
        std::string_view commandType;
        commandParser.Next(commandType);

        if (commandType == "uciok") {
            m_State = State::Ready;
        } else if (commandType == "id") {
            HandleIdCommand(commandParser);
        } else if (commandType == "option") {
            HandleOptionCommand(commandParser);
        } else if (commandType == "info") {
            HandleInfoCommand(commandParser);
        }
        //else if (commandType == "bestmove") {
        //    HandleBestMoveCommand(commandParser);
        //}

        // Ignore undefined commands
    }
}

void Engine::HandleOptionCommand(StringParser& sp) {
    std::string name;
    sp.Next(name);
    sp.Next(name, "type");

    std::string type;
    sp.Next(type, "default");

    if (type == "check") {
        bool value;
        sp.Next(value);
        m_Options.push_back(new Check(name, value));
    } else if (type == "spin") {
        int32_t value, min, max;
        sp.Next(value, "min");
        sp.Next(min, "max");
        sp.Next(max);
        m_Options.push_back(new Spin(name, value, min, max));
    } else if (type == "combo") {
        std::string default;
        sp.Next(default, "var");

        size_t defaultValueIndex = 0;
        std::vector<std::string> values;
        std::string value;
        while (sp.Next(value, "var")) {
            values.push_back(std::move(value));

            if (value == default)
                defaultValueIndex = values.size() - 1;
        }

        m_Options.push_back(new Combo(name, defaultValueIndex, std::move(values)));
    } else if (type == "button") {
        m_Options.push_back(new Button(name));
    } else if (type == "string") {
        std::string value;
        sp.Next(value, StringParser::Delimiter::End);
        m_Options.push_back(new String(name, value));
    }

    // Ignore undefined commands
}

void Engine::HandleIdCommand(StringParser& sp) {
    std::string_view id;
    sp.Next(id);

    if (id == "name")
        sp.Next(m_Name, StringParser::Delimiter::End);
    else if (id == "author")
        sp.Next(m_Author, StringParser::Delimiter::End);

    // Ignore undefined commands
}

void Engine::HandleInfoCommand(StringParser& sp) {
    std::string_view infoType;
    sp.Next(infoType);

    do {
        if (infoType == "string") {
            std::string_view string;
            sp.Next(string, StringParser::Delimiter::End);

            std::cout << "Engine sent string: " << string << "\n";

            return;
        } else if (infoType == "depth") {
            sp.Next(m_BestContinuation.Depth);

            std::cout << "Best continuation depth: " << m_BestContinuation.Depth << "\n";
        } else if (infoType == "pv") {
            m_BestContinuation.Continuation.clear();
            m_Board.Reset();

            std::string_view move;
            while (sp.Next(move))
                m_BestContinuation.Continuation.push_back(m_Board.Move(LongAlgebraicMove(move)));

            return;
        } else if (infoType == "cp") {
            sp.Next(m_BestContinuation.Score);
            m_BestContinuation.Mate = false;

            std::cout << "Best continuation score: " << m_BestContinuation.Score << " centipawns\n";
        } else if (infoType == "mate") {
            sp.Next(m_BestContinuation.Score);
            m_BestContinuation.Mate = true;

            std::cout << "Mate in " << m_BestContinuation.Score << " moves!\n";
        } else if (infoType == "refutation") {
            std::string_view move;
            sp.Next(move);

            std::string_view refutation;
            sp.Next(refutation, StringParser::Delimiter::End);

            std::cout << "Move: " << move << " refutation: " << refutation << "\n";

            return;
        }
    } while (sp.Next(infoType));

    // Ignore undefined commands
}

void Engine::PrintInfo() {
    std::cout << "Name: " << m_Name << "\n";
    std::cout << "Author: " << m_Author << "\n";

    std::cout << "\n---------------- OPTIONS ----------------\n";
    std::cout << "Number of options: " << m_Options.size() << "\n";

    for (Option* option : m_Options) {
        std::cout << "Option name: " << option->Name << " type: " << Option::TypeToString(option->Type);

        if (option->Type == Option::OptionType::Check) {
            Check* c = (Check*)option;
            std::cout << " Value: " << std::boolalpha << c->Value;
        } else if (option->Type == Option::OptionType::Spin) {
            Spin* s = (Spin*)option;
            std::cout << " Value: " << s->Value << " Min: " << s->Min << " Max: " << s->Max;
        } else if (option->Type == Option::OptionType::String) {
            String* s = (String*)option;
            std::cout << " Value: " << s->Value;
        } else if (option->Type == Option::OptionType::Combo) {
            Combo* c = (Combo*)option;
            std::cout << " Values: ";
            for (std::string& v : c->Values)
                std::cout << v << " ";

            std::cout << " Default: " << c->Values[c->Value];
        }

        std::cout << "\n";
    }
}

std::optional<Option*> Engine::FindOption(const std::string& name, Option::OptionType type) {
    auto position = std::find_if(m_Options.begin(), m_Options.end(),
        [&](Option* o) {
            if (o->Name.size() != name.size())
                return false;

            for (size_t i = 0; i < name.size(); i++)
                if (std::tolower(o->Name[i]) != std::tolower(name[i]))
                    return false;

            if (o->Type != type)
                return false;

            return true;
        }
    );

    return position == m_Options.end() ? std::nullopt : std::optional<Option*>{ *position };
}
