#include "json_utils.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace json {

// Value base class implementation
std::string Value::asString() const {
    if (type_ != Type::STRING) {
        throw std::runtime_error("Value is not a string");
    }
    return static_cast<const StringValue*>(this)->asString();
}

double Value::asNumber() const {
    if (type_ != Type::NUMBER) {
        throw std::runtime_error("Value is not a number");
    }
    return static_cast<const NumberValue*>(this)->asNumber();
}

bool Value::asBoolean() const {
    if (type_ != Type::BOOLEAN) {
        throw std::runtime_error("Value is not a boolean");
    }
    return static_cast<const BooleanValue*>(this)->asBoolean();
}

const Object& Value::asObject() const {
    if (type_ != Type::OBJECT) {
        throw std::runtime_error("Value is not an object");
    }
    return static_cast<const ObjectValue*>(this)->asObject();
}

const Array& Value::asArray() const {
    if (type_ != Type::ARRAY) {
        throw std::runtime_error("Value is not an array");
    }
    return static_cast<const ArrayValue*>(this)->asArray();
}

// NumberValue implementation
std::string NumberValue::toString() const {
    std::ostringstream oss;
    if (value_ == static_cast<int>(value_)) {
        oss << static_cast<int>(value_);
    } else {
        oss << value_;
    }
    return oss.str();
}

// ObjectValue implementation
std::string ObjectValue::toString() const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : value_) {
        if (!first) oss << ",";
        oss << "\"" << pair.first << "\":" << pair.second->toString();
        first = false;
    }
    oss << "}";
    return oss.str();
}

// ArrayValue implementation
std::string ArrayValue::toString() const {
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const auto& value : value_) {
        if (!first) oss << ",";
        oss << value->toString();
        first = false;
    }
    oss << "]";
    return oss.str();
}

// Simple JSON parser implementation
class Parser {
public:
    Parser(const std::string& json) : json_(json), pos_(0) {}

    std::shared_ptr<Value> parse() {
        skipWhitespace();
        return parseValue();
    }

private:
    std::string json_;
    size_t pos_;

    void skipWhitespace() {
        while (pos_ < json_.length() && std::isspace(json_[pos_])) {
            pos_++;
        }
    }

    char peek() {
        if (pos_ >= json_.length()) return '\0';
        return json_[pos_];
    }

    char advance() {
        if (pos_ >= json_.length()) return '\0';
        return json_[pos_++];
    }

    std::shared_ptr<Value> parseValue() {
        skipWhitespace();
        char c = peek();

        switch (c) {
            case '"': return parseString();
            case '{': return parseObject();
            case '[': return parseArray();
            case 't': case 'f': return parseBoolean();
            case 'n': return parseNull();
            default:
                if (c == '-' || std::isdigit(c)) {
                    return parseNumber();
                }
                throw std::runtime_error("Invalid JSON character");
        }
    }

    std::shared_ptr<Value> parseString() {
        if (advance() != '"') {
            throw std::runtime_error("Expected '\"'");
        }

        std::string result;
        while (pos_ < json_.length()) {
            char c = advance();
            if (c == '"') {
                return std::make_shared<StringValue>(result);
            }
            if (c == '\\') {
                c = advance();
                switch (c) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += c; break;
                }
            } else {
                result += c;
            }
        }
        throw std::runtime_error("Unterminated string");
    }

    std::shared_ptr<Value> parseNumber() {
        size_t start = pos_;

        if (peek() == '-') advance();

        if (!std::isdigit(peek())) {
            throw std::runtime_error("Invalid number");
        }

        while (std::isdigit(peek())) {
            advance();
        }

        if (peek() == '.') {
            advance();
            while (std::isdigit(peek())) {
                advance();
            }
        }

        if (peek() == 'e' || peek() == 'E') {
            advance();
            if (peek() == '+' || peek() == '-') advance();
            while (std::isdigit(peek())) {
                advance();
            }
        }

        std::string number_str = json_.substr(start, pos_ - start);
        double value = std::stod(number_str);
        return std::make_shared<NumberValue>(value);
    }

    std::shared_ptr<Value> parseBoolean() {
        if (json_.substr(pos_, 4) == "true") {
            pos_ += 4;
            return std::make_shared<BooleanValue>(true);
        } else if (json_.substr(pos_, 5) == "false") {
            pos_ += 5;
            return std::make_shared<BooleanValue>(false);
        }
        throw std::runtime_error("Invalid boolean value");
    }

    std::shared_ptr<Value> parseNull() {
        if (json_.substr(pos_, 4) == "null") {
            pos_ += 4;
            return std::make_shared<NullValue>();
        }
        throw std::runtime_error("Invalid null value");
    }

    std::shared_ptr<Value> parseObject() {
        if (advance() != '{') {
            throw std::runtime_error("Expected '{'");
        }

        auto obj = std::make_shared<ObjectValue>();
        skipWhitespace();

        if (peek() == '}') {
            advance();
            return obj;
        }

        while (true) {
            skipWhitespace();
            auto key = parseString();
            skipWhitespace();

            if (advance() != ':') {
                throw std::runtime_error("Expected ':'");
            }

            skipWhitespace();
            auto value = parseValue();
            obj->set(key->asString(), value);

            skipWhitespace();
            char c = advance();
            if (c == '}') {
                break;
            } else if (c != ',') {
                throw std::runtime_error("Expected ',' or '}'");
            }
        }

        return obj;
    }

    std::shared_ptr<Value> parseArray() {
        if (advance() != '[') {
            throw std::runtime_error("Expected '['");
        }

        auto arr = std::make_shared<ArrayValue>();
        skipWhitespace();

        if (peek() == ']') {
            advance();
            return arr;
        }

        while (true) {
            skipWhitespace();
            auto value = parseValue();
            arr->push(value);

            skipWhitespace();
            char c = advance();
            if (c == ']') {
                break;
            } else if (c != ',') {
                throw std::runtime_error("Expected ',' or ']'");
            }
        }

        return arr;
    }
};

// Public interface functions
std::shared_ptr<Value> parse(const std::string& json) {
    Parser parser(json);
    return parser.parse();
}

std::shared_ptr<Value> parseFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    return parse(content);
}

bool saveToFile(const std::string& filename, std::shared_ptr<Value> value) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << value->toString();
    return true;
}

// Helper functions
std::shared_ptr<Value> string(const std::string& value) {
    return std::make_shared<StringValue>(value);
}

std::shared_ptr<Value> number(double value) {
    return std::make_shared<NumberValue>(value);
}

std::shared_ptr<Value> boolean(bool value) {
    return std::make_shared<BooleanValue>(value);
}

std::shared_ptr<Value> object() {
    return std::make_shared<ObjectValue>();
}

std::shared_ptr<Value> array() {
    return std::make_shared<ArrayValue>();
}

std::shared_ptr<Value> null() {
    return std::make_shared<NullValue>();
}

} // namespace json