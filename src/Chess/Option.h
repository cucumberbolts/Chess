#pragma once

#include <limits>
#include <string>
#include <vector>

struct Option {
    enum class Type {
        Check,  // true or false
        Spin,   // integer value
        Combo,  // pre-defined string values (like an enum)
        Button, // no value, just send a command
        String  // text
    };

    static std::string_view TypeToString(Type t) {
        switch (t) {
            case Type::Check:   return "Check";
            case Type::Spin:    return "Spin";
            case Type::Combo:   return "Combo";
            case Type::Button:  return "Button";
            case Type::String:  return "String";
        }
    }

    Option(std::string_view name, Type type) : Name(name), Type(type) {}
    virtual ~Option() = default;

    std::string Name;
    Type Type;
};

struct Check : public Option {
    Check(std::string_view name, bool value)
        : Option(name, Type::Check), Value(value) {}

    inline Check& operator=(bool value) {
        Value = value;
        return *this;
    }

    bool Value;
};

struct Spin : public Option {
    Spin(std::string_view name, int32_t value, int32_t min = std::numeric_limits<int32_t>::min(), int32_t max = std::numeric_limits<int32_t>::max())
        : Option(name, Type::Spin), Value(value), Min(min), Max(max) {}

    inline Spin& operator=(int32_t value) {
        Value = value;
        return *this;
    }

    int32_t Value, Min, Max;
};

struct Combo : public Option {

};

struct Button : public Option {
    Button(std::string_view name)
        : Option(name, Type::Button) {}
};

struct String : public Option {
    String(std::string_view name, std::string_view value)
        : Option(name, Type::String), Value(value) {}

    inline String& operator=(std::string_view value) {
        Value = value;
        return *this;
    }

    std::string Value;
};
