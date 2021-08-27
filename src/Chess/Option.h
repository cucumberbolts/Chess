#pragma once

#include <functional>
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
    using OnCheckSet = std::function<bool(bool)>;

    Check(std::string_view name, bool value, OnCheckSet setCallback = nullptr)
        : Option(name, Type::Check), Value(value), m_SetCallback(setCallback) {}

    inline Check& operator=(bool value) {
        Value = value;
        m_SetCallback(value);
        return *this;
    }

    bool Value;
private:
    OnCheckSet m_SetCallback;
};

struct Spin : public Option {
    using OnSpinSet = std::function<bool(int32_t)>;

    Spin(std::string_view name, int32_t value, int32_t min = std::numeric_limits<int32_t>::min(), int32_t max = std::numeric_limits<int32_t>::max(), OnSpinSet setCallback = nullptr)
        : Option(name, Type::Spin), Value(value), Min(min), Max(max), m_SetCallback(setCallback) {}

    inline Spin& operator=(int32_t value) {
        Value = value;
        m_SetCallback(value);
        return *this;
    }

    int32_t Value, Min, Max;
private:
    OnSpinSet m_SetCallback;
};

struct Combo : public Option {

};

struct Button : public Option {
    using OnButtonSet = std::function<bool()>;

    Button(std::string_view name, OnButtonSet setCallback = nullptr)
        : Option(name, Type::Button), m_SetCallback(setCallback) {}

    inline void Press() { m_SetCallback(); }
private:
    OnButtonSet m_SetCallback;
};

struct String : public Option {
    using OnStringSet = std::function<bool(std::string_view)>;

    String(std::string_view name, std::string_view value, OnStringSet setCallback = nullptr)
        : Option(name, Type::String), Value(value), m_SetCallback(setCallback) {}

    inline String& operator=(std::string_view value) {
        Value = value;
        m_SetCallback(value);
        return *this;
    }

    std::string Value;
private:
    OnStringSet m_SetCallback;
};
