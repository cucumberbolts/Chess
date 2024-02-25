#pragma once

#include <exception>
#include <string>

class EnginePipeError : public std::exception {
public:
	EnginePipeError(const std::string& message) : m_Message(message) {}

    const char* what() const noexcept override { return m_Message.c_str(); }
private:
    std::string m_Message;
};

class EngineCreationFailure : public std::exception {
public:
	EngineCreationFailure(const std::string& message) : m_Message(message) {}

    const char* what() const noexcept override { return m_Message.c_str(); }
private:
    std::string m_Message;
};

class EngineInvalidOption : public std::exception {
public:
    EngineInvalidOption(const std::string& message) : m_Message(message) {}

    const char* what() const noexcept override { return m_Message.c_str(); }
private:
    std::string m_Message;
};

class EngineNotReady : public std::exception {
public:
    EngineNotReady() = default;

    const char* what() const noexcept override { return "Engine not ready"; }
};
