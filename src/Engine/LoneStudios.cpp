/*
 * LoneStudios - 2.5D Map Editor for LoneShooter
 * Compile: g++ -o cmds/LoneStudios.exe src/LoneStudios.cpp -lgdi32 -lcomdlg32 -lcomctl32 -lole32 -lshell32 -mwindows -O2 -static
 * Run: ./LoneStudios.exe
 */

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <shlobj.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <algorithm>
#include <cstdint>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")

int EDITOR_WIDTH = 1280;
int EDITOR_HEIGHT = 720;
const int MAP_WIDTH = 64;
const int MAP_HEIGHT = 64;
const int TILE_SIZE = 32;
const int SIDEBAR_WIDTH = 250;
const int TOOLBAR_HEIGHT = 40;
const int PROPERTIES_HEIGHT = 120;

HWND hMainWnd = NULL;
HDC backBufferDC = NULL;
HBITMAP backBufferDIB = NULL;
DWORD* backBufferPixels = NULL;

int worldMap[MAP_WIDTH][MAP_HEIGHT] = {0};

float cameraX = 0, cameraY = 0;
float zoom = 1.0f;
int selectedTool = 0;
int selectedTileType = 1;
bool keys[256] = {false};

int mapWidth = 64;
int mapHeight = 64;
wchar_t mapName[128] = L"Untitled";
wchar_t projectFolder[MAX_PATH] = L"";
float playerStartX = 10.0f;
float playerStartY = 32.0f;

struct BMPTexture {
    std::wstring name;
    std::wstring path;
    DWORD* pixels;
    int width, height;
};

std::vector<BMPTexture> bmpPalette;
int selectedPaletteIndex = -1;

int draggingObjectIndex = -1;
float dragStartX = 0, dragStartY = 0;


struct ObjectDef {
    std::wstring name;
    std::wstring spritePath;
    float defaultWidth;
    float defaultHeight;
    float collisionRadius;
    bool hasCollision;
    bool isPickup;
    int pickupType;
    int pickupValue;
    DWORD* pixels;
    int pixelW, pixelH;
};

struct PlacedObject {
    int defIndex;
    float x, y;
    float z;
    float width, height;
    float collisionRadius;
    bool hasCollision;
    int linkedDoorId;
};


std::vector<ObjectDef> objectDefs;
std::vector<PlacedObject> placedObjects;

struct TileDef {
    std::wstring name;
    std::wstring spritePath;
    DWORD* pixels;
    int pixelW, pixelH;
};

std::vector<TileDef> tileDefs;

enum FloorLevel {
    FLOOR_DEEP_BASEMENT = -2,
    FLOOR_BASEMENT = -1,
    FLOOR_GROUND = 0,
    FLOOR_FIRST = 1,
    FLOOR_SECOND = 2
};

enum CeilingType {
    CEILING_SKY = -1,
    CEILING_CRAWL = 3,
    CEILING_LOW = 5,
    CEILING_STANDARD = 7,
    CEILING_HIGH = 10
};

struct MapTile {
    int wallType;
    int floorLevel;
    int ceilingType;
    float floorOffset;
    float ceilingOffset;
    int floorTexture;
    int ceilingTexture;
    int sectorId;
};

struct Sector {
    std::wstring name;
    int floorLevel;
    int ceilingType;
    int floorTexture;
    int ceilingTexture;
};

std::vector<Sector> sectors;

enum DoorType {
    DOOR_MANUAL = 0,
    DOOR_AUTO = 1,
    DOOR_KEY_RED = 2,
    DOOR_KEY_BLUE = 3,
    DOOR_KEY_YELLOW = 4,
    DOOR_REMOTE = 5
};

enum DoorState {
    DOOR_CLOSED = 0,
    DOOR_OPENING = 1,
    DOOR_OPEN = 2,
    DOOR_CLOSING = 3
};

struct Door {
    int tileX, tileY;
    DoorType type;
    DoorState state;
    float openHeight;
    float currentHeight;
    float speed;
    float stayOpenTime;
    float timer;
    int remoteId;
    int textureIndex;
};

std::vector<Door> doors;

struct Stair {
    int startX, startY;
    int endX, endY;
    float startHeight;
    float endHeight;
    int numSteps;
    bool hasRailing;
    int stepTextureIndex;
};

std::vector<Stair> stairs;

#pragma pack(push, 1)
struct LSBHeader {
    char     magic[4];
    uint32_t version;
    uint32_t mapWidth;
    uint32_t mapHeight;
    float    playerStartX;
    float    playerStartY;
    float    playerStartAngle;
    uint32_t numWallTextures;
    uint32_t numFloorTextures;
    uint32_t numCeilingTextures;
    uint32_t numSectors;
    uint32_t numDoors;
    uint32_t numStairs;
    uint32_t numObjectDefs;
    uint32_t numObjects;
};

struct TileData {
    uint8_t  wallType;
    int8_t   floorLevel;
    int8_t   ceilingHeight;
    uint8_t  floorTexture;
    uint8_t  ceilingTexture;
    uint8_t  sectorId;
    uint8_t  flags;
    uint8_t  reserved;
};

struct DoorData {
    uint16_t tileX;
    uint16_t tileY;
    uint8_t  type;
    uint8_t  textureIndex;
    uint8_t  flags;
    uint8_t  reserved;
    float    openHeight;
    float    speed;
    float    stayOpenTime;
    uint16_t remoteId;
    uint16_t reserved2;
};

struct StairData {
    uint16_t startX, startY;
    uint16_t endX, endY;
    float    startHeight;
    float    endHeight;
    uint8_t  numSteps;
    uint8_t  textureIndex;
    uint8_t  flags;
    uint8_t  reserved;
};

struct PlacedObjectData {
    uint16_t defIndex;
    uint16_t flags;
    float    x, y;
    float    z;
    float    width, height;
    float    collisionRadius;
    uint16_t linkedId;
    uint16_t reserved;
};
#pragma pack(pop)

MapTile worldMapTiles[64][64];



int selectedObjectDefIndex = -1;
int selectedPlacedObjectIndex = -1;
bool isDragging = false;
int dragOffsetX = 0, dragOffsetY = 0;

wchar_t currentMapPath[MAX_PATH] = L"";

HBRUSH hBrushSidebar;
HBRUSH hBrushToolbar;
HBRUSH hBrushGrid;
HBRUSH hBrushSelected;
HPEN hPenGrid;
HPEN hPenSelected;
HFONT hFontUI;
HFONT hFontSmall;



PlacedObject g_clipboardObject;
bool g_hasObjectClipboard = false;
int selectedPlacedObjectIndexForMenu = -1;

wchar_t exePath[MAX_PATH];

void GetExeDir() {
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
}

void SaveEditorConfig() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_config.dat", exePath);
    FILE* f = _wfopen(path, L"wb");
    if (!f) return;
    int projLen = (int)wcslen(projectFolder);
    fwrite(&projLen, sizeof(int), 1, f);
    fwrite(projectFolder, sizeof(wchar_t), projLen, f);
    fclose(f);
}

void LoadEditorConfig() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_config.dat", exePath);
    FILE* f = _wfopen(path, L"rb");
    if (!f) return;
    int projLen = 0;
    fread(&projLen, sizeof(int), 1, f);
    if (projLen > 0 && projLen < MAX_PATH) {
        fread(projectFolder, sizeof(wchar_t), projLen, f);
        projectFolder[projLen] = L'\0';
    }
    fclose(f);
}


// Helper to get file name from path
std::wstring GetFileName(const std::wstring& path) {
    size_t lastSlash = path.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
}

