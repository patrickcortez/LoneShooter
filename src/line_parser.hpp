// Compile: (Included in loneshooter.cpp compilation)
// Run: N/A - Header only

#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <cwchar>

namespace LineParser {

struct LineOption;
struct LineResponse;
struct LineOptionsBlock;

struct LineOption {
    int optionId;
    std::wstring displayText;
    std::wstring responseText;
    int responseId;
    bool returnToParent;
    bool exitDialogue;
    
    LineOption() : optionId(0), responseId(-1), returnToParent(false), exitDialogue(false) {}

};

struct LineResponse {
    int responseId;
    std::vector<std::pair<int, std::wstring>> lines;
    bool hasNestedOptions;
    LineOptionsBlock* nestedOptions;
    bool returnToParent;
    bool exitDialogue;
    
    LineResponse() : responseId(0), hasNestedOptions(false), nestedOptions(nullptr), 
                     returnToParent(false), exitDialogue(false) {}
};

struct LineOptionsBlock {
    std::vector<LineOption> options;
    std::map<int, LineResponse> responses;
    
    LineOptionsBlock() {}
};

struct LineDialogue {
    std::wstring name;
    std::vector<std::pair<int, std::wstring>> introLines;
    bool hasOptions;
    LineOptionsBlock* rootOptions;
    std::wstring nameColor;
    std::wstring dialogueColor;
    
    LineDialogue() : hasOptions(false), rootOptions(nullptr), nameColor(L"White"), dialogueColor(L"White") {}
};

class Parser {
private:
    std::wstring content;
    size_t pos;
    int lineNumber;
    
    void error(const std::wstring& msg) {
        wchar_t buf[512];
        swprintf(buf, 512, L"Line Parser Error at line %d: %ls", lineNumber, msg.c_str());
        MessageBoxW(NULL, buf, L"Dialogue Error", MB_OK | MB_ICONERROR);
    }
    
    void skipWhitespace() {
        while (pos < content.size() && (content[pos] == L' ' || content[pos] == L'\n' || 
               content[pos] == L'\r' || content[pos] == L'\t')) {
            if (content[pos] == L'\n') lineNumber++;
            pos++;
        }
    }
    
    bool match(const std::wstring& str) {
        skipWhitespace();
        if (pos + str.size() > content.size()) return false;
        for (size_t i = 0; i < str.size(); i++) {
            if (content[pos + i] != str[i]) return false;
        }
        pos += str.size();
        return true;
    }
    
    bool expect(const std::wstring& str) {
        if (!match(str)) {
            wchar_t buf[256];
            swprintf(buf, 256, L"Expected '%s'", str.c_str());
            error(buf);
            return false;
        }
        return true;
    }
    
    bool peek(const std::wstring& str) {
        skipWhitespace();
        if (pos + str.size() > content.size()) return false;
        for (size_t i = 0; i < str.size(); i++) {
            if (content[pos + i] != str[i]) return false;
        }
        return true;
    }
    
    std::wstring parseQuotedString() {
        skipWhitespace();
        if (pos >= content.size() || content[pos] != L'"') return L"";
        pos++;
        std::wstring result;
        while (pos < content.size() && content[pos] != L'"') {
            if (content[pos] == L'\\' && pos + 1 < content.size()) {
                pos++;
                switch (content[pos]) {
                    case L'n': result += L'\n'; break;
                    case L'r': result += L'\r'; break;
                    case L't': result += L'\t'; break;
                    case L'"': result += L'"'; break;
                    case L'\\': result += L'\\'; break;
                    default: result += content[pos]; break;
                }
            } else {
                result += content[pos];
            }
            pos++;
        }
        if (pos < content.size()) pos++;
        return result;
    }
    
    int parseNumber() {
        skipWhitespace();
        std::wstring numStr;
        while (pos < content.size() && content[pos] >= L'0' && content[pos] <= L'9') {
            numStr += content[pos];
            pos++;
        }
        if (numStr.empty()) return -1;
        return _wtoi(numStr.c_str());
    }
    
    bool parseBool() {
        skipWhitespace();
        if (match(L"true")) return true;
        if (match(L"false")) return false;
        return false;
    }
    
    void parseLines(std::vector<std::pair<int, std::wstring>>& lines) {
        while (peek(L"@Line[")) {
            match(L"@Line[");
            int lineId = parseNumber();
            if (lineId == -1) error(L"Missing Line ID");
            expect(L"]:");
            std::wstring text = parseQuotedString();
            lines.push_back({lineId, text});
        }
    }
    
