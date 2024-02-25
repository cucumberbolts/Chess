#pragma once

#include <charconv>
#include <string_view>
#include <optional>

class StringParser {
public:
    StringParser(const std::string& data) : m_Data(data) {}
    StringParser(std::string&& data) : m_Data(std::move(data)) {}

    size_t Position() const { return m_TokenBegin; }

    bool JumpPast(std::string_view str, size_t offset = 0) {
        size_t position = m_Data.find(str, offset);
	    if (position == std::string::npos) {
            return false;
	    }

        m_TokenBegin = position;
        m_TokenEnd = position + str.size();
        return true;
    }

    std::optional<std::string_view> Next(std::string_view delim) {
        if (m_TokenEnd == m_Data.size())
            return std::nullopt;

        m_TokenBegin = m_Data.find_first_not_of(" \t\n", m_TokenEnd);
        if (m_TokenBegin == std::string::npos) {
            m_TokenEnd = m_Data.size();
            return std::nullopt;
        }

        uint64_t delimBegin = std::min(m_Data.find(delim, m_TokenBegin), m_Data.size());
        m_TokenEnd = delimBegin + delim.size();
        // Remove whitespace from the end
        delimBegin = m_Data.find_last_not_of(" \t", delimBegin - 1) + 1;

        return std::string_view(m_Data.data() + m_TokenBegin, delimBegin - m_TokenBegin);
    }

    template<typename T>
    std::optional<T> Next();

    template<>
    std::optional<std::string_view> Next<std::string_view>() {
        if (m_TokenEnd == m_Data.size())
            return std::nullopt;

        m_TokenBegin = m_Data.find_first_not_of(" \t\n", m_TokenEnd);
        if (m_TokenBegin == std::string::npos) {
            m_TokenEnd = m_Data.size();
            return std::nullopt;
        }

        m_TokenEnd = std::min(m_Data.find_first_of(" \t\n", m_TokenBegin), m_Data.size());

        return std::string_view(m_Data.data() + m_TokenBegin, m_TokenEnd - m_TokenBegin);
    }

    template<>
    std::optional<int32_t> Next<int32_t>() {
        auto result = Next<std::string_view>();
        if (!result)
            return std::nullopt;

        int32_t output;
        auto ec = std::from_chars(result.value().data(), result.value().data() + result.value().size(), output);
        if (ec.ec != std::errc{})
            return std::nullopt;

        return output;
    }

    template<>
    std::optional<bool> Next<bool>() {
        if (auto result = Next<std::string_view>()) {
            if (result.value() == "true")
                return std::optional{ true };
            if (result.value() == "false")
                return std::optional{ false };
        }

    	return std::nullopt;
    }

    std::optional<std::string_view> NextLine() {
        if (m_TokenEnd == m_Data.size())
            return std::nullopt;

        m_TokenBegin = m_Data.find_first_not_of(" \t", m_TokenEnd);
        if (m_TokenBegin == std::string::npos) {
            m_TokenEnd = m_Data.size();
            return std::nullopt;
        }

        m_TokenEnd = std::min(m_Data.find('\n', m_TokenBegin), m_Data.size());
        // Shrink the view by 1 if there is a carriage return
        size_t newlineBegin = m_TokenEnd - (m_Data[m_TokenEnd - 1] == '\r');
        m_TokenEnd++;
        
        return std::string_view(m_Data.data() + m_TokenBegin, newlineBegin - m_TokenBegin);
    }

    std::string_view ToEnd() {
        m_TokenBegin = std::min(m_Data.find_first_not_of(" \t\n", m_TokenBegin), m_Data.size());
        m_TokenEnd = m_Data.size();

        return { m_Data.data() + m_TokenBegin, m_TokenEnd - m_TokenBegin };
    }
private:
    std::string m_Data;
    size_t m_TokenBegin = 0, m_TokenEnd = 0;
};