// BFS Asset Crawler
std::wstring FindAssetPath(const std::wstring& inputPath) {
    // 1. Check if it exists directly
    if (GetFileAttributesW(inputPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return inputPath;
    }
    
    // 2. Try relative to project folder if input is relative
    if (projectFolder[0] != L'\0') {
        wchar_t testPath[MAX_PATH];
        swprintf(testPath, MAX_PATH, L"%ls\\%ls", projectFolder, inputPath.c_str());
         if (GetFileAttributesW(testPath) != INVALID_FILE_ATTRIBUTES) {
            return testPath;
        }
    }

    // 3. Crawler: BFS search for the filename
    std::wstring targetName = GetFileName(inputPath);
    if (targetName.empty()) return inputPath; // Can't search empty name

    std::queue<std::wstring> foldersToSearch;
    std::set<std::wstring> visited;

    // Start roots
    if (projectFolder[0] != L'\0') foldersToSearch.push(projectFolder);
    foldersToSearch.push(exePath); // Also search where the exe is

    int safetyCounter = 0;
    const int MAX_SEARCH_DIRS = 500; // Limit to prevent freezing on huge drives

    while (!foldersToSearch.empty() && safetyCounter < MAX_SEARCH_DIRS) {
        std::wstring currentDir = foldersToSearch.front();
        foldersToSearch.pop();

        if (visited.count(currentDir)) continue;
        visited.insert(currentDir);
        safetyCounter++;

        std::wstring searchPattern = currentDir + L"\\*";
        
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) continue;

                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // Enqueue subdirectory
                    std::wstring subDir = currentDir + L"\\" + findData.cFileName;
                    foldersToSearch.push(subDir);
                } else {
                    // Check file match
                    if (_wcsicmp(findData.cFileName, targetName.c_str()) == 0) {
                        FindClose(hFind);
                        return currentDir + L"\\" + findData.cFileName;
                    }
                }
            } while (FindNextFileW(hFind, &findData));
            FindClose(hFind);
        }
    }

    // Not found, return original best guess or empty? 
    // Return original so at least we have a path to show error for
    return inputPath;
}

DWORD* LoadBMPPixels(const wchar_t* filename, int* outW, int* outH) {
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

void WorldToScreen(float wx, float wy, int& sx, int& sy) {
    float isoX = (wx - wy) * (TILE_SIZE / 2.0f) * zoom;
    float isoY = (wx + wy) * (TILE_SIZE / 4.0f) * zoom;
    sx = (int)(isoX - cameraX + SIDEBAR_WIDTH + (EDITOR_WIDTH - SIDEBAR_WIDTH) / 2);
    sy = (int)(isoY - cameraY + TOOLBAR_HEIGHT + (EDITOR_HEIGHT - TOOLBAR_HEIGHT - PROPERTIES_HEIGHT) / 2);
}

void ScreenToWorld(int sx, int sy, float& wx, float& wy) {
    float relX = (sx - SIDEBAR_WIDTH - (EDITOR_WIDTH - SIDEBAR_WIDTH) / 2 + cameraX) / zoom;
    float relY = (sy - TOOLBAR_HEIGHT - (EDITOR_HEIGHT - TOOLBAR_HEIGHT - PROPERTIES_HEIGHT) / 2 + cameraY) / zoom;
    
    float isoX = relX / (TILE_SIZE / 2.0f);
    float isoY = relY / (TILE_SIZE / 4.0f);
    
    wx = (isoY + isoX) / 2.0f;
    wy = (isoY - isoX) / 2.0f;
}

void InitGraphics() {
    hBrushSidebar = CreateSolidBrush(RGB(45, 45, 48));
    hBrushToolbar = CreateSolidBrush(RGB(60, 60, 65));
    hBrushGrid = CreateSolidBrush(RGB(30, 30, 35));
    hBrushSelected = CreateSolidBrush(RGB(0, 120, 215));
    hPenGrid = CreatePen(PS_SOLID, 1, RGB(60, 60, 65));
    hPenSelected = CreatePen(PS_SOLID, 2, RGB(255, 200, 0));
    hFontUI = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    hFontSmall = CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void CleanupGraphics() {
    DeleteObject(hBrushSidebar);
    DeleteObject(hBrushToolbar);
    DeleteObject(hBrushGrid);
    DeleteObject(hBrushSelected);
    DeleteObject(hPenGrid);
    DeleteObject(hPenSelected);
    DeleteObject(hFontUI);
    DeleteObject(hFontSmall);
}

void SaveObjectDefs() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_objects.dat", exePath);
    
    FILE* f = _wfopen(path, L"wb");
    if (!f) return;
    
    int count = (int)objectDefs.size();
    fwrite(&count, sizeof(int), 1, f);
    
    for (auto& def : objectDefs) {
        int nameLen = (int)def.name.length();
        fwrite(&nameLen, sizeof(int), 1, f);
        fwrite(def.name.c_str(), sizeof(wchar_t), nameLen, f);
        
        int pathLen = (int)def.spritePath.length();
        fwrite(&pathLen, sizeof(int), 1, f);
        fwrite(def.spritePath.c_str(), sizeof(wchar_t), pathLen, f);
        
        fwrite(&def.defaultWidth, sizeof(float), 1, f);
        fwrite(&def.defaultHeight, sizeof(float), 1, f);
        fwrite(&def.collisionRadius, sizeof(float), 1, f);
        fwrite(&def.hasCollision, sizeof(bool), 1, f);
        fwrite(&def.isPickup, sizeof(bool), 1, f);
        fwrite(&def.pickupType, sizeof(int), 1, f);
        fwrite(&def.pickupValue, sizeof(int), 1, f);
    }
    
    fclose(f);
}

void LoadObjectDefs() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_objects.dat", exePath);
    
    FILE* f = _wfopen(path, L"rb");
    if (!f) return;
    
    int count = 0;
    fread(&count, sizeof(int), 1, f);
    
    for (int i = 0; i < count; i++) {
        ObjectDef def;
        
        int nameLen = 0;
        fread(&nameLen, sizeof(int), 1, f);
        wchar_t* nameBuf = new wchar_t[nameLen + 1];
        fread(nameBuf, sizeof(wchar_t), nameLen, f);
        nameBuf[nameLen] = L'\0';
        def.name = nameBuf;
        delete[] nameBuf;
        
        int pathLen = 0;
        fread(&pathLen, sizeof(int), 1, f);
        wchar_t* pathBuf = new wchar_t[pathLen + 1];
        fread(pathBuf, sizeof(wchar_t), pathLen, f);
        pathBuf[pathLen] = L'\0';
        def.spritePath = pathBuf;
        delete[] pathBuf;
        
        fread(&def.defaultWidth, sizeof(float), 1, f);
        fread(&def.defaultHeight, sizeof(float), 1, f);
        fread(&def.collisionRadius, sizeof(float), 1, f);
        fread(&def.hasCollision, sizeof(bool), 1, f);
        fread(&def.isPickup, sizeof(bool), 1, f);
        fread(&def.pickupType, sizeof(int), 1, f);
        fread(&def.pickupValue, sizeof(int), 1, f);
        
        if (def.collisionRadius <= 0.0f) def.collisionRadius = 0.5f;
        
        std::wstring resolved = FindAssetPath(def.spritePath);
        def.spritePath = resolved;
        def.pixels = LoadBMPPixels(resolved.c_str(), &def.pixelW, &def.pixelH);
        
        objectDefs.push_back(def);
    }
    
    fclose(f);
}

void SaveTileDefs() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_tiles.dat", exePath);
    
    FILE* f = _wfopen(path, L"wb");
    if (!f) return;
    
    int count = (int)tileDefs.size();
    fwrite(&count, sizeof(int), 1, f);
    
    for (auto& def : tileDefs) {
        int nameLen = (int)def.name.length();
        fwrite(&nameLen, sizeof(int), 1, f);
        fwrite(def.name.c_str(), sizeof(wchar_t), nameLen, f);
        
        int pathLen = (int)def.spritePath.length();
        fwrite(&pathLen, sizeof(int), 1, f);
        fwrite(def.spritePath.c_str(), sizeof(wchar_t), pathLen, f);
    }
    
    fclose(f);
}

