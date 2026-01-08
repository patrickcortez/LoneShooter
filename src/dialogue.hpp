// Compile: (Included in loneshooter.cpp compilation)
// Run: N/A - Header only

#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <stack>
#include "line_parser.hpp"

namespace DialogueSystem {

DWORD* dialogueBubblePixels = nullptr;
int dialogueBubbleW = 0, dialogueBubbleH = 0;
DWORD* dialogueChoicePixels = nullptr;
int dialogueChoiceW = 0, dialogueChoiceH = 0;
DWORD* dialogueChoiceSelectedPixels = nullptr;
int dialogueChoiceSelectedW = 0, dialogueChoiceSelectedH = 0;

bool assetsLoaded = false;

enum DialogueState {
    DIALOGUE_INACTIVE,
    DIALOGUE_ACTIVE,
    DIALOGUE_OPTION_SELECT,
    DIALOGUE_FINISHED
};

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

inline DWORD* LoadBMPPixelsInternal(const wchar_t* filename, int* outW, int* outH) {
    HBITMAP hBmp = (HBITMAP)LoadImageW(NULL, filename, IMAGE_BITMAP, 0, 0, 
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!hBmp) return nullptr;
    
    BITMAP bm;
    GetObject(hBmp, sizeof(bm), &bm);
    *outW = bm.bmWidth;
    *outH = bm.bmHeight;
    
    DWORD* pixels = new DWORD[bm.bmWidth * bm.bmHeight];
    
    HDC hdc = GetDC(NULL);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bm.bmWidth;
    bi.bmiHeader.biHeight = -bm.bmHeight;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    
    GetDIBits(hdc, hBmp, 0, bm.bmHeight, pixels, &bi, DIB_RGB_COLORS);
    ReleaseDC(NULL, hdc);
    DeleteObject(hBmp);
    
    return pixels;
}

inline void LoadDialogueAssets(const wchar_t* assetsPath) {
    if (assetsLoaded) return;
    
    wchar_t path[MAX_PATH];
    
    swprintf(path, MAX_PATH, L"%ls\\UI\\dialogue_bubble.bmp", assetsPath);
    dialogueBubblePixels = LoadBMPPixelsInternal(path, &dialogueBubbleW, &dialogueBubbleH);
    
    swprintf(path, MAX_PATH, L"%ls\\UI\\dialogue_choice.bmp", assetsPath);
    dialogueChoicePixels = LoadBMPPixelsInternal(path, &dialogueChoiceW, &dialogueChoiceH);
    
    swprintf(path, MAX_PATH, L"%ls\\UI\\dialogue_choice_selected.bmp", assetsPath);
    dialogueChoiceSelectedPixels = LoadBMPPixelsInternal(path, &dialogueChoiceSelectedW, &dialogueChoiceSelectedH);
    
    assetsLoaded = true;
}

inline void CleanupDialogueAssets() {
    if (dialogueBubblePixels) { delete[] dialogueBubblePixels; dialogueBubblePixels = nullptr; }
    if (dialogueChoicePixels) { delete[] dialogueChoicePixels; dialogueChoicePixels = nullptr; }
    if (dialogueChoiceSelectedPixels) { delete[] dialogueChoiceSelectedPixels; dialogueChoiceSelectedPixels = nullptr; }
    assetsLoaded = false;
}

inline Dialogue LoadDialogueFromJSON(const wchar_t* path, bool selectRandomLine = false) {
    Dialogue dialogue;
    dialogue.name = L"Unknown";
    
    FILE* f = _wfopen(path, L"rb");
    if (!f) return dialogue;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = new char[size + 1];
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    std::string json(buffer);
    delete[] buffer;
    
    auto findValue = [&json](const std::string& key) -> std::string {
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
    };
    
    auto toWide = [](const std::string& s) -> std::wstring {
        if (s.empty()) return L"";
        int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
        wchar_t* wstr = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wstr, len);
        std::wstring result(wstr);
        delete[] wstr;
        return result;
    };
    
