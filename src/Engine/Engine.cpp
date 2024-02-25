#include "Engine.h"

#include "Option.h"
#include "Utility/StringParser.h"

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
    while (m_State != State::Ready) {
        if (Receive(data))
			HandleCommand(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return true;
}

void Engine::Run() {
    if (m_State != State::Ready) {
        throw EngineNotReady();
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
    if (FindOption(name, Option::OptionType::Button)) {
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
    try {
        std::string data;
        while (m_State == State::Running) {
            if (Receive(data))
                HandleCommand(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    } catch (std::exception&) {
        m_ThreadException = std::current_exception();
    }
}

void Engine::HandleCommand(const std::string& text) {
    StringParser sp(text);
    
    while (auto c = sp.NextLine()) {
        std::string_view command = c.value();

        // Parse command
        if (command.empty())
            continue;

        StringParser commandParser{ std::string(command) };
        std::string_view commandType = commandParser.Next<std::string_view>().value_or("no command name given");

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
#if _DEBUG
    	else {
            std::cout << "Engine sent unknown command: " << command << "\n";
        }
#endif
    }
}

void Engine::HandleOptionCommand(StringParser& sp) {
    sp.JumpPast("name");
    std::string_view name = sp.Next("type").value_or("no name");

    auto t = sp.Next<std::string_view>();
    if (!t)
        throw EngineInvalidOption("Invalid 'option' command");
    std::string_view type = t.value();
    sp.JumpPast("default");

    if (type == "check") {
        bool value = sp.Next<bool>().value_or(false);
        m_Options.push_back(new Check(name, value));
    } else if (type == "spin") {
        int32_t value = sp.Next<int32_t>().value_or(0);
        sp.JumpPast("min");
        int32_t min = sp.Next<int32_t>().value_or(value);
        sp.JumpPast("max");
        int32_t max = sp.Next<int32_t>().value_or(value);
        m_Options.push_back(new Spin(name, value, min, max));
    } else if (type == "combo") {
        std::string_view defaultValue = sp.Next("var").value();

        size_t defaultValueIndex = 0;
        std::vector<std::string> values;
        while (auto value = sp.Next("var")) {
            if (value.value() == defaultValue)
                defaultValueIndex = values.size();

            values.push_back(std::string(value.value()));
        }

        m_Options.push_back(new Combo(name, defaultValueIndex, std::move(values)));
    } else if (type == "button") {
        m_Options.push_back(new Button(name));
    } else if (type == "string") {
        std::string_view value = sp.ToEnd();
        m_Options.push_back(new String(name, value));
    }

    // Ignore undefined commands
}

void Engine::HandleIdCommand(StringParser& sp) {
    auto id = sp.Next<std::string_view>();

    if (!id)
        throw EngineInvalidOption("Invalid 'id' command");

    if (id.value() == "name")
        m_Name = sp.ToEnd();
    else if (id.value() == "author")
        m_Author = sp.ToEnd();

    // Ignore undefined commands
}

void Engine::HandleInfoCommand(StringParser& sp) {
    while (auto result = sp.Next<std::string_view>()) {
        std::string_view infoType = result.value();

        if (infoType == "string") {
            std::string_view string = sp.ToEnd();

            // TODO: Make callback for this

            return;
        } else if (infoType == "depth") {
            m_BestContinuation.Depth = sp.Next<int32_t>().value();
        } else if (infoType == "pv") {
            m_BestContinuation.Continuation.clear();
            
            while (auto move = sp.Next<std::string_view>())
                m_BestContinuation.Continuation.emplace_back(move.value());

            m_UpdateCallback(m_BestContinuation);

            return;
        } else if (infoType == "cp") {
            m_BestContinuation.Score = sp.Next<int32_t>().value_or(0);
            m_BestContinuation.Mate = false;
        } else if (infoType == "mate") {
            m_BestContinuation.Score = sp.Next<int32_t>().value_or(0);
            m_BestContinuation.Mate = true;
        } else if (infoType == "refutation") {
            std::string_view move = sp.Next<std::string_view>().value_or("");

            std::string_view refutation = sp.ToEnd();

            return;
        }
    }

    // Ignore undefined commands
}

void Engine::PrintInfo() const {
    std::cout << "Name: " << m_Name << "\n";
    std::cout << "Author: " << m_Author << "\n";

    std::cout << "\n---------------- OPTIONS ----------------\n";
    std::cout << "Number of options: " << m_Options.size() << "\n";

    for (Option* option : m_Options) {
        std::cout << "Option Name: " << option->Name << " Type: " << Option::TypeToString(option->Type);

        switch (option->Type) {
            case Option::OptionType::Check: {
                Check* c = (Check*)option;
                std::cout << " Value: " << std::boolalpha << c->Value;
                break;
            }
            case Option::OptionType::Spin: {
                Spin* s = (Spin*)option;
                std::cout << " Value: " << s->Value << " Min: " << s->Min << " Max: " << s->Max;
                break;
            }
            case Option::OptionType::Combo: {
                Combo* c = (Combo*)option;
                std::cout << " Values: ";
                for (std::string& v : c->Values)
                    std::cout << v << " ";

                std::cout << " Default: " << c->Values[c->Value];
                break;
            }
            case Option::OptionType::Button: {
                break;
            }
            case Option::OptionType::String: {
                String* s = (String*)option;
                std::cout << " Value: " << s->Value;
                break;
            }
        }

        std::cout << "\n";
    }
}

void Engine::SetPosition(const std::string& fen) {
    if (m_State == State::Running) {
        // TODO: Can this be improved (better thread synchronization)? 
        Stop();

        // Clear the pipe
        std::string message;
        Receive(message);

        Send("position fen " + fen + "\n");

        Run();
    } else {
        Send("position fen " + fen + "\n");
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