void LoadTileDefs() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_tiles.dat", exePath);
    
    FILE* f = _wfopen(path, L"rb");
    if (!f) return;
    
    int count = 0;
    fread(&count, sizeof(int), 1, f);
    
    for (int i = 0; i < count; i++) {
        TileDef def;
        
        int nameLen = 0;
        fread(&nameLen, sizeof(int), 1, f);
        wchar_t* nameBuf = new wchar_t[nameLen + 1];
        fread(nameBuf, sizeof(wchar_t), nameLen, f);
        nameBuf[nameLen] = L'\0';
        def.name = nameBuf;
        delete[] nameBuf;
        
        int pathLen = 0;
        fread(&pathLen, sizeof(int), 1, f);
        wchar_t* pathBuf = new wchar_t[pathLen + 1];
        fread(pathBuf, sizeof(wchar_t), pathLen, f);
        pathBuf[pathLen] = L'\0';
        def.spritePath = pathBuf;
        delete[] pathBuf;
        
        std::wstring resolved = FindAssetPath(def.spritePath);
        def.spritePath = resolved;
        def.pixels = LoadBMPPixels(resolved.c_str(), &def.pixelW, &def.pixelH);
        
        tileDefs.push_back(def);
    }
    
    fclose(f);
}

bool BrowseForBMP(wchar_t* outPath, int maxLen) {
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = outPath;
    outPath[0] = L'\0';
    ofn.nMaxFile = maxLen;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
    ofn.lpstrTitle = L"Select BMP Sprite";
    ofn.lpstrInitialDir = projectFolder[0] ? projectFolder : NULL;
    
    return GetOpenFileNameW(&ofn) != 0;
}

bool BrowseForFolder(wchar_t* outPath, int maxLen, const wchar_t* title) {
    BROWSEINFOW bi = {};
    bi.hwndOwner = hMainWnd;
    bi.lpszTitle = title;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        SHGetPathFromIDListW(pidl, outPath);
        CoTaskMemFree(pidl);
        return true;
    }
    return false;
}

ObjectDef g_tempObjDef;

INT_PTR CALLBACK AddObjectDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    
    switch (msg) {
        case WM_INITDIALOG: {
            HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            
            CreateWindowExW(0, L"STATIC", L"Name:", WS_CHILD | WS_VISIBLE, 20, 20, 60, 20, hDlg, NULL, NULL, NULL);
            HWND hName = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 
                90, 18, 280, 24, hDlg, (HMENU)101, NULL, NULL);
            SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"Sprite:", WS_CHILD | WS_VISIBLE, 20, 50, 60, 20, hDlg, NULL, NULL, NULL);
            HWND hSprite = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_READONLY, 
                90, 48, 200, 24, hDlg, (HMENU)102, NULL, NULL);
            SendMessage(hSprite, WM_SETFONT, (WPARAM)hFont, TRUE);
            HWND hBrowse = CreateWindowExW(0, L"BUTTON", L"...", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
                295, 48, 75, 24, hDlg, (HMENU)106, NULL, NULL);
            SendMessage(hBrowse, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"Width:", WS_CHILD | WS_VISIBLE, 20, 80, 60, 20, hDlg, NULL, NULL, NULL);
            HWND hWidth = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"1.0", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 
                90, 78, 80, 24, hDlg, (HMENU)103, NULL, NULL);
            SendMessage(hWidth, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"Height:", WS_CHILD | WS_VISIBLE, 190, 80, 60, 20, hDlg, NULL, NULL, NULL);
            HWND hHeight = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"1.0", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 
                250, 78, 80, 24, hDlg, (HMENU)104, NULL, NULL);
            SendMessage(hHeight, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hCollision = CreateWindowExW(0, L"BUTTON", L"Has Collision", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 
                20, 115, 150, 24, hDlg, (HMENU)105, NULL, NULL);
            SendMessage(hCollision, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hOK = CreateWindowExW(0, L"BUTTON", L"Add", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
                200, 170, 80, 30, hDlg, (HMENU)IDOK, NULL, NULL);
            SendMessage(hOK, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hCancel = CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
                290, 170, 80, 30, hDlg, (HMENU)IDCANCEL, NULL, NULL);
            SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            SetFocus(hName);
            return FALSE;
        }
        
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 106: {
                    wchar_t path[MAX_PATH];
                    if (BrowseForBMP(path, MAX_PATH)) {
                        SetDlgItemTextW(hDlg, 102, path);
                    }
                    return TRUE;
                }
                case IDOK: {
                    wchar_t buf[256];
                    GetDlgItemTextW(hDlg, 101, buf, 256);
                    g_tempObjDef.name = buf;
                    
                    GetDlgItemTextW(hDlg, 102, buf, 256);
                    g_tempObjDef.spritePath = buf;
                    
                    GetDlgItemTextW(hDlg, 103, buf, 256);
                    g_tempObjDef.defaultWidth = (float)_wtof(buf);
                    
                    GetDlgItemTextW(hDlg, 104, buf, 256);
                    g_tempObjDef.defaultHeight = (float)_wtof(buf);
                    
                    g_tempObjDef.hasCollision = IsDlgButtonChecked(hDlg, 105) == BST_CHECKED;
                    
                    if (g_tempObjDef.name.empty() || g_tempObjDef.spritePath.empty()) {
                        MessageBoxW(hDlg, L"Name and Sprite are required.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
            }
            break;
            
        case WM_CLOSE:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
    }
    return FALSE;
}

void ShowAddObjectDialog() {
    g_tempObjDef = ObjectDef();
    g_tempObjDef.defaultWidth = 1.0f;
    g_tempObjDef.defaultHeight = 1.0f;
    g_tempObjDef.collisionRadius = 0.5f;
    g_tempObjDef.hasCollision = false;
    g_tempObjDef.isPickup = false;
    g_tempObjDef.pickupType = 0;
    g_tempObjDef.pickupValue = 0;
    g_tempObjDef.pixels = nullptr;
    g_tempObjDef.pixelW = 0;
    g_tempObjDef.pixelH = 0;
    
    #pragma pack(push, 4)
    struct {
        DLGTEMPLATE dlg;
        WORD menu;
        WORD wndClass;
        WCHAR title[12];
    } dlgTemplate = {};
    #pragma pack(pop)
    
    dlgTemplate.dlg.style = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    dlgTemplate.dlg.dwExtendedStyle = 0;
    dlgTemplate.dlg.cdit = 0;
    dlgTemplate.dlg.x = 0;
    dlgTemplate.dlg.y = 0;
    dlgTemplate.dlg.cx = 200;
    dlgTemplate.dlg.cy = 125;
    dlgTemplate.menu = 0;
    dlgTemplate.wndClass = 0;
    wcscpy(dlgTemplate.title, L"Add Object");
    
    INT_PTR result = DialogBoxIndirectParamW(GetModuleHandle(NULL), &dlgTemplate.dlg, hMainWnd, AddObjectDlgProc, 0);
    
    if (result == IDOK) {
        g_tempObjDef.pixels = LoadBMPPixels(g_tempObjDef.spritePath.c_str(), &g_tempObjDef.pixelW, &g_tempObjDef.pixelH);
        objectDefs.push_back(g_tempObjDef);
        SaveObjectDefs();
    }
}

void ShowAddTileDialog() {
    wchar_t path[MAX_PATH];
    if (!BrowseForBMP(path, MAX_PATH)) return;
    
    wchar_t* filename = wcsrchr(path, L'\\');
    if (!filename) filename = path;
    else filename++;
    
    TileDef def;
    def.name = filename;
    def.spritePath = path;
    def.pixels = LoadBMPPixels(path, &def.pixelW, &def.pixelH);
    
    tileDefs.push_back(def);
    SaveTileDefs();
}

void AddToBMPPalette() {
    wchar_t path[MAX_PATH];
    if (!BrowseForBMP(path, MAX_PATH)) return;
    
    wchar_t* filename = wcsrchr(path, L'\\');
    if (!filename) filename = path;
    else filename++;
    
    BMPTexture tex;
    tex.name = filename;
    tex.path = path;
    tex.pixels = LoadBMPPixels(path, &tex.width, &tex.height);
    
    bmpPalette.push_back(tex);
}

void SaveBMPPalette() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_palette.dat", exePath);
    
    FILE* f = _wfopen(path, L"wb");
    if (!f) return;
    
    int count = (int)bmpPalette.size();
    fwrite(&count, sizeof(int), 1, f);
    
    for (auto& tex : bmpPalette) {
        int nameLen = (int)tex.name.length();
        fwrite(&nameLen, sizeof(int), 1, f);
        fwrite(tex.name.c_str(), sizeof(wchar_t), nameLen, f);
        
        int pathLen = (int)tex.path.length();
        fwrite(&pathLen, sizeof(int), 1, f);
        fwrite(tex.path.c_str(), sizeof(wchar_t), pathLen, f);
    }
    
    fclose(f);
}