    std::string name = findValue("Name");
    if (name.empty()) name = findValue("name");
    if (!name.empty()) {
        dialogue.name = toWide(name);
    }
    
    std::vector<std::string> allLines;
    for (int i = 1; i <= 20; i++) {
        char lineKey[16];
        sprintf(lineKey, "line%d", i);
        std::string lineVal = findValue(lineKey);
        if (!lineVal.empty()) {
            allLines.push_back(lineVal);
        }
    }
    
    std::string opt1 = findValue("Option1");
    std::string opt2 = findValue("Option2");
    std::string lineWithOptions = findValue("Line");
    
    if (selectRandomLine && !allLines.empty()) {
        int idx = rand() % (int)allLines.size();
        DialogueLine dl;
        dl.text = toWide(allLines[idx]);
        dl.hasOptions = false;
        dialogue.lines.push_back(dl);
    } else {
        for (const auto& line : allLines) {
            DialogueLine dl;
            dl.text = toWide(line);
            dl.hasOptions = false;
            dialogue.lines.push_back(dl);
        }
    }
    
    if (!lineWithOptions.empty()) {
        DialogueLine dl;
        dl.text = toWide(lineWithOptions);
        dl.hasOptions = !opt1.empty();
        dl.option1 = toWide(opt1);
        dl.option2 = toWide(opt2);
        dialogue.lines.push_back(dl);
    }
    
    return dialogue;
}

inline void RenderSpriteToDC(HDC hdc, DWORD* pixels, int pxW, int pxH, int destX, int destY, int destW, int destH) {
    if (!pixels || pxW <= 0 || pxH <= 0) return;
    
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = pxW;
    bi.bmiHeader.biHeight = -pxH;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    
    StretchDIBits(hdc, destX, destY, destW, destH, 0, 0, pxW, pxH, pixels, &bi, DIB_RGB_COLORS, SRCCOPY);
}

inline COLORREF ResolveColor(const std::wstring& colorName) {
    if (colorName == L"Red") return RGB(255, 50, 50);
    if (colorName == L"Green") return RGB(50, 255, 50);
    if (colorName == L"Blue") return RGB(50, 50, 255);
    if (colorName == L"Yellow") return RGB(255, 255, 50);
    if (colorName == L"Cyan") return RGB(50, 255, 255);
    if (colorName == L"Magenta") return RGB(255, 50, 255);
    if (colorName == L"White") return RGB(255, 255, 255);
    if (colorName == L"Black") return RGB(0, 0, 0);
    if (colorName == L"Orange") return RGB(255, 165, 0);
    if (colorName == L"Purple") return RGB(160, 32, 240);
    if (colorName == L"Pink") return RGB(255, 192, 203);
    if (colorName == L"Gray") return RGB(128, 128, 128);
    if (colorName == L"Light Blue") return RGB(173, 216, 230);
    if (colorName == L"Gold") return RGB(200, 180, 100);
    return RGB(255, 255, 255); // Default White
}

