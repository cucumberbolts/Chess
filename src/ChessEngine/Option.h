#pragma once

#include <limits>
#include <string>
#include <vector>

// TODO: Refactor the chess component of the project into namespaces

// namespace Option?

struct Option {
    enum class OptionType {
        Check,  // boolean value
        Spin,   // integer value
        Combo,  // pre-defined string values (like an enum)
        Button, // no value, just send a command
        String  // text
    };

    static std::string_view TypeToString(OptionType t) {
        switch (t) {
            case OptionType::Check:   return "Check";
            case OptionType::Spin:    return "Spin";
            case OptionType::Combo:   return "Combo";
            case OptionType::Button:  return "Button";
            case OptionType::String:  return "String";
        }
    }

    Option(std::string_view name, OptionType type) : Name(name), Type(type) {}
    virtual ~Option() = default;

    std::string Name;
    OptionType Type;
};

struct Check : public Option {
    Check(std::string_view name, bool value)
        : Option(name, OptionType::Check), Value(value) {}

    inline Check& operator=(bool value) {
        Value = value;
        return *this;
    }

    bool Value;
};

struct Spin : public Option {
    Spin(std::string_view name, int32_t value, int32_t min = std::numeric_limits<int32_t>::min(), int32_t max = std::numeric_limits<int32_t>::max())
        : Option(name, OptionType::Spin), Value(value), Min(min), Max(max) {}

    inline Spin& operator=(int32_t value) {
        Value = value;
        return *this;
    }

    int32_t Value, Min, Max;
};

struct Combo : public Option {
    Combo(std::string_view name, size_t defaultValueIndex, std::vector<std::string>&& values)
        : Option(name, OptionType::Combo), Values(std::move(values)), Value(defaultValueIndex) {}

    std::vector<std::string> Values;
    size_t Value; // std::string_view to default value
};

struct Button : public Option {
    Button(std::string_view name)
        : Option(name, OptionType::Button) {}
};

struct String : public Option {
    String(std::string_view name, std::string_view value)
        : Option(name, OptionType::String), Value(value) {}

    inline String& operator=(std::string_view value) {
        Value = value;
        return *this;
    }

    std::string Value;
};