void LoadBMPPalette() {
    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%ls\\editor_palette.dat", exePath);
    
    FILE* f = _wfopen(path, L"rb");
    if (!f) return;
    
    int count = 0;
    fread(&count, sizeof(int), 1, f);
    
    for (int i = 0; i < count; i++) {
        BMPTexture tex;
        
        int nameLen = 0;
        fread(&nameLen, sizeof(int), 1, f);
        wchar_t* nameBuf = new wchar_t[nameLen + 1];
        fread(nameBuf, sizeof(wchar_t), nameLen, f);
        nameBuf[nameLen] = L'\0';
        tex.name = nameBuf;
        delete[] nameBuf;
        
        int pathLen = 0;
        fread(&pathLen, sizeof(int), 1, f);
        wchar_t* pathBuf = new wchar_t[pathLen + 1];
        fread(pathBuf, sizeof(wchar_t), pathLen, f);
        pathBuf[pathLen] = L'\0';
        tex.path = pathBuf;
        delete[] pathBuf;
        
        tex.pixels = LoadBMPPixels(tex.path.c_str(), &tex.width, &tex.height);
        
        bmpPalette.push_back(tex);
    }
    
    fclose(f);
}


void SaveMapLSM(const wchar_t* filepath) {
    FILE* f = _wfopen(filepath, L"w");
    if (!f) return;
    
    char narrowPath[MAX_PATH];
    char narrowFolder[MAX_PATH];
    wcstombs(narrowFolder, projectFolder, MAX_PATH);
    
    fprintf(f, "@Map: {\n");
    fprintf(f, "    @Size: %d, %d\n", mapWidth, mapHeight);
    fprintf(f, "    @PlayerStart: %.2f, %.2f\n", playerStartX, playerStartY);
    fprintf(f, "    @AssetFolder: \"%s\"\n", narrowFolder);
    fprintf(f, "}\n\n");
    
    fprintf(f, "@Tiles: {\n");
    for (int i = 0; i < (int)tileDefs.size(); i++) {
        wcstombs(narrowPath, tileDefs[i].spritePath.c_str(), MAX_PATH);
        char* relPath = narrowPath;
        if (narrowFolder[0] && strstr(narrowPath, narrowFolder) == narrowPath) {
            size_t len = strlen(narrowFolder);
            if (narrowPath[len] == '\\' || narrowPath[len] == '/') len++;
            relPath = narrowPath + len;
        }
        fprintf(f, "    @Texture[%d]: \"%s\"\n", i, relPath);
    }
    fprintf(f, "    @Data: {\n");
    for (int y = 0; y < mapHeight; y++) {
        fprintf(f, "        ");
        for (int x = 0; x < mapWidth; x++) {
            fprintf(f, "%d ", worldMap[x][y]);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "    }\n");
    fprintf(f, "}\n\n");
    
    fprintf(f, "@Objects: {\n");
    for (int i = 0; i < (int)placedObjects.size(); i++) {
        PlacedObject& obj = placedObjects[i];
        fprintf(f, "    @Object[%d]: {\n", i + 1);
        if (obj.defIndex >= 0 && obj.defIndex < (int)objectDefs.size()) {
            wcstombs(narrowPath, objectDefs[obj.defIndex].spritePath.c_str(), MAX_PATH);
            char* relPath = narrowPath;
            if (narrowFolder[0] && strstr(narrowPath, narrowFolder) == narrowPath) {
                size_t len = strlen(narrowFolder);
                if (narrowPath[len] == '\\' || narrowPath[len] == '/') len++;
                relPath = narrowPath + len;
            }
            fprintf(f, "        @Sprite: \"%s\"\n", relPath);
        }
        fprintf(f, "        @Position: %.2f, %.2f\n", obj.x, obj.y);
        fprintf(f, "        @Size: %.2f, %.2f\n", obj.width, obj.height);
        fprintf(f, "        @Collision: %s\n", obj.hasCollision ? "true" : "false");
        fprintf(f, "    }\n");
    }
    fprintf(f, "}\n");

    
    fclose(f);
    wcscpy(currentMapPath, filepath);
}

void SaveMapDialog() {
    wchar_t path[MAX_PATH] = L"";
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFilter = L"LoneShooter Map (*.lsm)\0*.lsm\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = L"lsm";
    ofn.lpstrTitle = L"Save Map";
    ofn.lpstrInitialDir = projectFolder[0] ? projectFolder : NULL;
    
    if (GetSaveFileNameW(&ofn)) {
        SaveMapLSM(path);
    }
}

void LoadMapLSM(const wchar_t* filepath) {
    FILE* f = _wfopen(filepath, L"r");
    if (!f) return;
    
    placedObjects.clear();
    tileDefs.clear();
    objectDefs.clear();
    memset(worldMap, 0, sizeof(worldMap));
    
    char line[2048];
    char pathBuf[MAX_PATH];
    wchar_t widePath[MAX_PATH];
    
    while (fgets(line, 2048, f)) {
        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        if (strstr(trimmed, "@Project:")) {
            if (sscanf(trimmed, "@Project: \"%[^\"]\"", pathBuf) == 1) {
                mbstowcs(projectFolder, pathBuf, MAX_PATH);
            }
        }
        else if (strstr(trimmed, "@AssetFolder:")) {
            if (sscanf(trimmed, "@AssetFolder: \"%[^\"]\"", pathBuf) == 1) {
                mbstowcs(projectFolder, pathBuf, MAX_PATH);
            }
        }
        else if (strstr(trimmed, "@Size:")) {
            int w = 0, h = 0;
            if (sscanf(trimmed, "@Size: %d , %d", &w, &h) == 2) {
                mapWidth = (w > 0 && w <= 256) ? w : 64;
                mapHeight = (h > 0 && h <= 256) ? h : 64;
            }
        }
        else if (strstr(trimmed, "@PlayerStart:")) {
            sscanf(trimmed, "@PlayerStart: %f, %f", &playerStartX, &playerStartY);
        }
    }
    
    if (projectFolder[0]) {
        SaveEditorConfig();
    }
    
    rewind(f);
    
    bool inTileData = false;
    int dataRow = 0;
    bool inObjects = false;
    PlacedObject currentObj = {};
    bool inCurrentObject = false;
    
    while (fgets(line, 2048, f)) {
        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        if (strstr(trimmed, "@Data:")) {
            inTileData = true;
            dataRow = 0;
        }
        else if (strstr(trimmed, "@Texture[")) {
            int idx = -1;
            if (sscanf(trimmed, "@Texture[%d]: \"%[^\"]\"", &idx, pathBuf) == 2 && idx >= 0) {
                mbstowcs(widePath, pathBuf, MAX_PATH);
                std::wstring resolved = FindAssetPath(widePath);
                
                if (idx >= (int)tileDefs.size()) {
                    tileDefs.resize(idx + 1);
                }
                
                TileDef def;
                def.name = GetFileName(resolved);
                def.spritePath = resolved;
                def.pixels = LoadBMPPixels(resolved.c_str(), &def.pixelW, &def.pixelH);
                tileDefs[idx] = def;
            }
        }
        else if (inTileData && trimmed[0] != '}' && trimmed[0] != '\n' && trimmed[0] != '\r' && trimmed[0] != '@') {
            if (dataRow < mapHeight) {
                char* tok = trimmed;
                for (int x = 0; x < mapWidth && *tok; x++) {
                    while (*tok == ' ') tok++;
                    worldMap[x][dataRow] = atoi(tok);
                    while (*tok && *tok != ' ' && *tok != '\n' && *tok != '\r') tok++;
                }
                dataRow++;
            }
        }
        else if (strstr(trimmed, "@Objects:")) {
            inTileData = false;
            inObjects = true;
        }
        else if (inObjects && strstr(trimmed, "@Object[")) {
            if (inCurrentObject) {
                placedObjects.push_back(currentObj);
            }
            currentObj = {};
            currentObj.defIndex = -1;
            currentObj.z = 0.0f;
            currentObj.collisionRadius = 0.5f;
            currentObj.linkedDoorId = -1;
            inCurrentObject = true;
        }
        else if (inCurrentObject && strstr(trimmed, "@Sprite:")) {
            if (sscanf(trimmed, "@Sprite: \"%[^\"]\"", pathBuf) == 1) {
                mbstowcs(widePath, pathBuf, MAX_PATH);
                std::wstring resolved = FindAssetPath(widePath);
                
                bool foundDef = false;
                for (int i = 0; i < (int)objectDefs.size(); i++) {
                    if (_wcsicmp(objectDefs[i].spritePath.c_str(), resolved.c_str()) == 0) {
                        currentObj.defIndex = i;
                        foundDef = true;
                        break;
                    }
                }
                
                if (!foundDef) {
                    ObjectDef newDef;
                    newDef.name = GetFileName(resolved);
                    newDef.spritePath = resolved;
                    newDef.pixels = LoadBMPPixels(resolved.c_str(), &newDef.pixelW, &newDef.pixelH);
                    newDef.defaultWidth = 1.0f;
                    if (newDef.pixelW > 0)
                        newDef.defaultHeight = (float)newDef.pixelH / (float)newDef.pixelW;
                    else
                        newDef.defaultHeight = 1.0f;
                    newDef.hasCollision = true;
                    newDef.collisionRadius = 0.5f;
                    newDef.isPickup = false;
                    newDef.pickupType = 0;
                    newDef.pickupValue = 0;
                    
                    objectDefs.push_back(newDef);
                    currentObj.defIndex = (int)objectDefs.size() - 1;
                }
            }
        }
        else if (inCurrentObject && strstr(trimmed, "@Position:")) {
            sscanf(trimmed, "@Position: %f, %f", &currentObj.x, &currentObj.y);
        }
        else if (inCurrentObject && strstr(trimmed, "@Size:")) {
            sscanf(trimmed, "@Size: %f, %f", &currentObj.width, &currentObj.height);
        }
        else if (inCurrentObject && strstr(trimmed, "@Collision:")) {
            currentObj.hasCollision = strstr(trimmed, "true") != nullptr;
        }
        else if (trimmed[0] == '}') {
            if (inCurrentObject) {
                placedObjects.push_back(currentObj);
                inCurrentObject = false;
            }
            inTileData = false;
        }

    }
    
    fclose(f);
    wcscpy(currentMapPath, filepath);
}

void LoadMapDialog() {
    wchar_t path[MAX_PATH] = L"";
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFilter = L"LoneShooter Map (*.lsm)\0*.lsm\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = L"Load Map";
    ofn.lpstrInitialDir = projectFolder[0] ? projectFolder : NULL;
    
    if (GetOpenFileNameW(&ofn)) {
        LoadMapLSM(path);
    }
}

struct NewMapData {
    wchar_t name[128];
    int width;
    int height;
    wchar_t folder[MAX_PATH];
    bool confirmed;
};

INT_PTR CALLBACK NewMapDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    static NewMapData* data = nullptr;
    
    switch (msg) {
        case WM_INITDIALOG: {
            data = (NewMapData*)lParam;
            HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            
            CreateWindowExW(0, L"STATIC", L"Map Name:", WS_CHILD | WS_VISIBLE, 20, 20, 80, 20, hDlg, NULL, NULL, NULL);
            HWND hName = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", data->name, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 
                110, 18, 300, 24, hDlg, (HMENU)201, NULL, NULL);
            SendMessage(hName, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"Width:", WS_CHILD | WS_VISIBLE, 20, 55, 60, 20, hDlg, NULL, NULL, NULL);
            wchar_t dimBuf[32];
            swprintf(dimBuf, 32, L"%d", data->width);
            HWND hWidth = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dimBuf, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER, 
                80, 53, 60, 24, hDlg, (HMENU)202, NULL, NULL);
            SendMessage(hWidth, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"Height:", WS_CHILD | WS_VISIBLE, 160, 55, 60, 20, hDlg, NULL, NULL, NULL);
            swprintf(dimBuf, 32, L"%d", data->height);
            HWND hHeight = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", dimBuf, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER, 
                220, 53, 60, 24, hDlg, (HMENU)203, NULL, NULL);
            SendMessage(hHeight, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"Project Folder:", WS_CHILD | WS_VISIBLE, 20, 90, 90, 20, hDlg, NULL, NULL, NULL);
            HWND hFolder = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", data->folder, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_READONLY, 
                110, 88, 250, 24, hDlg, (HMENU)204, NULL, NULL);
            SendMessage(hFolder, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hBrowseFolder = CreateWindowExW(0, L"BUTTON", L"...", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
                365, 88, 40, 24, hDlg, (HMENU)205, NULL, NULL);
            SendMessage(hBrowseFolder, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            CreateWindowExW(0, L"STATIC", L"(Assets folder with BMPs)", WS_CHILD | WS_VISIBLE, 110, 115, 200, 20, hDlg, NULL, NULL, NULL);
            
            HWND hOK = CreateWindowExW(0, L"BUTTON", L"Create", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
                240, 180, 80, 30, hDlg, (HMENU)IDOK, NULL, NULL);
            SendMessage(hOK, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hCancel = CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
                330, 180, 80, 30, hDlg, (HMENU)IDCANCEL, NULL, NULL);
            SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            SetFocus(hName);
            return FALSE;
        }
        
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 205: {
                    wchar_t folderPath[MAX_PATH];
                    if (BrowseForFolder(folderPath, MAX_PATH, L"Select Project/Assets Folder")) {
                        SetDlgItemTextW(hDlg, 204, folderPath);
                        wcscpy(data->folder, folderPath);
                    }
                    return TRUE;
                }
                case IDOK: {
                    wchar_t nameBuf[128];
                    GetDlgItemTextW(hDlg, 201, nameBuf, 128);
                    wcscpy(data->name, nameBuf);
                    
                    data->width = GetDlgItemInt(hDlg, 202, NULL, FALSE);
                    data->height = GetDlgItemInt(hDlg, 203, NULL, FALSE);
                    GetDlgItemTextW(hDlg, 204, data->folder, MAX_PATH);
                    
                    if (data->width < 8) data->width = 8;
                    if (data->width > 256) data->width = 256;
                    if (data->height < 8) data->height = 8;
                    if (data->height > 256) data->height = 256;
                    
                    data->confirmed = true;
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
            }
            break;
            
        case WM_CLOSE:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
    }
    return FALSE;
}