inline void RenderDialogueBox(HDC hdc, int screenW, int screenH, const std::wstring& name, const std::wstring& text, bool showOptions, const std::vector<std::wstring>& options, int selectedOption, const std::wstring& nameColorStr = L"Gold", const std::wstring& textColorStr = L"White") {
    int boxH = 140;
    int boxY = screenH - boxH - 20;
    int boxX = 50;
    int boxW = screenW - 100;
    
    if (dialogueBubblePixels && dialogueBubbleW > 0) {
        RenderSpriteToDC(hdc, dialogueBubblePixels, dialogueBubbleW, dialogueBubbleH, boxX, boxY, boxW, boxH);
    } else {
        HBRUSH bgBrush = CreateSolidBrush(RGB(20, 20, 30));
        RECT bgRect = {boxX, boxY, boxX + boxW, boxY + boxH};
        FillRect(hdc, &bgRect, bgBrush);
        DeleteObject(bgBrush);
        
        HPEN borderPen = CreatePen(PS_SOLID, 3, RGB(200, 180, 100));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        HBRUSH hollowBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hollowBrush);
        Rectangle(hdc, boxX, boxY, boxX + boxW, boxY + boxH);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(borderPen);
    }
    
    SetBkMode(hdc, TRANSPARENT);
    
    HFONT nameFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    HFONT textFont = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    
    HFONT oldFont = (HFONT)SelectObject(hdc, nameFont);
    SetTextColor(hdc, ResolveColor(nameColorStr));
    TextOutW(hdc, boxX + 20, boxY + 12, name.c_str(), (int)name.length());
    
    SelectObject(hdc, textFont);
    SetTextColor(hdc, ResolveColor(textColorStr));
    RECT textRect = {boxX + 20, boxY + 45, boxX + boxW - 20, boxY + 95};
    DrawTextW(hdc, text.c_str(), -1, &textRect, DT_LEFT | DT_WORDBREAK);
    
    if (showOptions && options.size() > 0) {
        int choiceW = 200;
        int choiceH = 30;
        int optY = boxY - choiceH - 10;
        int spacing = 20;
        int totalW = (int)options.size() * choiceW + ((int)options.size() - 1) * spacing;
        int startX = boxX + (boxW - totalW) / 2;
        
        for (int i = 0; i < (int)options.size(); i++) {
            int optX = startX + i * (choiceW + spacing);
            bool isSelected = (selectedOption == i);
            
            if (isSelected && dialogueChoiceSelectedPixels) {
                RenderSpriteToDC(hdc, dialogueChoiceSelectedPixels, dialogueChoiceSelectedW, dialogueChoiceSelectedH, optX, optY, choiceW, choiceH);
            } else if (dialogueChoicePixels) {
                RenderSpriteToDC(hdc, dialogueChoicePixels, dialogueChoiceW, dialogueChoiceH, optX, optY, choiceW, choiceH);
            } else {
                HBRUSH bgBrush = CreateSolidBrush(isSelected ? RGB(100, 100, 0) : RGB(40, 40, 50));
                RECT bgRect = {optX, optY, optX + choiceW, optY + choiceH};
                FillRect(hdc, &bgRect, bgBrush);
                DeleteObject(bgBrush);
                
                HPEN borderPen = CreatePen(PS_SOLID, 2, isSelected ? RGB(255, 255, 0) : RGB(100, 100, 100));
                HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
                HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
                Rectangle(hdc, optX, optY, optX + choiceW, optY + choiceH);
                SelectObject(hdc, oldBrush);
                SelectObject(hdc, oldPen);
                DeleteObject(borderPen);
            }
            
            SetTextColor(hdc, isSelected ? RGB(255, 255, 0) : RGB(180, 180, 180));
            SIZE sz;
            GetTextExtentPoint32W(hdc, options[i].c_str(), (int)options[i].length(), &sz);
            TextOutW(hdc, optX + (choiceW - sz.cx) / 2, optY + (choiceH - sz.cy) / 2, options[i].c_str(), (int)options[i].length());
        }
    } else if (!showOptions) {
        SetTextColor(hdc, RGB(150, 150, 150));
        TextOutA(hdc, boxX + boxW - 150, boxY + boxH - 30, "[E] Continue", 12);
    }
    
    SelectObject(hdc, oldFont);
    DeleteObject(nameFont);
    DeleteObject(textFont);
}

class DialogueController {
private:
    LineParser::LineDialogue dialogue;
    std::stack<LineParser::LineOptionsBlock*> optionStack;
    std::vector<std::pair<int, std::wstring>>* currentLines;
    size_t currentLineIndex;
    bool showingOptions;
    bool dialogueActive;
    bool dialogueFinished;
    int selectedOptionIndex;
    bool randomLineMode;
    bool pendingReturn;
    bool pendingExit;
    LineParser::LineOptionsBlock* pendingNestedOptions;
    
public:
    DialogueController() : currentLines(nullptr), currentLineIndex(0), 
                          showingOptions(false), dialogueActive(false), 
                          dialogueFinished(false), selectedOptionIndex(0),
                          randomLineMode(false), pendingReturn(false),
                          pendingExit(false),
                          pendingNestedOptions(nullptr) {}
    
