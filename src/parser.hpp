// Compile: (Included in loneshooter.cpp compilation)
// Run: N/A - Header only

#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cwchar>

namespace JSON {

enum ValueType {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
};

struct Value;

struct Value {
    ValueType type;
    bool boolValue;
    double numValue;
    std::wstring strValue;
    std::vector<Value> arrayValue;
    std::map<std::wstring, Value> objectValue;
    
    Value() : type(JSON_NULL), boolValue(false), numValue(0) {}
    
    bool isNull() const { return type == JSON_NULL; }
    bool isBool() const { return type == JSON_BOOL; }
    bool isNumber() const { return type == JSON_NUMBER; }
    bool isString() const { return type == JSON_STRING; }
    bool isArray() const { return type == JSON_ARRAY; }
    bool isObject() const { return type == JSON_OBJECT; }
    
    bool asBool() const { return boolValue; }
    double asNumber() const { return numValue; }
    int asInt() const { return (int)numValue; }
    const std::wstring& asString() const { return strValue; }
    const std::vector<Value>& asArray() const { return arrayValue; }
    const std::map<std::wstring, Value>& asObject() const { return objectValue; }
    
    bool has(const std::wstring& key) const {
        if (type != JSON_OBJECT) return false;
        return objectValue.find(key) != objectValue.end();
    }
    
    const Value& get(const std::wstring& key) const {
        static Value nullVal;
        if (type != JSON_OBJECT) return nullVal;
        auto it = objectValue.find(key);
        if (it == objectValue.end()) return nullVal;
        return it->second;
    }
    
    const Value& get(size_t index) const {
        static Value nullVal;
        if (type != JSON_ARRAY || index >= arrayValue.size()) return nullVal;
        return arrayValue[index];
    }
    
    size_t size() const {
        if (type == JSON_ARRAY) return arrayValue.size();
        if (type == JSON_OBJECT) return objectValue.size();
        return 0;
    }
    
    std::vector<std::wstring> keys() const {
        std::vector<std::wstring> result;
        if (type == JSON_OBJECT) {
            for (const auto& kv : objectValue) {
                result.push_back(kv.first);
            }
        }
        return result;
    }
};

class Parser {
private:
    std::wstring json;
    size_t pos;
    
    void skipWhitespace() {
        while (pos < json.size() && (json[pos] == L' ' || json[pos] == L'\n' || json[pos] == L'\r' || json[pos] == L'\t')) {
            pos++;
        }
    }
    
    std::wstring parseString() {
        if (json[pos] != L'"') return L"";
        pos++;
        std::wstring result;
        while (pos < json.size() && json[pos] != L'"') {
            if (json[pos] == L'\\' && pos + 1 < json.size()) {
                pos++;
                switch (json[pos]) {
                    case L'n': result += L'\n'; break;
                    case L'r': result += L'\r'; break;
                    case L't': result += L'\t'; break;
                    case L'\\': result += L'\\'; break;
                    case L'"': result += L'"'; break;
                    case L'/': result += L'/'; break;
                    case L'u': {
                        if (pos + 4 < json.size()) {
                            wchar_t hex[5] = {json[pos+1], json[pos+2], json[pos+3], json[pos+4], 0};
                            wchar_t ch = (wchar_t)wcstol(hex, nullptr, 16);
                            result += ch;
                            pos += 4;
                        }
                        break;
                    }
                    default: result += json[pos]; break;
                }
            } else {
                result += json[pos];
            }
            pos++;
        }
        if (pos < json.size()) pos++;
        return result;
    }
    
    double parseNumber() {
        size_t start = pos;
        if (json[pos] == L'-') pos++;
        while (pos < json.size() && (json[pos] >= L'0' && json[pos] <= L'9')) pos++;
        if (pos < json.size() && json[pos] == L'.') {
            pos++;
            while (pos < json.size() && (json[pos] >= L'0' && json[pos] <= L'9')) pos++;
        }
        if (pos < json.size() && (json[pos] == L'e' || json[pos] == L'E')) {
            pos++;
            if (pos < json.size() && (json[pos] == L'+' || json[pos] == L'-')) pos++;
            while (pos < json.size() && (json[pos] >= L'0' && json[pos] <= L'9')) pos++;
        }
        std::wstring numStr = json.substr(start, pos - start);
        return wcstod(numStr.c_str(), nullptr);
    }
    
    Value parseValue() {
        Value val;
        skipWhitespace();
        
        if (pos >= json.size()) return val;
        
        wchar_t c = json[pos];
        
        if (c == L'"') {
            val.type = JSON_STRING;
            val.strValue = parseString();
        }
        else if (c == L'{') {
            val.type = JSON_OBJECT;
            pos++;
            skipWhitespace();
            while (pos < json.size() && json[pos] != L'}') {
                skipWhitespace();
                if (json[pos] != L'"') break;
                std::wstring key = parseString();
                skipWhitespace();
                if (pos < json.size() && json[pos] == L':') pos++;
                skipWhitespace();
                Value v = parseValue();
                val.objectValue[key] = v;
                skipWhitespace();
                if (pos < json.size() && json[pos] == L',') pos++;
                skipWhitespace();
            }
            if (pos < json.size()) pos++;
        }
        else if (c == L'[') {
            val.type = JSON_ARRAY;
            pos++;
            skipWhitespace();
            while (pos < json.size() && json[pos] != L']') {
                Value v = parseValue();
                val.arrayValue.push_back(v);
                skipWhitespace();
                if (pos < json.size() && json[pos] == L',') pos++;
                skipWhitespace();
            }
            if (pos < json.size()) pos++;
        }
        else if (c == L't' && pos + 3 < json.size() && json.substr(pos, 4) == L"true") {
            val.type = JSON_BOOL;
            val.boolValue = true;
            pos += 4;
        }
        else if (c == L'f' && pos + 4 < json.size() && json.substr(pos, 5) == L"false") {
            val.type = JSON_BOOL;
            val.boolValue = false;
            pos += 5;
        }
        else if (c == L'n' && pos + 3 < json.size() && json.substr(pos, 4) == L"null") {
            val.type = JSON_NULL;
            pos += 4;
        }
        else if (c == L'-' || (c >= L'0' && c <= L'9')) {
            val.type = JSON_NUMBER;
            val.numValue = parseNumber();
        }
        
        return val;
    }
    
public:
    Value parse(const std::wstring& jsonStr) {
        json = jsonStr;
        pos = 0;
        return parseValue();
    }
    
    Value parse(const std::string& jsonStr) {
        json.clear();
        if (jsonStr.empty()) return Value();
        int len = MultiByteToWideChar(CP_UTF8, 0, jsonStr.c_str(), -1, NULL, 0);
        wchar_t* wstr = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, jsonStr.c_str(), -1, wstr, len);
        json = wstr;
        delete[] wstr;
        pos = 0;
        return parseValue();
    }
};

inline Value ParseFile(const wchar_t* path) {
    FILE* f = _wfopen(path, L"rb");
    if (!f) return Value();
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = new char[size + 1];
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    std::string content(buffer);
    delete[] buffer;
    
    Parser parser;
    return parser.parse(content);
}

inline Value ParseString(const std::wstring& jsonStr) {
    Parser parser;
    return parser.parse(jsonStr);
}

inline Value ParseString(const std::string& jsonStr) {
    Parser parser;
    return parser.parse(jsonStr);
}

}