void ShowNewMapDialog() {
    NewMapData data;
    wcscpy(data.name, L"NewMap");
    data.width = 64;
    data.height = 64;
    wcscpy(data.folder, projectFolder);
    data.confirmed = false;
    
    #pragma pack(push, 4)
    struct {
        DLGTEMPLATE dlg;
        WORD menu;
        WORD wndClass;
        WCHAR title[12];
    } dlgTemplate = {};
    #pragma pack(pop)
    
    dlgTemplate.dlg.style = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    dlgTemplate.dlg.dwExtendedStyle = 0;
    dlgTemplate.dlg.cdit = 0;
    dlgTemplate.dlg.x = 0;
    dlgTemplate.dlg.y = 0;
    dlgTemplate.dlg.cx = 250;
    dlgTemplate.dlg.cy = 130;
    dlgTemplate.menu = 0;
    dlgTemplate.wndClass = 0;
    wcscpy(dlgTemplate.title, L"New Map");
    
    INT_PTR result = DialogBoxIndirectParamW(GetModuleHandle(NULL), &dlgTemplate.dlg, hMainWnd, NewMapDlgProc, (LPARAM)&data);
    
    if (result == IDOK && data.confirmed) {
        memset(worldMap, 0, sizeof(worldMap));
        placedObjects.clear();

        wcscpy(mapName, data.name);
        wcscpy(projectFolder, data.folder);
        SaveEditorConfig();
        mapWidth = data.width;
        mapHeight = data.height;
        currentMapPath[0] = L'\0';
        cameraX = 0;
        cameraY = 0;
    }
}