    LineOption parseOption() {
        LineOption opt;
        opt.optionId = 0;
        opt.responseId = -1;
        
        if (match(L"@Option[")) {
            opt.optionId = parseNumber();
            if (opt.optionId == -1) error(L"Missing Option ID");
            expect(L"]:");
            skipWhitespace();
            expect(L"{");
            
            while (!peek(L"}")) {
                if (peek(L"@Line[")) {
                    match(L"@Line[");
                    int lId = parseNumber();
                    if (lId == -1) error(L"Missing Line ID");
                    expect(L"]:");
                    opt.displayText = parseQuotedString();
                } else if (peek(L"@Response[")) {
                    match(L"@Response[");
                    opt.responseId = parseNumber();
                    if (opt.responseId == -1) error(L"Missing Response ID");
                    expect(L"]:");
                    opt.responseText = parseQuotedString();
                } else if (peek(L"@Return:")) {
                    match(L"@Return:");
                    opt.returnToParent = parseBool();
                } else if (peek(L"@Exit:")) {
                    match(L"@Exit:");
                    opt.exitDialogue = parseBool();
                } else {
                    pos++;
                }
            }
            match(L"}");
        }
        return opt;
    }
    
    LineResponse parseResponse() {
        LineResponse resp;
        
        if (match(L"@")) {
            resp.responseId = parseNumber();
            if (resp.responseId == -1) error(L"Missing Response Key ID");
            expect(L":");
            skipWhitespace();
            expect(L"{");
            
            while (!peek(L"}")) {
                if (peek(L"@Line[")) {
                    std::vector<std::pair<int, std::wstring>> lines;
                    parseLines(lines);
                    resp.lines = lines;
                } else if (peek(L"@Options:") || peek(L"Options:")) {
                    resp.hasNestedOptions = true;
                    resp.nestedOptions = parseOptionsBlock();
                } else if (peek(L"@Return:")) {
                    match(L"@Return:");
                    resp.returnToParent = parseBool();
                } else if (peek(L"@Exit:")) {
                    match(L"@Exit:");
                    resp.exitDialogue = parseBool();
                } else {
                    pos++;
                }
            }
            match(L"}");
        }
        return resp;
    }
    
    LineOptionsBlock* parseOptionsBlock() {
        LineOptionsBlock* block = new LineOptionsBlock();
        
        if (match(L"@Options:") || match(L"Options:")) {
            skipWhitespace();
            expect(L"{");
            
            while (!peek(L"}")) {
                if (peek(L"@Option[")) {
                    LineOption opt = parseOption();
                    block->options.push_back(opt);
                } else if (peek(L"@Response:")) {
                    match(L"@Response:");
                    skipWhitespace();
                    expect(L"{");
                    
                    while (!peek(L"}")) {
                        if (peek(L"@")) {
                            LineResponse resp = parseResponse();
                            block->responses[resp.responseId] = resp;
                        } else {
                            pos++;
                        }
                    }
                    match(L"}");
                } else {
                    pos++;
                }
            }
            match(L"}");
        }
        return block;
    }

public:
    LineDialogue parse(const std::wstring& fileContent) {
        LineDialogue dialogue;
        content = fileContent;
        pos = 0;
        lineNumber = 1;
        
        while (pos < content.size()) {
            if (peek(L"@Name:")) {
                match(L"@Name:");
                dialogue.name = parseQuotedString();
            } else if (peek(L"@Color:")) {
                match(L"@Color:");
                dialogue.nameColor = parseQuotedString();
            } else if (peek(L"@D-Color:")) {
                match(L"@D-Color:");
                dialogue.dialogueColor = parseQuotedString();
            } else if (peek(L"@Dialogue:")) {
                match(L"@Dialogue:");
                skipWhitespace();
                expect(L"{");
                
                while (!peek(L"}") && pos < content.size()) {
                    if (peek(L"@Line[")) {
                        parseLines(dialogue.introLines);
                    } else if (peek(L"@Options:")) {
                        dialogue.hasOptions = true;
                        dialogue.rootOptions = parseOptionsBlock();
                    } else {
                        pos++;
                    }
                }
                match(L"}");
            } else {
                pos++;
            }
        }
        
        return dialogue;
    }
};

inline void FreeLineOptionsBlock(LineOptionsBlock* block) {
    if (!block) return;
    for (auto& pair : block->responses) {
        if (pair.second.nestedOptions) {
            FreeLineOptionsBlock(pair.second.nestedOptions);
        }
    }
    delete block;
}

inline void FreeLineDialogue(LineDialogue& dialogue) {
    if (dialogue.rootOptions) {
        FreeLineOptionsBlock(dialogue.rootOptions);
        dialogue.rootOptions = nullptr;
    }
}

inline LineDialogue ParseLineFile(const wchar_t* path) {
    LineDialogue dialogue;
    
    FILE* f = _wfopen(path, L"rb");
    if (!f) return dialogue;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = new char[size + 1];
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
    wchar_t* wbuffer = new wchar_t[wlen];
    MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wbuffer, wlen);
    
    std::wstring content(wbuffer);
    delete[] buffer;
    delete[] wbuffer;
    
    Parser parser;
    dialogue = parser.parse(content);
    
    return dialogue;
}

}
