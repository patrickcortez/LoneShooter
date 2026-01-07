// Compile: (Included in loneshooter.cpp compilation)
// Run: N/A - Header only

#pragma once
#include <windows.h>
#include <string>
#include <vector>

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

inline void RenderDialogueBox(HDC hdc, int screenW, int screenH, const std::wstring& name, const std::wstring& text, bool showOptions, const std::wstring& opt1, const std::wstring& opt2, int selectedOption) {
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
    SetTextColor(hdc, RGB(200, 180, 100));
    TextOutW(hdc, boxX + 20, boxY + 12, name.c_str(), (int)name.length());
    
    SelectObject(hdc, textFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    RECT textRect = {boxX + 20, boxY + 45, boxX + boxW - 20, boxY + 95};
    DrawTextW(hdc, text.c_str(), -1, &textRect, DT_LEFT | DT_WORDBREAK);
    
    if (showOptions) {
        int optY = boxY + boxH - 45;
        int choiceW = 200;
        int choiceH = 30;
        int spacing = 20;
        int startX = boxX + 60;
        
        if (selectedOption == 0 && dialogueChoiceSelectedPixels) {
            RenderSpriteToDC(hdc, dialogueChoiceSelectedPixels, dialogueChoiceSelectedW, dialogueChoiceSelectedH, startX, optY, choiceW, choiceH);
        } else if (dialogueChoicePixels) {
            RenderSpriteToDC(hdc, dialogueChoicePixels, dialogueChoiceW, dialogueChoiceH, startX, optY, choiceW, choiceH);
        }
        
        int opt2X = startX + choiceW + spacing;
        if (selectedOption == 1 && dialogueChoiceSelectedPixels) {
            RenderSpriteToDC(hdc, dialogueChoiceSelectedPixels, dialogueChoiceSelectedW, dialogueChoiceSelectedH, opt2X, optY, choiceW, choiceH);
        } else if (dialogueChoicePixels) {
            RenderSpriteToDC(hdc, dialogueChoicePixels, dialogueChoiceW, dialogueChoiceH, opt2X, optY, choiceW, choiceH);
        }
        
        SetTextColor(hdc, RGB(255, 255, 255));
        SIZE sz1, sz2;
        GetTextExtentPoint32W(hdc, opt1.c_str(), (int)opt1.length(), &sz1);
        GetTextExtentPoint32W(hdc, opt2.c_str(), (int)opt2.length(), &sz2);
        TextOutW(hdc, startX + (choiceW - sz1.cx) / 2, optY + (choiceH - sz1.cy) / 2, opt1.c_str(), (int)opt1.length());
        TextOutW(hdc, opt2X + (choiceW - sz2.cx) / 2, optY + (choiceH - sz2.cy) / 2, opt2.c_str(), (int)opt2.length());
    } else {
        SetTextColor(hdc, RGB(150, 150, 150));
        TextOutA(hdc, boxX + boxW - 150, boxY + boxH - 30, "[E] Continue", 12);
    }
    
    SelectObject(hdc, oldFont);
    DeleteObject(nameFont);
    DeleteObject(textFont);
}

}