void FloodFill(int x, int y, int oldTile, int newTile) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return;
    if (worldMap[x][y] != oldTile) return;
    if (oldTile == newTile) return;
    
    std::vector<std::pair<int,int>> stack;
    stack.push_back({x, y});
    
    while (!stack.empty()) {
        auto [cx, cy] = stack.back();
        stack.pop_back();
        
        if (cx < 0 || cx >= mapWidth || cy < 0 || cy >= mapHeight) continue;
        if (worldMap[cx][cy] != oldTile) continue;
        
        worldMap[cx][cy] = newTile;
        
        stack.push_back({cx + 1, cy});
        stack.push_back({cx - 1, cy});
        stack.push_back({cx, cy + 1});
        stack.push_back({cx, cy - 1});
    }
}

DWORD MakeColor(int r, int g, int b) {
    return (r << 16) | (g << 8) | b;
}

void RenderEditor() {
    for (int i = 0; i < EDITOR_WIDTH * EDITOR_HEIGHT; i++) {
        backBufferPixels[i] = MakeColor(30, 30, 35);
    }
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int sx, sy;
            WorldToScreen((float)x, (float)y, sx, sy);
            
            int halfW = (int)(TILE_SIZE / 2.0f * zoom);
            int quarterH = (int)(TILE_SIZE / 4.0f * zoom);
            
            if (sx + halfW < SIDEBAR_WIDTH || sx - halfW > EDITOR_WIDTH) continue;
            if (sy + quarterH < TOOLBAR_HEIGHT || sy - quarterH > EDITOR_HEIGHT - PROPERTIES_HEIGHT) continue;
            
            int tileType = worldMap[x][y];
            TileDef* tileTex = nullptr;
            
            if (tileType > 0 && tileType <= (int)tileDefs.size()) {
                tileTex = &tileDefs[tileType - 1];
            }
            
            for (int py = -quarterH; py <= quarterH; py++) {
                int screenY = sy + py;
                if (screenY < TOOLBAR_HEIGHT || screenY >= EDITOR_HEIGHT - PROPERTIES_HEIGHT) continue;
                
                int rowWidth = (quarterH > 0) ? (halfW - abs(py) * halfW / quarterH) : halfW;
                for (int px = -rowWidth; px <= rowWidth; px++) {
                    int screenX = sx + px;
                    if (screenX < SIDEBAR_WIDTH || screenX >= EDITOR_WIDTH) continue;
                    
                    DWORD color;
                    if (tileTex && tileTex->pixels && tileTex->pixelW > 0 && tileTex->pixelH > 0) {
                        float u = (float)(px + rowWidth) / (float)(rowWidth * 2 + 1);
                        float v = (float)(py + quarterH) / (float)(quarterH * 2 + 1);
                        int texX = (int)(u * tileTex->pixelW) % tileTex->pixelW;
                        int texY = (int)(v * tileTex->pixelH) % tileTex->pixelH;
                        if (texX < 0) texX = 0;
                        if (texY < 0) texY = 0;
                        color = tileTex->pixels[texY * tileTex->pixelW + texX];
                    } else if (tileType == 0) {
                        color = MakeColor(25, 25, 30);
                    } else {
                        color = MakeColor(80, 80, 80);
                    }
                    
                    if ((color & 0x00FFFFFF) != 0x00FF00FF) {
                        backBufferPixels[screenY * EDITOR_WIDTH + screenX] = color;
                    }
                }
            }
        }
    }
    
    std::vector<std::pair<float, int>> objOrder;
    for (int i = 0; i < (int)placedObjects.size(); i++)
        objOrder.push_back({placedObjects[i].x + placedObjects[i].y, i});
    std::sort(objOrder.begin(), objOrder.end());
    
    for (auto& op : objOrder) {
        PlacedObject& obj = placedObjects[op.second];
        if (obj.defIndex < 0 || obj.defIndex >= (int)objectDefs.size()) continue;
        
        int sx, sy;
        WorldToScreen(obj.x, obj.y, sx, sy);
        
        ObjectDef& def = objectDefs[obj.defIndex];
        if (!def.pixels) continue;
        
        int drawW = (int)(32 * obj.width * zoom);
        int drawH = (int)(32 * obj.height * zoom);
        
        for (int py = 0; py < drawH; py++) {
            int screenY = sy - drawH + py;
            if (screenY < TOOLBAR_HEIGHT || screenY >= EDITOR_HEIGHT - PROPERTIES_HEIGHT) continue;
            
            int srcY = py * def.pixelH / drawH;
            
            for (int px = 0; px < drawW; px++) {
                int screenX = sx - drawW / 2 + px;
                if (screenX < SIDEBAR_WIDTH || screenX >= EDITOR_WIDTH) continue;
                
                int srcX = px * def.pixelW / drawW;
                DWORD col = def.pixels[srcY * def.pixelW + srcX];
                
                int alpha = (col >> 24) & 0xFF;
                if (alpha != 0 && (col & 0x00FFFFFF) != 0x00FF00FF) {
                    backBufferPixels[screenY * EDITOR_WIDTH + screenX] = col;
                }
            }
        }
    }
}

void RenderUI() {
    RECT sidebarRect = {0, TOOLBAR_HEIGHT, SIDEBAR_WIDTH, EDITOR_HEIGHT - PROPERTIES_HEIGHT};
    FillRect(backBufferDC, &sidebarRect, hBrushSidebar);
    
    RECT toolbarRect = {0, 0, EDITOR_WIDTH, TOOLBAR_HEIGHT};
    FillRect(backBufferDC, &toolbarRect, hBrushToolbar);
    
    RECT propsRect = {0, EDITOR_HEIGHT - PROPERTIES_HEIGHT, EDITOR_WIDTH, EDITOR_HEIGHT};
    FillRect(backBufferDC, &propsRect, hBrushSidebar);
    
    SetBkMode(backBufferDC, TRANSPARENT);
    SetTextColor(backBufferDC, RGB(255, 255, 255));
    SelectObject(backBufferDC, hFontUI);
    
    TextOutW(backBufferDC, 10, 10, L"LoneStudios - Map Editor", 24);
    
    TextOutW(backBufferDC, 10, TOOLBAR_HEIGHT + 10, L"TILES", 5);
    
    int tileY = TOOLBAR_HEIGHT + 35;
    for (int i = 0; i < (int)tileDefs.size(); i++) {
        RECT itemRect = {5, tileY, SIDEBAR_WIDTH - 5, tileY + 25};
        if (selectedTool == 0 && selectedTileType == i + 1) {
            FillRect(backBufferDC, &itemRect, hBrushSelected);
        }
        TextOutW(backBufferDC, 15, tileY + 4, tileDefs[i].name.c_str(), (int)tileDefs[i].name.length());
        tileY += 28;
    }
    
    RECT addTileRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    DrawEdge(backBufferDC, &addTileRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"+ Add Tile", 10);
    tileY += 35;
    
    TextOutW(backBufferDC, 10, tileY, L"OBJECTS", 7);
    tileY += 25;
    
    for (int i = 0; i < (int)objectDefs.size(); i++) {
        RECT itemRect = {5, tileY, SIDEBAR_WIDTH - 5, tileY + 25};
        if (selectedTool == 1 && selectedObjectDefIndex == i) {
            FillRect(backBufferDC, &itemRect, hBrushSelected);
        }
        TextOutW(backBufferDC, 15, tileY + 4, objectDefs[i].name.c_str(), (int)objectDefs[i].name.length());
        tileY += 28;
    }
    
    RECT addObjRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    DrawEdge(backBufferDC, &addObjRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"+ Add Object", 12);
    tileY += 35;
    
    TextOutW(backBufferDC, 10, tileY, L"BMP PALETTE", 11);
    tileY += 25;
    
    for (int i = 0; i < (int)bmpPalette.size(); i++) {
        RECT itemRect = {5, tileY, SIDEBAR_WIDTH - 5, tileY + 25};
        if (selectedPaletteIndex == i) {
            FillRect(backBufferDC, &itemRect, hBrushSelected);
        }
        TextOutW(backBufferDC, 15, tileY + 4, bmpPalette[i].name.c_str(), (int)bmpPalette[i].name.length());
        tileY += 28;
    }
    
    RECT addPaletteRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    DrawEdge(backBufferDC, &addPaletteRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"+ Add BMP", 9);
    tileY += 35;
    
    TextOutW(backBufferDC, 10, tileY, L"TOOLS", 5);
    tileY += 25;
    
    RECT selectRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    if (selectedTool == 6) {
        FillRect(backBufferDC, &selectRect, hBrushSelected);
    }
    DrawEdge(backBufferDC, &selectRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"Select / Move", 13);
    tileY += 35;


    
    RECT eraserRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    if (selectedTool == 2) {
        FillRect(backBufferDC, &eraserRect, hBrushSelected);
    }
    DrawEdge(backBufferDC, &eraserRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"Eraser", 6);
    tileY += 35;
    
    RECT bucketRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    if (selectedTool == 5) {
        FillRect(backBufferDC, &bucketRect, hBrushSelected);
    }
    DrawEdge(backBufferDC, &bucketRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"Bucket Fill", 11);
    tileY += 40;
    
    TextOutW(backBufferDC, 10, tileY, L"FILE", 4);
    tileY += 25;
    
    RECT newMapRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    DrawEdge(backBufferDC, &newMapRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"New Map", 7);
    tileY += 30;
    
    RECT saveRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    DrawEdge(backBufferDC, &saveRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"Save Map (.lsm)", 15);
    tileY += 30;
    
    RECT loadRect = {10, tileY, SIDEBAR_WIDTH - 10, tileY + 25};
    DrawEdge(backBufferDC, &loadRect, EDGE_RAISED, BF_RECT);
    TextOutW(backBufferDC, 15, tileY + 4, L"Load Map", 8);
    
    wchar_t statusBuf[512];
    swprintf(statusBuf, 512, L"Map: %ls (%dx%d)  |  Zoom: %.1fx  |  Objects: %d  |  %ls", 
        mapName, mapWidth, mapHeight, zoom, (int)placedObjects.size(),
        currentMapPath[0] ? currentMapPath : L"Unsaved");
    TextOutW(backBufferDC, 10, EDITOR_HEIGHT - PROPERTIES_HEIGHT + 10, statusBuf, (int)wcslen(statusBuf));
    
    TextOutW(backBufferDC, 10, EDITOR_HEIGHT - PROPERTIES_HEIGHT + 35, L"[WASD] Pan  [Scroll] Zoom  [Ctrl+N] New  [Ctrl+S] Save  [Ctrl+L] Load  [Del] Remove", 82);
    TextOutW(backBufferDC, 10, EDITOR_HEIGHT - PROPERTIES_HEIGHT + 55, L"Select a tile or object from sidebar, then click on map to place", 64);
}

