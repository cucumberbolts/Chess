#include "Engine.h"

#include "Option.h"
#include "StringParser.h"

#include <algorithm>
#include <iostream>
#include <sstream>

Engine::~Engine() {
    for (auto option : m_Options)
        delete option;
}

bool Engine::Init() {
    Send("uci\n");

    //loop till "uciok"
    std::string data;
    uint64_t time = GetTime();
    while (m_State != State::Ready) {
        if (GetTime() - time > 500) {  // Check every 500 milliseconds to avoid calling PeekNamedPipe() a lot
            Receive(data);
            HandleCommand(data);

            time = GetTime();
        }
    }

    std::cout << "Engine initialized!\n";

    return true;
}

bool Engine::SetOption(const std::string& name) {
    if (auto option = FindOption(name)) {
        if (option.value()->Type == Option::Type::Button) {
            std::stringstream ss;
            ss << "setoption name " << name << "\r\n";

            Send(ss.str());

            return true;
        }
    }

    return false;
}

bool Engine::SetOption(const std::string& name, bool value) {
    if (auto option = FindOption(name)) {
        if (option.value()->Type == Option::Type::Check) {
            Check* check = (Check*)option.value();

            check->Value = value;

            std::stringstream ss;
            ss << "setoption name " << name << " value " << std::boolalpha << check->Value << "\r\n";

            Send(ss.str());

            return true;
        }
    }

    return false;
}

bool Engine::SetOption(const std::string& name, int32_t value) {
    if (auto option = FindOption(name)) {
        if (option.value()->Type == Option::Type::Spin) {
            Spin* spin = (Spin*)option.value();

            spin->Value = value;

            std::stringstream ss;
            ss << "setoption name " << name << " value " << spin->Value << "\r\n";

            Send(ss.str());

            return true;
        }
    }

    return false;
}

bool Engine::SetOption(const std::string& name, const std::string& value) {
    if (auto option = FindOption(name)) {
        if (option.value()->Type == Option::Type::String) {
            String* string = (String*)(option.value());

            string->Value = value;

            std::stringstream ss;
            ss << "setoption " << name << " value " << string->Value << "\r\n";

            Send(ss.str());

            return true;
        } else if (option.value()->Type == Option::Type::Combo) {
            return false;
        }
    }

    return false;
}

void Engine::Run() {
    if (m_State != State::Ready) {
        std::cout << "Engine is not ready!\n";
        return;
    }

    std::string data;
    uint64_t time = GetTime();
    while (m_State == State::Running) {
        if (GetTime() - time > 500) {  // Check every 500 milliseconds to avoid calling PeekNamedPipe() a lot
            Receive(data);
            HandleCommand(data);

            time = GetTime();
        }
    }
}

void Engine::HandleCommand(const std::string& text) {
    StringParser sp(text);

    std::string_view command;

    while (sp.Next(command, StringParser::Newline)) {
        // Parse command
        if (command.empty())
            continue;

        StringParser commandParser{ std::string(command) };
        std::string_view commandType;
        commandParser.Next(commandType);

        if (commandType == "uciok") {
            m_State = State::Ready;
            return;
        } else if (commandType == "id") {
            HandleIdCommand(commandParser);
        } else if (commandType == "option") {
            HandleOptionCommand(commandParser);
        } else {
            std::cout << "ERROR: Unknown command \"" << commandType << "\"! Skipping command...\n";
        }
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
        //m_Options.push_back(new Combo());
    } else if (type == "button") {
        m_Options.push_back(new Button(name));
    } else if (type == "string") {
        std::string value;
        sp.Next(value, StringParser::End);
        m_Options.push_back(new String(name, value));
    } else {
        std::cout << "ERROR: Unknown type command!\n";
    }
}

void Engine::HandleIdCommand(StringParser& sp) {
    std::string_view id;
    sp.Next(id);

    if (id == "name")
        sp.Next(m_Name, StringParser::End);
    else if (id == "author")
        sp.Next(m_Author, StringParser::End);
    else
        std::cout << "ERROR: Unknown id command!\n";
}

void Engine::PrintInfo() {
    std::cout << "Name: " << m_Name << "\n";
    std::cout << "Author: " << m_Author << "\n";

    std::cout << "\n---------------- OPTIONS ----------------\n";
    std::cout << "Number of options: " << m_Options.size() << "\n";

    for (Option* option : m_Options) {
        std::cout << "Option name: " << option->Name << " type: " << Option::TypeToString(option->Type);

        if (option->Type == Option::Type::Check) {
            Check* s = (Check*)option;
            std::cout << " Value: " << std::boolalpha << s->Value;
        } else if (option->Type == Option::Type::Spin) {
            Spin* s = (Spin*)option;
            std::cout << " Value: " << s->Value << " Min: " << s->Min << " Max: " << s->Max;
        } else if (option->Type == Option::Type::String) {
            String* s = (String*)option;
            std::cout << " Value: " << s->Value;
        }

        std::cout << "\n";
    }
}

std::optional<Option*> Engine::FindOption(const std::string& name) {
    auto position = std::find_if(m_Options.begin(), m_Options.end(),
        [&](Option* o) {
            if (o->Name.size() != name.size())
                return false;

            for (size_t i = 0; i < name.size(); i++)
                if (std::tolower(o->Name[i]) != std::tolower(name[i]))
                    return false;

            return true;
        }
    );

    return position == m_Options.end() ? std::nullopt : std::optional<Option*>{ *position };
}