    ~DialogueController() {
        Cleanup();
    }
    
    void Cleanup() {
        LineParser::FreeLineDialogue(dialogue);
        while (!optionStack.empty()) optionStack.pop();
        currentLines = nullptr;
        currentLineIndex = 0;
        showingOptions = false;
        dialogueActive = false;
        dialogueFinished = false;
        selectedOptionIndex = 0;
        randomLineMode = false;
        pendingReturn = false;
        pendingExit = false;
        pendingNestedOptions = nullptr;
    }
    
    bool LoadFromLine(const wchar_t* path) {
        Cleanup();
        dialogue = LineParser::ParseLineFile(path);
        if (dialogue.name.empty() && dialogue.introLines.empty()) {
            return false;
        }
        return true;
    }
    
    void Start() {
        if (dialogue.name.empty() && dialogue.introLines.empty()) return;
        currentLines = &dialogue.introLines;
        currentLineIndex = 0;
        showingOptions = false;
        dialogueActive = true;
        dialogueFinished = false;
        selectedOptionIndex = 0;
        randomLineMode = false;
        if (dialogue.hasOptions && dialogue.rootOptions) {
            optionStack.push(dialogue.rootOptions);
        }
    }
    
    void StartRandom() {
        if (dialogue.name.empty() && dialogue.introLines.empty()) return;
        currentLines = &dialogue.introLines;
        if (currentLines->empty()) return;
        currentLineIndex = rand() % currentLines->size();
        showingOptions = false;
        dialogueActive = true;
        dialogueFinished = false;
        selectedOptionIndex = 0;
        randomLineMode = true;
    }
    
    void AdvanceLine() {
        if (!dialogueActive || showingOptions) return;
        
        if (randomLineMode) {
            dialogueFinished = true;
            dialogueActive = false;
            return;
        }
        
        currentLineIndex++;
        if (currentLines && currentLineIndex >= currentLines->size()) {
            if (pendingExit) {
                dialogueFinished = true;
                dialogueActive = false;
            } else if (pendingNestedOptions) {
                optionStack.push(pendingNestedOptions);
                pendingNestedOptions = nullptr;
                showingOptions = true;
                selectedOptionIndex = 0;
            } else if (pendingReturn) {
                pendingReturn = false;
                showingOptions = true;
                selectedOptionIndex = 0;
            } else {
                bool hasOpts = (!optionStack.empty() && optionStack.top() && optionStack.top()->options.size() > 0) ||
                              (dialogue.hasOptions && dialogue.rootOptions && dialogue.rootOptions->options.size() > 0);
                if (hasOpts) {
                    currentLineIndex = currentLines->size() - 1;
                    if (optionStack.empty() && dialogue.rootOptions) {
                        optionStack.push(dialogue.rootOptions);
                    }
                    showingOptions = true;
                    selectedOptionIndex = 0;
                } else {
                    dialogueFinished = true;
                    dialogueActive = false;
                }
            }
        }
    }
    