void HandleClick(int mx, int my, bool rightClick) {
    if (mx < SIDEBAR_WIDTH && my > TOOLBAR_HEIGHT && my < EDITOR_HEIGHT - PROPERTIES_HEIGHT) {
        int tileY = TOOLBAR_HEIGHT + 35;
        
        for (int i = 0; i < (int)tileDefs.size(); i++) {
            if (my >= tileY && my < tileY + 25) {
                if (rightClick) {
                    if (tileDefs[i].pixels) delete[] tileDefs[i].pixels;
                    tileDefs.erase(tileDefs.begin() + i);
                    SaveTileDefs();
                } else {
                    selectedTool = 0;
                    selectedTileType = i + 1;
                }
                return;
            }
            tileY += 28;
        }
        
        if (my >= tileY && my < tileY + 25) {
            ShowAddTileDialog();
            return;
        }
        tileY += 35;
        
        tileY += 25;
        
        for (int i = 0; i < (int)objectDefs.size(); i++) {
            if (my >= tileY && my < tileY + 25) {
                selectedTool = 1;
                selectedObjectDefIndex = i;
                return;
            }
            tileY += 28;
        }
        
        if (my >= tileY && my < tileY + 25) {
            ShowAddObjectDialog();
            return;
        }
        tileY += 35;
        
        tileY += 25;
        
        for (int i = 0; i < (int)bmpPalette.size(); i++) {
            if (my >= tileY && my < tileY + 25) {
                selectedPaletteIndex = i;
                return;
            }
            tileY += 28;
        }
        
        if (my >= tileY && my < tileY + 25) {
            AddToBMPPalette();
            SaveBMPPalette();
            return;
        }
        tileY += 35;
        
        tileY += 25;
        
        if (my >= tileY && my < tileY + 25) {
            selectedTool = 6;
            return;
        }
        tileY += 35;

        
        if (my >= tileY && my < tileY + 25) {
            selectedTool = 2;
            return;
        }
        tileY += 35;
        
        if (my >= tileY && my < tileY + 25) {
            selectedTool = 5;
            return;
        }
        tileY += 35;
        
        tileY += 25;
        
        if (my >= tileY && my < tileY + 25) {
            ShowNewMapDialog();
            return;
        }
        tileY += 30;
        
        if (my >= tileY && my < tileY + 25) {
            SaveMapDialog();
            return;
        }
        tileY += 30;
        
        if (my >= tileY && my < tileY + 25) {
            LoadMapDialog();
            return;
        }
        
        return;
    }
    
    if (mx > SIDEBAR_WIDTH && my > TOOLBAR_HEIGHT && my < EDITOR_HEIGHT - PROPERTIES_HEIGHT) {
        float wx, wy;
        ScreenToWorld(mx, my, wx, wy);
        
        int gridX = (int)wx;
        int gridY = (int)wy;
        
        if (selectedTool == 0 && gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
            worldMap[gridX][gridY] = selectedTileType;
        }
        else if (selectedTool == 1 && selectedObjectDefIndex >= 0) {
            PlacedObject obj;
            obj.defIndex = selectedObjectDefIndex;
            obj.x = wx;
            obj.y = wy;
            obj.z = 0.0f;
            obj.width = objectDefs[selectedObjectDefIndex].defaultWidth;
            obj.height = objectDefs[selectedObjectDefIndex].defaultHeight;
            obj.collisionRadius = objectDefs[selectedObjectDefIndex].collisionRadius;
            obj.hasCollision = objectDefs[selectedObjectDefIndex].hasCollision;
            obj.linkedDoorId = -1;
            placedObjects.push_back(obj);
        }

        else if (selectedTool == 2 && gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
            worldMap[gridX][gridY] = 0;
        }
        else if (selectedTool == 5 && gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
            int oldTile = worldMap[gridX][gridY];
            FloodFill(gridX, gridY, oldTile, selectedTileType);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool mouseDown = false;
    
    switch (msg) {
        case WM_CREATE: {
            HDC hdc = GetDC(hwnd);
            
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = EDITOR_WIDTH;
            bmi.bmiHeader.biHeight = -EDITOR_HEIGHT;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            
            backBufferDC = CreateCompatibleDC(hdc);
            backBufferDIB = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&backBufferPixels, NULL, 0);
            SelectObject(backBufferDC, backBufferDIB);
            
            ReleaseDC(hwnd, hdc);
            
            SetTimer(hwnd, 1, 16, NULL);
            return 0;
        }
        
        case WM_TIMER: {
            float camSpeed = 8.0f;
            if (keys['W']) cameraY -= camSpeed;
            if (keys['S'] && !(GetKeyState(VK_CONTROL) & 0x8000)) cameraY += camSpeed;
            if (keys['A']) cameraX -= camSpeed;
            if (keys['D']) cameraX += camSpeed;
            if (keys[VK_LEFT]) cameraX -= camSpeed;
            if (keys[VK_RIGHT]) cameraX += camSpeed;
            if (keys[VK_UP]) cameraY -= camSpeed;
            if (keys[VK_DOWN]) cameraY += camSpeed;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        
        case WM_ERASEBKGND:
            return 1;
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RenderEditor();
            RenderUI();
            
            BitBlt(hdc, 0, 0, EDITOR_WIDTH, EDITOR_HEIGHT, backBufferDC, 0, 0, SRCCOPY);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            mouseDown = true;
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);
            
            if (mx > SIDEBAR_WIDTH && my > TOOLBAR_HEIGHT && my < EDITOR_HEIGHT - PROPERTIES_HEIGHT) {
                float wx, wy;
                ScreenToWorld(mx, my, wx, wy);
                
                if (selectedTool == 6) {
                    // Try selecting Objects (Sprites)
                    for (int i = 0; i < (int)placedObjects.size(); i++) {
                         PlacedObject& obj = placedObjects[i];
                         int sx, sy;
                         WorldToScreen(obj.x, obj.y, sx, sy);
                         int dw = (int)(32 * obj.width * zoom);
                         int dh = (int)(32 * obj.height * zoom);
                         
                         int screenLeft = sx - dw / 2;
                         int screenTop = sy - dh;
                         
                         if (dw > 0 && dh > 0 && mx >= screenLeft && mx < screenLeft + dw && my >= screenTop && my < screenTop + dh) {
                             // Pixel-Perfect Hit Test
                             ObjectDef& def = objectDefs[obj.defIndex];
                             bool hit = true;
                             
                             if (def.pixels && def.pixelW > 0 && def.pixelH > 0) {
                                 int localX = mx - screenLeft;
                                 int localY = my - screenTop;
                                 
                                 // Map to texture coordinates
                                 int texX = localX * def.pixelW / dw;
                                 int texY = localY * def.pixelH / dh;
                                 
                                 if (texX >= 0 && texX < def.pixelW && texY >= 0 && texY < def.pixelH) {
                                     DWORD color = def.pixels[texY * def.pixelW + texX];
                                     // Check against transparency key (Magenta)
                                     if ((color & 0x00FFFFFF) == 0x00FF00FF) {
                                         hit = false;
                                     }
                                 } else {
                                     hit = false;
                                 }
                             }
                             
                             if (hit) {
                                 selectedObjectDefIndex = -1;
                                 draggingObjectIndex = i;
                                 dragStartX = wx;
                                 dragStartY = wy;
                                 return 0;
                             }
                         }
                    }
                }
            }
            
            HandleClick(mx, my, false);
            return 0;
        }
        
        case WM_LBUTTONUP: {
            mouseDown = false;
            draggingObjectIndex = -1;
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);
            
            if (draggingObjectIndex >= 0 && draggingObjectIndex < (int)placedObjects.size()) {
                float wx, wy;
                ScreenToWorld(mx, my, wx, wy);
                PlacedObject& obj = placedObjects[draggingObjectIndex];
                obj.x += (wx - dragStartX);
                obj.y += (wy - dragStartY);
                dragStartX = wx;
                dragStartY = wy;
                return 0;
            }
            
            if (mouseDown) {
                if (mx > SIDEBAR_WIDTH && my > TOOLBAR_HEIGHT && my < EDITOR_HEIGHT - PROPERTIES_HEIGHT) {
                    float wx, wy;
                    ScreenToWorld(mx, my, wx, wy);
                    int gridX = (int)wx;
                    int gridY = (int)wy;
                    
                    if (selectedTool == 0 && gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
                        worldMap[gridX][gridY] = selectedTileType;
                    }
                    else if (selectedTool == 2 && gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
                        worldMap[gridX][gridY] = 0;
                    }
                }
            }
            return 0;
        }
        
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == 2001) { // Copy Object
                if (selectedPlacedObjectIndexForMenu >= 0 && selectedPlacedObjectIndexForMenu < (int)placedObjects.size()) {
                    g_clipboardObject = placedObjects[selectedPlacedObjectIndexForMenu];
                    g_hasObjectClipboard = true;
                }
            }
            else if (id == 2002) { // Paste Object
                if (g_hasObjectClipboard) {
                    PlacedObject newObj = g_clipboardObject;
                    newObj.x += 1.0f;
                    newObj.y += 1.0f;
                    placedObjects.push_back(newObj);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            else if (id == 2003) { // Delete Object
                if (selectedPlacedObjectIndexForMenu >= 0 && selectedPlacedObjectIndexForMenu < (int)placedObjects.size()) {
                    placedObjects.erase(placedObjects.begin() + selectedPlacedObjectIndexForMenu);
                    selectedPlacedObjectIndexForMenu = -1;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }

        case WM_RBUTTONUP: {

            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);
            
            if (mx > SIDEBAR_WIDTH && my > TOOLBAR_HEIGHT && my < EDITOR_HEIGHT - PROPERTIES_HEIGHT) {
                float wx, wy;
                ScreenToWorld(mx, my, wx, wy);

                // Hit test objects FIRST (reverse order for painter's algorithm)
                int objectHitIndex = -1;
                for (int i = (int)placedObjects.size() - 1; i >= 0; i--) {
                    PlacedObject& obj = placedObjects[i];
                    if (obj.defIndex < 0 || obj.defIndex >= (int)objectDefs.size()) continue;
                    
                    int sx, sy;
                    WorldToScreen(obj.x, obj.y, sx, sy);
                    int dw = (int)(32 * obj.width * zoom);
                    int dh = (int)(32 * obj.height * zoom);
                    
                    if (mx >= sx - dw/2 && mx <= sx + dw/2 && my >= sy - dh && my <= sy) {
                        objectHitIndex = i;
                        break;
                    }
                }
                
                if (objectHitIndex != -1) {
                    selectedPlacedObjectIndexForMenu = objectHitIndex;
                    InvalidateRect(hwnd, NULL, FALSE);
                    
                    HMENU hPopup = CreatePopupMenu();
                    AppendMenuW(hPopup, MF_STRING, 2001, L"Copy");
                    if (g_hasObjectClipboard) AppendMenuW(hPopup, MF_STRING, 2002, L"Paste");
                    AppendMenuW(hPopup, MF_SEPARATOR, 0, NULL);
                    AppendMenuW(hPopup, MF_STRING, 2003, L"Delete");
                    
                    POINT pt;
                    GetCursorPos(&pt);
                    TrackPopupMenu(hPopup, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                    DestroyMenu(hPopup);
                    return 0;
                }
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                zoom *= 1.1f;
                if (zoom > 4.0f) zoom = 4.0f;
            } else {
                zoom /= 1.1f;
                if (zoom < 0.25f) zoom = 0.25f;
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            keys[wParam] = true;
            switch (wParam) {
                case VK_OEM_PLUS:
                case VK_ADD:
                    zoom *= 1.2f;
                    if (zoom > 4.0f) zoom = 4.0f;
                    break;
                case VK_OEM_MINUS:
                case VK_SUBTRACT:
                    zoom /= 1.2f;
                    if (zoom < 0.25f) zoom = 0.25f;
                    break;
                case 'S':
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        SaveMapDialog();
                    }
                    break;
                case 'L':
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        LoadMapDialog();
                    }
                    break;
                case 'N':
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        ShowNewMapDialog();
                    }
                    break;
                case VK_DELETE:
                    if (!placedObjects.empty()) {
                        placedObjects.pop_back();
                    }
                    break;
            }
            return 0;
        }
        
        case WM_KEYUP: {
            keys[wParam] = false;
            return 0;
        }
        
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            if (backBufferDIB) DeleteObject(backBufferDIB);
            if (backBufferDC) DeleteDC(backBufferDC);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance; (void)lpCmdLine;
    
    CoInitialize(NULL);
    
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    GetExeDir();
    LoadEditorConfig();
    InitGraphics();
    LoadObjectDefs();
    LoadTileDefs();
    LoadBMPPalette();
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"LoneStudiosClass";
    RegisterClassExW(&wc);
    
    RECT windowRect = {0, 0, EDITOR_WIDTH, EDITOR_HEIGHT};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    hMainWnd = CreateWindowExW(0, L"LoneStudiosClass", L"LoneStudios - Map Editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        NULL, NULL, hInstance, NULL);
    
    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    CleanupGraphics();
    
    for (auto& def : objectDefs) {
        if (def.pixels) delete[] def.pixels;
    }
    for (auto& def : tileDefs) {
        if (def.pixels) delete[] def.pixels;
    }
    for (auto& tex : bmpPalette) {
        if (tex.pixels) delete[] tex.pixels;
    }
    
    CoUninitialize();
    
    return (int)msg.wParam;
}
