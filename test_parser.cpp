#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <windows.h>

// Mock of dialogue.hpp structures
struct DialogueLine {
    std::wstring text;
    bool hasOptions;
    std::wstring option1;
    std::wstring option2;
};

struct Dialogue {
    std::wstring name;
    std::vector<DialogueLine> lines;
};

// Pasted findValue logic from dialogue.hpp
std::string findValue(const std::string& json, const std::string& key) {
    std::string lowerJson = json;
    std::string lowerKey = key;
    for (auto& c : lowerJson) c = (char)tolower(c);
    for (auto& c : lowerKey) c = (char)tolower(c);
    
    size_t pos = lowerJson.find("\"" + lowerKey + "\"");
    if (pos == std::string::npos) return "";
    pos = json.find(":", pos);
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t')) pos++;
    if (pos >= json.size()) return "";
    if (json[pos] == '"') {
        size_t start = pos + 1;
        size_t end = json.find("\"", start);
        if (end == std::string::npos) return "";
        return json.substr(start, end - start);
    }
    return "";
}

int main() {
    std::string json = R"(
{
    "Name": "Leader",
    "Dialogue": {
        "Line1": "Well Done Lance, You did the Organization proud. You have just completed your task ",
        "Line2": "of eliminating a Otherworthy Elder God, those poor townsfolk are free of its influence now.",
        "Line3": {
            "Line": "Are you ready to get out of here?",
            "Options": {
                "Option1": "Yes",
                "Option2": "Not yet"
            }
        }
    }
}
)";

    std::string opt1 = findValue(json, "Option1");
    std::string opt2 = findValue(json, "Option2");
    std::string lineVal = findValue(json, "Line");

    std::cout << "Line: " << lineVal << "\n";
    std::cout << "Option1: " << opt1 << "\n";
    std::cout << "Option2: " << opt2 << "\n";
    
    bool hasOptions = !opt1.empty();
    std::cout << "HasOptions: " << (hasOptions ? "TRUE" : "FALSE") << "\n";
    
    return 0;
}
