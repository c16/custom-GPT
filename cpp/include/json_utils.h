#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace json {
    class Value;
    using Object = std::map<std::string, std::shared_ptr<Value>>;
    using Array = std::vector<std::shared_ptr<Value>>;

    enum class Type {
        STRING,
        NUMBER,
        BOOLEAN,
        OBJECT,
        ARRAY,
        NULL_VALUE
    };

    class Value {
    public:
        Value(Type type) : type_(type) {}
        virtual ~Value() = default;

        Type getType() const { return type_; }

        virtual std::string toString() const = 0;

        // Type checking
        bool isString() const { return type_ == Type::STRING; }
        bool isNumber() const { return type_ == Type::NUMBER; }
        bool isBoolean() const { return type_ == Type::BOOLEAN; }
        bool isObject() const { return type_ == Type::OBJECT; }
        bool isArray() const { return type_ == Type::ARRAY; }
        bool isNull() const { return type_ == Type::NULL_VALUE; }

        // Value getters (with type checking)
        virtual std::string asString() const;
        virtual double asNumber() const;
        virtual bool asBoolean() const;
        virtual const Object& asObject() const;
        virtual const Array& asArray() const;

    private:
        Type type_;
    };

    class StringValue : public Value {
    public:
        StringValue(const std::string& value) : Value(Type::STRING), value_(value) {}
        std::string toString() const override { return "\"" + value_ + "\""; }
        std::string asString() const override { return value_; }

    private:
        std::string value_;
    };

    class NumberValue : public Value {
    public:
        NumberValue(double value) : Value(Type::NUMBER), value_(value) {}
        std::string toString() const override;
        double asNumber() const override { return value_; }

    private:
        double value_;
    };

    class BooleanValue : public Value {
    public:
        BooleanValue(bool value) : Value(Type::BOOLEAN), value_(value) {}
        std::string toString() const override { return value_ ? "true" : "false"; }
        bool asBoolean() const override { return value_; }

    private:
        bool value_;
    };

    class ObjectValue : public Value {
    public:
        ObjectValue() : Value(Type::OBJECT) {}
        ObjectValue(const Object& value) : Value(Type::OBJECT), value_(value) {}
        std::string toString() const override;
        const Object& asObject() const override { return value_; }

        void set(const std::string& key, std::shared_ptr<Value> value) {
            value_[key] = value;
        }

        std::shared_ptr<Value> get(const std::string& key) const {
            auto it = value_.find(key);
            return it != value_.end() ? it->second : nullptr;
        }

        bool has(const std::string& key) const {
            return value_.find(key) != value_.end();
        }

    private:
        Object value_;
    };

    class ArrayValue : public Value {
    public:
        ArrayValue() : Value(Type::ARRAY) {}
        ArrayValue(const Array& value) : Value(Type::ARRAY), value_(value) {}
        std::string toString() const override;
        const Array& asArray() const override { return value_; }

        void push(std::shared_ptr<Value> value) {
            value_.push_back(value);
        }

        size_t size() const { return value_.size(); }
        std::shared_ptr<Value> at(size_t index) const {
            return index < value_.size() ? value_[index] : nullptr;
        }

    private:
        Array value_;
    };

    class NullValue : public Value {
    public:
        NullValue() : Value(Type::NULL_VALUE) {}
        std::string toString() const override { return "null"; }
    };

    // Parser functions
    std::shared_ptr<Value> parse(const std::string& json);
    std::shared_ptr<Value> parseFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename, std::shared_ptr<Value> value);

    // Helper functions for creating values
    std::shared_ptr<Value> string(const std::string& value);
    std::shared_ptr<Value> number(double value);
    std::shared_ptr<Value> boolean(bool value);
    std::shared_ptr<Value> object();
    std::shared_ptr<Value> array();
    std::shared_ptr<Value> null();
}