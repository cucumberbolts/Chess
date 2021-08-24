#include "Engine.h"

#include "Option.h"
#include "StringParser.h"

#include <iostream>
#include <sstream>

Engine::~Engine() {
    for (auto option : m_Options)
        delete option;
}

bool Engine::Init() {
    std::string message;
    Receive(message); // Receive startup info (we don't need it)

    Send("uci\n");
    Receive(message);

    StringParser sp(message);
    std::string_view command;
    while (sp.Next(command, "\r\n")) {
        // Parse commands here
        if (command.empty()) {
            continue;
        } else if (command == "uciok") {
            break;
        } else {
            StringParser commandParser{ std::string(command) };
            std::string_view commandType;
            commandParser.Next(commandType);

            if (commandType == "id")
                HandleIdCommand("");
            else if (commandType == "option")
                HandleOptionCommand(commandParser);
            else
                std::cout << "ERROR: Unknown command \"" << commandType << "\"!\n";
        }
    }
    return true;
}

void Engine::HandleOptionCommand(StringParser& sp) {
    //std::cout << "Option command: " << restOfCommand << "\n";

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
        m_Options.push_back(new String(name, "value"));
    } else {
        std::cout << "ERROR: Unknown type command!\n";
    }
}

void Engine::HandleIdCommand(std::string_view restOfCommand) {
    std::cout << "ID command: " << restOfCommand << "\n";

    std::string_view id(restOfCommand.data(), restOfCommand.find(' '));

    if (id == "name")
        m_Name.assign(restOfCommand.data() + restOfCommand.find(' ') + 1, restOfCommand.size() - restOfCommand.find(' '));
    else if (id == "author")
        m_Author.assign(restOfCommand.data() + restOfCommand.find(' ') + 1, restOfCommand.size() - restOfCommand.find(' '));
    else
        std::cout << "ERROR: Unknown id command!\n";
}

void Engine::PrintOptions() {
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
