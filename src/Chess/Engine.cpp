#include "Engine.h"

#include "Option.h"
#include "StringParser.h"

#include <iostream>

Engine::~Engine() {
    for (auto option : m_Options)
        delete option;
}

bool Engine::Init() {
    std::string message;
    Receive(message); // Receive startup info (we don't need it)

    Send("uci\n");
    Receive(message);

    StringParser sp(std::move(message));

    std::string_view command;
    
    while (sp.Next(command, StringParser::Newline)) {
        // Parse command
        if (command.empty()) {
            continue;
        } else if (command == "uciok") {
            break;
        } else {
            StringParser commandParser{ std::string(command) };
            std::string_view commandType;
            commandParser.Next(commandType);

            if (commandType == "id")
                HandleIdCommand(commandParser);
            else if (commandType == "option")
                HandleOptionCommand(commandParser);
            else
                std::cout << "ERROR: Unknown command \"" << commandType << "\"!\n";
        }
    }
    return true;
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
