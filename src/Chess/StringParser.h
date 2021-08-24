#pragma once

#include <charconv>
#include <optional>
#include <string_view>

class StringParser {
public:
    StringParser(const std::string& data) : m_Data(data) {}
    StringParser(std::string&& data) : m_Data(std::move(data)) {}

    inline bool Next(std::string_view& output) {
        if (m_TokenEnd == m_Data.size())
            return false;

        m_TokenBegin = m_Data.find_first_not_of(" \t", m_TokenEnd);

        size_t whitespaceBegin = m_Data.find_first_of(" \t\n", m_TokenBegin);
        if (whitespaceBegin == std::string::npos)
            whitespaceBegin = m_Data.size();

        size_t whitespaceEnd = m_Data.find_first_not_of(" \t\n", whitespaceBegin);
        if (whitespaceEnd == std::string::npos)
            whitespaceEnd = m_Data.size();

        output = std::string_view(m_Data.data() + m_TokenBegin, whitespaceBegin - m_TokenBegin);
        m_TokenEnd = whitespaceEnd;

        return true;
    }

    inline bool Next(std::string_view& output, std::string_view stop) {
        if (m_TokenEnd == m_Data.size())
            return false;

        m_TokenBegin = m_TokenEnd;
        m_TokenEnd = m_Data.find(stop, m_TokenBegin);

        if (m_TokenEnd == std::string_view::npos) {
            output = std::string_view(m_Data.data() + m_TokenBegin);  // Read to the end of m_Data
            m_TokenEnd = m_Data.size();
        } else {
            output = std::string_view(m_Data.data() + m_TokenBegin, m_TokenEnd - m_TokenBegin);
            m_TokenEnd += stop.size();  // Prepare for the next read
        }

        // Strip whitespace from ends
        size_t tokenBegin = output.find_first_not_of(" \t");
        if (tokenBegin != std::string_view::npos)
            output.remove_prefix(tokenBegin);

        size_t tokenEnd = output.find_last_not_of(" \t");
        if (tokenEnd != std::string_view::npos)
            output.remove_suffix(output.size() - tokenEnd - 1);

        return true;
    }

    inline bool Next(std::string& output, std::string_view stop = "") {
        std::string_view result;
        bool success = Next(result, stop);
        output = result;
        return success;
    }

    inline bool Next(int32_t& output, std::string_view stop = "") {
        std::string_view result;
        bool success = stop.empty() ? Next(result) : Next(result, stop);

        auto ec = std::from_chars(result.data(), result.data() + result.size(), output);
        success = (ec.ec == std::errc{});

        return success;
    }

    inline bool Next(bool& output, std::string_view stop = "") {
        std::string_view result;
        bool success = stop.empty() ? Next(result) : Next(result, stop);

        if (result == "true")
            output = true;
        else if (result == "false")
            output = false;
        else
            success = false;

        return success;
    }
private:
    std::string m_Data;
    size_t m_TokenBegin = 0, m_TokenEnd = 0;
};