    void SelectOption(int optionIndex) {
        if (!showingOptions || optionStack.empty()) return;
        
        LineParser::LineOptionsBlock* currentBlock = optionStack.top();
        if (optionIndex < 0 || optionIndex >= (int)currentBlock->options.size()) return;
        
        LineParser::LineOption& selectedOpt = currentBlock->options[optionIndex];
        int responseId = selectedOpt.responseId;
        
        // Initialize pending flags from Option-level config
        pendingReturn = selectedOpt.returnToParent;
        pendingExit = selectedOpt.exitDialogue;
        pendingNestedOptions = nullptr;

        auto it = currentBlock->responses.find(responseId);
        if (it != currentBlock->responses.end()) {
            LineParser::LineResponse& resp = it->second;
            
            // Merge Response-level flags (OR logic)
            pendingReturn = pendingReturn || resp.returnToParent;
            pendingExit = pendingExit || resp.exitDialogue;
            pendingNestedOptions = (resp.hasNestedOptions) ? resp.nestedOptions : nullptr;
            
            if (!resp.lines.empty()) {
                currentLines = &resp.lines;
                currentLineIndex = 0;
                showingOptions = false;
            } else {
                // No lines, execute logic immediately
                if (pendingExit) {
                    dialogueFinished = true;
                    dialogueActive = false;
                    showingOptions = false;
                } else if (pendingNestedOptions) {
                    optionStack.push(pendingNestedOptions);
                    pendingNestedOptions = nullptr;
                    showingOptions = true;
                    selectedOptionIndex = 0;
                } else if (pendingReturn) {
                    pendingReturn = false;
                    showingOptions = true;
                    selectedOptionIndex = 0;
                } else {
                    // Logic: if we didn't exit, didn't return, didn't nest, and had no lines...
                    // Check if we have options to return to (implicit loop)
                    bool hasOpts = (!optionStack.empty() && optionStack.top() && optionStack.top()->options.size() > 0) ||
                                  (dialogue.hasOptions && dialogue.rootOptions && dialogue.rootOptions->options.size() > 0);
                    
                    if (hasOpts) {
                        showingOptions = true;
                        selectedOptionIndex = 0;
                    } else {
                        dialogueFinished = true;
                        dialogueActive = false;
                        showingOptions = false;
                    }
                }
            }
        } else {
            // No response found. Execute Option-level flags immediately.
            if (pendingExit) {
                dialogueFinished = true;
                dialogueActive = false;
                showingOptions = false;
            } else if (pendingReturn) {
                pendingReturn = false;
                showingOptions = true;
                selectedOptionIndex = 0;
            } else {
                // Default fallback: Implicit loop if possible
                bool hasOpts = (!optionStack.empty() && optionStack.top() && optionStack.top()->options.size() > 0) ||
                              (dialogue.hasOptions && dialogue.rootOptions && dialogue.rootOptions->options.size() > 0);
                
                if (hasOpts) {
                    showingOptions = true;
                    selectedOptionIndex = 0;
                } else {
                    dialogueFinished = true;
                    dialogueActive = false;
                    showingOptions = false;
                }
            }
        }
    }
    
    void MoveSelectionUp() {
        if (showingOptions && selectedOptionIndex > 0) {
            selectedOptionIndex--;
        }
    }
    
    std::wstring GetNameColor() const { return dialogue.nameColor; }
    std::wstring GetDialogueColor() const { return dialogue.dialogueColor; }
    
    void MoveSelectionDown() {
        if (showingOptions && !optionStack.empty()) {
            if (selectedOptionIndex < (int)optionStack.top()->options.size() - 1) {
                selectedOptionIndex++;
            }
        }
    }
    
    void MoveSelectionLeft() {
        MoveSelectionUp();
    }
    
    void MoveSelectionRight() {
        MoveSelectionDown();
    }
    
    void ConfirmSelection() {
        if (showingOptions) {
            SelectOption(selectedOptionIndex);
        }
    }
    
    bool IsActive() const { return dialogueActive; }
    bool IsFinished() const { return dialogueFinished; }
    bool IsShowingOptions() const { return showingOptions; }
    int GetSelectedOptionIndex() const { return selectedOptionIndex; }
    
    std::wstring GetCurrentText() const {
        if (!currentLines || currentLineIndex >= currentLines->size()) return L"";
        return (*currentLines)[currentLineIndex].second;
    }
    
    std::wstring GetSpeakerName() const {
        return dialogue.name;
    }
    
    std::vector<std::wstring> GetCurrentOptions() const {
        std::vector<std::wstring> result;
        if (!showingOptions || optionStack.empty()) return result;
        for (const auto& opt : optionStack.top()->options) {
            result.push_back(opt.displayText);
        }
        return result;
    }
    
    size_t GetOptionCount() const {
        if (!showingOptions || optionStack.empty()) return 0;
        return optionStack.top()->options.size();
    }
};

}
