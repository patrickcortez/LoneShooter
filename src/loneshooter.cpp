/*
 * LoneShooter - Open World 2.5D Raycaster
 * Compile:g++ -o bin/LoneShooter64.exe src/loneshooter.cpp -lgdi32 -lwinmm -mwindows -lole32 -loleaut32 -luuid -lcomctl32 -lopengl32 -msse2 -O2 -static 2>&1
 * Run: ./LoneShooter.exe
 * Controls: WASD=Move, Mouse=Look, ESC=Quit
 * By Patrick Andrew Cortez
 */

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shlobj.h>
#include <dbghelp.h>
#include <olectl.h>
#include <ole2.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

std::unordered_map<std::string, bool> g_EventCache;
inline void TriggerEvent(const std::string& eventName, bool state = true) {
    g_EventCache[eventName] = state;
}
inline bool HasEvent(const std::string& eventName) {
    return g_EventCache[eventName];
}
#include <process.h>
#include <mmsystem.h>
#include "pathfinder.hpp"
#include "neural.hpp"
#include "dialogue.hpp"
#include "npcs.hpp"
#include <emmintrin.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include <GL/gl.h>
#pragma comment(lib, "opengl32.lib")

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_BGRA 0x80E1
#define GL_TEXTURE0 0x84C0

#define GL_COMPUTE_SHADER 0x91B9
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_ALL_BARRIER_BITS 0xFFFFFFFF
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_RGBA32F 0x8814
#define GL_R32F 0x822E
#define GL_RGBA8 0x8058
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_TEXTURE_FETCH_BARRIER_BIT 0x00000008
#define GL_RED 0x1903

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum type);
typedef void (*PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
typedef void (*PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (*PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void (*PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (*PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (*PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void (*PFNGLUSEPROGRAMPROC)(GLuint program);
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (*PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (*PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (*PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (*PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (*PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (*PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (*PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (*PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void (*PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (*PFNGLDELETESHADERPROC)(GLuint shader);

typedef void (*PFNGLDISPATCHCOMPUTEPROC)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void (*PFNGLMEMORYBARRIERPROC)(GLbitfield barriers);
typedef void (*PFNGLBINDIMAGETEXTUREPROC)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void (*PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void (*PFNGLTEXSTORAGE2DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (*PFNGLGETTEXIMAGEPROC)(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETESHADERPROC glDeleteShader;

PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
PFNGLMEMORYBARRIERPROC glMemoryBarrier;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLTEXSTORAGE2DPROC glTexStorage2D;

HDC g_glDC = NULL;
HGLRC g_glRC = NULL;
GLuint g_vcrProgram = 0;
GLuint g_vcrTexture = 0;
GLuint g_vcrVAO = 0;
GLuint g_vcrVBO = 0;
bool g_glInitialized = false;
float g_glTime = 0.0f;
GLint g_locTex = -1;
GLint g_locTime = -1;
GLint g_locResolution = -1;

GLuint g_raycastProgram = 0;
GLuint g_renderTex = 0;
GLuint g_zBufferTex = 0;
GLuint g_mapSSBO = 0;
GLuint g_grassTex = 0;
GLuint g_borderWallTex = 0;
GLuint g_gateWallTex = 0;
bool g_gpuRaycastAvailable = false;
GLint g_rcLocPlayerPos = -1;
GLint g_rcLocPlayerAngle = -1;
GLint g_rcLocPlayerPitch = -1;
GLint g_rcLocBossActive = -1;
GLint g_rcLocScreenSize = -1;

GLuint g_spriteBatchProgram = 0;
GLuint g_spriteAtlasTex = 0;
GLuint g_spriteVAO = 0;
GLuint g_spriteVBO = 0;
bool g_gpuSpritesAvailable = false;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern bool g_FullscreenMode;
extern bool g_DevConsole;
extern float g_MouseSensitivity;
bool g_EnableVHS = true;
bool g_PerformanceMode = false;
bool g_EnableMouseLook = true; // Restored missing global
bool pendingGameReset = false; // Global flag for deferred reset
int g_PendingUpgrades = 0;
bool g_LevelUpWindowOpen = false;
float g_BonusSpeed = 0.0f;
extern wchar_t g_GameVersion[32];

// Forward declarations
void InitPostProcess();
void ApplyPostProcess();
void FinalizePostProcess(HDC memDC);
void PopulateSpatialGrids();
inline DWORD MakeColor(int r, int g, int b) {
    return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

struct Resolution { int w, h; const wchar_t* name; };
Resolution g_Resolutions[] = {
    {800, 600, L"800 x 600"},
    {1024, 768, L"1024 x 768"},
    {1280, 720, L"1280 x 720 (HD)"},
    {1280, 960, L"1280 x 960"},
    {1366, 768, L"1366 x 768"},
    {1600, 900, L"1600 x 900"},
    {1920, 1080, L"1920 x 1080 (Full HD)"}
};
int g_NumResolutions = sizeof(g_Resolutions) / sizeof(g_Resolutions[0]);
int g_SelectedResolution = 1;

float g_RenderDistance = 30.0f;

bool LoadSettingsJSON() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    
    wchar_t settingsPath[MAX_PATH];
    swprintf(settingsPath, MAX_PATH, L"%ls\\configs\\settings.json", exePath);
    
    FILE* f = _wfopen(settingsPath, L"rb");
    if (!f) return false;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = new char[size + 1];
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    auto findValue = [&](const char* key) -> const char* {
        const char* pos = strstr(buffer, key);
        if (!pos) return nullptr;
        pos = strchr(pos, ':');
        if (!pos) return nullptr;
        pos++;
        while (*pos == ' ' || *pos == '\t') pos++;
        return pos;
    };
    
    auto parseBool = [](const char* val) -> bool {
        return strstr(val, "true") == val;
    };
    
    auto parseDouble = [](const char* val) -> double {
        return atof(val);
    };
    
    auto parseInt = [](const char* val) -> int {
        return atoi(val);
    };
    
    const char* consoleVal = findValue("\"Console\"");
    if (consoleVal) g_DevConsole = parseBool(consoleVal);
    
    const char* fullscreenVal = findValue("\"Fullscreen\"");
    if (fullscreenVal) g_FullscreenMode = parseBool(fullscreenVal);
    
    const char* widthVal = findValue("\"Width\"");
    if (widthVal) SCREEN_WIDTH = parseInt(widthVal);
    
    const char* heightVal = findValue("\"Height\"");
    if (heightVal) SCREEN_HEIGHT = parseInt(heightVal);
    
    const char* sensitivityVal = findValue("\"Mouse Sensitivity\"");
    if (sensitivityVal) g_MouseSensitivity = (float)parseDouble(sensitivityVal);

    const char* vhsVal = findValue("\"EnableVHS\"");
    if (vhsVal) g_EnableVHS = parseBool(vhsVal);

    const char* perfVal = findValue("\"Performance\"");
    if (perfVal) g_PerformanceMode = parseBool(perfVal);
    
    const char* renderDistVal = findValue("\"RenderDistance\"");
    if (renderDistVal) g_RenderDistance = (float)parseDouble(renderDistVal);
    
    const char* versionVal = findValue("\"Version\"");
    if (versionVal) {
        const char* start = strchr(versionVal, '"');
        if (start) {
            start++;
            const char* end = strchr(start, '"');
            if (end) {
                int len = (int)(end - start);
                if (len > 30) len = 30;
                for (int i = 0; i < len; i++) g_GameVersion[i] = (wchar_t)start[i];
                g_GameVersion[len] = L'\0';
            }
        }
    }
    
    for (int i = 0; i < g_NumResolutions; i++) {
        if (g_Resolutions[i].w == SCREEN_WIDTH && g_Resolutions[i].h == SCREEN_HEIGHT) {
            g_SelectedResolution = i;
            break;
        }
    }
    
    delete[] buffer;
    return true;
}

void SaveSettingsJSON() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    
    wchar_t settingsPath[MAX_PATH];
    swprintf(settingsPath, MAX_PATH, L"%ls\\configs\\settings.json", exePath);
    
    FILE* f = _wfopen(settingsPath, L"w");
    if (!f) return;
    
    char versionA[64];
    WideCharToMultiByte(CP_UTF8, 0, g_GameVersion, -1, versionA, 64, NULL, NULL);
    
    fprintf(f, "{\n");
    fprintf(f, "    \"Settings\": {\n");
    fprintf(f, "        \"Console\": %s,\n", g_DevConsole ? "true" : "false");
    fprintf(f, "        \"Fullscreen\": %s,\n", g_FullscreenMode ? "true" : "false");
    fprintf(f, "        \"Width\": %d,\n", SCREEN_WIDTH);
    fprintf(f, "        \"Height\": %d,\n", SCREEN_HEIGHT);
    fprintf(f, "        \"Mouse Sensitivity\": %.1f,\n", g_MouseSensitivity);
    fprintf(f, "        \"EnableVHS\": %s,\n", g_EnableVHS ? "true" : "false");
    fprintf(f, "        \"Performance\": %s,\n", g_PerformanceMode ? "true" : "false");
    fprintf(f, "        \"RenderDistance\": %.1f\n", g_RenderDistance);
    fprintf(f, "    },\n");
    fprintf(f, "    \"Version\": \"%s\"\n", versionA);
    fprintf(f, "}\n");
    fclose(f);
}

#define IDC_RESOLUTION_COMBO 1001
#define IDC_SENSITIVITY_SLIDER 1002
#define IDC_SENSITIVITY_LABEL 1003
#define IDC_FULLSCREEN_CHECK 1004
#define IDC_CONSOLE_CHECK 1005
#define IDC_PLAY_BUTTON 1006
#define IDC_CANCEL_BUTTON 1007
#define IDC_VERSION_LABEL 1008
#define IDC_VHS_CHECK 1009
#define IDC_PERFORMANCE_CHECK 1010
#define IDC_RENDERDIST_SLIDER 1011
#define IDC_RENDERDIST_LABEL 1012
#define IDC_CONTINUE_BUTTON 1013

HWND g_hSettingsDialog = NULL;
bool g_SettingsConfirmed = false;
bool g_LoadGameRequested = false;

LRESULT CALLBACK SettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HBITMAP hBannerBmp = NULL;
    static int bannerW = 0, bannerH = 0;
    
    switch (msg) {
        case WM_CREATE: {
            wchar_t exePath[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            wchar_t* lastSlash = wcsrchr(exePath, L'\\');
            if (lastSlash) *lastSlash = L'\0';
            
            wchar_t fontPath[MAX_PATH];
            swprintf(fontPath, MAX_PATH, L"%ls\\assets\\fonts\\VCR_OSD_MONO_1.001.ttf", exePath);
            int fontAdded = AddFontResourceExW(fontPath, FR_PRIVATE, 0);
            
            HFONT hFont = NULL;
            if (fontAdded > 0) {
                hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"VCR OSD Mono");
            }
            if (!hFont) {
                hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            }
            
            wchar_t bannerPath[MAX_PATH];
            swprintf(bannerPath, MAX_PATH, L"%ls\\assets\\UI\\banner.bmp", exePath);
            
            hBannerBmp = (HBITMAP)LoadImageW(NULL, bannerPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBannerBmp) {
                BITMAP bm;
                GetObject(hBannerBmp, sizeof(BITMAP), &bm);
                bannerW = bm.bmWidth;
                bannerH = bm.bmHeight;
            } else {
                HFONT hTitleFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
                HWND hTitle = CreateWindowExW(0, L"STATIC", L"Lone Shooter", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 15, 400, 35, hwnd, NULL, NULL, NULL);
                SendMessage(hTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            }
            
            CreateWindowExW(0, L"STATIC", L"Resolution:", WS_CHILD | WS_VISIBLE, 30, 70, 100, 25, hwnd, NULL, NULL, NULL);
            HWND hCombo = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 140, 67, 220, 200, hwnd, (HMENU)IDC_RESOLUTION_COMBO, NULL, NULL);
            SendMessage(hCombo, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            int screenW = GetSystemMetrics(SM_CXSCREEN);
            int screenH = GetSystemMetrics(SM_CYSCREEN);
            for (int i = 0; i < g_NumResolutions; i++) {
                if (g_Resolutions[i].w <= screenW && g_Resolutions[i].h <= screenH) {
                    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)g_Resolutions[i].name);
                }
            }
            
            int comboIdx = 0;
            for (int i = 0; i < g_NumResolutions; i++) {
                if (g_Resolutions[i].w <= screenW && g_Resolutions[i].h <= screenH) {
                    if (i == g_SelectedResolution) {
                        SendMessage(hCombo, CB_SETCURSEL, comboIdx, 0);
                        break;
                    }
                    comboIdx++;
                }
            }
            
            CreateWindowExW(0, L"STATIC", L"Mouse Sensitivity:", WS_CHILD | WS_VISIBLE, 30, 110, 140, 25, hwnd, NULL, NULL, NULL);
            HWND hSlider = CreateWindowExW(0, TRACKBAR_CLASSW, NULL, WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 170, 107, 150, 30, hwnd, (HMENU)IDC_SENSITIVITY_SLIDER, NULL, NULL);
            SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
            SendMessage(hSlider, TBM_SETPOS, TRUE, (int)(g_MouseSensitivity * 10));
            SendMessage(hSlider, TBM_SETTICFREQ, 2, 0);
            
            wchar_t sensText[32];
            swprintf(sensText, 32, L"%.1f", g_MouseSensitivity);
            HWND hSensLabel = CreateWindowExW(0, L"STATIC", sensText, WS_CHILD | WS_VISIBLE | SS_CENTER, 325, 112, 40, 20, hwnd, (HMENU)IDC_SENSITIVITY_LABEL, NULL, NULL);
            SendMessage(hSensLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hFullscreen = CreateWindowExW(0, L"BUTTON", L"Fullscreen", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 150, 150, 25, hwnd, (HMENU)IDC_FULLSCREEN_CHECK, NULL, NULL);
            SendMessage(hFullscreen, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hFullscreen, BM_SETCHECK, g_FullscreenMode ? BST_CHECKED : BST_UNCHECKED, 0);
            
            HWND hConsole = CreateWindowExW(0, L"BUTTON", L"Developer Console", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 200, 150, 180, 25, hwnd, (HMENU)IDC_CONSOLE_CHECK, NULL, NULL);
            SendMessage(hConsole, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hConsole, BM_SETCHECK, g_DevConsole ? BST_CHECKED : BST_UNCHECKED, 0);

            HWND hVHS = CreateWindowExW(0, L"BUTTON", L"VHS Filter", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 185, 150, 25, hwnd, (HMENU)IDC_VHS_CHECK, NULL, NULL);
            SendMessage(hVHS, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hVHS, BM_SETCHECK, g_EnableVHS ? BST_CHECKED : BST_UNCHECKED, 0);

            HWND hPerf = CreateWindowExW(0, L"BUTTON", L"Performance Mode", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 200, 185, 180, 25, hwnd, (HMENU)IDC_PERFORMANCE_CHECK, NULL, NULL);
            SendMessage(hPerf, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hPerf, BM_SETCHECK, g_PerformanceMode ? BST_CHECKED : BST_UNCHECKED, 0);
            
            CreateWindowExW(0, L"STATIC", L"Render Distance:", WS_CHILD | WS_VISIBLE, 30, 220, 140, 25, hwnd, NULL, NULL, NULL);
            HWND hRenderSlider = CreateWindowExW(0, TRACKBAR_CLASSW, NULL, WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 170, 217, 150, 30, hwnd, (HMENU)IDC_RENDERDIST_SLIDER, NULL, NULL);
            SendMessage(hRenderSlider, TBM_SETRANGE, TRUE, MAKELPARAM(10, 30));
            SendMessage(hRenderSlider, TBM_SETPOS, TRUE, (int)g_RenderDistance);
            SendMessage(hRenderSlider, TBM_SETTICFREQ, 5, 0);
            
            wchar_t distText[32];
            swprintf(distText, 32, L"%.0f", g_RenderDistance);
            HWND hDistLabel = CreateWindowExW(0, L"STATIC", distText, WS_CHILD | WS_VISIBLE | SS_CENTER, 325, 222, 40, 20, hwnd, (HMENU)IDC_RENDERDIST_LABEL, NULL, NULL);
            SendMessage(hDistLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            wchar_t verText[64];
            swprintf(verText, 64, L"Version: %ls", g_GameVersion);
            HWND hVersion = CreateWindowExW(0, L"STATIC", verText, WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 260, 400, 20, hwnd, (HMENU)IDC_VERSION_LABEL, NULL, NULL);
            SendMessage(hVersion, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hContinue = CreateWindowExW(0, L"BUTTON", L"Continue", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 30, 295, 100, 40, hwnd, (HMENU)IDC_CONTINUE_BUTTON, NULL, NULL);
            SendMessage(hContinue, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hPlay = CreateWindowExW(0, L"BUTTON", L"Play", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 150, 295, 100, 40, hwnd, (HMENU)IDC_PLAY_BUTTON, NULL, NULL);
            SendMessage(hPlay, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND hCancel = CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 295, 100, 40, hwnd, (HMENU)IDC_CANCEL_BUTTON, NULL, NULL);
            SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            HWND* children = new HWND[10];
            children[0] = GetDlgItem(hwnd, IDC_RESOLUTION_COMBO);
            return 0;
        }
        case WM_HSCROLL: {
            if ((HWND)lParam == GetDlgItem(hwnd, IDC_SENSITIVITY_SLIDER)) {
                int pos = (int)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
                g_MouseSensitivity = pos / 10.0f;
                wchar_t sensText[32];
                swprintf(sensText, 32, L"%.1f", g_MouseSensitivity);
                SetDlgItemTextW(hwnd, IDC_SENSITIVITY_LABEL, sensText);
            }
            if ((HWND)lParam == GetDlgItem(hwnd, IDC_RENDERDIST_SLIDER)) {
                int pos = (int)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
                g_RenderDistance = (float)pos;
                wchar_t distText[32];
                swprintf(distText, 32, L"%d", pos);
                SetDlgItemTextW(hwnd, IDC_RENDERDIST_LABEL, distText);
            }
            return 0;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            if (wmId == IDC_PLAY_BUTTON || wmId == IDC_CONTINUE_BUTTON) {
                HWND hCombo = GetDlgItem(hwnd, IDC_RESOLUTION_COMBO);
                int sel = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                if (sel != CB_ERR) {
                    int screenW = GetSystemMetrics(SM_CXSCREEN);
                    int screenH = GetSystemMetrics(SM_CYSCREEN);
                    int idx = 0;
                    for (int i = 0; i < g_NumResolutions; i++) {
                        if (g_Resolutions[i].w <= screenW && g_Resolutions[i].h <= screenH) {
                            if (idx == sel) {
                                SCREEN_WIDTH = g_Resolutions[i].w;
                                SCREEN_HEIGHT = g_Resolutions[i].h;
                                g_SelectedResolution = i;
                                break;
                            }
                            idx++;
                        }
                    }
                }
                
                g_FullscreenMode = (SendMessage(GetDlgItem(hwnd, IDC_FULLSCREEN_CHECK), BM_GETCHECK, 0, 0) == BST_CHECKED);
                g_DevConsole = (SendMessage(GetDlgItem(hwnd, IDC_CONSOLE_CHECK), BM_GETCHECK, 0, 0) == BST_CHECKED);
                g_EnableVHS = (SendMessage(GetDlgItem(hwnd, IDC_VHS_CHECK), BM_GETCHECK, 0, 0) == BST_CHECKED);
                g_PerformanceMode = (SendMessage(GetDlgItem(hwnd, IDC_PERFORMANCE_CHECK), BM_GETCHECK, 0, 0) == BST_CHECKED);
                
                SaveSettingsJSON();
                g_SettingsConfirmed = true;
                if (wmId == IDC_CONTINUE_BUTTON) g_LoadGameRequested = true;
                DestroyWindow(hwnd);
            } else if (wmId == IDC_CANCEL_BUTTON) {
                g_SettingsConfirmed = false;
                DestroyWindow(hwnd);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (hBannerBmp && bannerW > 0 && bannerH > 0) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBannerBmp);
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int scaledW = clientRect.right;
                int scaledH = scaledW * bannerH / bannerW;
                SetStretchBltMode(hdc, HALFTONE);
                StretchBlt(hdc, 0, 5, scaledW, scaledH, hdcMem, 0, 0, bannerW, bannerH, SRCCOPY);
                SelectObject(hdcMem, hOldBmp);
                DeleteDC(hdcMem);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            if (hBannerBmp) {
                DeleteObject(hBannerBmp);
                hBannerBmp = NULL;
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool ShowSettingsMenu(HINSTANCE hInstance) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);
    
    LoadSettingsJSON();
    
    WNDCLASSEXW wcSettings = {};
    wcSettings.cbSize = sizeof(wcSettings);
    wcSettings.style = CS_HREDRAW | CS_VREDRAW;
    wcSettings.lpfnWndProc = SettingsDialogProc;
    wcSettings.hInstance = hInstance;
    wcSettings.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcSettings.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcSettings.lpszClassName = L"LoneShooterSettingsClass";
    RegisterClassExW(&wcSettings);
    
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int dialogW = 400;
    int dialogH = 400;
    int posX = (screenW - dialogW) / 2;
    int posY = (screenH - dialogH) / 2;
    
    g_hSettingsDialog = CreateWindowExW(WS_EX_DLGMODALFRAME, L"LoneShooterSettingsClass", L"Lone Shooter",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        posX, posY, dialogW, dialogH,
        NULL, NULL, hInstance, NULL);
    
    ShowWindow(g_hSettingsDialog, SW_SHOW);
    UpdateWindow(g_hSettingsDialog);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return g_SettingsConfirmed;
}

volatile bool musicRunning = true;
extern bool bossActive;
extern bool preBossPhase;
HMIDIOUT hMidiOut;

void MidiMsg(DWORD msg) {
    midiOutShortMsg(hMidiOut, msg);
}

void NoteOn(int ch, int note, int vel) {
    MidiMsg(0x90 | ch | (note << 8) | (vel << 16));
}

void NoteOff(int ch, int note) {
    MidiMsg(0x80 | ch | (note << 8));
}

void SetInstrument(int ch, int instr) {
    MidiMsg(0xC0 | ch | (instr << 8));
}

void SetVolume(int ch, int vol) {
    MidiMsg(0xB0 | ch | (7 << 8) | (vol << 16));
}

void InitAudio() {
    midiOutOpen(&hMidiOut, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
    
    // Mix Volumes
    SetVolume(0, 85);  // Music Guitar (Lower)
    SetVolume(1, 100); // Music Bass
    SetVolume(2, 127); // GUN (Max)
    SetInstrument(2, 127); // Gunshot
    SetVolume(3, 127); // Score (Max)
    SetInstrument(3, 112); // Tinkle Bell (Better than Glock?)
    SetVolume(9, 127); // Drums (Max)
    
    SetInstrument(0, 30); // Distortion Guitar (More sustain)
    SetInstrument(1, 33); // Fingered Bass
}

void CleanupAudio() {
    midiOutReset(hMidiOut);
    midiOutClose(hMidiOut);
}

// Bazooka Sounds
void PlayBazookaFireSound() {
    static wchar_t path[MAX_PATH] = {0};
    static bool opened = false;
    if (path[0] == 0) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        swprintf(path, MAX_PATH, L"\"%ls\\assets\\sound-effects\\bazooka_firing.mp3\"", exePath);
    }
    if (!opened) {
        wchar_t cmd[512];
        swprintf(cmd, 512, L"open %ls type mpegvideo alias bazookafire", path);
        mciSendStringW(cmd, NULL, 0, NULL);
        mciSendStringW(L"setaudio bazookafire volume to 1000", NULL, 0, NULL);
        opened = true;
    }
    mciSendStringW(L"play bazookafire from 0", NULL, 0, NULL);
}

void PlayBazookaExplosionSound() {
    static wchar_t path[MAX_PATH] = {0};
    static bool opened = false;
    if (path[0] == 0) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        swprintf(path, MAX_PATH, L"\"%ls\\assets\\sound-effects\\bazooka_explosion.mp3\"", exePath);
    }
    if (!opened) {
        wchar_t cmd[512];
        swprintf(cmd, 512, L"open %ls type mpegvideo alias bazookaexp", path);
        mciSendStringW(cmd, NULL, 0, NULL);
        mciSendStringW(L"setaudio bazookaexp volume to 1000", NULL, 0, NULL);
        opened = true;
    }
    mciSendStringW(L"play bazookaexp from 0", NULL, 0, NULL);
}

void PlayGunSound(int type = 0) {
    if (type == 2) { // Bazooka
        PlayBazookaFireSound();
    } else if (type == 1) { // Shotgun
        NoteOn(2, 41, 127); // Lower Gunshot
        NoteOn(9, 36, 127); // Kick
        NoteOn(9, 57, 127); // Crash Cymbal
    } else { // Pistol
        NoteOn(2, 45, 127); // Low Gunshot
        NoteOn(9, 36, 127); // Kick
        NoteOn(9, 57, 127); // Crash Cymbal
    }
}

void PlayReloadSound(int stage) {
    if (stage == 0) NoteOn(9, 37, 100); // Side Stick (Click out)
    if (stage == 1) NoteOn(9, 75, 90);  // Claves (Click in)
    if (stage == 2) NoteOn(9, 39, 100); // Hand Clap (Slide/Slap)
}

void PlayStepSound() {
    NoteOn(9, 42, 40); // Quiet Hi-Hat
}

void PlayScoreSound() {
    NoteOn(3, 84, 127);
}

void PlayHealSound() {
    NoteOn(3, 72, 127);
    NoteOn(3, 76, 127);
    NoteOn(3, 79, 127);
}

void PlaySlamSound() {
    static wchar_t slamPath[MAX_PATH] = {0};
    if (slamPath[0] == 0) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        swprintf(slamPath, MAX_PATH, L"\"%ls\\assets\\sound-effects\\claw-impact.mp3\"", exePath);
    }
    
    wchar_t cmd[512];
    mciSendStringW(L"close slamsfx", NULL, 0, NULL);
    swprintf(cmd, 512, L"open %ls type mpegvideo alias slamsfx", slamPath);
    mciSendStringW(cmd, NULL, 0, NULL);
    mciSendStringW(L"setaudio slamsfx volume to 1000", NULL, 0, NULL);
    mciSendStringW(L"play slamsfx from 0", NULL, 0, NULL);
}

void PlayMarshallAttackSound() {
    static wchar_t mashPath[MAX_PATH] = {0};
    if (mashPath[0] == 0) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        swprintf(mashPath, MAX_PATH, L"\"%ls\\assets\\sound-effects\\hammer-effect.mp3\"", exePath);
    }
    
    wchar_t cmd[512];
    mciSendStringW(L"close mashsfx", NULL, 0, NULL);
    swprintf(cmd, 512, L"open %ls type mpegvideo alias mashsfx", mashPath);
    mciSendStringW(cmd, NULL, 0, NULL);
    mciSendStringW(L"setaudio mashsfx volume to 1000", NULL, 0, NULL);
    mciSendStringW(L"play mashsfx from 0", NULL, 0, NULL);
}

void PlayPlayerHurtSound() {
    static wchar_t hurtPath[MAX_PATH] = {0};
    static bool opened = false;
    if (hurtPath[0] == 0) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        swprintf(hurtPath, MAX_PATH, L"\"%ls\\assets\\sound-effects\\player_hurt.mp3\"", exePath);
    }
    
    if (!opened) {
        wchar_t cmd[512];
        swprintf(cmd, 512, L"open %ls type mpegvideo alias hurtsfx", hurtPath);
        mciSendStringW(cmd, NULL, 0, NULL);
        mciSendStringW(L"setaudio hurtsfx volume to 1000", NULL, 0, NULL);
        opened = true;
    }
    mciSendStringW(L"play hurtsfx from 0", NULL, 0, NULL);
}

int enemyHurtSoundIndex = 0;
wchar_t enemyHurtPaths[3][MAX_PATH] = {0};

void PlayEnemyHurtSound() {
    static bool initialized = false;
    if (!initialized) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        for (int i = 0; i < 3; i++) {
            swprintf(enemyHurtPaths[i], MAX_PATH, L"%ls\\assets\\sound-effects\\enemy_hurt%d.wav", exePath, i + 1);
        }
        initialized = true;
    }
    
    enemyHurtSoundIndex = (enemyHurtSoundIndex + 1) % 3;
    PlaySoundW(enemyHurtPaths[enemyHurtSoundIndex], NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

wchar_t marshallHurtPath[MAX_PATH] = {0};

void PlayMarshallHurtSound() {
    static bool initialized = false;
    if (!initialized) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) *lastSlash = L'\0';
        swprintf(marshallHurtPath, MAX_PATH, L"%ls\\assets\\sound-effects\\marshall_hurt.wav", exePath);
        initialized = true;
    }
    
    PlaySoundW(marshallHurtPath, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

void PlayEnemyFireSound() {
    static wchar_t path[MAX_PATH] = {0};
    static int currentAlias = 0;
    if (path[0] == 0) {
        wchar_t exePath[MAX_PATH]; GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\'); if (lastSlash) *lastSlash = L'\0';
        swprintf(path, MAX_PATH, L"\"%ls\\assets\\sound-effects\\gunshot-enemy.mp3\"", exePath);
        
        // Initialize pool
        for (int i = 0; i < 8; i++) {
            wchar_t cmd[512];
            swprintf(cmd, 512, L"open %ls type mpegvideo alias enemyfire%d", path, i);
            mciSendStringW(cmd, NULL, 0, NULL);
            swprintf(cmd, 512, L"setaudio enemyfire%d volume to 1000", i);
            mciSendStringW(cmd, NULL, 0, NULL);
        }
    }
    
    wchar_t playCmd[256];
    swprintf(playCmd, 256, L"play enemyfire%d from 0", currentAlias);
    mciSendStringW(playCmd, NULL, 0, NULL);
    
    currentAlias = (currentAlias + 1) % 8;
}

void PlayOfficerWhistleSound() {
    static wchar_t path[MAX_PATH] = {0};
    if (path[0] == 0) {
        wchar_t exePath[MAX_PATH]; GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\'); if (lastSlash) *lastSlash = L'\0';
        swprintf(path, MAX_PATH, L"\"%ls\\assets\\sound-effects\\officer-whistle.mp3\"", exePath);
    }
    wchar_t cmd[512];
    mciSendStringW(L"close offwhistle", NULL, 0, NULL);
    swprintf(cmd, 512, L"open %ls type mpegvideo alias offwhistle", path);
    mciSendStringW(cmd, NULL, 0, NULL);
    mciSendStringW(L"setaudio offwhistle volume to 1000", NULL, 0, NULL);
    mciSendStringW(L"play offwhistle from 0", NULL, 0, NULL);
}

void PlayOfficerCommandSound() {
    static wchar_t path[MAX_PATH] = {0};
    if (path[0] == 0) {
        wchar_t exePath[MAX_PATH]; GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\'); if (lastSlash) *lastSlash = L'\0';
        swprintf(path, MAX_PATH, L"\"%ls\\assets\\sound-effects\\officer-command.wav\"", exePath);
    }
    wchar_t cmd[512];
    mciSendStringW(L"close offcmd", NULL, 0, NULL);
    swprintf(cmd, 512, L"open %ls type waveaudio alias offcmd", path);
    mciSendStringW(cmd, NULL, 0, NULL);
    mciSendStringW(L"setaudio offcmd volume to 1000", NULL, 0, NULL);
    mciSendStringW(L"play offcmd from 0", NULL, 0, NULL);
}

void PlayOfficerRetreatSound() {
    static wchar_t path[MAX_PATH] = {0};
    if (path[0] == 0) {
        wchar_t exePath[MAX_PATH]; GetModuleFileNameW(NULL, exePath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(exePath, L'\\'); if (lastSlash) *lastSlash = L'\0';
        swprintf(path, MAX_PATH, L"\"%ls\\assets\\sound-effects\\officer-retreat.wav\"", exePath);
    }
    wchar_t cmd[512];
    mciSendStringW(L"close offret", NULL, 0, NULL);
    swprintf(cmd, 512, L"open %ls type waveaudio alias offret", path);
    mciSendStringW(cmd, NULL, 0, NULL);
    mciSendStringW(L"setaudio offret volume to 1000", NULL, 0, NULL);
    mciSendStringW(L"play offret from 0", NULL, 0, NULL);
}

void BackgroundMusic(void* arg) {
    const int E2 = 40;
    const int E3 = 52; 
    const int D3 = 50;
    const int C3 = 48;
    const int B2 = 47;
    const int AS2 = 46;
    const int A2 = 45;

    while (musicRunning) {
        if (preBossPhase) {
            // Silence during buildup
            Sleep(100);
            continue;
        }
        if (bossActive) {
            // Scary Boss Music: Low drones, dissonant chords, fast tempo
            SetInstrument(0, 30); // Distortion Guitar
            SetInstrument(1, 32); // Acoustic Bass
            
            // Minor 2nd drone - very unsettling
            NoteOn(1, 28, 100); // E1 low drone
            NoteOn(1, 29, 80);  // F1 - dissonant with E
            
            for (int i = 0; i < 8 && musicRunning && bossActive; i++) {
                // Staccato power chords descending chromatically
                int note = E3 - i;
                NoteOn(0, note, 120);
                NoteOn(0, note + 6, 120); // Tritone - devil's interval
                Sleep(100);
                NoteOff(0, note);
                NoteOff(0, note + 6);
                
                // Drum hits
                NoteOn(9, 36, 127); // Kick
                Sleep(100);
            }
            
            NoteOff(1, 28);
            NoteOff(1, 29);
            
            // Crash and rebuild tension
            NoteOn(9, 49, 127); // Crash
            NoteOn(9, 38, 127); // Snare
            Sleep(200);
        } else {
            // Normal Action Music
            int riff[] = { E2, E3, E2, D3, E2, C3, E2, AS2, E2, B2, E2 };
            
            NoteOn(1, E2-12, 100);

            for (int i = 0; i < 11; i++) {
                if (!musicRunning || bossActive) break;
                int note = riff[i];
                
                NoteOn(0, note, 110);
                NoteOn(0, note + 7, 110);
                
                Sleep(150);
                
                NoteOff(0, note);
                NoteOff(0, note + 7);
                
                if (i < 10) {
                     NoteOn(0, E2, 80);
                     NoteOn(0, E2+7, 80);
                     Sleep(150);
                     NoteOff(0, E2);
                     NoteOff(0, E2+7);
                }
            }
            
            NoteOn(9, 38, 127);
            Sleep(150);
            NoteOn(9, 38, 127);
            NoteOn(9, 49, 127);
            Sleep(150);
            
            NoteOff(1, E2-12);
        }
    }
}

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;
bool g_FullscreenMode = true;
bool g_DevConsole = false;
float g_MouseSensitivity = 1.0f;
wchar_t g_GameVersion[32] = L"1.0";
bool g_PauseMenuOpen = false;
const int MAP_WIDTH = 64;
const int MAP_HEIGHT = 64;
const float PI = 3.14159265f;
const float FOV = PI / 3.0f;

const int TRIG_TABLE_SIZE = 4096;
float sinTable[TRIG_TABLE_SIZE];
float cosTable[TRIG_TABLE_SIZE];

void InitTrigTables() {
    for (int i = 0; i < TRIG_TABLE_SIZE; i++) {
        float angle = (float)i / TRIG_TABLE_SIZE * 2.0f * PI;
        sinTable[i] = sinf(angle);
        cosTable[i] = cosf(angle);
    }
}

// Global GDI Objects
HBRUSH hBrushMapBG;
HBRUSH hBrushWall1; // RGB(0, 80, 0)
HBRUSH hBrushWall2; // RGB(100, 60, 30)
HBRUSH hBrushWall3; // RGB(40, 60, 30)
HBRUSH hBrushPlayer; // RGB(0, 255, 0)
HBRUSH hBrushSpire; // RGB(255, 165, 0)
HBRUSH hBrushMedkit; // RGB(0, 150, 255)
HBRUSH hBrushBlack; 
HBRUSH hBrushDarkGray;
HBRUSH hBrushRed;
HBRUSH hBrushGreen;
HBRUSH hBrushBlue;
HBRUSH hBrushWhite;
HBRUSH hBrushHollow;
HBRUSH hBrushDarkRed;
HBRUSH hBrushGold;
HBRUSH hBrushMagenta;
HBRUSH hBrushPurple;

HPEN hPenPlayer; // RGB(0, 255, 0)
HPEN hPenRange; // RGB(0, 255, 255) DOT
HPEN hPenFOV; // RGB(0, 200, 0)
HPEN hPenRed; 
HPEN hPenLaser; // RGB(255, 0, 0) Width 5
HPEN hPenWhite; // RGB(255, 255, 255) Width 2

HFONT hFontDebug;
HFONT hFontHUD;
HFONT hFontBig;
HFONT hFontSmall;
HFONT hFontMedium;
HFONT hFontTitle;
HFONT hFontPixel;

void InitGraphics() {
    hBrushMapBG = CreateSolidBrush(RGB(20, 20, 20));
    hBrushWall1 = CreateSolidBrush(RGB(0, 80, 0));
    hBrushWall2 = CreateSolidBrush(RGB(100, 60, 30));
    hBrushWall3 = CreateSolidBrush(RGB(40, 60, 30));
    hBrushPlayer = CreateSolidBrush(RGB(0, 255, 0));
    hBrushSpire = CreateSolidBrush(RGB(255, 165, 0));
    hBrushMedkit = CreateSolidBrush(RGB(0, 150, 255));
    
    hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
    hBrushDarkGray = CreateSolidBrush(RGB(40, 40, 40));
    hBrushRed = CreateSolidBrush(RGB(200, 0, 0));
    hBrushGreen = CreateSolidBrush(RGB(0, 180, 0));
    hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
    hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
    hBrushHollow = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    hBrushDarkRed = CreateSolidBrush(RGB(50, 0, 0));
    hBrushGold = CreateSolidBrush(RGB(150, 100, 0));
    hBrushMagenta = CreateSolidBrush(RGB(200, 0, 200));
    hBrushPurple = CreateSolidBrush(RGB(148, 0, 211));

    hPenPlayer = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    hPenRange = CreatePen(PS_DOT, 1, RGB(0, 255, 255));
    hPenFOV = CreatePen(PS_SOLID, 1, RGB(0, 200, 0));
    hPenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    hPenLaser = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
    hPenWhite = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));

    hFontDebug = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    hFontHUD = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    hFontBig = CreateFontW(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    hFontSmall = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    hFontMedium = CreateFontW(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    hFontTitle = CreateFontW(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    hFontPixel = CreateFontW(64, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"VCR OSD Mono");
    
    InitPostProcess();
}

void CleanupGraphics() {
    DeleteObject(hBrushMapBG);
    DeleteObject(hBrushWall1);
    DeleteObject(hBrushWall2);
    DeleteObject(hBrushWall3);
    DeleteObject(hBrushPlayer);
    DeleteObject(hBrushSpire);
    DeleteObject(hBrushMedkit);
    DeleteObject(hBrushBlack);
    DeleteObject(hBrushDarkGray);
    DeleteObject(hBrushRed);
    DeleteObject(hBrushGreen);
    DeleteObject(hBrushBlue);
    DeleteObject(hBrushWhite);
    DeleteObject(hBrushDarkRed);
    DeleteObject(hBrushGold);
    DeleteObject(hBrushMagenta);
    DeleteObject(hBrushPurple);
    
    DeleteObject(hPenPlayer);
    DeleteObject(hPenRange);
    DeleteObject(hPenFOV);
    DeleteObject(hPenRed);
    DeleteObject(hPenLaser);
    DeleteObject(hPenWhite);

    DeleteObject(hFontDebug);
    DeleteObject(hFontHUD);
    DeleteObject(hFontBig);
    DeleteObject(hFontSmall);
    DeleteObject(hFontMedium);
    DeleteObject(hFontTitle);
    DeleteObject(hFontPixel);
}

inline float FastSin(float angle) {
    while (angle < 0) angle += 2.0f * PI;
    while (angle >= 2.0f * PI) angle -= 2.0f * PI;
    int index = (int)(angle / (2.0f * PI) * TRIG_TABLE_SIZE) % TRIG_TABLE_SIZE;
    return sinTable[index];
}

inline float FastCos(float angle) {
    while (angle < 0) angle += 2.0f * PI;
    while (angle >= 2.0f * PI) angle -= 2.0f * PI;
    int index = (int)(angle / (2.0f * PI) * TRIG_TABLE_SIZE) % TRIG_TABLE_SIZE;
    return cosTable[index];
}

inline bool IsInFrustum(float spriteX, float spriteY, float playerX, float playerY, float playerAngle, float marginAngle) {
    float dx = spriteX - playerX;
    float dy = spriteY - playerY;
    float angle = atan2f(dy, dx) - playerAngle;
    while (angle > PI) angle -= 2 * PI;
    while (angle < -PI) angle += 2 * PI;
    return fabsf(angle) < marginAngle;
}

int worldMap[MAP_WIDTH][MAP_HEIGHT];

struct Player {
    float x, y;
    float angle;
    float pitch;
    int health;
    int maxHealth = 100;
    int level = 1;
    int xp = 0;
    int xpToNextLevel = 100;
};

struct Enemy {
    float x = 0.0f, y = 0.0f;
    float distance = 0.0f;
    bool active = false;
    float speed = 0.0f;
    int spriteIndex = 0;
    int health = 0;
    int maxHealth = 0;
    float hurtTimer = 0.0f;
    bool isShooter = false;
    float fireTimer = 0.0f;
    float firingTimer = 0.0f;
    bool isMarshall = false;
    int state = 0;
    float healTimer = 0.0f;
    float summonTimer = 0.0f;
    float attackTimer = 0.0f;
    int tacticState = 0;
    int flankDir = 0;
    float tacticTimer = 0.0f;
    std::vector<std::pair<int,int>> path;
    int pathIndex = 0;
    float pathRecalcTimer = 0.0f;
    NeuralAI::NeuralNet brain;
    bool hasNeuralBrain = false;
    int dodgeDir = 0;
    bool isPhalanx = false;
    bool isSpearGuy = false;
    int spearState = 0; // 0: Move, 1: Idle, 2: Dash, 3: Block
    float dashCooldown = 0.0f;
    float blockCooldown = 0.0f;
    float spearTimer = 0.0f;
    int dashDir = 0;

    bool isOfficer = false;
    bool isDefectedOfficer = false;
    bool isDefectedGunner = false;
    bool isParagon = false;
    int officerState = 0; // 0: Seek gunners, 1: Form Line, 2: Firing Volley, 3: Retreat
    int prevOfficerState = -1;
    bool isAllied = false;
    bool isEnemy = true;
    float officerCooldown = 0.0f;
    float targetX = 0, targetY = 0;
    bool hunting = false;
    int targetEnemyIndex = 0;
    int targetClawIndex = 0;
};

enum MarshallCommand { CMD_NONE, CMD_RALLY, CMD_PINCER, CMD_PHALANX };
MarshallCommand activeCommand = CMD_NONE;
bool militiaActive = false;
float militiaFormTimer = 0.0f;
int militiaCount = 0;
int militiaMaxCount = 0;
float militiaMessageTimer = 0.0f;
bool militiaBarActive = false;

// Officer globals
bool officerSpawned = false;
bool defectedOfficerActive = false;
float defectedRespawnTimer = 0.0f;
float playerKnockbackX = 0.0f;
float playerKnockbackY = 0.0f;

struct EnemyBullet {
    float x, y;
    float dirX, dirY;
    float speed;
    bool active;
    bool isLaser; // true = claw laser (10 dmg), false = enemy bullet (5 dmg)
};

struct TreeSprite {
    float x, y;
    float distance;
};

struct GrassSprite {
    float x, y;
};

struct RockSprite {
    float x, y;
    int variant;
};

struct BigRock {
    float x, y;
    int variant;
    float radius;
};

enum HealingTowerState { TOWER_DORMANT, TOWER_CHARGING, TOWER_READY, TOWER_ACTIVE, TOWER_COOLDOWN };

struct HealingTower {
    float x, y;
    HealingTowerState state;
    float timer;         // For animation (pulsing) and active duration
    float cooldownTimer; // For cooldown phase
    float animTimer;     // For sprite swapping
    int pulseFrame;      // 0 or 1 for charging sprites
    
    struct Particle {
        float angle;
        float dist;
        float height;
        float speed;
    };
    std::vector<Particle> particles;
};

struct JohnSecret {
    bool isSpawned;
    bool naturalSpawn;
    float angle;
    float x, y;
    float hopOffset;
    float hopTimer;
    bool isHopping;
    int fullCirclesCompleted;
    float prevAngle;
};

struct BushSprite {
    float x, y;
};

const int GRID_CELL_SIZE = 4;
const int GRID_WIDTH = (MAP_WIDTH + GRID_CELL_SIZE - 1) / GRID_CELL_SIZE;
const int GRID_HEIGHT = (MAP_HEIGHT + GRID_CELL_SIZE - 1) / GRID_CELL_SIZE;

std::vector<int> treeGrid[17][17];
std::vector<int> grassGrid[17][17];
std::vector<int> rockGrid[17][17];
std::vector<int> bushGrid[17][17];
std::vector<int> bigRockGrid[17][17];

struct Cloud {
    float x, y;
    float height;
    float speed;
};

struct Bullet {
    float x, y;
    float dirX, dirY;
    float speed;
    bool active;
    int damage;
    float startX, startY;
    float maxRange;
};

enum ClawState { CLAW_DORMANT, CLAW_IDLE, CLAW_CHASING, CLAW_SLAMMING, CLAW_RISING, CLAW_RETURNING, 
                 CLAW_PH2_AWAKEN, CLAW_PH2_DROPPING, CLAW_PH2_ANCHORED, CLAW_PH2_DEAD, CLAW_PH2_RISING };

struct Claw {
    float x, y;
    float homeX, homeY;
    float groundY;
    ClawState state;
    float timer;
    int index;
    bool dealtDamage;
    // Phase 2
    int health;
    int animFrame;
    float animTimer;
    bool hurt;
    float hurtTimer;
};

// --- 3D Engine Structs (Ported from LoneMaker) ---
struct Vec3 { float x, y, z; };
struct Mat4 { float m[4][4]; };
struct Vertex { Vec3 pos; };
struct Triangle { int p1, p2, p3; DWORD color; bool selected; };
struct Object3D {
    Vec3 pos;
    Vec3 rot;
    std::vector<Vertex> verts;
    std::vector<Triangle> tris;
};

// --- 3D Math ---
Vec3 Add(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
Vec3 Sub(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
Vec3 Mul(Vec3 v, float s) { return {v.x*s, v.y*s, v.z*s}; }
float Dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 Cross(Vec3 a, Vec3 b) { return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }
float Length(Vec3 v) { return sqrtf(Dot(v, v)); }
Vec3 Normalize(Vec3 v) { float l = Length(v); if(l==0) return {0,0,0}; return Mul(v, 1.0f/l); }

Mat4 MatrixIdentity() {
    Mat4 mat = {0};
    mat.m[0][0] = 1; mat.m[1][1] = 1; mat.m[2][2] = 1; mat.m[3][3] = 1;
    return mat;
}
Mat4 MatrixRotationY(float angle) {
    Mat4 mat = MatrixIdentity();
    mat.m[0][0] = cosf(angle); mat.m[0][2] = -sinf(angle);
    mat.m[2][0] = sinf(angle); mat.m[2][2] = cosf(angle);
    return mat;
}
Mat4 MatrixRotationX(float angle) {
    Mat4 mat = MatrixIdentity();
    mat.m[1][1] = cosf(angle); mat.m[1][2] = -sinf(angle);
    mat.m[2][1] = sinf(angle); mat.m[2][2] = cosf(angle);
    return mat;
}
Mat4 MatrixTranslation(float x, float y, float z) {
    Mat4 mat = MatrixIdentity();
    mat.m[3][0] = x; mat.m[3][1] = y; mat.m[3][2] = z;
    return mat;
}
Mat4 MatrixPerspective(float fov, float aspect, float znear, float zfar) {
    Mat4 mat = {0};
    float tanHalf = tanf(fov / 2.0f);
    mat.m[0][0] = 1.0f / (aspect * tanHalf);
    mat.m[1][1] = 1.0f / tanHalf;
    mat.m[2][2] = zfar / (zfar - znear);
    mat.m[2][3] = 1.0f;
    mat.m[3][2] = (-zfar * znear) / (zfar - znear);
    return mat;
}
Mat4 MatrixMultiply(Mat4 a, Mat4 b) {
    Mat4 c = {0};
    for(int i=0; i<4; i++) for(int j=0; j<4; j++) for(int k=0; k<4; k++)
        c.m[i][j] += a.m[i][k] * b.m[k][j];
    return c;
}
Vec3 TransformPoint(Mat4 m, Vec3 i) {
    Vec3 o;
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
    if (w != 0.0f) { o.x /= w; o.y /= w; o.z /= w; }
    return o;
}
float EdgeFunc(int x1, int y1, int x2, int y2, int px, int py) {
    return (float)((px - x1) * (y2 - y1) - (py - y1) * (x2 - x1));
}

// Spawn Player at (10, 32) facing East (0.0) towards center (32, 32)

struct Fireball {
    float x, y;
    float dirX, dirY;
    float speed;
    bool active;
};

struct Rocket {
    float x, y;
    float dirX, dirY;
    float speed;
    bool active;
    bool isEnemy;
    float z;
    float verticalSpeed;
    float targetX, targetY; // Only used for enemy homing rockets
    float startX, startY;
    float maxRange;
    float safetyTimer;
};

struct RocketTrail {
    float x, y;
    float life; // Fades out
    bool active;
};

struct Explosion {
    float x, y;
    float timer; // Duration of explosion anim
    bool active;
};

struct Medkit {
    float x, y;
    bool active;
    float respawnTimer;
    static const float RESPAWN_TIME;
    static const int HEAL_AMOUNT = 25;
};
const float Medkit::RESPAWN_TIME = 10.0f;

Player player = {10.0f, 32.0f, 0.0f, 0.0f, 100};

void GivePlayerXP(int amount) {
    player.xp += amount;
    while (player.xp >= player.xpToNextLevel) {
        player.xp -= player.xpToNextLevel;
        player.level++;
        player.xpToNextLevel += 20;
        g_PendingUpgrades++;
    }
    if (g_PendingUpgrades > 0 && !g_LevelUpWindowOpen) {
        g_LevelUpWindowOpen = true;
    }
}

int GetEnemyXP(const Enemy& e) {
    if (e.isMarshall) return 200;
    if (e.isSpearGuy) return 15;
    if (e.isShooter) return 10;
    if (e.spriteIndex == 4) return 15;
    return 5;
}
std::vector<Enemy> enemies;
std::vector<Enemy> pendingEnemies;
std::vector<TreeSprite> trees;
std::vector<GrassSprite> grasses;
std::vector<RockSprite> rocks;
std::vector<BigRock> bigRocks;
std::vector<BushSprite> bushes;

std::vector<Cloud> clouds;
std::vector<Bullet> bullets;
std::vector<Fireball> fireballs;
std::vector<EnemyBullet> enemyBullets;
Medkit medkits[3] = {{0, 0, false, 0}, {0, 0, false, 0}, {0, 0, false, 0}};
float healFlashTimer = 0;

bool bossActive = false;
bool preBossPhase = false;
float preBossTimer = 0;
float bossEventTimer = 0;
float fireballSpawnTimer = 0;
int bossHealth = 1500;
float bossHurtTimer = 0;
float playerHurtTimer = 0;
bool bossDead = false;
bool victoryScreen = false;
float screenShakeTimer = 0;
float screenShakeIntensity = 0;
float shooterSpawnTimer = 3.0f;
float bossSpawnTimer = 0;

int maxMeleeSpawn = 3;
int maxShooterSpawn = 1;
float spawnCapTimer = 20.0f;
const int MELEE_CAP = 15;
const int SHOOTER_CAP = 5;

bool phase2Active = false;
bool forceFieldActive = false;
bool enragedMode = false;
int phase2BossFrame = 0;
float phase2BossAnimTimer = 0;
DWORD* spirePhase2Pixels[3];
int spirePhase2W[3], spirePhase2H[3];

DWORD* clawPhase2Pixels[4];
int clawPhase2W[4], clawPhase2H[4];
DWORD* clawHurtPixels = nullptr;
int clawHurtW = 0, clawHurtH = 0;

int activeLaserClaw = -1;
int lastActiveClaw = 5; // For sequential ordering
float laserTimer = 0;

DWORD* laserPixels = nullptr;
int laserW = 0, laserH = 0;

enum GravitalState { GRAVITAL_CHASE, GRAVITAL_SLAM, GRAVITAL_RECOVER };
struct Gravital {
    float x, y, z;
    float targetX, targetY;
    bool active;
    int health;
    float hurtTimer;
    GravitalState state;
    float slamTimer;
    float animTimer;
    int animFrame;
};
std::vector<Gravital> gravitals;

DWORD* gravitalPixels = nullptr;
int gravitalW = 0, gravitalH = 0;
DWORD* gravitalHurtPixels = nullptr;
int gravitalHurtW = 0, gravitalHurtH = 0;

int playerDamage = 1;
bool godMode = false;
bool marshallSpawned = false;
DWORD* marshallPixels = nullptr;
int marshallW = 0, marshallH = 0;
DWORD* marshallHurtPixels = nullptr;
int marshallHurtW = 0, marshallHurtH = 0;
// Marshall UI
bool marshallHealthBarActive = false;
int marshallHP = 0;
int marshallMaxHP = 100;

std::vector<Enemy> allies;
std::vector<Enemy> pendingAllies;
bool marshallKilled = false;
bool paragonsUnlocked = false;
float paragonMessageTimer = 0;
float gunRecoil = 0;
DWORD* paragonPixels = nullptr;
int paragonW = 0, paragonH = 0;
DWORD* paragonHurtPixels = nullptr;
int paragonHurtW = 0, paragonHurtH = 0;
float paragonSummonCooldown = 0;

bool consoleActive = false;
std::wstring consoleBuffer = L"";
bool showStats = false;
int fpsCounter = 0;
int currentFPS = 0;
DWORD fpsLastTime = 0;

wchar_t errorMessage[256] = L"";
float errorTimer = 0;
wchar_t consoleError[256] = L"";
std::vector<std::wstring> missingAssets;
bool assetsFolderMissing = false;

std::vector<Object3D> scene3D;
float* zBuffer = nullptr;

bool bazookaUnlocked = false;
std::vector<Rocket> rockets;
std::vector<RocketTrail> rocketTrails;
std::vector<Explosion> explosions;

bool postBossPhase = false;
DialogueSystem::DialogueController dialogueController;
DialogueSystem::DialogueController playerDialogueController;
NPCSystem::NPC* currentTalkingNPC = nullptr;
float whiteFadeTimer = 0;
bool whiteFadeToVictory = false;

bool playerNearGate = false;
bool gateDialogueActive = false;

DWORD* leaderIdlePixels = nullptr;
int leaderIdleW = 0, leaderIdleH = 0;
DWORD* leaderTalkingPixels = nullptr;
int leaderTalkingW = 0, leaderTalkingH = 0;
DWORD* followerPixels = nullptr;
int followerW = 0, followerH = 0;

bool spectatorMode = false;
float spectatorX = 0, spectatorY = 0, spectatorAngle = 0, spectatorPitch = 0;
float savedPlayerX = 0, savedPlayerY = 0, savedPlayerAngle = 0;

DWORD* playerSpritePixels = nullptr;
int playerSpriteW = 0, playerSpriteH = 0;

struct Grave {
    float x, y;
};
std::vector<Grave> graves;
DWORD* gravePixels = nullptr;
int graveW = 0, graveH = 0;

DWORD* compassPixels = nullptr;
int compassW = 0, compassH = 0;

HealingTower healingTower;
DWORD* htDormantPixels = nullptr;
int htDormantW = 0, htDormantH = 0;
DWORD* htChargingPixels[2] = {nullptr, nullptr};
int htChargingW[2] = {0, 0}, htChargingH[2] = {0, 0};
DWORD* htReadyPixels = nullptr;
int htReadyW = 0, htReadyH = 0;
DWORD* htParticlePixels = nullptr;
int htParticleW = 0, htParticleH = 0;

JohnSecret johnSecret;
DWORD* johnDefaultPixels = nullptr; int johnDefaultW = 0, johnDefaultH = 0;
DWORD* johnInteractPixels = nullptr; int johnInteractW = 0, johnInteractH = 0;

// Prototypes
void LoadModelCurrentDir(const wchar_t* filename, float x, float z);
void Render3DScene();

void ShowError(const wchar_t* msg) {
    wcscpy(errorMessage, msg);
    errorTimer = 3.0f;
}

float gunSwayX = 0, gunSwayY = 0;
float gunSwayPhase = 0;
bool isFiring = false;
float fireTimer = 0;
bool isMoving = false;

int ammo = 8;
int maxAmmo = 8;
int weaponAmmo[3] = {8, 5, 4}; // Pistol, Shotgun, Bazooka
int weaponMaxAmmo[3] = {8, 5, 4};
bool isReloading = false;
float reloadTimer = 0;
float reloadDuration = 3.0f;
float gunReloadOffset = 0;
int reloadStage = 0;

int score = 0;
float scoreTimer = 0;
wchar_t scoreMsg[64] = L"";
const wchar_t* praiseMsgs[] = {L"Nice Shot!", L"Damn Son", L"Daddy Chill"};

int highScore = 0;

bool hordeActive = false;
float hordeMessageTimer = 0;

bool viewRange = false;
int currentWeapon = 0;
bool gunUpgraded = false;
float upgradeMessageTimer = 0;

void EnsureAppDataFolder(wchar_t* outPath) {
    wchar_t appData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appData))) {
        swprintf(outPath, MAX_PATH, L"%ls\\LoneShooter", appData);
        CreateDirectoryW(outPath, NULL);
    } else {
        // Fallback to old behavior if AppData can't be resolved
        GetModuleFileNameW(NULL, outPath, MAX_PATH);
        wchar_t* lastBackSlash = wcsrchr(outPath, L'\\');
        wchar_t* lastForwardSlash = wcsrchr(outPath, L'/');
        wchar_t* lastSlash = lastBackSlash;
        if (lastForwardSlash && (!lastSlash || lastForwardSlash > lastSlash)) lastSlash = lastForwardSlash;
        if (lastSlash) *lastSlash = L'\0';
    }
}

void GetHighScorePath(wchar_t* path) {
    wchar_t basePath[MAX_PATH];
    EnsureAppDataFolder(basePath);
    swprintf(path, MAX_PATH, L"%ls\\highscore.dat", basePath);
}

void LoadHighScore() {
    wchar_t path[MAX_PATH];
    GetHighScorePath(path);
    FILE* f = _wfopen(path, L"rb");
    if (f) {
        fread(&highScore, sizeof(int), 1, f);
        fclose(f);
    }
}

void SaveHighScore() {
    wchar_t path[MAX_PATH];
    GetHighScorePath(path);
    FILE* f = _wfopen(path, L"wb");
    if (f) {
        fwrite(&highScore, sizeof(int), 1, f);
        fclose(f);
    }
}

void GetProgPath(wchar_t* path) {
    wchar_t basePath[MAX_PATH];
    EnsureAppDataFolder(basePath);
    swprintf(path, MAX_PATH, L"%ls\\prog.dat", basePath);
}

void LoadGraves() {
    wchar_t path[MAX_PATH];
    GetProgPath(path);
    FILE* f = _wfopen(path, L"rb");
    if (f) {
        int count = 0;
        fread(&count, sizeof(int), 1, f);
        for(int i=0; i<count; i++) {
            Grave g;
            fread(&g, sizeof(Grave), 1, f);
            graves.push_back(g);
        }
        fclose(f);
    }
}

void SaveGraves() {
    wchar_t path[MAX_PATH];
    GetProgPath(path);
    FILE* f = _wfopen(path, L"wb");
    if (f) {
        int count = (int)graves.size();
        fwrite(&count, sizeof(int), 1, f);
        for(const auto& g : graves) {
            fwrite(&g, sizeof(Grave), 1, f);
        }
        fclose(f);
    }
}

HWND hMainWnd;
DWORD* backBufferPixels = NULL;
HDC backBufferDC = NULL;
HBITMAP backBufferDIB = NULL;
HDC g_renderDC = NULL;
HBITMAP g_renderBitmap = NULL;
HBITMAP g_renderOldBitmap = NULL;

// --- UI System ---
DWORD* uiWindowPixels = nullptr; int uiWindowW = 0, uiWindowH = 0;
DWORD* uiButtonPixels = nullptr; int uiButtonW = 0, uiButtonH = 0;
DWORD* uiButtonHoverPixels = nullptr; int uiButtonHoverW = 0, uiButtonHoverH = 0;
DWORD* uiButtonPressedPixels = nullptr; int uiButtonPressedW = 0, uiButtonPressedH = 0;

struct UIState {
    int mouseX = 0;
    int mouseY = 0;
    bool mouseDown = false;
    bool mouseReleased = false;
};
UIState g_ui;

void DrawUI9Slice(DWORD* dest, int destW, int destH, DWORD* src, int srcW, int srcH, int dx, int dy, int dw, int dh, int cornerSize) {
    if (!src || srcW <= 0 || srcH <= 0 || !dest) return;
    for (int y = 0; y < dh; y++) {
        int dstY = dy + y;
        if (dstY < 0 || dstY >= destH) continue;
        
        int sy;
        if (y < cornerSize) sy = y;
        else if (y >= dh - cornerSize) sy = srcH - (dh - y);
        else sy = cornerSize + (y - cornerSize) * (srcH - 2 * cornerSize) / (dh - 2 * cornerSize);
        if (sy < 0) sy = 0; if (sy >= srcH) sy = srcH - 1;
        
        for (int x = 0; x < dw; x++) {
            int dstX = dx + x;
            if (dstX < 0 || dstX >= destW) continue;
            
            int sx;
            if (x < cornerSize) sx = x;
            else if (x >= dw - cornerSize) sx = srcW - (dw - x);
            else sx = cornerSize + (x - cornerSize) * (srcW - 2 * cornerSize) / (dw - 2 * cornerSize);
            if (sx < 0) sx = 0; if (sx >= srcW) sx = srcW - 1;
            
            DWORD col = src[sy * srcW + sx];
            if ((col & 0x00FFFFFF) != 0x00FF00FF) {
                dest[dstY * destW + dstX] = col;
            }
        }
    }
}

bool DoUIButton(int x, int y, int w, int h) {
    bool hovered = (g_ui.mouseX >= x && g_ui.mouseX <= x + w && g_ui.mouseY >= y && g_ui.mouseY <= y + h);
    bool clicked = false;
    
    DWORD* tex = uiButtonPixels;
    int texW = uiButtonW, texH = uiButtonH;
    
    if (hovered) {
        if (g_ui.mouseDown) {
            tex = uiButtonPressedPixels;
            texW = uiButtonPressedW; texH = uiButtonPressedH;
        } else {
            tex = uiButtonHoverPixels;
            texW = uiButtonHoverW; texH = uiButtonHoverH;
            if (g_ui.mouseReleased) {
                clicked = true;
            }
        }
    }
    
    DrawUI9Slice(backBufferPixels, SCREEN_WIDTH, SCREEN_HEIGHT, tex, texW, texH, x, y, w, h, 4);
    return clicked;
}


DWORD* grassPixels = NULL;
DWORD* npcPixels = NULL;

float marshallX = 0;
float marshallY = 0;
DWORD* treePixels = NULL;
DWORD* cloudPixels = NULL;
DWORD* gunPixels = NULL;
DWORD* gunfirePixels = NULL;
DWORD* bulletPixels = NULL;
DWORD* playerBulletPixels = NULL;
int playerBulletW = 0, playerBulletH = 0;
DWORD* spearguyMovePixels = NULL; int spearguyMoveW = 0, spearguyMoveH = 0;
DWORD* spearguyIdlePixels = NULL; int spearguyIdleW = 0, spearguyIdleH = 0;
DWORD* spearguyDashPixels = NULL; int spearguyDashW = 0, spearguyDashH = 0;
DWORD* spearguyBlockPixels = NULL; int spearguyBlockW = 0, spearguyBlockH = 0;
DWORD* spearguyHurtPixels = NULL; int spearguyHurtW = 0, spearguyHurtH = 0;
DWORD* healthbarPixels[11] = {NULL};
DWORD* xpBarPixels[11] = {NULL};
int xpBarW = 0, xpBarH = 0;
int grassW = 0, grassH = 0;
DWORD* enemyPixels[5] = {NULL};
int enemyW[5] = {0};
int enemyH[5] = {0};
DWORD* enemy5HurtPixels = NULL;
int enemy5HurtW = 0, enemy5HurtH = 0;
DWORD* gunnerPixels = NULL;
int gunnerW = 0, gunnerH = 0;
DWORD* gunnerFiringPixels = NULL;
int gunnerFiringW = 0, gunnerFiringH = 0;
DWORD* gunnerHurtPixels = NULL;
int gunnerHurtW = 0, gunnerHurtH = 0;

DWORD* officerMovePixels = NULL; int officerMoveW = 0, officerMoveH = 0;
DWORD* officerIdlePixels = NULL; int officerIdleW = 0, officerIdleH = 0;
DWORD* officerHurtPixels = NULL; int officerHurtW = 0, officerHurtH = 0;
DWORD* officerFirePixels = NULL; int officerFireW = 0, officerFireH = 0;

DWORD* defectedMovingPixels = NULL; int defectedMovingW = 0, defectedMovingH = 0;
DWORD* defectedIdlePixels = NULL; int defectedIdleW = 0, defectedIdleH = 0;
DWORD* defectedFiringPixels = NULL; int defectedFiringW = 0, defectedFiringH = 0;

DWORD* defectedGunnerPixels = NULL; int defectedGunnerW = 0, defectedGunnerH = 0;
DWORD* defectedGunnerFiringPixels = NULL; int defectedGunnerFiringW = 0, defectedGunnerFiringH = 0;
DWORD* grassPlantPixels = NULL;
int grassPlantW = 0, grassPlantH = 0;
DWORD* rockPixels[3] = {NULL};
int rockW[3] = {0}, rockH[3] = {0};
DWORD* bigRockPixels[3] = {NULL};
int bigRockW[3] = {0}, bigRockH[3] = {0};

DWORD* bushPixels = NULL;
int bushW = 0, bushH = 0;
DWORD* borderWallPixels = NULL;
int borderWallW = 0, borderWallH = 0;
DWORD* gateWallPixels = NULL;
int gateWallW = 0, gateWallH = 0;
int treeW = 0, treeH = 0;
int cloudW = 0, cloudH = 0;
int gunW = 0, gunH = 0;
int gunfireW = 0, gunfireH = 0;
DWORD* gunUpgrade1Pixels = NULL;
DWORD* gunfire1Pixels = NULL;
int gunUpgrade1W = 0, gunUpgrade1H = 0;
int gunfire1W = 0, gunfire1H = 0;
DWORD* gunUpgrade2Pixels = NULL;
DWORD* gunfire2Pixels = NULL;
int gunUpgrade2W = 0, gunUpgrade2H = 0;
int gunfire2W = 0, gunfire2H = 0;
DWORD* rocketProjPixels = NULL;
int rocketProjW = 0, rocketProjH = 0;
DWORD* rocketTrailPixels = NULL;
int rocketTrailW = 0, rocketTrailH = 0;
DWORD* explosionPixels = NULL;
int explosionW = 0, explosionH = 0;
int bulletW = 0, bulletH = 0;
int healthbarW = 0, healthbarH = 0;
DWORD* spirePixels = NULL;
int spireW = 0, spireH = 0;
DWORD* spireAwakePixels = NULL;
int spireAwakeW = 0, spireAwakeH = 0;
DWORD* spireHurtPixels = NULL;
int spireHurtW = 0, spireHurtH = 0;
DWORD* spireDeathPixels = NULL;
int spireDeathW = 0, spireDeathH = 0;
DWORD* fireballPixels = NULL;
int fireballW = 0, fireballH = 0;
DWORD* medkitPixels = NULL;
int medkitW = 0, medkitH = 0;

DWORD* clawDormantPixels = NULL;
int clawDormantW = 0, clawDormantH = 0;
DWORD* clawActivePixels = NULL;
int clawActiveW = 0, clawActiveH = 0;
DWORD* clawActivatingPixels = NULL;
int clawActivatingW = 0, clawActivatingH = 0;
float preBossPulseTimer = 0;
bool preBossPulseFrame = false;

Claw claws[6];
int activeClawIndex = 0;
float clawReturnSpeed = 3.0f;

DWORD* errorPixels = NULL;
int errorW = 0, errorH = 0;

// Post-Processing Globals
DWORD* renderBuffer = NULL;
int* distortionLUT = NULL;
int* redOffsetLUT = NULL;
int* blueOffsetLUT = NULL;

const char* g_vcrVertexShader = 
    "#version 120\n"
    "attribute vec2 aPos;\n"
    "attribute vec2 aTexCoord;\n"
    "varying vec2 vTexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos.x, -aPos.y, 0.0, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

const char* g_vcrFragmentShader = 
    "#version 120\n"
    "varying vec2 vTexCoord;\n"
    "uniform sampler2D tex;\n"
    "uniform float time;\n"
    "uniform vec2 resolution;\n"
    "\n"
    "vec2 barrelDistort(vec2 uv, float k, float kcube, float scale) {\n"
    "    vec2 centered = uv - 0.5;\n"
    "    float r2 = dot(centered, centered) * 4.0;\n"
    "    float f = 1.0 + r2 * (k + kcube * sqrt(r2));\n"
    "    return centered * f * scale + 0.5;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    float k = 0.15;\n"
    "    float kcube = 0.10;\n"
    "    float scale = 0.86;\n"
    "    \n"
    "    vec2 uvR = barrelDistort(vTexCoord, k + 0.02, kcube, scale);\n"
    "    vec2 uvG = barrelDistort(vTexCoord, k, kcube, scale);\n"
    "    vec2 uvB = barrelDistort(vTexCoord, k - 0.02, kcube, scale);\n"
    "    \n"
    "    float r = 0.0, g = 0.0, b = 0.0;\n"
    "    if (uvR.x >= 0.0 && uvR.x <= 1.0 && uvR.y >= 0.0 && uvR.y <= 1.0)\n"
    "        r = texture2D(tex, uvR).r;\n"
    "    if (uvG.x >= 0.0 && uvG.x <= 1.0 && uvG.y >= 0.0 && uvG.y <= 1.0)\n"
    "        g = texture2D(tex, uvG).g;\n"
    "    if (uvB.x >= 0.0 && uvB.x <= 1.0 && uvB.y >= 0.0 && uvB.y <= 1.0)\n"
    "        b = texture2D(tex, uvB).b;\n"
    "    \n"
    "    vec3 col = vec3(r, g, b);\n"
    "    \n"
    "    float scanline = sin((vTexCoord.y * resolution.y + time * 2.0) * 3.14159 * 0.5) * 0.5 + 0.5;\n"
    "    col *= 0.9 + 0.1 * scanline;\n"
    "    \n"
    "    float noise = fract(sin(dot(vTexCoord * time, vec2(12.9898, 78.233))) * 43758.5453);\n"
    "    col += (noise - 0.5) * 0.03;\n"
    "    \n"
    "    vec2 uvCheck = barrelDistort(vTexCoord, k, kcube, scale);\n"
    "    if (uvCheck.x < 0.0 || uvCheck.x > 1.0 || uvCheck.y < 0.0 || uvCheck.y > 1.0)\n"
    "        col = vec3(0.0);\n"
    "    \n"
    "    gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

const char* g_raycastComputeShader =
    "#version 430\n"
    "layout(local_size_x = 16, local_size_y = 16) in;\n"
    "layout(rgba8, binding = 0) writeonly uniform image2D renderOutput;\n"
    "layout(r32f, binding = 1) writeonly uniform image2D zBufferOutput;\n"
    "layout(std430, binding = 2) readonly buffer MapData { int worldMap[4096]; };\n"
    "layout(binding = 3) uniform sampler2D grassTex;\n"
    "layout(binding = 4) uniform sampler2D borderWallTex;\n"
    "layout(binding = 5) uniform sampler2D gateWallTex;\n"
    "uniform vec2 playerPos;\n"
    "uniform float playerAngle;\n"
    "uniform float playerPitch;\n"
    "uniform int bossActive;\n"
    "uniform ivec2 screenSize;\n"
    "const float PI = 3.14159265;\n"
    "const float FOV = PI / 3.0;\n"
    "const int MAP_SIZE = 64;\n"
    "\n"
    "int getMap(int x, int y) {\n"
    "    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) return 3;\n"
    "    return worldMap[x * MAP_SIZE + y];\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);\n"
    "    if (pixelCoord.x >= screenSize.x || pixelCoord.y >= screenSize.y) return;\n"
    "    int x = pixelCoord.x;\n"
    "    int y = pixelCoord.y;\n"
    "    float rayAngle = (playerAngle - FOV / 2.0) + (float(x) / float(screenSize.x)) * FOV;\n"
    "    float rayDirX = cos(rayAngle);\n"
    "    float rayDirY = sin(rayAngle);\n"
    "    int mapX = int(playerPos.x);\n"
    "    int mapY = int(playerPos.y);\n"
    "    float sideDistX, sideDistY;\n"
    "    float deltaDistX = (rayDirX == 0.0) ? 1e30 : abs(1.0 / rayDirX);\n"
    "    float deltaDistY = (rayDirY == 0.0) ? 1e30 : abs(1.0 / rayDirY);\n"
    "    int stepX, stepY;\n"
    "    if (rayDirX < 0.0) { stepX = -1; sideDistX = (playerPos.x - float(mapX)) * deltaDistX; }\n"
    "    else { stepX = 1; sideDistX = (float(mapX) + 1.0 - playerPos.x) * deltaDistX; }\n"
    "    if (rayDirY < 0.0) { stepY = -1; sideDistY = (playerPos.y - float(mapY)) * deltaDistY; }\n"
    "    else { stepY = 1; sideDistY = (float(mapY) + 1.0 - playerPos.y) * deltaDistY; }\n"
    "    bool hitWall = false;\n"
    "    int side = 0;\n"
    "    int wallType = 0;\n"
    "    float distanceToWall = 0.0;\n"
    "    for (int i = 0; i < 128 && !hitWall && distanceToWall < 90.0; i++) {\n"
    "        if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }\n"
    "        else { sideDistY += deltaDistY; mapY += stepY; side = 1; }\n"
    "        if (mapX < 0 || mapX >= MAP_SIZE || mapY < 0 || mapY >= MAP_SIZE) {\n"
    "            hitWall = true; wallType = 3; distanceToWall = 90.0;\n"
    "        } else if (getMap(mapX, mapY) > 0) {\n"
    "            hitWall = true; wallType = getMap(mapX, mapY);\n"
    "            if (side == 0) distanceToWall = (float(mapX) - playerPos.x + float(1 - stepX) / 2.0) / rayDirX;\n"
    "            else distanceToWall = (float(mapY) - playerPos.y + float(1 - stepY) / 2.0) / rayDirY;\n"
    "        }\n"
    "    }\n"
    "    float correctedDist = distanceToWall * cos(rayAngle - playerAngle);\n"
    "    float wallX;\n"
    "    if (side == 0) wallX = playerPos.y + distanceToWall * rayDirY;\n"
    "    else wallX = playerPos.x + distanceToWall * rayDirX;\n"
    "    wallX = fract(wallX);\n"
    "    int ceiling, floorLine;\n"
    "    if (wallType == 3 && distanceToWall >= 90.0) {\n"
    "        ceiling = 0; floorLine = screenSize.y / 2 + int(playerPitch);\n"
    "    } else {\n"
    "        int wallHeight = int(float(screenSize.y) / correctedDist);\n"
    "        ceiling = screenSize.y / 2 - wallHeight / 2 + int(playerPitch);\n"
    "        floorLine = screenSize.y / 2 + wallHeight / 2 + int(playerPitch);\n"
    "    }\n"
    "    vec4 color = vec4(0.0);\n"
    "    float zVal = 1000.0;\n"
    "    int halfH = screenSize.y / 2;\n"
    "    if (y <= halfH + int(playerPitch)) {\n"
    "        float skyGrad = float(y) / float(halfH);\n"
    "        vec3 skyCol;\n"
    "        if (bossActive != 0) {\n"
    "            skyCol = vec3(0.588 + 0.392 * (1.0 - skyGrad), 0.078 * (1.0 - skyGrad), 0.078 * (1.0 - skyGrad));\n"
    "        } else {\n"
    "            skyCol = vec3(0.118 + 0.314 * (1.0 - skyGrad), 0.235 + 0.471 * (1.0 - skyGrad), 0.392 + 0.608 * (1.0 - skyGrad));\n"
    "        }\n"
    "        float horizonFog = skyGrad * skyGrad;\n"
    "        skyCol = mix(skyCol, vec3(0.5), horizonFog);\n"
    "        color = vec4(skyCol, 1.0);\n"
    "        zVal = 1000.0;\n"
    "    }\n"
    "    if (y > halfH + int(playerPitch)) {\n"
    "        float rowDist = float(halfH) / float(y - halfH);\n"
    "        float floorX = playerPos.x + cos(rayAngle) * rowDist;\n"
    "        float floorY = playerPos.y + sin(rayAngle) * rowDist;\n"
    "        vec2 texCoord = fract(vec2(floorX, floorY));\n"
    "        vec4 floorCol = texture(grassTex, texCoord);\n"
    "        float shade = 1.0 - (rowDist / 20.0);\n"
    "        shade = max(shade, 0.15);\n"
    "        float fogFactor = clamp((rowDist - 17.0) / 13.0, 0.0, 1.0);\n"
    "        vec3 finalCol = floorCol.rgb * shade * (1.0 - fogFactor) + vec3(0.5) * fogFactor;\n"
    "        color = vec4(finalCol, 1.0);\n"
    "        zVal = rowDist;\n"
    "    }\n"
    "    if (y >= ceiling && y <= floorLine) {\n"
    "        float shade = 1.0 - (correctedDist / 50.0);\n"
    "        shade = max(shade, 0.1);\n"
    "        if (side == 1) shade *= 0.8;\n"
    "        float fogFactor = clamp((correctedDist - 17.0) / 13.0, 0.0, 1.0);\n"
    "        if (wallType == 3 && distanceToWall < 90.0) {\n"
    "            int wallHeight = floorLine - ceiling;\n"
    "            if (wallHeight <= 0) wallHeight = 1;\n"
    "            float texYf = float(y - ceiling) / float(wallHeight);\n"
    "            vec4 wallCol = texture(borderWallTex, vec2(wallX, texYf));\n"
    "            if (wallCol.a > 0.0) {\n"
    "                vec3 finalCol = wallCol.rgb * shade * (1.0 - fogFactor) + vec3(0.5) * fogFactor;\n"
    "                color = vec4(finalCol, 1.0);\n"
    "                zVal = correctedDist;\n"
    "            }\n"
    "        } else if (wallType == 4) {\n"
    "            int wallHeight = floorLine - ceiling;\n"
    "            if (wallHeight <= 0) wallHeight = 1;\n"
    "            float texYf = float(y - ceiling) / float(wallHeight);\n"
    "            vec4 wallCol = texture(gateWallTex, vec2(wallX, texYf));\n"
    "            if (wallCol.a > 0.0) {\n"
    "                vec3 finalCol = wallCol.rgb * shade * (1.0 - fogFactor) + vec3(0.5) * fogFactor;\n"
    "                color = vec4(finalCol, 1.0);\n"
    "                zVal = correctedDist;\n"
    "            }\n"
    "        } else if (wallType != 3 && wallType != 4) {\n"
    "            vec3 wallCol;\n"
    "            if (wallType == 2) wallCol = vec3(0.235, 0.392, 0.157);\n"
    "            else wallCol = vec3(0.549, 0.392, 0.235);\n"
    "            vec3 finalCol = wallCol * shade * (1.0 - fogFactor) + vec3(0.5) * fogFactor;\n"
    "            color = vec4(finalCol, 1.0);\n"
    "            zVal = correctedDist;\n"
    "        }\n"
    "    }\n"
    "    imageStore(renderOutput, pixelCoord, color);\n"
    "    imageStore(zBufferOutput, pixelCoord, vec4(zVal, 0.0, 0.0, 0.0));\n"
    "}\n";

bool InitOpenGL(HWND hwnd) {
    g_glDC = GetDC(hwnd);
    if (!g_glDC) return false;
    
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    
    int pixelFormat = ChoosePixelFormat(g_glDC, &pfd);
    if (!pixelFormat) return false;
    
    if (!SetPixelFormat(g_glDC, pixelFormat, &pfd)) return false;
    
    g_glRC = wglCreateContext(g_glDC);
    if (!g_glRC) return false;
    
    if (!wglMakeCurrent(g_glDC, g_glRC)) return false;
    
    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
    glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
    glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    
    if (!glCreateShader || !glShaderSource || !glCompileShader || !glCreateProgram ||
        !glAttachShader || !glLinkProgram || !glUseProgram || !glGetUniformLocation) {
        return false;
    }
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &g_vcrVertexShader, NULL);
    glCompileShader(vs);
    
    GLint compiled = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(vs);
        return false;
    }
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &g_vcrFragmentShader, NULL);
    glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }
    
    g_vcrProgram = glCreateProgram();
    glAttachShader(g_vcrProgram, vs);
    glAttachShader(g_vcrProgram, fs);
    glLinkProgram(g_vcrProgram);
    
    GLint linked = 0;
    glGetProgramiv(g_vcrProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    g_locTex = glGetUniformLocation(g_vcrProgram, "tex");
    g_locTime = glGetUniformLocation(g_vcrProgram, "time");
    g_locResolution = glGetUniformLocation(g_vcrProgram, "resolution");
    
    glGenTextures(1, &g_vcrTexture);
    glBindTexture(GL_TEXTURE_2D, g_vcrTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    if (glGenVertexArrays && glBindVertexArray && glGenBuffers && glBindBuffer && glBufferData) {
        float quadVerts[] = {
            -1.0f, -1.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f
        };
        
        glGenVertexArrays(1, &g_vcrVAO);
        glBindVertexArray(g_vcrVAO);
        
        glGenBuffers(1, &g_vcrVBO);
        glBindBuffer(GL_ARRAY_BUFFER, g_vcrVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)wglGetProcAddress("glDispatchCompute");
    glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)wglGetProcAddress("glMemoryBarrier");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)wglGetProcAddress("glBindImageTexture");
    glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase");
    glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D");
    
    if (glDispatchCompute && glMemoryBarrier && glBindImageTexture && glBindBufferBase && glTexStorage2D) {
        GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(cs, 1, &g_raycastComputeShader, NULL);
        glCompileShader(cs);
        
        GLint compiled = 0;
        glGetShaderiv(cs, GL_COMPILE_STATUS, &compiled);
        if (compiled) {
            g_raycastProgram = glCreateProgram();
            glAttachShader(g_raycastProgram, cs);
            glLinkProgram(g_raycastProgram);
            
            GLint linked = 0;
            glGetProgramiv(g_raycastProgram, GL_LINK_STATUS, &linked);
            if (linked) {
                g_rcLocPlayerPos = glGetUniformLocation(g_raycastProgram, "playerPos");
                g_rcLocPlayerAngle = glGetUniformLocation(g_raycastProgram, "playerAngle");
                g_rcLocPlayerPitch = glGetUniformLocation(g_raycastProgram, "playerPitch");
                g_rcLocBossActive = glGetUniformLocation(g_raycastProgram, "bossActive");
                g_rcLocScreenSize = glGetUniformLocation(g_raycastProgram, "screenSize");
                
                glGenTextures(1, &g_renderTex);
                glBindTexture(GL_TEXTURE_2D, g_renderTex);
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                glGenTextures(1, &g_zBufferTex);
                glBindTexture(GL_TEXTURE_2D, g_zBufferTex);
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, SCREEN_WIDTH, SCREEN_HEIGHT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                
                glGenBuffers(1, &g_mapSSBO);
                
                g_gpuRaycastAvailable = true;
            }
        }
        glDeleteShader(cs);
    }
    
    g_glInitialized = true;
    return true;
}

void InitPostProcess() {
    if (renderBuffer) free(renderBuffer);
    renderBuffer = (DWORD*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(DWORD));
    
    if (distortionLUT) free(distortionLUT);
    if (redOffsetLUT) free(redOffsetLUT);
    if (blueOffsetLUT) free(blueOffsetLUT);
    distortionLUT = NULL;
    redOffsetLUT = NULL;
    blueOffsetLUT = NULL;
}

void ApplyPostProcess() {
    if (!renderBuffer) return;
    
    g_glTime += 0.016f;
    
    if (g_glInitialized && g_vcrProgram && backBufferPixels) {
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindTexture(GL_TEXTURE_2D, g_vcrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, renderBuffer);
        
        if (g_EnableVHS) {
            glUseProgram(g_vcrProgram);
            glUniform1i(g_locTex, 0);
            glUniform1f(g_locTime, g_glTime);
            glUniform2f(g_locResolution, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
            
            if (g_vcrVAO && glBindVertexArray) {
                glBindVertexArray(g_vcrVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        } else {
            glUseProgram(0);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
        
        glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, backBufferPixels);
    } else {
        if (backBufferPixels) {
            memcpy(backBufferPixels, renderBuffer, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(DWORD));
        }
    }
}

void FinalizePostProcess(HDC memDC) {
    (void)memDC;
    if (!g_glInitialized || !g_vcrProgram || !renderBuffer) return;
    
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindTexture(GL_TEXTURE_2D, g_vcrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, renderBuffer);
    
    if (g_EnableVHS) {
        glUseProgram(g_vcrProgram);
        glUniform1i(g_locTex, 0);
        glUniform1f(g_locTime, g_glTime);
        glUniform2f(g_locResolution, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
        
        if (g_vcrVAO && glBindVertexArray) {
            glBindVertexArray(g_vcrVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    } else {
        glUseProgram(0);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    
    SwapBuffers(g_glDC);
}

bool keys[256] = {false};
wchar_t loadStatus[256] = L"Loading...";

bool IsPositionColliding(float x, float y, float radius) {
    if (x - radius < 0 || x + radius >= MAP_WIDTH || y - radius < 0 || y + radius >= MAP_HEIGHT) return true;

    int minX = (int)std::floor(x - radius);
    int maxX = (int)std::floor(x + radius);
    int minY = (int)std::floor(y - radius);
    int maxY = (int)std::floor(y + radius);

    for (int ix = minX; ix <= maxX; ix++) {
        for (int iy = minY; iy <= maxY; iy++) {
            if (ix >= 0 && ix < MAP_WIDTH && iy >= 0 && iy < MAP_HEIGHT) {
                if (worldMap[ix][iy] != 0) {
                    float testX = x;
                    float testY = y;
                    if (x < ix) testX = ix; else if (x > ix + 1) testX = ix + 1;
                    if (y < iy) testY = iy; else if (y > iy + 1) testY = iy + 1;
                    float dx = x - testX;
                    float dy = y - testY;
                    if (dx * dx + dy * dy < radius * radius) {
                        return true;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        if (claws[i].state == CLAW_PH2_ANCHORED) {
            float dx = x - claws[i].x;
            float dy = y - claws[i].y;
            float totalRadius = 1.5f + radius;
            if (dx * dx + dy * dy < totalRadius * totalRadius) return true;
        }
    }
    
    for (const auto& br : bigRocks) {
        float dx = x - br.x;
        float dy = y - br.y;
        float totalRadius = br.radius + radius;
        if (dx * dx + dy * dy < totalRadius * totalRadius) return true;
    }

    // Center spire is always present
    float dx = x - 32.0f;
    float dy = y - 32.0f;
    float totalRadius = 3.0f + radius;
    if (dx * dx + dy * dy < totalRadius * totalRadius) return true;

    return false;
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

void TryLoadAssets() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t* lastBackSlash = wcsrchr(exePath, L'\\');
    wchar_t* lastForwardSlash = wcsrchr(exePath, L'/');
    wchar_t* lastSlash = lastBackSlash;
    if (lastForwardSlash && (!lastSlash || lastForwardSlash > lastSlash)) lastSlash = lastForwardSlash;
    if (lastSlash) *lastSlash = L'\0';
    
    wchar_t path[MAX_PATH];
    missingAssets.clear();
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\error.bmp", exePath);
    errorPixels = LoadBMPPixels(path, &errorW, &errorH);
    
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\UI\\window.bmp", exePath);
    uiWindowPixels = LoadBMPPixels(path, &uiWindowW, &uiWindowH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\UI\\button.bmp", exePath);
    uiButtonPixels = LoadBMPPixels(path, &uiButtonW, &uiButtonH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\UI\\button-hover.bmp", exePath);
    uiButtonHoverPixels = LoadBMPPixels(path, &uiButtonHoverW, &uiButtonHoverH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\UI\\button-pressed.bmp", exePath);
    uiButtonPressedPixels = LoadBMPPixels(path, &uiButtonPressedW, &uiButtonPressedH);
swprintf(path, MAX_PATH, L"%ls\\assets\\grass.bmp", exePath);
    grassPixels = LoadBMPPixels(path, &grassW, &grassH);
    if (!grassPixels) { 
        missingAssets.push_back(L"grass.bmp");
        if (errorPixels) { grassPixels = errorPixels; grassW = errorW; grassH = errorH; } 
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\bullet.bmp", exePath);
    bulletPixels = LoadBMPPixels(path, &bulletW, &bulletH);
    if (!bulletPixels) { missingAssets.push_back(L"bullet.bmp"); if (errorPixels) { bulletPixels = errorPixels; bulletW = errorW; bulletH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\player-bullet.bmp", exePath);
    playerBulletPixels = LoadBMPPixels(path, &playerBulletW, &playerBulletH);
    if (!playerBulletPixels) { missingAssets.push_back(L"player-bullet.bmp"); if (errorPixels) { playerBulletPixels = errorPixels; playerBulletW = errorW; playerBulletH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\Spearguy\\spearguy-move.bmp", exePath);
    spearguyMovePixels = LoadBMPPixels(path, &spearguyMoveW, &spearguyMoveH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\Spearguy\\spearguy-idle.bmp", exePath);
    spearguyIdlePixels = LoadBMPPixels(path, &spearguyIdleW, &spearguyIdleH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\Spearguy\\spearguy-dashing.bmp", exePath);
    spearguyDashPixels = LoadBMPPixels(path, &spearguyDashW, &spearguyDashH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\Spearguy\\spearguy-block.bmp", exePath);
    spearguyBlockPixels = LoadBMPPixels(path, &spearguyBlockW, &spearguyBlockH);
    swprintf(path, MAX_PATH, L"%ls\\assets\\Spearguy\\spearguy-hurt.bmp", exePath);
    spearguyHurtPixels = LoadBMPPixels(path, &spearguyHurtW, &spearguyHurtH);
    for(int i=0; i<5; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\enemy%d.bmp", exePath, i+1);
        enemyPixels[i] = LoadBMPPixels(path, &enemyW[i], &enemyH[i]);
        if (!enemyPixels[i]) { wchar_t name[32]; swprintf(name, 32, L"enemy%d.bmp", i+1); missingAssets.push_back(name); if (errorPixels) { enemyPixels[i] = errorPixels; enemyW[i] = errorW; enemyH[i] = errorH; } }
    }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\enemy5_hurt.bmp", exePath);
    enemy5HurtPixels = LoadBMPPixels(path, &enemy5HurtW, &enemy5HurtH);
    if (!enemy5HurtPixels) { missingAssets.push_back(L"enemy5_hurt.bmp"); if (errorPixels) { enemy5HurtPixels = errorPixels; enemy5HurtW = errorW; enemy5HurtH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gunner.bmp", exePath);
    gunnerPixels = LoadBMPPixels(path, &gunnerW, &gunnerH);
    if (!gunnerPixels) { missingAssets.push_back(L"gunner.bmp"); if (errorPixels) { gunnerPixels = errorPixels; gunnerW = errorW; gunnerH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gunner_firing.bmp", exePath);
    gunnerFiringPixels = LoadBMPPixels(path, &gunnerFiringW, &gunnerFiringH);
    if (!gunnerFiringPixels) { missingAssets.push_back(L"gunner_firing.bmp"); if (errorPixels) { gunnerFiringPixels = errorPixels; gunnerFiringW = errorW; gunnerFiringH = errorH; } }
    
    // Officer & Defected Assets
    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\enemy\\officer-move.bmp", exePath);
    officerMovePixels = LoadBMPPixels(path, &officerMoveW, &officerMoveH);
    if (!officerMovePixels) { missingAssets.push_back(L"officer-move.bmp"); if (errorPixels) { officerMovePixels = errorPixels; officerMoveW = errorW; officerMoveH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\enemy\\officer-idle.bmp", exePath);
    officerIdlePixels = LoadBMPPixels(path, &officerIdleW, &officerIdleH);
    if (!officerIdlePixels) { missingAssets.push_back(L"officer-idle.bmp"); if (errorPixels) { officerIdlePixels = errorPixels; officerIdleW = errorW; officerIdleH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\enemy\\officer-hurt.bmp", exePath);
    officerHurtPixels = LoadBMPPixels(path, &officerHurtW, &officerHurtH);
    if (!officerHurtPixels) { missingAssets.push_back(L"officer-hurt.bmp"); if (errorPixels) { officerHurtPixels = errorPixels; officerHurtW = errorW; officerHurtH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\enemy\\officer-firing.bmp", exePath);
    officerFirePixels = LoadBMPPixels(path, &officerFireW, &officerFireH);
    if (!officerFirePixels) { missingAssets.push_back(L"officer-firing.bmp"); if (errorPixels) { officerFirePixels = errorPixels; officerFireW = errorW; officerFireH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\defected\\defected-moving.bmp", exePath);
    defectedMovingPixels = LoadBMPPixels(path, &defectedMovingW, &defectedMovingH);
    if (!defectedMovingPixels) { missingAssets.push_back(L"defected-moving.bmp"); if (errorPixels) { defectedMovingPixels = errorPixels; defectedMovingW = errorW; defectedMovingH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\defected\\defected-idle.bmp", exePath);
    defectedIdlePixels = LoadBMPPixels(path, &defectedIdleW, &defectedIdleH);
    if (!defectedIdlePixels) { missingAssets.push_back(L"defected-idle.bmp"); if (errorPixels) { defectedIdlePixels = errorPixels; defectedIdleW = errorW; defectedIdleH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\officer\\defected\\defected-firing.bmp", exePath);
    defectedFiringPixels = LoadBMPPixels(path, &defectedFiringW, &defectedFiringH);
    if (!defectedFiringPixels) { missingAssets.push_back(L"defected-firing.bmp"); if (errorPixels) { defectedFiringPixels = errorPixels; defectedFiringW = errorW; defectedFiringH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\defected\\gunner\\gunner-defected.bmp", exePath);
    defectedGunnerPixels = LoadBMPPixels(path, &defectedGunnerW, &defectedGunnerH);
    if (!defectedGunnerPixels) { missingAssets.push_back(L"gunner-defected.bmp"); if (errorPixels) { defectedGunnerPixels = errorPixels; defectedGunnerW = errorW; defectedGunnerH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\defected\\gunner\\gunner-defected-firing.bmp", exePath);
    defectedGunnerFiringPixels = LoadBMPPixels(path, &defectedGunnerFiringW, &defectedGunnerFiringH);
    if (!defectedGunnerFiringPixels) { missingAssets.push_back(L"gunner-defected-firing.bmp"); if (errorPixels) { defectedGunnerFiringPixels = errorPixels; defectedGunnerFiringW = errorW; defectedGunnerFiringH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\tree.bmp", exePath);
    treePixels = LoadBMPPixels(path, &treeW, &treeH);
    if (!treePixels) { missingAssets.push_back(L"tree.bmp"); if (errorPixels) { treePixels = errorPixels; treeW = errorW; treeH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\cloud.bmp", exePath);
    cloudPixels = LoadBMPPixels(path, &cloudW, &cloudH);
    if (!cloudPixels) { missingAssets.push_back(L"cloud.bmp"); if (errorPixels) { cloudPixels = errorPixels; cloudW = errorW; cloudH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gun.bmp", exePath);
    gunPixels = LoadBMPPixels(path, &gunW, &gunH);
    if (!gunPixels) { missingAssets.push_back(L"gun.bmp"); if (errorPixels) { gunPixels = errorPixels; gunW = errorW; gunH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gunfire.bmp", exePath);
    gunfirePixels = LoadBMPPixels(path, &gunfireW, &gunfireH);
    if (!gunfirePixels) { missingAssets.push_back(L"gunfire.bmp"); if (errorPixels) { gunfirePixels = errorPixels; gunfireW = errorW; gunfireH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gun_upgrade1.bmp", exePath);
    gunUpgrade1Pixels = LoadBMPPixels(path, &gunUpgrade1W, &gunUpgrade1H);
    if (!gunUpgrade1Pixels) { gunUpgrade1Pixels = gunPixels; gunUpgrade1W = gunW; gunUpgrade1H = gunH; }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gunfire1.bmp", exePath);
    gunfire1Pixels = LoadBMPPixels(path, &gunfire1W, &gunfire1H);
    if (!gunfire1Pixels) { gunfire1Pixels = gunfirePixels; gunfire1W = gunfireW; gunfire1H = gunfireH; }
    
    // Bazooka Assets
    swprintf(path, MAX_PATH, L"%ls\\assets\\gun_upgrade2.bmp", exePath);
    gunUpgrade2Pixels = LoadBMPPixels(path, &gunUpgrade2W, &gunUpgrade2H);
    if (!gunUpgrade2Pixels) { gunUpgrade2Pixels = gunPixels; gunUpgrade2W = gunW; gunUpgrade2H = gunH; }

    swprintf(path, MAX_PATH, L"%ls\\assets\\gunfire2.bmp", exePath);
    gunfire2Pixels = LoadBMPPixels(path, &gunfire2W, &gunfire2H);
    if (!gunfire2Pixels) { gunfire2Pixels = gunfirePixels; gunfire2W = gunfireW; gunfire2H = gunfireH; }

    swprintf(path, MAX_PATH, L"%ls\\assets\\rocket_proj.bmp", exePath);
    rocketProjPixels = LoadBMPPixels(path, &rocketProjW, &rocketProjH);
    if (!rocketProjPixels) { missingAssets.push_back(L"rocket_proj.bmp"); if (bulletPixels) { rocketProjPixels = bulletPixels; rocketProjW = bulletW; rocketProjH = bulletH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\rocket_trail.bmp", exePath);
    rocketTrailPixels = LoadBMPPixels(path, &rocketTrailW, &rocketTrailH);
    if (!rocketTrailPixels) { missingAssets.push_back(L"rocket_trail.bmp"); if (bulletPixels) { rocketTrailPixels = bulletPixels; rocketTrailW = bulletW; rocketTrailH = bulletH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\explosion_impact.bmp", exePath);
    explosionPixels = LoadBMPPixels(path, &explosionW, &explosionH);
    if (!explosionPixels) { explosionPixels = cloudPixels; explosionW = cloudW; explosionH = cloudH; }
    
    const wchar_t* healthbarNames[] = {L"healthbar_0.bmp", L"healthbar_10.bmp", L"healthbar_20.bmp", L"healthbar_30.bmp", L"healthbar_40.bmp", L"healthbar_50.bmp", L"healthbar_60.bmp", L"healthbar_70.bmp", L"healthbar_80.bmp", L"healthbar_90.bmp", L"healthbar_full.bmp"};
    for (int i = 0; i < 11; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\healthbar_UI\\%ls", exePath, healthbarNames[i]);
        healthbarPixels[i] = LoadBMPPixels(path, &healthbarW, &healthbarH);
        if (!healthbarPixels[i]) { missingAssets.push_back(healthbarNames[i]); if (errorPixels) { healthbarPixels[i] = errorPixels; healthbarW = errorW; healthbarH = errorH; } }
    }
    
    const wchar_t* xpBarNames[] = {L"xp-0.bmp", L"xp-10.bmp", L"xp-20.bmp", L"xp-30.bmp", L"xp-40.bmp", L"xp-50.bmp", L"xp-60.bmp", L"xp-70.bmp", L"xp-80.bmp", L"xp-90.bmp", L"xp-100.bmp"};
    for (int i = 0; i < 11; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\player_sprite\\player-xp\\%ls", exePath, xpBarNames[i]);
        xpBarPixels[i] = LoadBMPPixels(path, &xpBarW, &xpBarH);
        if (!xpBarPixels[i]) { missingAssets.push_back(xpBarNames[i]); if (errorPixels) { xpBarPixels[i] = errorPixels; xpBarW = errorW; xpBarH = errorH; } }
    }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\spire_resting.bmp", exePath);
    spirePixels = LoadBMPPixels(path, &spireW, &spireH);
    if (!spirePixels) { missingAssets.push_back(L"spire_resting.bmp"); if (errorPixels) { spirePixels = errorPixels; spireW = errorW; spireH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\spire_awake.bmp", exePath);
    spireAwakePixels = LoadBMPPixels(path, &spireAwakeW, &spireAwakeH);
    if (!spireAwakePixels) { missingAssets.push_back(L"spire_awake.bmp"); if (errorPixels) { spireAwakePixels = errorPixels; spireAwakeW = errorW; spireAwakeH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\Spire_hurt.bmp", exePath);
    spireHurtPixels = LoadBMPPixels(path, &spireHurtW, &spireHurtH);
    if (!spireHurtPixels) { missingAssets.push_back(L"Spire_hurt.bmp"); if (errorPixels) { spireHurtPixels = errorPixels; spireHurtW = errorW; spireHurtH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\Spire_Death.bmp", exePath);
    spireDeathPixels = LoadBMPPixels(path, &spireDeathW, &spireDeathH);
    if (!spireDeathPixels) { missingAssets.push_back(L"Spire_Death.bmp"); if (errorPixels) { spireDeathPixels = errorPixels; spireDeathW = errorW; spireDeathH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\fireball.bmp", exePath);
    fireballPixels = LoadBMPPixels(path, &fireballW, &fireballH);
    if (!fireballPixels) { missingAssets.push_back(L"fireball.bmp"); if (errorPixels) { fireballPixels = errorPixels; fireballW = errorW; fireballH = errorH; } }
    
    for(int i=0; i<3; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\spire_phase2\\spire_frame%d.bmp", exePath, i+1);
        spirePhase2Pixels[i] = LoadBMPPixels(path, &spirePhase2W[i], &spirePhase2H[i]);
        if(!spirePhase2Pixels[i]) {
            wchar_t name[64]; swprintf(name, 64, L"spire_frame%d.bmp", i+1); missingAssets.push_back(name);
            if (errorPixels) { spirePhase2Pixels[i] = errorPixels; spirePhase2W[i] = errorW; spirePhase2H[i] = errorH; }
        }
    }
    
    for(int i=0; i<4; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\claw_awaken\\claw_frame%d.bmp", exePath, i+1);
        clawPhase2Pixels[i] = LoadBMPPixels(path, &clawPhase2W[i], &clawPhase2H[i]);
        if(!clawPhase2Pixels[i]) {
             wchar_t name[64]; swprintf(name, 64, L"claw_frame%d.bmp", i+1); missingAssets.push_back(name);
             if (errorPixels) { clawPhase2Pixels[i] = errorPixels; clawPhase2W[i] = errorW; clawPhase2H[i] = errorH; }
        }
    }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\claw_awaken\\claw_hurt.bmp", exePath);
    clawHurtPixels = LoadBMPPixels(path, &clawHurtW, &clawHurtH);
    if (!clawHurtPixels) { missingAssets.push_back(L"claw_hurt.bmp"); if (errorPixels) { clawHurtPixels = errorPixels; clawHurtW = errorW; clawHurtH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\claw_attack\\laser.bmp", exePath);
    laserPixels = LoadBMPPixels(path, &laserW, &laserH);
    if (!laserPixels) { missingAssets.push_back(L"laser.bmp"); if (errorPixels) { laserPixels = errorPixels; laserW = errorW; laserH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\items\\Medkit.bmp", exePath);
    medkitPixels = LoadBMPPixels(path, &medkitW, &medkitH);
    if (!medkitPixels) { missingAssets.push_back(L"Medkit.bmp"); if (errorPixels) { medkitPixels = errorPixels; medkitW = errorW; medkitH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\claw_dormant.bmp", exePath);
    clawDormantPixels = LoadBMPPixels(path, &clawDormantW, &clawDormantH);
    if (!clawDormantPixels) { missingAssets.push_back(L"claw_dormant.bmp"); if (errorPixels) { clawDormantPixels = errorPixels; clawDormantW = errorW; clawDormantH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\claw_active.bmp", exePath);
    clawActivePixels = LoadBMPPixels(path, &clawActiveW, &clawActiveH);
    if (!clawActivePixels) { missingAssets.push_back(L"claw_active.bmp"); if (errorPixels) { clawActivePixels = errorPixels; clawActiveW = errorW; clawActiveH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\spire\\claw_activating.bmp", exePath);
    clawActivatingPixels = LoadBMPPixels(path, &clawActivatingW, &clawActivatingH);
    if (!clawActivatingPixels) { missingAssets.push_back(L"claw_activating.bmp"); if (errorPixels) { clawActivatingPixels = errorPixels; clawActivatingW = errorW; clawActivatingH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\Marshall\\marshall.bmp", exePath);
    marshallPixels = LoadBMPPixels(path, &marshallW, &marshallH);
    if (!marshallPixels) { missingAssets.push_back(L"marshall.bmp"); if (errorPixels) { marshallPixels = errorPixels; marshallW = errorW; marshallH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\Marshall\\marshall_hurt.bmp", exePath);
    marshallHurtPixels = LoadBMPPixels(path, &marshallHurtW, &marshallHurtH);
    if (!marshallHurtPixels) { missingAssets.push_back(L"marshall_hurt.bmp"); if (errorPixels) { marshallHurtPixels = errorPixels; marshallHurtW = errorW; marshallHurtH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\gunner_hurt.bmp", exePath);
    gunnerHurtPixels = LoadBMPPixels(path, &gunnerHurtW, &gunnerHurtH);
    if (!gunnerHurtPixels) { missingAssets.push_back(L"gunner_hurt.bmp"); if (errorPixels) { gunnerHurtPixels = errorPixels; gunnerHurtW = errorW; gunnerHurtH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\Viper\\Viper.bmp", exePath);
    paragonPixels = LoadBMPPixels(path, &paragonW, &paragonH);
    if (!paragonPixels) { missingAssets.push_back(L"Viper.bmp"); if (errorPixels) { paragonPixels = errorPixels; paragonW = errorW; paragonH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\Viper\\Viper_hurt.bmp", exePath);
    paragonHurtPixels = LoadBMPPixels(path, &paragonHurtW, &paragonHurtH);
    if (!paragonHurtPixels) { missingAssets.push_back(L"Viper_hurt.bmp"); if (errorPixels) { paragonHurtPixels = errorPixels; paragonHurtW = errorW; paragonHurtH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\environment\\plants\\grass_plant.bmp", exePath);
    grassPlantPixels = LoadBMPPixels(path, &grassPlantW, &grassPlantH);
    if (!grassPlantPixels) { missingAssets.push_back(L"grass_plant.bmp"); if (errorPixels) { grassPlantPixels = errorPixels; grassPlantW = errorW; grassPlantH = errorH; } }
    
    for (int i = 0; i < 3; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\environment\\small_rocks\\rock%d.bmp", exePath, i + 1);
        rockPixels[i] = LoadBMPPixels(path, &rockW[i], &rockH[i]);
        if (!rockPixels[i]) { wchar_t name[32]; swprintf(name, 32, L"rock%d.bmp", i+1); missingAssets.push_back(name); if (errorPixels) { rockPixels[i] = errorPixels; rockW[i] = errorW; rockH[i] = errorH; } }
    }

    for (int i = 0; i < 3; i++) {
        swprintf(path, MAX_PATH, L"%ls\\assets\\environment\\big_rocks\\BigRock%d.bmp", exePath, i + 1);
        bigRockPixels[i] = LoadBMPPixels(path, &bigRockW[i], &bigRockH[i]);
        if (!bigRockPixels[i]) { 
            // Fallback to small rocks if big ones missing, or error pixel
            if (rockPixels[i]) { bigRockPixels[i] = rockPixels[i]; bigRockW[i] = rockW[i]; bigRockH[i] = rockH[i]; }
            else if (errorPixels) { bigRockPixels[i] = errorPixels; bigRockW[i] = errorW; bigRockH[i] = errorH; }
            
            wchar_t name[32]; swprintf(name, 32, L"BigRock%d.bmp", i+1); missingAssets.push_back(name); 
        }
    }

    
    swprintf(path, MAX_PATH, L"%ls\\assets\\environment\\plants\\bush.bmp", exePath);
    bushPixels = LoadBMPPixels(path, &bushW, &bushH);
    if (!bushPixels) { missingAssets.push_back(L"bush.bmp"); if (errorPixels) { bushPixels = errorPixels; bushW = errorW; bushH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\gravital.bmp", exePath);
    gravitalPixels = LoadBMPPixels(path, &gravitalW, &gravitalH);
    if (!gravitalPixels) { missingAssets.push_back(L"gravital.bmp"); if (errorPixels) { gravitalPixels = errorPixels; gravitalW = errorW; gravitalH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\gravital_hurt.bmp", exePath);
    gravitalHurtPixels = LoadBMPPixels(path, &gravitalHurtW, &gravitalHurtH);
    if (!gravitalHurtPixels) { missingAssets.push_back(L"gravital_hurt.bmp"); if (errorPixels) { gravitalHurtPixels = errorPixels; gravitalHurtW = errorW; gravitalHurtH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\walls\\border1.bmp", exePath);
    borderWallPixels = LoadBMPPixels(path, &borderWallW, &borderWallH);
    if (!borderWallPixels) { missingAssets.push_back(L"border1.bmp"); }

    swprintf(path, MAX_PATH, L"%ls\\assets\\walls\\gate1.bmp", exePath);
    gateWallPixels = LoadBMPPixels(path, &gateWallW, &gateWallH);
    if (!gateWallPixels) { missingAssets.push_back(L"gate1.bmp"); }

    swprintf(path, MAX_PATH, L"%ls\\assets\\the_leader\\leader_idle.bmp", exePath);
    leaderIdlePixels = LoadBMPPixels(path, &leaderIdleW, &leaderIdleH);
    if (!leaderIdlePixels) { missingAssets.push_back(L"leader_idle.bmp"); if (errorPixels) { leaderIdlePixels = errorPixels; leaderIdleW = errorW; leaderIdleH = errorH; } }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\the_leader\\leader_talking.bmp", exePath);
    leaderTalkingPixels = LoadBMPPixels(path, &leaderTalkingW, &leaderTalkingH);
    if (!leaderTalkingPixels) { leaderTalkingPixels = leaderIdlePixels; leaderTalkingW = leaderIdleW; leaderTalkingH = leaderIdleH; }
    
    swprintf(path, MAX_PATH, L"%ls\\assets\\the_leader\\followers.bmp", exePath);
    followerPixels = LoadBMPPixels(path, &followerW, &followerH);
    if (!followerPixels) { missingAssets.push_back(L"followers.bmp"); if (errorPixels) { followerPixels = errorPixels; followerW = errorW; followerH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\player_sprite\\player.bmp", exePath);
    playerSpritePixels = LoadBMPPixels(path, &playerSpriteW, &playerSpriteH);
    if (!playerSpritePixels) { missingAssets.push_back(L"player.bmp"); if (errorPixels) { playerSpritePixels = errorPixels; playerSpriteW = errorW; playerSpriteH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\UI\\compass.bmp", exePath);
    compassPixels = LoadBMPPixels(path, &compassW, &compassH);
    if (!compassPixels) { missingAssets.push_back(L"compass.bmp"); if (errorPixels) { compassPixels = errorPixels; compassW = errorW; compassH = errorH; } }

    swprintf(path, MAX_PATH, L"%ls\\assets\\items\\grave.bmp", exePath);
    gravePixels = LoadBMPPixels(path, &graveW, &graveH);
    if (!gravePixels) { missingAssets.push_back(L"grave.bmp"); if (errorPixels) { gravePixels = errorPixels; graveW = errorW; graveH = errorH; } }

    // Healing Tower Assets
    swprintf(path, MAX_PATH, L"%ls\\assets\\healing_tower\\healing_tower_dormant.bmp", exePath);
    htDormantPixels = LoadBMPPixels(path, &htDormantW, &htDormantH);
    if (!htDormantPixels) { 
        missingAssets.push_back(L"healing_tower_dormant.bmp"); 
        // Fallback to error or rock
        if (bigRockPixels[0]) { htDormantPixels = bigRockPixels[0]; htDormantW = bigRockW[0]; htDormantH = bigRockH[0]; }
        else if (errorPixels) { htDormantPixels = errorPixels; htDormantW = errorW; htDormantH = errorH; } 
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\healing_tower\\healing_tower_charging1.bmp", exePath);
    htChargingPixels[0] = LoadBMPPixels(path, &htChargingW[0], &htChargingH[0]);
    if (!htChargingPixels[0]) { 
        htChargingPixels[0] = htDormantPixels; htChargingW[0] = htDormantW; htChargingH[0] = htDormantH;
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\healing_tower\\healing_tower_charging2.bmp", exePath);
    htChargingPixels[1] = LoadBMPPixels(path, &htChargingW[1], &htChargingH[1]);
    if (!htChargingPixels[1]) { 
         htChargingPixels[1] = htChargingPixels[0]; htChargingW[1] = htChargingW[0]; htChargingH[1] = htChargingH[0];
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\healing_tower\\healing_tower_ready.bmp", exePath);
    htReadyPixels = LoadBMPPixels(path, &htReadyW, &htReadyH);
    if (!htReadyPixels) { 
        htReadyPixels = htDormantPixels; htReadyW = htDormantW; htReadyH = htDormantH;
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\healing_tower\\healing_particle.bmp", exePath);
    htParticlePixels = LoadBMPPixels(path, &htParticleW, &htParticleH);
    if (!htParticlePixels) { 
        missingAssets.push_back(L"healing_particle.bmp"); 
        if (errorPixels) { htParticlePixels = errorPixels; htParticleW = errorW; htParticleH = errorH; } 
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\secret\\john-default.bmp", exePath);
    johnDefaultPixels = LoadBMPPixels(path, &johnDefaultW, &johnDefaultH);
    if (!johnDefaultPixels) {
        missingAssets.push_back(L"secret\\john-default.bmp");
    }

    swprintf(path, MAX_PATH, L"%ls\\assets\\secret\\john-interact.bmp", exePath);
    johnInteractPixels = LoadBMPPixels(path, &johnInteractW, &johnInteractH);
    if (!johnInteractPixels) {
        missingAssets.push_back(L"secret\\john-interact.bmp");
    }

    swprintf(loadStatus, 256, L"G:%ls S:%ls A:%ls H:%ls D:%ls F:%ls M:%ls C:%ls BR:%ls", gunPixels?L"OK":L"X", spirePixels?L"OK":L"X", spireAwakePixels?L"OK":L"X", spireHurtPixels?L"OK":L"X", spireDeathPixels?L"OK":L"X", fireballPixels?L"OK":L"X", medkitPixels?L"OK":L"X", clawDormantPixels?L"OK":L"X", bigRockPixels[0]?L"OK":L"X");

    
    if (!errorPixels && !gunPixels && !spirePixels && !treePixels && !grassPixels) {
        assetsFolderMissing = true;
    }
    
    wchar_t assetsDir[MAX_PATH];
    swprintf(assetsDir, MAX_PATH, L"%ls\\assets", exePath);
    DialogueSystem::LoadDialogueAssets(assetsDir);
}

void GenerateWorld() {
    srand((unsigned)time(NULL));
    
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            if (x <= 3 || x >= MAP_WIDTH-4 || y <= 3 || y >= MAP_HEIGHT-4) {
                worldMap[x][y] = 3;
            } else {
                worldMap[x][y] = 0;
            }
        }
    }
    
    int gateCenter = MAP_WIDTH / 2;
    for (int d = 0; d <= 3; d++) {
        worldMap[gateCenter][d] = 4;
        worldMap[gateCenter][MAP_HEIGHT - 1 - d] = 4;
        worldMap[d][gateCenter] = 4;
        worldMap[MAP_WIDTH - 1 - d][gateCenter] = 4;
    }

    
    int outerTrees = g_PerformanceMode ? 300 : 600;
    for (int i = 0; i < outerTrees; i++) {
        int side = rand() % 4;
        float tx, ty;
        switch (side) {
            case 0: tx = -15.0f + (rand() % 180) / 10.0f; ty = -15.0f + (rand() % ((MAP_HEIGHT + 30) * 10)) / 10.0f; break;
            case 1: tx = MAP_WIDTH - 3.0f + (rand() % 180) / 10.0f; ty = -15.0f + (rand() % ((MAP_HEIGHT + 30) * 10)) / 10.0f; break;
            case 2: tx = -15.0f + (rand() % ((MAP_WIDTH + 30) * 10)) / 10.0f; ty = -15.0f + (rand() % 180) / 10.0f; break;
            case 3: tx = -15.0f + (rand() % ((MAP_WIDTH + 30) * 10)) / 10.0f; ty = MAP_HEIGHT - 3.0f + (rand() % 180) / 10.0f; break;
        }
        TreeSprite tree = {tx, ty, 0};
        trees.push_back(tree);
    }
    
    int innerTrees = g_PerformanceMode ? 200 : 400;
    for (int i = 0; i < innerTrees; i++) {
        int side = rand() % 4;
        float tx, ty;
        switch (side) {
            case 0: tx = 3.0f + (rand() % 30) / 10.0f; ty = 3.0f + (rand() % ((MAP_HEIGHT - 6) * 10)) / 10.0f; break;
            case 1: tx = MAP_WIDTH - 6.0f + (rand() % 30) / 10.0f; ty = 3.0f + (rand() % ((MAP_HEIGHT - 6) * 10)) / 10.0f; break;
            case 2: tx = 3.0f + (rand() % ((MAP_WIDTH - 6) * 10)) / 10.0f; ty = 3.0f + (rand() % 30) / 10.0f; break;
            case 3: tx = 3.0f + (rand() % ((MAP_WIDTH - 6) * 10)) / 10.0f; ty = MAP_HEIGHT - 6.0f + (rand() % 30) / 10.0f; break;
        }
        TreeSprite tree = {tx, ty, 0};
        trees.push_back(tree);
    }
    
    int baseNum = g_PerformanceMode ? 125 : 250;
    int varNum = g_PerformanceMode ? 25 : 50;
    int numTrees = baseNum + rand() % varNum;
    for (int i = 0; i < numTrees; i++) {
        float tx = 8.0f + (rand() % ((MAP_WIDTH - 16) * 10)) / 10.0f;
        float ty = 8.0f + (rand() % ((MAP_HEIGHT - 16) * 10)) / 10.0f;
        float distToPlayer = sqrtf((tx - 32)*(tx - 32) + (ty - 32)*(ty - 32));
        if (distToPlayer > 10.0f) {
            TreeSprite tree = {tx, ty, 0};
            trees.push_back(tree);
        }
    }
    
    int clearX = (int)player.x;
    int clearY = (int)player.y;
    for (int dx = -4; dx <= 4; dx++) {
        for (int dy = -4; dy <= 4; dy++) {
            int cx = clearX + dx;
            int cy = clearY + dy;
            if (cx > 3 && cx < MAP_WIDTH-4 && cy > 3 && cy < MAP_HEIGHT-4) {
                worldMap[cx][cy] = 0;
            }
        }
    }
    
    for (int i = 0; i < 50; i++) {
        Cloud cloud;
        cloud.x = -50.0f + (rand() % 1500) / 10.0f;
        cloud.y = -50.0f + (rand() % 1500) / 10.0f;
        cloud.height = 15.0f + (rand() % 100) / 10.0f;
        cloud.speed = 0.5f + (rand() % 100) / 100.0f;
        clouds.push_back(cloud);
    }
    
    int rockVariant = 0;
    for (int i = 0; i < 5000; i++) {
        float gx = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
        float gy = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
        float distToCenter = sqrtf((gx - 32)*(gx - 32) + (gy - 32)*(gy - 32));
        if (distToCenter > 6.0f && worldMap[(int)gx][(int)gy] == 0) {
            GrassSprite grass = {gx, gy};
            grasses.push_back(grass);
        }
    }
    
    for (int i = 0; i < 350; i++) {
        float rx = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
        float ry = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
        float distToCenter = sqrtf((rx - 32)*(rx - 32) + (ry - 32)*(ry - 32));
        if (distToCenter > 6.0f && worldMap[(int)rx][(int)ry] == 0) {
            RockSprite rock = {rx, ry, rockVariant};
            rocks.push_back(rock);
            rockVariant = (rockVariant + 1) % 3;
        }
    }
    
    // Generate Big Rocks
    int bigRockVariant = 0;
    for (int i = 0; i < 65; i++) {
        float bx = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
        float by = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
        float distToCenter = sqrtf((bx - 32)*(bx - 32) + (by - 32)*(by - 32));
        
        float distToPlayerStart = sqrtf((bx - 10.0f)*(bx - 10.0f) + (by - 32.0f)*(by - 32.0f));
        if (distToPlayerStart > 8.0f && distToCenter > 10.0f && worldMap[(int)bx][(int)by] == 0) {
            // Check distance to other big rocks to avoid stacking
            bool overlaps = false;
            for(const auto& existing : bigRocks) {
                float dx = bx - existing.x;
                float dy = by - existing.y;
                if (dx*dx + dy*dy < 0.49f) { overlaps = true; break; }
            }
            if (!overlaps) {
                BigRock br = {bx, by, bigRockVariant, 1.2f};
                bigRocks.push_back(br);
                bigRockVariant = (bigRockVariant + 1) % 3;
            }
        }
    }

    
    for (int i = 0; i < 80; i++) {
        float bx = 6.0f + (rand() % ((MAP_WIDTH - 12) * 10)) / 10.0f;
        float by = 6.0f + (rand() % ((MAP_HEIGHT - 12) * 10)) / 10.0f;
        float distToCenter = sqrtf((bx - 32)*(bx - 32) + (by - 32)*(by - 32));
        if (distToCenter > 8.0f && worldMap[(int)bx][(int)by] == 0) {
            BushSprite bush = {bx, by};
            bushes.push_back(bush);
        }
    }


    // Initialize Healing Tower
    healingTower.x = 20.0f; // Fixed spawn for now
    healingTower.y = 20.0f;
    healingTower.state = TOWER_DORMANT;
    healingTower.timer = 0;
    healingTower.cooldownTimer = 0;
    healingTower.animTimer = 0;
    healingTower.pulseFrame = 0;
    healingTower.particles.clear();
    
    // Initialize John
    johnSecret.isSpawned = false;
    johnSecret.naturalSpawn = false;
    johnSecret.angle = 0.0f;
    johnSecret.x = healingTower.x + 2.0f;
    johnSecret.y = healingTower.y;
    johnSecret.hopOffset = 0.0f;
    johnSecret.hopTimer = 0.0f;
    johnSecret.isHopping = false;
    johnSecret.fullCirclesCompleted = 0;
    johnSecret.prevAngle = 0.0f;
    if (rand() % 100 == 0) { // 1% chance
        johnSecret.isSpawned = true;
        johnSecret.naturalSpawn = true;
        player.maxHealth *= 2;
        player.health = player.maxHealth;
    }
    
    PopulateSpatialGrids();
}

void PopulateSpatialGrids() {
    for (int x = 0; x < 17; x++) {
        for (int y = 0; y < 17; y++) {
            treeGrid[x][y].clear();
            grassGrid[x][y].clear();
            rockGrid[x][y].clear();
            bushGrid[x][y].clear();
            bigRockGrid[x][y].clear();
        }
    }
    
    for (size_t i = 0; i < trees.size(); i++) {
        int gx = (int)(trees[i].x / GRID_CELL_SIZE);
        int gy = (int)(trees[i].y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < 17 && gy >= 0 && gy < 17) {
            treeGrid[gx][gy].push_back((int)i);
        }
    }
    
    for (size_t i = 0; i < grasses.size(); i++) {
        int gx = (int)(grasses[i].x / GRID_CELL_SIZE);
        int gy = (int)(grasses[i].y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < 17 && gy >= 0 && gy < 17) {
            grassGrid[gx][gy].push_back((int)i);
        }
    }
    
    for (size_t i = 0; i < rocks.size(); i++) {
        int gx = (int)(rocks[i].x / GRID_CELL_SIZE);
        int gy = (int)(rocks[i].y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < 17 && gy >= 0 && gy < 17) {
            rockGrid[gx][gy].push_back((int)i);
        }
    }
    
    for (size_t i = 0; i < bushes.size(); i++) {
        int gx = (int)(bushes[i].x / GRID_CELL_SIZE);
        int gy = (int)(bushes[i].y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < 17 && gy >= 0 && gy < 17) {
            bushGrid[gx][gy].push_back((int)i);
        }
    }
    
    for (size_t i = 0; i < bigRocks.size(); i++) {
        int gx = (int)(bigRocks[i].x / GRID_CELL_SIZE);
        int gy = (int)(bigRocks[i].y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < 17 && gy >= 0 && gy < 17) {
            bigRockGrid[gx][gy].push_back((int)i);
        }
    }
}

void UpdateJohn(float deltaTime) {
    if (!johnSecret.isSpawned) return;
    
    if (johnSecret.isHopping) {
        johnSecret.hopTimer += deltaTime * 5.0f;
        if (johnSecret.hopTimer >= 3.14159f) {
            johnSecret.isHopping = false;
            johnSecret.hopTimer = 0.0f;
            johnSecret.hopOffset = 0.0f;
        } else {
            johnSecret.hopOffset = sinf(johnSecret.hopTimer) * 0.5f;
        }
    } else {
        johnSecret.hopOffset = 0.0f;
        johnSecret.angle += deltaTime * 0.5f;
        
        if (healingTower.state == TOWER_ACTIVE) {
            if (johnSecret.angle - johnSecret.prevAngle >= 6.28318f) {
                johnSecret.prevAngle = johnSecret.angle;
                johnSecret.isHopping = true;
                johnSecret.hopTimer = 0.0f;
            }
        } else {
            johnSecret.prevAngle = johnSecret.angle;
        }
    }
    
    johnSecret.x = healingTower.x + cosf(johnSecret.angle) * 2.0f;
    johnSecret.y = healingTower.y + sinf(johnSecret.angle) * 2.0f;
}

void UpdateHealingTower(float deltaTime) {
    float distToPlayer = sqrtf((healingTower.x - player.x)*(healingTower.x - player.x) + (healingTower.y - player.y)*(healingTower.y - player.y));
    
    switch (healingTower.state) {
        case TOWER_DORMANT:
            if (score >= 100) {
                healingTower.state = TOWER_CHARGING;
                healingTower.timer = 0;
            }
            break;
        case TOWER_CHARGING:
            healingTower.animTimer += deltaTime;
            if (healingTower.animTimer >= 0.5f) {
                healingTower.pulseFrame = !healingTower.pulseFrame;
                healingTower.animTimer = 0;
            }
            if (score >= 150) {
                healingTower.state = TOWER_READY;
                healingTower.particles.clear();
                for(int i=0; i<8; i++) {
                    HealingTower::Particle p;
                    // Circle distribution
                    p.angle = (i / 8.0f) * 2 * PI;
                    p.dist = 1.5f;
                    p.height = 0.5f; 
                    p.speed = 1.0f;
                    healingTower.particles.push_back(p);
                }
            }
            break;
        case TOWER_READY:
            for(auto& p : healingTower.particles) {
                p.angle += p.speed * deltaTime;
                if (p.angle > 2*PI) p.angle -= 2*PI;
            }
            
            if (distToPlayer < 3.0f) {
                 ShowError(L"Press E to activate healing"); 
                 if (keys['E']) {
                     healingTower.state = TOWER_ACTIVE;
                     healingTower.timer = 10.0f;
                 }
            }
            break;
        case TOWER_ACTIVE:
            healingTower.timer -= deltaTime;
            
            for(auto& p : healingTower.particles) {
                p.angle += p.speed * 1.0f * deltaTime; // Reduced from 2.0f
                 if (p.angle > 2*PI) p.angle -= 2*PI;
                if (p.dist < 8.0f) p.dist += 5.0f * deltaTime;
            }
            
            if (distToPlayer < 8.0f) {
                player.health += (int)(50 * deltaTime); 
                if (player.health > player.maxHealth) player.health = player.maxHealth;
                healFlashTimer = 0.5f; 
            }
            
            if (healingTower.timer <= 0) {
                healingTower.state = TOWER_COOLDOWN;
                healingTower.cooldownTimer = 10.0f; // Increased from 5.0f
                healingTower.particles.clear();
            }
            break;
        case TOWER_COOLDOWN:
            healingTower.animTimer += deltaTime;
            if (healingTower.animTimer >= 0.5f) {
                healingTower.pulseFrame = !healingTower.pulseFrame;
                healingTower.animTimer = 0;
            }
            healingTower.cooldownTimer -= deltaTime;
            if (healingTower.cooldownTimer <= 0) {
                healingTower.state = TOWER_READY;
                healingTower.particles.clear();
                for(int i=0; i<8; i++) {
                    HealingTower::Particle p;
                    p.angle = (i / 8.0f) * 2 * PI;
                    p.dist = 1.5f;
                    p.height = 0.5f;
                    p.speed = 1.0f;
                    healingTower.particles.push_back(p);
                }
            }
            break;
    }
}

void SpawnMedkit() {

    for (int i = 0; i < 3; i++) {
        do {
            medkits[i].x = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
            medkits[i].y = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
        } while (IsPositionColliding(medkits[i].x, medkits[i].y, 0.4f) || 
                 sqrtf((medkits[i].x - 32)*(medkits[i].x - 32) + (medkits[i].y - 32)*(medkits[i].y - 32)) < 5.0f);
        medkits[i].active = true;
        medkits[i].respawnTimer = 0;
    }
}

void InitClaws() {
    for(int i = 0; i < 6; i++) {
        float angle = (i * 60.0f) * (PI / 180.0f);
        claws[i].homeX = 32.0f + cosf(angle) * 16.0f;
        claws[i].homeY = 32.0f + sinf(angle) * 16.0f;
        claws[i].x = claws[i].homeX;
        claws[i].y = claws[i].homeY;
        claws[i].groundY = claws[i].homeY;
        claws[i].state = CLAW_DORMANT;
        claws[i].timer = 0;
        claws[i].index = i;
        claws[i].dealtDamage = false;
    }
    activeClawIndex = 0;
}

void SpawnEnemies() {
    enemies.clear();
    enemyBullets.clear();
    
    for (int i = 0; i < 3; i++) {
        Enemy enemy;
        bool validSpawn = false;
        do {
            enemy.x = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
            enemy.y = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
            
            float distToPlayer = sqrtf((enemy.x - player.x)*(enemy.x - player.x) + (enemy.y - player.y)*(enemy.y - player.y));
            if (!IsPositionColliding(enemy.x, enemy.y, 0.5f) && distToPlayer >= 10.0f) {
                validSpawn = true;
            }
        } while (!validSpawn);
        enemy.active = true;
        enemy.speed = 1.5f + (rand() % 100) / 100.0f;
        enemy.distance = 0;
        enemy.spriteIndex = rand() % 5;
        if (enemy.spriteIndex == 4) {
            enemy.health = 4;
        } else {
            enemy.health = 1;
        }
        enemy.hurtTimer = 0;
        enemy.isShooter = false;
        enemy.fireTimer = 0;
        enemy.firingTimer = 0;
        enemy.isMarshall = false;
        enemy.tacticState = 0;
        enemy.flankDir = 0;
        enemy.tacticTimer = 0;
        enemy.path.clear();
        enemy.pathIndex = 0;
        enemy.pathRecalcTimer = 0;
        NeuralAI::InheritBrain(enemy.brain);
        enemy.hasNeuralBrain = true;
        enemies.push_back(enemy);
    }
}

void SpawnGravitals(float centerX, float centerY) {
    for (int i = 0; i < 8; i++) {
        float angle = (i / 8.0f) * 2 * PI;
        Gravital g;
        g.x = centerX + cosf(angle) * 6.0f; // Radius 6.0
        g.y = centerY + sinf(angle) * 6.0f;
        g.z = 0.75f; // Initial height (lowered)
        g.targetX = player.x;
        g.targetY = player.y;
        g.active = true;
        g.health = 30;
        g.hurtTimer = 0;
        g.state = GRAVITAL_CHASE;
        g.slamTimer = 0;
        g.animTimer = 0;
        g.animFrame = 0;
        gravitals.push_back(g);
    }
}

void UpdateGravitals(float deltaTime) {
    for (auto& g : gravitals) {
        if (!g.active) continue;
        
        if (g.hurtTimer > 0) g.hurtTimer -= deltaTime;
        
        float dx = player.x - g.x;
        float dy = player.y - g.y;
        float dist = sqrtf(dx*dx + dy*dy);
        
        if (g.state == GRAVITAL_CHASE) {
            float speed = 5.0f; 
            if (dist > 0.1f) {
                g.x += (dx / dist) * speed * deltaTime;
                g.y += (dy / dist) * speed * deltaTime;
            }
            
            // Maintain height
            if (g.z < 0.75f) g.z += 5.0f * deltaTime; 
            if (g.z > 0.75f) g.z = 0.75f;

            if (dist < 4.0f) {
                g.state = GRAVITAL_SLAM;
                g.slamTimer = 0.5f; // Windup
                g.targetX = player.x;
                g.targetY = player.y;
            }
        } else if (g.state == GRAVITAL_SLAM) {
            if (g.slamTimer > 0) {
                 g.slamTimer -= deltaTime;
            } else {
                 // Slam Down
                 g.z -= 15.0f * deltaTime; // Fast drop
                 
                 // Dash towards target
                 float dashSpeed = 12.0f;
                 float dashDx = g.targetX - g.x;
                 float dashDy = g.targetY - g.y;
                 float dashDist = sqrtf(dashDx*dashDx + dashDy*dashDy);
                 if (dashDist > 0.1f) {
                     g.x += (dashDx / dashDist) * dashSpeed * deltaTime;
                     g.y += (dashDy / dashDist) * dashSpeed * deltaTime;
                 }
                 
                 if (g.z <= 0) {
                     g.z = 0;
                     
                     // Recalculate distance after dash
                     float finalDx = player.x - g.x;
                     float finalDy = player.y - g.y;
                     float finalDist = sqrtf(finalDx*finalDx + finalDy*finalDy);
                     
                     // Impact
                     if (finalDist < 3.0f) { // AoE Radius
                         if (!godMode) {
                            player.health -= 5;
                            playerHurtTimer = 0.3f;
                            // Knockback (Normalized)
                            if (finalDist > 0.1f) {
                                float push = 5.0f; // Increased force
                                player.x += (finalDx / finalDist) * push;
                                player.y += (finalDy / finalDist) * push;
                            }
                         }
                     }
                      // Visuals
                     if (finalDist < 15.0f) {
                         screenShakeTimer = 0.3f;
                         screenShakeIntensity = 15.0f;
                     }
                     g.state = GRAVITAL_RECOVER;
                 }
            }
        } else if (g.state == GRAVITAL_RECOVER) {
            g.z += 5.0f * deltaTime; // Fast rise
            if (g.z >= 0.75f) {
                g.z = 0.75f;
                g.state = GRAVITAL_CHASE;
            }
        }
        
        // Simple map bounds
        if (g.x < 1) g.x = 1; if (g.x > MAP_WIDTH-2) g.x = MAP_WIDTH-2;
        if (g.y < 1) g.y = 1; if (g.y > MAP_HEIGHT-2) g.y = MAP_HEIGHT-2;
    }
}



inline void ApplyHurtFlash_Fast(DWORD* pixels, int count, float intensity) {
    if (intensity > 1.0f) intensity = 1.0f;
    int redAdd = (int)(intensity * 127.5f);
    int darkMul = (int)((1.0f - intensity * 0.5f) * 256);
    
    __m128i zero = _mm_setzero_si128();
    __m128i redAddVec = _mm_set1_epi16((short)redAdd);
    __m128i darkMulVec = _mm_set1_epi16((short)darkMul);
    __m128i max255 = _mm_set1_epi16(255);
    
    int i = 0;
    for (; i < count - 3; i += 4) {
        __m128i pix = _mm_loadu_si128((__m128i*)&pixels[i]);
        
        __m128i lo = _mm_unpacklo_epi8(pix, zero);
        __m128i hi = _mm_unpackhi_epi8(pix, zero);
        
        __m128i bLo = _mm_and_si128(lo, _mm_set1_epi32(0xFFFF));
        __m128i gLo = _mm_and_si128(_mm_srli_epi32(lo, 16), _mm_set1_epi32(0xFFFF));
        __m128i bHi = _mm_and_si128(hi, _mm_set1_epi32(0xFFFF));
        __m128i gHi = _mm_and_si128(_mm_srli_epi32(hi, 16), _mm_set1_epi32(0xFFFF));
        
        bLo = _mm_srli_epi16(_mm_mullo_epi16(bLo, darkMulVec), 8);
        gLo = _mm_srli_epi16(_mm_mullo_epi16(gLo, darkMulVec), 8);
        bHi = _mm_srli_epi16(_mm_mullo_epi16(bHi, darkMulVec), 8);
        gHi = _mm_srli_epi16(_mm_mullo_epi16(gHi, darkMulVec), 8);
        
        __m128i rLo = _mm_and_si128(lo, _mm_set1_epi32(0xFFFF0000));
        __m128i rHi = _mm_and_si128(hi, _mm_set1_epi32(0xFFFF0000));
        rLo = _mm_srli_epi32(rLo, 16);
        rHi = _mm_srli_epi32(rHi, 16);
        rLo = _mm_add_epi16(rLo, redAddVec);
        rHi = _mm_add_epi16(rHi, redAddVec);
        rLo = _mm_min_epi16(rLo, max255);
        rHi = _mm_min_epi16(rHi, max255);
        
        lo = _mm_or_si128(bLo, _mm_slli_epi32(gLo, 8));
        lo = _mm_or_si128(lo, _mm_slli_epi32(rLo, 16));
        hi = _mm_or_si128(bHi, _mm_slli_epi32(gHi, 8));
        hi = _mm_or_si128(hi, _mm_slli_epi32(rHi, 16));
        
        __m128i result = _mm_packus_epi16(
            _mm_packs_epi32(lo, hi),
            _mm_packs_epi32(lo, hi)
        );
        
        pixels[i] = _mm_cvtsi128_si32(lo);
        pixels[i+1] = _mm_cvtsi128_si32(_mm_srli_si128(lo, 4));
        pixels[i+2] = _mm_cvtsi128_si32(hi);
        pixels[i+3] = _mm_cvtsi128_si32(_mm_srli_si128(hi, 4));
    }
    
    for (; i < count; i++) {
        DWORD col = pixels[i];
        int r = ((col >> 16) & 0xFF) + redAdd;
        int g = (((col >> 8) & 0xFF) * darkMul) >> 8;
        int b = ((col & 0xFF) * darkMul) >> 8;
        if (r > 255) r = 255;
        pixels[i] = (r << 16) | (g << 8) | b;
    }
}

inline void ApplyHealFlash_Fast(DWORD* pixels, int count, int alpha) {
    __m128i alphaVec = _mm_set1_epi32(alpha << 8);
    __m128i greenMask = _mm_set1_epi32(0x0000FF00);
    __m128i maxGreen = _mm_set1_epi32(0x0000FF00);
    __m128i otherMask = _mm_set1_epi32(0xFFFF00FF);
    
    int i = 0;
    for (; i < count - 3; i += 4) {
        __m128i pix = _mm_loadu_si128((__m128i*)&pixels[i]);
        __m128i green = _mm_and_si128(pix, greenMask);
        green = _mm_add_epi32(green, alphaVec);
        green = _mm_min_epi16(green, maxGreen);
        green = _mm_and_si128(green, greenMask);
        __m128i other = _mm_and_si128(pix, otherMask);
        __m128i result = _mm_or_si128(other, green);
        _mm_storeu_si128((__m128i*)&pixels[i], result);
    }
    
    for (; i < count; i++) {
        DWORD col = pixels[i];
        int r = (col >> 16) & 0xFF;
        int g = ((col >> 8) & 0xFF) + alpha;
        int b = col & 0xFF;
        if (g > 255) g = 255;
        pixels[i] = (r << 16) | (g << 8) | b;
    }
}

inline void ApplyBrightFade_Fast(DWORD* pixels, int count, int fadeAmount) {
    __m128i fadeVec = _mm_set1_epi16((short)fadeAmount);
    __m128i max255 = _mm_set1_epi16(255);
    __m128i zero = _mm_setzero_si128();
    
    int i = 0;
    for (; i < count - 3; i += 4) {
        __m128i pix = _mm_loadu_si128((__m128i*)&pixels[i]);
        __m128i lo = _mm_unpacklo_epi8(pix, zero);
        __m128i hi = _mm_unpackhi_epi8(pix, zero);
        __m128i invLo = _mm_sub_epi16(max255, lo);
        __m128i invHi = _mm_sub_epi16(max255, hi);
        __m128i addLo = _mm_srli_epi16(_mm_mullo_epi16(invLo, fadeVec), 8);
        __m128i addHi = _mm_srli_epi16(_mm_mullo_epi16(invHi, fadeVec), 8);
        lo = _mm_add_epi16(lo, addLo);
        hi = _mm_add_epi16(hi, addHi);
        __m128i result = _mm_packus_epi16(lo, hi);
        _mm_storeu_si128((__m128i*)&pixels[i], result);
    }
    
    for (; i < count; i++) {
        DWORD col = pixels[i];
        int r = (col >> 16) & 0xFF;
        int g = (col >> 8) & 0xFF;
        int b = col & 0xFF;
        r = r + (((255 - r) * fadeAmount) >> 8);
        g = g + (((255 - g) * fadeAmount) >> 8);
        b = b + (((255 - b) * fadeAmount) >> 8);
        pixels[i] = (r << 16) | (g << 8) | b;
    }
}

inline void ApplyVictoryBright_Fast(DWORD* pixels, int count) {
    __m128i mulVec = _mm_set1_epi16(77);
    __m128i addVec = _mm_set1_epi16(179);
    __m128i zero = _mm_setzero_si128();
    
    int i = 0;
    for (; i < count - 3; i += 4) {
        __m128i pix = _mm_loadu_si128((__m128i*)&pixels[i]);
        __m128i lo = _mm_unpacklo_epi8(pix, zero);
        __m128i hi = _mm_unpackhi_epi8(pix, zero);
        lo = _mm_add_epi16(_mm_srli_epi16(_mm_mullo_epi16(lo, mulVec), 8), addVec);
        hi = _mm_add_epi16(_mm_srli_epi16(_mm_mullo_epi16(hi, mulVec), 8), addVec);
        __m128i result = _mm_packus_epi16(lo, hi);
        _mm_storeu_si128((__m128i*)&pixels[i], result);
    }
    
    for (; i < count; i++) {
        DWORD col = pixels[i];
        int r = ((col >> 16) & 0xFF) * 77 / 256 + 179;
        int g = ((col >> 8) & 0xFF) * 77 / 256 + 179;
        int b = (col & 0xFF) * 77 / 256 + 179;
        pixels[i] = (r << 16) | (g << 8) | b;
    }
}

inline DWORD BlendWithFog(int r, int g, int b, float dist, float fogStart, float fogEnd) {
    // Fog color (gray-blue for normal, dark red for boss)
    int fogR = bossActive ? 40 : 80;
    int fogG = bossActive ? 20 : 85;
    int fogB = bossActive ? 20 : 90;
    
    float fogFactor = (dist - fogStart) / (fogEnd - fogStart);
    if (fogFactor < 0) fogFactor = 0;
    if (fogFactor > 1) fogFactor = 1;
    
    r = (int)(r * (1 - fogFactor) + fogR * fogFactor);
    g = (int)(g * (1 - fogFactor) + fogG * fogFactor);
    b = (int)(b * (1 - fogFactor) + fogB * fogFactor);
    
    return MakeColor(r, g, b);
}

struct RaycastParams {
    int startX;
    int endX;
    HANDLE startEvent;
    HANDLE doneEvent;
    volatile bool running;
};

RaycastParams* threadParams = nullptr;
HANDLE* rayThreads = nullptr;
int numRayThreads = 0;
volatile bool raycastRunning = true;
HANDLE staticDoneEvents[16];

struct SpriteRender { float x, y, dist; int type; float scale; int variant; bool isHurt; float height; bool isFiring; };
std::vector<SpriteRender> g_allSprites;

unsigned __stdcall RaycastWorker(void* param) {
    RaycastParams* rp = (RaycastParams*)param;
    
    while (rp->running) {
        WaitForSingleObject(rp->startEvent, INFINITE);
        if (!rp->running) break;
        
        for (int x = rp->startX; x < rp->endX; x++) {
            float rayAngle = (player.angle - FOV / 2.0f) + ((float)x / SCREEN_WIDTH) * FOV;
            float rayDirX = FastCos(rayAngle);
            float rayDirY = FastSin(rayAngle);
            
            int mapX = (int)player.x;
            int mapY = (int)player.y;
            
            float sideDistX, sideDistY;
            float deltaDistX = (rayDirX == 0) ? 1e30f : fabsf(1.0f / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30f : fabsf(1.0f / rayDirY);
            
            int stepX, stepY;
            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (player.x - mapX) * deltaDistX;
            } else {
                stepX = 1;
                sideDistX = (mapX + 1.0f - player.x) * deltaDistX;
            }
            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (player.y - mapY) * deltaDistY;
            } else {
                stepY = 1;
                sideDistY = (mapY + 1.0f - player.y) * deltaDistY;
            }
            
            bool hitWall = false;
            int side = 0;
            int wallType = 0;
            float distanceToWall = 0;
            
            while (!hitWall && distanceToWall < 90.0f) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                } else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                
                if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
                    hitWall = true;
                    wallType = 3;
                    distanceToWall = 90.0f;
                } else if (worldMap[mapX][mapY] > 0) {
                    hitWall = true;
                    wallType = worldMap[mapX][mapY];
                    if (side == 0) {
                        distanceToWall = (mapX - player.x + (1 - stepX) / 2.0f) / rayDirX;
                    } else {
                        distanceToWall = (mapY - player.y + (1 - stepY) / 2.0f) / rayDirY;
                    }
                }
            }
            
            float correctedDist = distanceToWall * cosf(rayAngle - player.angle);
            
            float wallX;
            if (side == 0) {
                wallX = player.y + distanceToWall * rayDirY;
            } else {
                wallX = player.x + distanceToWall * rayDirX;
            }
            wallX -= floorf(wallX);
            
            int ceiling, floorLine;
            if (wallType == 3 && distanceToWall >= 90.0f) {
                ceiling = 0;
                floorLine = SCREEN_HEIGHT / 2 + (int)player.pitch;
            } else {
                int wallHeight = (int)(SCREEN_HEIGHT / correctedDist);
                ceiling = SCREEN_HEIGHT / 2 - wallHeight / 2 + (int)player.pitch;
                floorLine = SCREEN_HEIGHT / 2 + wallHeight / 2 + (int)player.pitch;
            }
            
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                if (y <= SCREEN_HEIGHT / 2 + (int)player.pitch) {
                    float skyGradient = (float)y / (SCREEN_HEIGHT / 2);
                    int r, g, b;
                    
                    if (bossActive) {
                        r = (int)(150 + 100 * (1 - skyGradient));
                        g = (int)(20 * (1 - skyGradient));
                        b = (int)(20 * (1 - skyGradient));
                    } else {
                        r = (int)(30 + 80 * (1 - skyGradient));
                        g = (int)(60 + 120 * (1 - skyGradient));
                        b = (int)(100 + 155 * (1 - skyGradient));
                    }
                    
                    float horizonFog = skyGradient * skyGradient;
                    r = (int)(r * (1.0f - horizonFog) + 128 * horizonFog);
                    g = (int)(g * (1.0f - horizonFog) + 128 * horizonFog);
                    b = (int)(b * (1.0f - horizonFog) + 128 * horizonFog);
                    
                    renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(r, g, b);
                    zBuffer[y * SCREEN_WIDTH + x] = 1000.0f;
                }
                
                if (y > SCREEN_HEIGHT / 2 + (int)player.pitch) {
                    float rowDist = (SCREEN_HEIGHT / 2.0f) / (y - SCREEN_HEIGHT / 2.0f);
                    float floorX = player.x + FastCos(rayAngle) * rowDist;
                    float floorY = player.y + FastSin(rayAngle) * rowDist;
                    
                    if (grassPixels && grassW > 0) {
                        int texX = (int)(fmodf(floorX, 1.0f) * grassW);
                        int texY = (int)(fmodf(floorY, 1.0f) * grassH);
                        if (texX < 0) texX += grassW;
                        if (texY < 0) texY += grassH;
                        texX %= grassW; texY %= grassH;
                        DWORD col = grassPixels[texY * grassW + texX];
                        int bb = (col >> 0) & 0xFF;
                        int gg = (col >> 8) & 0xFF;
                        int rr = (col >> 16) & 0xFF;
                        float shade = 1.0f - (rowDist / 20.0f);
                        if (shade < 0.15f) shade = 0.15f;
                        
                        float fogStart = 17.0f;
                        float fogEnd = 30.0f;
                        float fogFactor = (rowDist - fogStart) / (fogEnd - fogStart);
                        if (fogFactor < 0.0f) fogFactor = 0.0f;
                        if (fogFactor > 1.0f) fogFactor = 1.0f;
                        
                        int finalR = (int)(rr * shade * (1.0f - fogFactor) + 128 * fogFactor);
                        int finalG = (int)(gg * shade * (1.0f - fogFactor) + 128 * fogFactor);
                        int finalB = (int)(bb * shade * (1.0f - fogFactor) + 128 * fogFactor);
                        
                        renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(finalR, finalG, finalB);
                    } else {
                        float shade = 1.0f - (rowDist / 40.0f);
                        if (shade < 0.1f) shade = 0.1f;
                        int c = (int)(80 * shade);
                        
                        float fogFactor = (rowDist - 17.0f) / 13.0f;
                        if (fogFactor < 0.0f) fogFactor = 0.0f;
                        if (fogFactor > 1.0f) fogFactor = 1.0f;
                        
                        int finalR = (int)((c/2) * (1.0f - fogFactor) + 128 * fogFactor);
                        int finalG = (int)(c * (1.0f - fogFactor) + 128 * fogFactor);
                        int finalB = (int)((c/2) * (1.0f - fogFactor) + 128 * fogFactor);
                        
                        renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(finalR, finalG, finalB);
                    }
                    
                    zBuffer[y * SCREEN_WIDTH + x] = rowDist;
                }
                
                if (y >= ceiling && y <= floorLine) {
                    float shade = 1.0f - (correctedDist / 50.0f);
                    if (shade < 0.1f) shade = 0.1f;
                    if (side == 1) shade *= 0.8f;
                    
                    float fogFactor = (correctedDist - 17.0f) / 13.0f;
                    if (fogFactor < 0.0f) fogFactor = 0.0f;
                    if (fogFactor > 1.0f) fogFactor = 1.0f;
                    
                    if (wallType == 3 && distanceToWall < 90.0f && borderWallPixels && borderWallW > 0 && borderWallH > 0) {
                        int texX = (int)(wallX * borderWallW);
                        if (texX < 0) texX = 0;
                        if (texX >= borderWallW) texX = borderWallW - 1;
                        
                        int wallHeight = floorLine - ceiling;
                        if (wallHeight <= 0) wallHeight = 1;
                        float texYf = (float)(y - ceiling) / (float)wallHeight;
                        int texY = (int)(texYf * borderWallH);
                        if (texY < 0) texY = 0;
                        if (texY >= borderWallH) texY = borderWallH - 1;
                        
                        DWORD col = borderWallPixels[texY * borderWallW + texX];
                        int bb = (col >> 0) & 0xFF;
                        int gg = (col >> 8) & 0xFF;
                        int rr = (col >> 16) & 0xFF;
                        int aa = (col >> 24) & 0xFF;
                        
                        if (aa > 0) {
                            int rFinal = (int)(rr * shade * (1.0f - fogFactor) + 128 * fogFactor);
                            int gFinal = (int)(gg * shade * (1.0f - fogFactor) + 128 * fogFactor);
                            int bFinal = (int)(bb * shade * (1.0f - fogFactor) + 128 * fogFactor);
                            renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(rFinal, gFinal, bFinal);
                            zBuffer[y * SCREEN_WIDTH + x] = correctedDist;
                        }
                    } else if (wallType == 4 && gateWallPixels && gateWallW > 0 && gateWallH > 0) {
                        int texX = (int)(wallX * gateWallW);
                        if (texX < 0) texX = 0;
                        if (texX >= gateWallW) texX = gateWallW - 1;
                        
                        int wallHeight = floorLine - ceiling;
                        if (wallHeight <= 0) wallHeight = 1;
                        float texYf = (float)(y - ceiling) / (float)wallHeight;
                        int texY = (int)(texYf * gateWallH);
                        if (texY < 0) texY = 0;
                        if (texY >= gateWallH) texY = gateWallH - 1;
                        
                        DWORD col = gateWallPixels[texY * gateWallW + texX];
                        int bb = (col >> 0) & 0xFF;
                        int gg = (col >> 8) & 0xFF;
                        int rr = (col >> 16) & 0xFF;
                        int aa = (col >> 24) & 0xFF;
                        
                        if (aa > 0) {
                            int rFinal = (int)(rr * shade * (1.0f - fogFactor) + 128 * fogFactor);
                            int gFinal = (int)(gg * shade * (1.0f - fogFactor) + 128 * fogFactor);
                            int bFinal = (int)(bb * shade * (1.0f - fogFactor) + 128 * fogFactor);
                            renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(rFinal, gFinal, bFinal);
                            zBuffer[y * SCREEN_WIDTH + x] = correctedDist;
                        }
                    } else if (wallType != 3 && wallType != 4) {
                        int r, g, b;
                        if (wallType == 2) {
                            r = (int)(60 * shade); g = (int)(100 * shade); b = (int)(40 * shade);
                        } else {
                            r = (int)(140 * shade); g = (int)(100 * shade); b = (int)(60 * shade);
                        }
                        
                        int rFinal = (int)(r * (1.0f - fogFactor) + 128 * fogFactor);
                        int gFinal = (int)(g * (1.0f - fogFactor) + 128 * fogFactor);
                        int bFinal = (int)(b * (1.0f - fogFactor) + 128 * fogFactor);
                        
                        renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(rFinal, gFinal, bFinal);
                        
                        zBuffer[y * SCREEN_WIDTH + x] = correctedDist;
                    }
                }
            }
        }
        
        SetEvent(rp->doneEvent);
    }
    
    return 0;
}

void InitThreadPool() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    numRayThreads = sysInfo.dwNumberOfProcessors;
    if (numRayThreads < 1) numRayThreads = 1;
    if (numRayThreads > 16) numRayThreads = 16;
    
    rayThreads = new HANDLE[numRayThreads];
    threadParams = new RaycastParams[numRayThreads];
    
    int columnsPerThread = SCREEN_WIDTH / numRayThreads;
    
    for (int i = 0; i < numRayThreads; i++) {
        threadParams[i].startX = i * columnsPerThread;
        threadParams[i].endX = (i == numRayThreads - 1) ? SCREEN_WIDTH : (i + 1) * columnsPerThread;
        threadParams[i].startEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        threadParams[i].doneEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        threadParams[i].running = true;
        rayThreads[i] = (HANDLE)_beginthreadex(NULL, 0, RaycastWorker, &threadParams[i], 0, NULL);
    }
}

void CleanupThreadPool() {
    for (int i = 0; i < numRayThreads; i++) {
        threadParams[i].running = false;
        SetEvent(threadParams[i].startEvent);
    }
    WaitForMultipleObjects(numRayThreads, rayThreads, TRUE, 1000);
    for (int i = 0; i < numRayThreads; i++) {
        CloseHandle(rayThreads[i]);
        CloseHandle(threadParams[i].startEvent);
        CloseHandle(threadParams[i].doneEvent);
    }
    delete[] rayThreads;
    delete[] threadParams;
}

void CastRays() {
    for (int i = 0; i < numRayThreads; i++) {
        staticDoneEvents[i] = threadParams[i].doneEvent;
        SetEvent(threadParams[i].startEvent);
    }
    
    WaitForMultipleObjects(numRayThreads, staticDoneEvents, TRUE, INFINITE);
}

bool g_mapUploaded = false;

void UploadMapToGPU() {
    if (!g_gpuRaycastAvailable || !g_mapSSBO) return;
    
    int mapData[MAP_WIDTH * MAP_HEIGHT];
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            mapData[x * MAP_HEIGHT + y] = worldMap[x][y];
        }
    }
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_mapSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(mapData), mapData, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    g_mapUploaded = true;
}

void UploadTextureToGPU(GLuint& tex, DWORD* pixels, int w, int h, int texUnit) {
    if (!pixels || w <= 0 || h <= 0) return;
    
    if (tex == 0) {
        glGenTextures(1, &tex);
    }
    
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
}

void GPUCastRays() {
    if (!g_gpuRaycastAvailable) {
        CastRays();
        return;
    }
    
    if (!g_mapUploaded) {
        UploadMapToGPU();
        UploadTextureToGPU(g_grassTex, grassPixels, grassW, grassH, 3);
        UploadTextureToGPU(g_borderWallTex, borderWallPixels, borderWallW, borderWallH, 4);
        UploadTextureToGPU(g_gateWallTex, gateWallPixels, gateWallW, gateWallH, 5);
    }
    
    glUseProgram(g_raycastProgram);
    
    glUniform2f(g_rcLocPlayerPos, player.x, player.y);
    glUniform1f(g_rcLocPlayerAngle, player.angle);
    glUniform1f(g_rcLocPlayerPitch, player.pitch);
    glUniform1i(g_rcLocBossActive, bossActive ? 1 : 0);
    
    GLint screenSizeLoc = glGetUniformLocation(g_raycastProgram, "screenSize");
    if (screenSizeLoc >= 0) {
        typedef void (*PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint* value);
        PFNGLUNIFORM2IVPROC glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
        if (glUniform2iv) {
            GLint screenSize[2] = {SCREEN_WIDTH, SCREEN_HEIGHT};
            glUniform2iv(screenSizeLoc, 1, screenSize);
        }
    }
    
    glBindImageTexture(0, g_renderTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glBindImageTexture(1, g_zBufferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, g_mapSSBO);
    
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, g_grassTex);
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, g_borderWallTex);
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, g_gateWallTex);
    
    GLuint groupsX = (SCREEN_WIDTH + 15) / 16;
    GLuint groupsY = (SCREEN_HEIGHT + 15) / 16;
    glDispatchCompute(groupsX, groupsY, 1);
    
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    glBindTexture(GL_TEXTURE_2D, g_renderTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, renderBuffer);
    
    static float* gpuZBuffer = nullptr;
    if (!gpuZBuffer) {
        gpuZBuffer = new float[SCREEN_WIDTH * SCREEN_HEIGHT];
    }
    glBindTexture(GL_TEXTURE_2D, g_zBufferTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, gpuZBuffer);
    
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        zBuffer[i] = gpuZBuffer[i];
    }
    
    glUseProgram(0);
}

// --- 3D Rasterizer ---
void RasterizeTri(Vec3 v1, Vec3 v2, Vec3 v3, DWORD color) {
    int x1 = (int)((v1.x + 1) * 0.5f * SCREEN_WIDTH);
    int y1 = (int)((1 - v1.y) * 0.5f * SCREEN_HEIGHT);
    int x2 = (int)((v2.x + 1) * 0.5f * SCREEN_WIDTH);
    int y2 = (int)((1 - v2.y) * 0.5f * SCREEN_HEIGHT);
    int x3 = (int)((v3.x + 1) * 0.5f * SCREEN_WIDTH);
    int y3 = (int)((1 - v3.y) * 0.5f * SCREEN_HEIGHT);
    
    int minX = std::max(0, std::min(x1, std::min(x2, x3)));
    int minY = std::max(0, std::min(y1, std::min(y2, y3)));
    int maxX = std::min(SCREEN_WIDTH-1, std::max(x1, std::max(x2, x3)));
    int maxY = std::min(SCREEN_HEIGHT-1, std::max(y1, std::max(y2, y3)));
    
    float area = EdgeFunc(x1, y1, x2, y2, x3, y3);
    if(area == 0) return;
    
    for(int y=minY; y<=maxY; y++) {
        for(int x=minX; x<=maxX; x++) {
            float w0 = EdgeFunc(x2, y2, x3, y3, x, y);
            float w1 = EdgeFunc(x3, y3, x1, y1, x, y);
            float w2 = EdgeFunc(x1, y1, x2, y2, x, y);
            
            bool inside = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
            
            if(inside) {
                w0/=area; w1/=area; w2/=area;
                float z = 1.0f / (w0/v1.z + w1/v2.z + w2/v3.z);
                
                // Z-Buffer Check
                if(z < zBuffer[y * SCREEN_WIDTH + x]) {
                    zBuffer[y * SCREEN_WIDTH + x] = z;
                    renderBuffer[y * SCREEN_WIDTH + x] = color;
                }
            }
        }
    }
}

void LoadModelCurrentDir(const wchar_t* filename, float x, float z) {
    // Helper to find file in current dir or assets
    FILE* f = _wfopen(filename, L"rb");
    if(!f) return;
    
    int magic, objCount;
    fread(&magic, 4, 1, f);
    fread(&objCount, 4, 1, f);
    for(int i=0; i<objCount; i++) {
        Object3D obj;
        fread(&obj.pos, sizeof(Vec3), 1, f);
        fread(&obj.rot, sizeof(Vec3), 1, f);
        
        // Offset & Scale
        float scale = 5.0f;
        obj.pos.x = (obj.pos.x * scale) + x;
        obj.pos.y = (obj.pos.y * scale); // Ground level
        obj.pos.z = (obj.pos.z * scale) + z;
        
        for(auto& v : obj.verts) {
            v.pos = Mul(v.pos, scale);
        }
        
        int vCount, tCount;
        fread(&vCount, 4, 1, f);
        fread(&tCount, 4, 1, f);
        obj.verts.resize(vCount);
        obj.tris.resize(tCount);
        fread(obj.verts.data(), sizeof(Vertex), vCount, f);
        fread(obj.tris.data(), sizeof(Triangle), tCount, f);
        scene3D.push_back(obj);
    }
    fclose(f);
}

void Render3DScene() {
    Vec3 lightDir = Normalize({0.5f, 1.0f, -0.5f});
    
    Mat4 matTrans = MatrixTranslation(-player.x, -2.0f, -player.y);
    
    Mat4 matRotY = MatrixRotationY(-player.angle + PI/2);
    Mat4 matRotX = MatrixRotationX(-player.pitch/100.0f);
    Mat4 matProj = MatrixPerspective(FOV, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);
    
    Mat4 matView = MatrixMultiply(matRotX, MatrixMultiply(matRotY, matTrans));
    // Actually MatrixMultiply order: M = R * T
    
    for(auto& obj : scene3D) {
        Mat4 modelMat = MatrixMultiply(MatrixRotationY(obj.rot.y), MatrixTranslation(obj.pos.x, obj.pos.y, obj.pos.z));
        // We accumulate transformation
        
        for(auto& tri : obj.tris) {
            Vec3 v1 = TransformPoint(modelMat, obj.verts[tri.p1].pos);
            Vec3 v2 = TransformPoint(modelMat, obj.verts[tri.p2].pos);
            Vec3 v3 = TransformPoint(modelMat, obj.verts[tri.p3].pos);
            
            // Lighting
            Vec3 normal = Normalize(Cross(Sub(v2,v1), Sub(v3,v1)));
            float intensity = Dot(normal, lightDir);
            if(intensity < 0.2f) intensity = 0.2f;
            
            // View Points
            Vec3 tv1 = TransformPoint(matView, v1);
            Vec3 tv2 = TransformPoint(matView, v2);
            Vec3 tv3 = TransformPoint(matView, v3);
            
            // Clip
            if(tv1.z < 0.1f || tv2.z < 0.1f || tv3.z < 0.1f) continue;
            
            // Project
            Vec3 p1 = TransformPoint(matProj, tv1);
            Vec3 p2 = TransformPoint(matProj, tv2);
            Vec3 p3 = TransformPoint(matProj, tv3);
            
            DWORD c = tri.color;
            int r = (c >> 16) & 0xFF; int g = (c >> 8) & 0xFF; int b = (c) & 0xFF;
            r*=intensity; g*=intensity; b*=intensity;
            DWORD litColor = (r<<16)|(g<<8)|b;
            
            RasterizeTri(p1, p2, p3, litColor);
        }
    }
}

void RenderSprite(DWORD* pixels, int pxW, int pxH, float sx, float sy, float dist, float scale, float heightOffset = 0.0f) {
    if (dist < 0.5f || dist > g_RenderDistance) return;
    if (!pixels || pxW <= 0 || pxH <= 0) return;
    
    float dx = sx - player.x;
    float dy = sy - player.y;
    float spriteAngle = atan2f(dy, dx) - player.angle;
    while (spriteAngle > PI) spriteAngle -= 2 * PI;
    while (spriteAngle < -PI) spriteAngle += 2 * PI;
    if (fabsf(spriteAngle) > FOV) return;
    
    float spriteScreenX = (0.5f + spriteAngle / FOV) * SCREEN_WIDTH;
    float spriteHeight = (SCREEN_HEIGHT / dist) * scale;
    float spriteWidth = spriteHeight;
    
    int floorLineAtDist = SCREEN_HEIGHT / 2 + (int)((SCREEN_HEIGHT / 2.0f) / dist);
    int verticalOffset = (int)((heightOffset * SCREEN_HEIGHT) / dist);
    int drawEndY = floorLineAtDist - verticalOffset;
    int drawStartY = (int)(drawEndY - spriteHeight);
    int drawStartX = (int)(spriteScreenX - spriteWidth / 2);
    int drawEndX = (int)(spriteScreenX + spriteWidth / 2);
    
    if (drawEndX <= 0 || drawStartX >= SCREEN_WIDTH) return;
    if (drawEndY <= 0 || drawStartY >= SCREEN_HEIGHT) return;
    
    int clampedStartX = drawStartX < 0 ? 0 : drawStartX;
    int clampedEndX = drawEndX > SCREEN_WIDTH ? SCREEN_WIDTH : drawEndX;
    int clampedStartY = drawStartY < 0 ? 0 : drawStartY;
    int clampedEndY = drawEndY > SCREEN_HEIGHT ? SCREEN_HEIGHT : drawEndY;
    
    float shade = 1.0f - (dist / 40.0f);
    if (shade < 0.15f) shade = 0.15f;
    int shadeFixed = (int)(shade * 256);
    
    float fogFactor = (dist - 17.0f) / 13.0f;
    if (fogFactor < 0.0f) fogFactor = 0.0f;
    if (fogFactor > 1.0f) fogFactor = 1.0f;
    int fogFixed = (int)(fogFactor * 256);
    int invFogFixed = 256 - fogFixed;
    
    float invSpriteWidth = 1.0f / spriteWidth;
    float invSpriteHeight = 1.0f / spriteHeight;
    
    for (int x = clampedStartX; x < clampedEndX; x++) {
        int tx = (int)(((x - drawStartX) * invSpriteWidth) * pxW);
        if (tx < 0 || tx >= pxW) continue;
        
        int texRowBase = tx;
        
        for (int y = clampedStartY; y < clampedEndY; y++) {
            int bufIdx = y * SCREEN_WIDTH + x;
            if (dist > zBuffer[bufIdx]) continue;
            
            int ty = (int)(((y - drawStartY) * invSpriteHeight) * pxH);
            if (ty < 0 || ty >= pxH) continue;
            
            DWORD col = pixels[ty * pxW + tx];
            int a = (col >> 24) & 0xFF;
            if (a == 0) continue;
            
            int b = (col >> 0) & 0xFF;
            int g = (col >> 8) & 0xFF;
            int r = (col >> 16) & 0xFF;
            
            // Apply shade then fog
            int rShaded = (r * shadeFixed) >> 8;
            int gShaded = (g * shadeFixed) >> 8;
            int bShaded = (b * shadeFixed) >> 8;
            
            int rFinal = (rShaded * invFogFixed + 128 * fogFixed) >> 8;
            int gFinal = (gShaded * invFogFixed + 128 * fogFixed) >> 8;
            int bFinal = (bShaded * invFogFixed + 128 * fogFixed) >> 8;
            
            renderBuffer[bufIdx] = MakeColor(rFinal, gFinal, bFinal);
        }
    }
}

void RenderSprites() {
    g_allSprites.clear();
    
    DWORD* sPix = NULL;
    int sW, sH;
    if (bossDead) {
        sPix = spireDeathPixels;
        sW = spireDeathW;
        sH = spireDeathH;
    } else if (bossHurtTimer > 0 && bossActive) {
        sPix = spireHurtPixels;
        sW = spireHurtW;
        sH = spireHurtH;
    }
    if (sPix == NULL) {
        if (phase2Active && !enragedMode) {
            sPix = spirePhase2Pixels[phase2BossFrame];
            sW = spirePhase2W[phase2BossFrame];
            sH = spirePhase2H[phase2BossFrame];
        } else if (bossActive && !bossDead) {
            sPix = spireAwakePixels;
            sW = spireAwakeW;
            sH = spireAwakeH;
        } else {
            sPix = spirePixels;
            sW = spireW;
            sH = spireH;
        }
    }
    
    float dx = 32.0f - player.x;
    float dy = 32.0f - player.y;
    float dist = sqrtf(dx*dx + dy*dy);
    g_allSprites.push_back({32.0f, 32.0f, dist, 2, 8.0f, 0, false, 0.0f, false});

    // Render Graves
    for(const auto& g : graves) {
        float gdx = g.x - player.x;
        float gdy = g.y - player.y;
        float gdist = sqrtf(gdx*gdx + gdy*gdy);
        g_allSprites.push_back({g.x, g.y, gdist, 99, 1.0f, 0, false, 0.0f, false});
    }

    for(auto& fb : fireballs) {
        if(!fb.active) continue;
        float fdx = fb.x - player.x;
        float fdy = fb.y - player.y;
        float fdist = sqrtf(fdx*fdx + fdy*fdy);
        g_allSprites.push_back({fb.x, fb.y, fdist, 3, 2.0f, 0, false, 0.0f, false});
    }
    
    for (int i = 0; i < 3; i++) {
        if (medkits[i].active) {
            float mdx = medkits[i].x - player.x;
            float mdy = medkits[i].y - player.y;
            float mdist = sqrtf(mdx*mdx + mdy*mdy);
            g_allSprites.push_back({medkits[i].x, medkits[i].y, mdist, 4, 0.8f, 0, false, 0.0f, false});
        }
    }

    int playerCellX = (int)(player.x / GRID_CELL_SIZE);
    int playerCellY = (int)(player.y / GRID_CELL_SIZE);
    int searchRadius = 8;
    
    for (int cx = playerCellX - searchRadius; cx <= playerCellX + searchRadius; cx++) {
        for (int cy = playerCellY - searchRadius; cy <= playerCellY + searchRadius; cy++) {
            if (cx < 0 || cx >= 17 || cy < 0 || cy >= 17) continue;
            for (int idx : treeGrid[cx][cy]) {
                TreeSprite& tree = trees[idx];
                if (!IsInFrustum(tree.x, tree.y, player.x, player.y, player.angle, FOV * 0.7f)) continue;
                float dx = tree.x - player.x;
                float dy = tree.y - player.y;
                float distSq = dx*dx + dy*dy;
                if (distSq < 900.0f) {
                    float dist = sqrtf(distSq);
                    g_allSprites.push_back({tree.x, tree.y, dist, 0, 6.0f, 0, false, 0.0f, false});
                }
            }
        }
    }
    
    for (int cx = playerCellX - searchRadius; cx <= playerCellX + searchRadius; cx++) {
        for (int cy = playerCellY - searchRadius; cy <= playerCellY + searchRadius; cy++) {
            if (cx < 0 || cx >= 17 || cy < 0 || cy >= 17) continue;
            for (int idx : grassGrid[cx][cy]) {
                GrassSprite& grass = grasses[idx];
                if (!IsInFrustum(grass.x, grass.y, player.x, player.y, player.angle, FOV * 0.7f)) continue;
                float dx = grass.x - player.x;
                float dy = grass.y - player.y;
                float distSq = dx*dx + dy*dy;
                if (distSq < 625.0f) {
                    float dist = sqrtf(distSq);
                    g_allSprites.push_back({grass.x, grass.y, dist, 11, 0.3f, 0, false, 0.0f, false});
                }
            }
        }
    }
    
    for (int cx = playerCellX - searchRadius; cx <= playerCellX + searchRadius; cx++) {
        for (int cy = playerCellY - searchRadius; cy <= playerCellY + searchRadius; cy++) {
            if (cx < 0 || cx >= 17 || cy < 0 || cy >= 17) continue;
            for (int idx : rockGrid[cx][cy]) {
                RockSprite& rock = rocks[idx];
                if (!IsInFrustum(rock.x, rock.y, player.x, player.y, player.angle, FOV * 0.7f)) continue;
                float dx = rock.x - player.x;
                float dy = rock.y - player.y;
                float distSq = dx*dx + dy*dy;
                if (distSq < 900.0f) {
                    float dist = sqrtf(distSq);
                    g_allSprites.push_back({rock.x, rock.y, dist, 12, 0.3f, rock.variant, false, 0.0f, false});
                }
            }
        }
    }
    
    for (int cx = playerCellX - searchRadius; cx <= playerCellX + searchRadius; cx++) {
        for (int cy = playerCellY - searchRadius; cy <= playerCellY + searchRadius; cy++) {
            if (cx < 0 || cx >= 17 || cy < 0 || cy >= 17) continue;
            for (int idx : bigRockGrid[cx][cy]) {
                BigRock& br = bigRocks[idx];
                if (!IsInFrustum(br.x, br.y, player.x, player.y, player.angle, FOV * 0.7f)) continue;
                float dx = br.x - player.x;
                float dy = br.y - player.y;
                float distSq = dx*dx + dy*dy;
                if (distSq < 1225.0f) {
                    float dist = sqrtf(distSq);
                    g_allSprites.push_back({br.x, br.y, dist, 14, 1.5f, br.variant, false, 0.0f, false});
                }
            }
        }
    }
    
    // Healing Tower
    {
        float dx = healingTower.x - player.x;
        float dy = healingTower.y - player.y;
        float distSq = dx*dx + dy*dy;
        if (distSq < 1225.0f) {
            float dist = sqrtf(distSq);
             // Type 20
             int variant = 0; // 0=Dormant, 1=Charging0, 2=Charging1, 3=Ready
             if (healingTower.state == TOWER_DORMANT) variant = 0;
             else if (healingTower.state == TOWER_CHARGING || healingTower.state == TOWER_COOLDOWN) variant = 1 + healingTower.pulseFrame;
             else variant = 3;
             
             // 2.0f scale for tower
             g_allSprites.push_back({healingTower.x, healingTower.y, dist, 20, 2.0f, variant, false, 0.0f, false});
        }
        
        // Particles
        for(const auto& p : healingTower.particles) {
             float px = healingTower.x + cosf(p.angle) * p.dist;
             float py = healingTower.y + sinf(p.angle) * p.dist;
             
             if (!IsInFrustum(px, py, player.x, player.y, player.angle, FOV * 0.7f)) continue;
             float pdx = px - player.x;
             float pdy = py - player.y;
             float pdistSq = pdx*pdx + pdy*pdy;
             if (pdistSq < 1225.0f) {
                 float pdist = sqrtf(pdistSq);
                 g_allSprites.push_back({px, py, pdist, 21, 0.5f, 0, false, p.height, false});
             }
        }
    }

    if (johnSecret.isSpawned) {
        float dx = johnSecret.x - player.x;
        float dy = johnSecret.y - player.y;
        float distSq = dx*dx + dy*dy;
        if (distSq < 1225.0f) {
            float dist = sqrtf(distSq);
            int variant = (healingTower.state == TOWER_ACTIVE) ? 1 : 0;
            g_allSprites.push_back({johnSecret.x, johnSecret.y, dist, 30, 0.5f, variant, false, johnSecret.hopOffset, false});
        }
    }

    for (int cx = playerCellX - searchRadius; cx <= playerCellX + searchRadius; cx++) {
        for (int cy = playerCellY - searchRadius; cy <= playerCellY + searchRadius; cy++) {
            if (cx < 0 || cx >= 17 || cy < 0 || cy >= 17) continue;
            for (int idx : bushGrid[cx][cy]) {
                BushSprite& bush = bushes[idx];
                if (!IsInFrustum(bush.x, bush.y, player.x, player.y, player.angle, FOV * 0.7f)) continue;
                float dx = bush.x - player.x;
                float dy = bush.y - player.y;
                float distSq = dx*dx + dy*dy;
                if (distSq < 900.0f) {
                    float dist = sqrtf(distSq);
                    g_allSprites.push_back({bush.x, bush.y, dist, 13, 0.6f, 0, false, 0.0f, false});
                }
            }
        }
    }
    
    for (auto& b : bullets) {
        if (b.active) {
            float dx = b.x - player.x;
            float dy = b.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            g_allSprites.push_back({b.x, b.y, dist, 22, 0.3f, 0, false, 0.0f, false});
        }
    }
    
    for (auto& enemy : enemies) {
        if (enemy.active) {
            float dx = enemy.x - player.x;
            float dy = enemy.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (enemy.isOfficer) {
                g_allSprites.push_back({enemy.x, enemy.y, dist, 24, 1.0f, enemy.officerState, (enemy.hurtTimer > 0), 0.0f, enemy.firingTimer > 0});
            } else if (enemy.isSpearGuy) {
                g_allSprites.push_back({enemy.x, enemy.y, dist, 23, 1.0f, enemy.spearState, (enemy.hurtTimer > 0), 0.0f, false});
            } else if (enemy.isMarshall) {
                g_allSprites.push_back({enemy.x, enemy.y, dist, 9, 2.5f, (enemy.hurtTimer > 0 ? 1 : 0), false, 0.0f, false});
            } else if (enemy.isShooter) {
                g_allSprites.push_back({enemy.x, enemy.y, dist, 6, 1.0f, 0, (enemy.hurtTimer > 0), 0.0f, enemy.firingTimer > 0});
            } else {
                g_allSprites.push_back({enemy.x, enemy.y, dist, 1, 1.0f, enemy.spriteIndex, (enemy.spriteIndex == 4 && enemy.hurtTimer > 0), 0.0f, false});
            }
        }
    }
    
    for (auto& eb : enemyBullets) {
        if (eb.active) {
            float dx = eb.x - player.x;
            float dy = eb.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            int bulletType = eb.isLaser ? 8 : 7;
            float scale = eb.isLaser ? 1.5f : 0.5f;
            float height = eb.isLaser ? 1.0f : 0.0f;
            g_allSprites.push_back({eb.x, eb.y, dist, bulletType, scale, 0, false, height, false});
        }
    }
    
    for (auto& p : allies) {
        if (p.active) {
            float dx = p.x - player.x;
            float dy = p.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (p.isDefectedOfficer) {
                g_allSprites.push_back({p.x, p.y, dist, 25, 1.0f, p.officerState, (p.hurtTimer > 0), 0.0f, p.firingTimer > 0});
            } else if (p.isDefectedGunner) {
                g_allSprites.push_back({p.x, p.y, dist, 26, 1.0f, 0, (p.hurtTimer > 0), 0.0f, p.firingTimer > 0});
            } else if (p.isParagon) {
                g_allSprites.push_back({p.x, p.y, dist, 10, 1.0f, 0, (p.hurtTimer > 0), 0.0f, false});
            }
        }
    }
    
    // Bazooka Projectiles
    for (auto& r : rockets) {
        if (r.active) {
            float dx = r.x - player.x;
            float dy = r.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            g_allSprites.push_back({r.x, r.y, dist, 14, 0.5f, 0, false, r.z, false});
        }
    }
    
    for (auto& t : rocketTrails) {
        if (t.active) {
            float dx = t.x - player.x;
            float dy = t.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            g_allSprites.push_back({t.x, t.y, dist, 16, 0.5f, 0, false, 0.0f, false});
        }
    }
    
    for (auto& ex : explosions) {
        if (ex.active) {
            float dx = ex.x - player.x;
            float dy = ex.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            g_allSprites.push_back({ex.x, ex.y, dist, 15, 1.5f, 0, false, ex.timer, false});
        }
    }
    
    for(int i = 0; i < 6; i++) {
        float cdx = claws[i].x - player.x;
        float cdy = claws[i].y - player.y;
        float cdist = sqrtf(cdx*cdx + cdy*cdy);
        int clawVariant = 0;
        bool isClawHurt = false;
        
        if (phase2Active && !enragedMode) {
            if (claws[i].state == CLAW_PH2_DEAD) {
                clawVariant = -1;
            } else {
                clawVariant = claws[i].animFrame;
                isClawHurt = (claws[i].hurtTimer > 0);
            }
        } else if (preBossPhase) {
            clawVariant = 3;
        } else if (!bossActive && !bossDead) {
            int activatedClaws = score / 50;
            if (activatedClaws > 6) activatedClaws = 6;
            if (i < activatedClaws) {
                clawVariant = 2;
            } else {
                clawVariant = 0;
            }
        } else {
            clawVariant = (claws[i].state == CLAW_DORMANT || bossDead) ? 0 : 1;
        }
        
        float clawHeight = 6.0f;
        if (claws[i].state == CLAW_PH2_ANCHORED) {
            clawHeight = 0.5f; // Ground level
        } else if (claws[i].state == CLAW_PH2_DROPPING) {
            float progress = 1.0f - (claws[i].timer / 2.0f); // 0 to 1
            if (progress < 0) progress = 0; if (progress > 1) progress = 1;
            // Lerp from 6.0 to 0.5
            clawHeight = 6.0f * (1.0f - progress) + 0.5f * progress;
        } else if (claws[i].state == CLAW_SLAMMING) {
            float progress = 1.0f - (claws[i].timer / 0.5f);
            if (progress < 0) progress = 0;
            if (progress > 1) progress = 1;
            clawHeight = 6.0f * (1.0f - progress);
        } else if (claws[i].state == CLAW_RISING) {
            float progress = 1.0f - (claws[i].timer / 1.0f);
            if (progress < 0) progress = 0;
            if (progress > 1) progress = 1;
            clawHeight = 6.0f * progress;
        } else if (claws[i].state == CLAW_RETURNING) {
            clawHeight = 6.0f;
        } else if (claws[i].state == CLAW_PH2_RISING) {
            float progress = 1.0f - (claws[i].timer / 2.0f); // 0 to 1 over 2s
            if (progress < 0) progress = 0; if (progress > 1) progress = 1;
            clawHeight = 0.5f * (1.0f - progress) + 6.0f * progress; // 0.5 to 6.0
        } else if (claws[i].state == CLAW_PH2_DEAD) {
            clawHeight = 6.0f;
        }
        
        g_allSprites.push_back({claws[i].x, claws[i].y, cdist, 5, 8.0f, clawVariant, isClawHurt, clawHeight, false});
    }
    
    if (postBossPhase) {
        for (auto& npc : NPCSystem::npcs) {
            if (!npc.active) continue;
            float dx = npc.x - player.x;
            float dy = npc.y - player.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 50.0f && dist > 0.5f) {
                int type = (npc.name == L"Leader") ? 17 : 18;
                g_allSprites.push_back({npc.x, npc.y, dist, type, 1.0f, 0, npc.isTalking, 0.0f, false});
            }
        }
    }
    
    if (spectatorMode) {
        float pdx = savedPlayerX - player.x; // player.x is now camera/spectator pos
        float pdy = savedPlayerY - player.y;
        float pdist = sqrtf(pdx*pdx + pdy*pdy);
        if (pdist < 50.0f && pdist > 0.5f) {
             g_allSprites.push_back({savedPlayerX, savedPlayerY, pdist, 19, 1.0f, 0, false, 0.0f, false});
        }
    }
    
    // Gravitals
    for(auto& g : gravitals) {
        if (!g.active) continue;
        float dx = g.x - player.x;
        float dy = g.y - player.y;
        float dist = sqrtf(dx*dx + dy*dy);
        // Height 2.0f to float above ground
        g_allSprites.push_back({g.x, g.y, dist, 69, 5.0f, 0, (g.hurtTimer > 0), g.z, false});
    }
    
    std::sort(g_allSprites.begin(), g_allSprites.end(), [](const SpriteRender& a, const SpriteRender& b) {
        return a.dist > b.dist;
    });
    
    for (size_t i = 0; i < g_allSprites.size(); ++i) {
        auto& sp = g_allSprites[i];

        if (sp.type == 99) {
             if (gravePixels) RenderSprite(gravePixels, graveW, graveH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 0) {
            RenderSprite(treePixels, treeW, treeH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 1) {
            if (sp.isHurt) {
                RenderSprite(enemy5HurtPixels, enemy5HurtW, enemy5HurtH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else {
                int idx = sp.variant;
                if (idx < 0) idx = 0; if (idx > 4) idx = 4;
                RenderSprite(enemyPixels[idx], enemyW[idx], enemyH[idx], sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 2) {
            RenderSprite(sPix, sW, sH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 14 && sp.height == 0.0f) { // Big Rock (distinguish from rocket by height)
             RenderSprite(bigRockPixels[sp.variant], bigRockW[sp.variant], bigRockH[sp.variant], sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 13) {
            if (bushPixels) {
                RenderSprite(bushPixels, bushW, bushH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 3) {
            RenderSprite(fireballPixels, fireballW, fireballH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 4) {
             RenderSprite(medkitPixels, medkitW, medkitH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 14) { // Rocket
             RenderSprite(rocketProjPixels, rocketProjW, rocketProjH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 15) { // Explosion
             RenderSprite(explosionPixels, explosionW, explosionH, sp.x, sp.y, sp.dist, sp.scale * (1.0f + (1.0f - sp.height)), 0.0f);
        } else if (sp.type == 69) { // Gravital
             if (sp.isHurt && gravitalHurtPixels) RenderSprite(gravitalHurtPixels, gravitalHurtW, gravitalHurtH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
             else if (gravitalPixels) RenderSprite(gravitalPixels, gravitalW, gravitalH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 16) { // Trail
             RenderSprite(rocketTrailPixels, rocketTrailW, rocketTrailH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 5) {
            if (phase2Active && sp.variant >= 0 && !enragedMode) {
                if (sp.isHurt) {
                    RenderSprite(clawHurtPixels, clawHurtW, clawHurtH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                } else {
                    int idx = sp.variant;
                    if (idx < 0) idx = 0; if (idx > 3) idx = 3;
                    RenderSprite(clawPhase2Pixels[idx], clawPhase2W[idx], clawPhase2H[idx], sp.x, sp.y, sp.dist, sp.scale, sp.height);
                }
            } else {
                if (sp.variant == 0 || sp.variant == -1) {
                    RenderSprite(clawDormantPixels, clawDormantW, clawDormantH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                } else if (sp.variant == 2) {
                    if (clawActivatingPixels) {
                        RenderSprite(clawActivatingPixels, clawActivatingW, clawActivatingH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                    } else {
                        RenderSprite(clawActivePixels, clawActiveW, clawActiveH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                    }
                } else if (sp.variant == 3) {
                    if (preBossPulseFrame && clawActivatingPixels) {
                        RenderSprite(clawActivatingPixels, clawActivatingW, clawActivatingH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                    } else {
                        RenderSprite(clawActivePixels, clawActiveW, clawActiveH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                    }
                } else {
                    RenderSprite(clawActivePixels, clawActiveW, clawActiveH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
                }
            }
        } else if (sp.type == 6) {
            if (sp.isHurt && gunnerHurtPixels) {
                RenderSprite(gunnerHurtPixels, gunnerHurtW, gunnerHurtH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else if (sp.isFiring) {
                RenderSprite(gunnerFiringPixels, gunnerFiringW, gunnerFiringH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else {
                RenderSprite(gunnerPixels, gunnerW, gunnerH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 9) { // Marshall
            if (sp.variant == 1) {
                RenderSprite(marshallHurtPixels, marshallHurtW, marshallHurtH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else {
                RenderSprite(marshallPixels, marshallW, marshallH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 7) {
            RenderSprite(bulletPixels, bulletW, bulletH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 8) {
            if (laserPixels) {
                RenderSprite(laserPixels, laserW, laserH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else {
                RenderSprite(bulletPixels, bulletW, bulletH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 10) {
            if (sp.isHurt && paragonHurtPixels) {
                RenderSprite(paragonHurtPixels, paragonHurtW, paragonHurtH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else if (paragonPixels) {
                RenderSprite(paragonPixels, paragonW, paragonH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 11) {
            if (grassPlantPixels) {
                RenderSprite(grassPlantPixels, grassPlantW, grassPlantH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 12) {
            int v = sp.variant;
            if (v < 0 || v > 2) v = 0;
            if (rockPixels[v]) {
                RenderSprite(rockPixels[v], rockW[v], rockH[v], sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 13) {
            if (bushPixels) {
                RenderSprite(bushPixels, bushW, bushH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 17) {
            if (sp.isHurt && leaderTalkingPixels) {
                RenderSprite(leaderTalkingPixels, leaderTalkingW, leaderTalkingH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            } else if (leaderIdlePixels) {
                RenderSprite(leaderIdlePixels, leaderIdleW, leaderIdleH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 18) {
            if (followerPixels) {
                RenderSprite(followerPixels, followerW, followerH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 19) {
            if (playerSpritePixels) {
                 RenderSprite(playerSpritePixels, playerSpriteW, playerSpriteH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
            }
        } else if (sp.type == 20) { // Healing Tower
             DWORD* pix = htDormantPixels; 
             int w = htDormantW, h = htDormantH;
             if (sp.variant == 1) { pix = htChargingPixels[0]; w = htChargingW[0]; h = htChargingH[0]; }
             else if (sp.variant == 2) { pix = htChargingPixels[1]; w = htChargingW[1]; h = htChargingH[1]; }
             else if (sp.variant == 3) { pix = htReadyPixels; w = htReadyW; h = htReadyH; }
             
             if (pix) RenderSprite(pix, w, h, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 21) { // Particles
             if (htParticlePixels) RenderSprite(htParticlePixels, htParticleW, htParticleH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 22) {
             if (playerBulletPixels) RenderSprite(playerBulletPixels, playerBulletW, playerBulletH, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 23) {
             DWORD* pix = spearguyMovePixels; int w = spearguyMoveW; int h = spearguyMoveH;
             if (sp.isHurt) { pix = spearguyHurtPixels; w = spearguyHurtW; h = spearguyHurtH; }
             else if (sp.variant == 1) { pix = spearguyIdlePixels; w = spearguyIdleW; h = spearguyIdleH; }
             else if (sp.variant == 2) { pix = spearguyDashPixels; w = spearguyDashW; h = spearguyDashH; }
             else if (sp.variant == 3) { pix = spearguyBlockPixels; w = spearguyBlockW; h = spearguyBlockH; }
             if (!pix && sp.isHurt) { pix = spearguyMovePixels; w = spearguyMoveW; h = spearguyMoveH; }
             if (pix) RenderSprite(pix, w, h, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 24) { // Officer
             DWORD* pix = officerMovePixels; int w = officerMoveW; int h = officerMoveH;
             if (sp.isHurt) { pix = officerHurtPixels; w = officerHurtW; h = officerHurtH; }
             else if (sp.isFiring) { pix = officerFirePixels; w = officerFireW; h = officerFireH; }
             else if (sp.variant == 1) { pix = officerIdlePixels; w = officerIdleW; h = officerIdleH; }
             if (!pix && sp.isHurt) { pix = officerMovePixels; w = officerMoveW; h = officerMoveH; }
             if (pix) RenderSprite(pix, w, h, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 25) { // Defected Officer
             DWORD* pix = defectedMovingPixels; int w = defectedMovingW; int h = defectedMovingH;
             if (sp.isHurt) { pix = officerHurtPixels; w = officerHurtW; h = officerHurtH; } // Uses officer hurt
             else if (sp.isFiring) { pix = defectedFiringPixels; w = defectedFiringW; h = defectedFiringH; }
             else if (sp.variant == 1) { pix = defectedIdlePixels; w = defectedIdleW; h = defectedIdleH; }
             if (!pix && sp.isHurt) { pix = defectedMovingPixels; w = defectedMovingW; h = defectedMovingH; }
             if (pix) RenderSprite(pix, w, h, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 26) { // Defected Gunner
             DWORD* pix = defectedGunnerPixels; int w = defectedGunnerW; int h = defectedGunnerH;
             if (sp.isHurt) { pix = gunnerHurtPixels; w = gunnerHurtW; h = gunnerHurtH; } // Uses gunner hurt
             else if (sp.isFiring) { pix = defectedGunnerFiringPixels; w = defectedGunnerFiringW; h = defectedGunnerFiringH; }
             if (!pix && sp.isHurt) { pix = defectedGunnerPixels; w = defectedGunnerW; h = defectedGunnerH; }
             if (pix) RenderSprite(pix, w, h, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        } else if (sp.type == 30) { // John
             DWORD* pix = (sp.variant == 1) ? johnInteractPixels : johnDefaultPixels;
             int w = (sp.variant == 1) ? johnInteractW : johnDefaultW;
             int h = (sp.variant == 1) ? johnInteractH : johnDefaultH;
             if (pix) RenderSprite(pix, w, h, sp.x, sp.y, sp.dist, sp.scale, sp.height);
        }
    }
}

void UpdateClouds(float deltaTime) {
    for (auto& cloud : clouds) {
        cloud.x += cloud.speed * deltaTime;
        if (cloud.x > 100.0f) cloud.x = -50.0f;
    }
}

void RenderClouds() {
    if (!cloudPixels || cloudW <= 0 || cloudH <= 0) return;
    
    for (auto& cloud : clouds) {
        if (!IsInFrustum(cloud.x, cloud.y, player.x, player.y, player.angle, FOV * 0.7f)) continue;
        
        float dx = cloud.x - player.x;
        float dy = cloud.y - player.y;
        float distSq = dx*dx + dy*dy;
        
        if (distSq < 25.0f || distSq > 10000.0f) continue;
        float dist = sqrtf(distSq);
        
        float cloudAngle = atan2f(dy, dx) - player.angle;
        while (cloudAngle > PI) cloudAngle -= 2 * PI;
        while (cloudAngle < -PI) cloudAngle += 2 * PI;
        if (fabsf(cloudAngle) > FOV) continue;
        
        float cloudScreenX = (0.5f + cloudAngle / FOV) * SCREEN_WIDTH;
        float cloudSize = (SCREEN_HEIGHT * 0.8f) / (dist * 0.08f);
        if (cloudSize > 350) cloudSize = 350;
        if (cloudSize < 30) continue;
        
        int horizon = SCREEN_HEIGHT / 2 + (int)player.pitch;
        int skyY = 60 + (int)((cloud.height - 15.0f) * 3.0f) + (int)player.pitch;
        if (skyY < 20) skyY = 20;
        if (skyY > horizon - 50) skyY = horizon - 50;
        
        int drawStartX = (int)(cloudScreenX - cloudSize / 2);
        int drawEndX = (int)(cloudScreenX + cloudSize / 2);
        int drawStartY = skyY;
        int drawEndY = (int)(skyY + cloudSize * 0.5f);
        if (drawEndY > horizon) drawEndY = horizon;
        
        float fade = 1.0f - (dist / 100.0f);
        if (fade < 0.4f) fade = 0.4f;
        int fadeFix = (int)(fade * 256);
        
        for (int x = drawStartX; x < drawEndX; x++) {
            if (x < 0 || x >= SCREEN_WIDTH) continue;
            float texX = (float)(x - drawStartX) / (drawEndX - drawStartX);
            
            for (int y = drawStartY; y < drawEndY; y++) {
                if (y < 0 || y >= horizon) continue;
                float texY = (float)(y - drawStartY) / (drawEndY - drawStartY);
                
                int tx = (int)(texX * cloudW);
                int ty = (int)(texY * cloudH);
                if (tx < 0 || tx >= cloudW || ty < 0 || ty >= cloudH) continue;
                
                DWORD col = cloudPixels[ty * cloudW + tx];
                int b = (col >> 0) & 0xFF;
                int g = (col >> 8) & 0xFF;
                int r = (col >> 16) & 0xFF;
                int a = (col >> 24) & 0xFF;
                if (a == 0) continue;
                
                renderBuffer[y * SCREEN_WIDTH + x] = MakeColor(
                    (r * fadeFix) >> 8, (g * fadeFix) >> 8, (b * fadeFix) >> 8);
            }
        }
    }
}

void UpdateEnemies(float deltaTime) {
    marshallHealthBarActive = false;
    militiaBarActive = false;
    
    if (postBossPhase) {
        for (auto& e : enemies) e.active = false;
        enemies.clear();
        return;
    }
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        if (enemy.hurtTimer > 0) enemy.hurtTimer -= deltaTime;
        
        if (enemy.hasNeuralBrain && !enemy.isMarshall) {
            enemy.brain.survivalTime += deltaTime;
        }
        
        // Marshall UI Sync
        if (enemy.isMarshall) {
             marshallHealthBarActive = true;
             marshallHP = enemy.health;
             marshallX = enemy.x;
             marshallY = enemy.y;
             
             militiaBarActive = true;
             militiaCount = 0;
             for (const auto& e : enemies) {
                 if (e.active && !e.isMarshall) militiaCount++;
             }
             if (militiaCount > militiaMaxCount) militiaMaxCount = militiaCount;
        }
        
        // Marshall AI
        if (enemy.isMarshall) {
             // Default Command Reset
             activeCommand = CMD_NONE; (enemy.state == 1) ? CMD_PINCER : CMD_NONE;

             // Heal Logic: Retreat if HP < 7, Return to Chase if HP >= 150
             if (enemy.health < 7 && enemy.state != 2) {
                 enemy.state = 2; // Retreat
                 // Spawn Phalanx Minions immediately
                 for(int i=0; i<8; i++) {
                     Enemy p;
                     p.x = enemy.x + (rand()%10 - 5);
                     p.y = enemy.y + (rand()%10 - 5);
                     
                     // Boundary Check
                     if (p.x < 5.0f) p.x = 5.0f;
                     if (p.x > MAP_WIDTH - 5.0f) p.x = MAP_WIDTH - 5.0f;
                     if (p.y < 5.0f) p.y = 5.0f;
                     if (p.y > MAP_HEIGHT - 5.0f) p.y = MAP_HEIGHT - 5.0f;
                     
                     // Spire Check
                     float spdx = p.x - 32.0f;
                     float spdy = p.y - 32.0f;
                     if (sqrtf(spdx*spdx + spdy*spdy) < 6.0f) {
                         float ang = atan2f(spdy, spdx);
                         p.x = 32.0f + cosf(ang) * 6.5f;
                         p.y = 32.0f + sinf(ang) * 6.5f;
                     }
                     
                     // Wall Check - simple skip if invalid after adjustment
                     if (worldMap[(int)p.x][(int)p.y] != 0) continue;

                     p.active = true;
                     p.health = 4;
                     p.isPhalanx = true;
                     p.speed = 4.0f;
                     pendingEnemies.push_back(p);
                 }
             }
             if (enemy.health >= marshallMaxHP && enemy.state == 2) {
                 enemy.state = 1; // Return to Chase
             }
             
             if (enemy.state == 2) { // Retreat & Heal (PHALANX)
                 activeCommand = CMD_PHALANX;
                 float targetX = player.x; float targetY = player.y;
                 float closestDist = sqrtf((player.x - enemy.x)*(player.x - enemy.x) + (player.y - enemy.y)*(player.y - enemy.y));
                 for (auto& e : allies) {
                     if (e.active && e.isAllied) {
                         float d = sqrtf((e.x - enemy.x)*(e.x - enemy.x) + (e.y - enemy.y)*(e.y - enemy.y));
                         if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; }
                     }
                 }
                 float dx = enemy.x - targetX; 
                 float dy = enemy.y - targetY;
                 float dist = sqrtf(dx*dx + dy*dy);
                 
                 float retreatSpeed = 7.5f;
                 if (dist < 16.0f) {
                     retreatSpeed = 10.0f;
                 }
                 
                 enemy.pathRecalcTimer -= deltaTime;
                 if (enemy.pathRecalcTimer <= 0 || enemy.path.empty()) {
                     float retreatTargetX = enemy.x + (dx/dist) * 15.0f;
                     float retreatTargetY = enemy.y + (dy/dist) * 15.0f;
                     if (retreatTargetX < 7.0f) retreatTargetX = 7.0f;
                     if (retreatTargetX > MAP_WIDTH - 7.0f) retreatTargetX = MAP_WIDTH - 7.0f;
                     if (retreatTargetY < 7.0f) retreatTargetY = 7.0f;
                     if (retreatTargetY > MAP_HEIGHT - 7.0f) retreatTargetY = MAP_HEIGHT - 7.0f;
                     
                     enemy.path = Pathfinder::FindPath(enemy.x, enemy.y, retreatTargetX, retreatTargetY);
                     enemy.pathIndex = 0;
                     enemy.pathRecalcTimer = 0.3f;
                 }
                 
                 float pathTargetX, pathTargetY;
                 if (Pathfinder::GetNextPathPoint(enemy.x, enemy.y, enemy.path, enemy.pathIndex, pathTargetX, pathTargetY)) {
                     float pdx = pathTargetX - enemy.x;
                     float pdy = pathTargetY - enemy.y;
                     float pdist = sqrtf(pdx*pdx + pdy*pdy);
                     if (pdist > 0.1f) {
                         float mx = (pdx / pdist) * retreatSpeed * deltaTime;
                         float my = (pdy / pdist) * retreatSpeed * deltaTime;
                         float nextX = enemy.x + mx;
                         float nextY = enemy.y + my;
                         float cdx = nextX - 32.0f;
                         float cdy = nextY - 32.0f;
                         if (nextX >= 7.0f && nextX <= MAP_WIDTH - 7.0f && worldMap[(int)nextX][(int)enemy.y] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(nextX, enemy.y, 0.4f)) enemy.x = nextX;
                         cdx = enemy.x - 32.0f;
                         cdy = nextY - 32.0f;
                         if (nextY >= 7.0f && nextY <= MAP_HEIGHT - 7.0f && worldMap[(int)enemy.x][(int)nextY] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(enemy.x, nextY, 0.4f)) enemy.y = nextY;
                     }
                 } else {
                     float mx = (dx/dist) * retreatSpeed * deltaTime;
                     float my = (dy/dist) * retreatSpeed * deltaTime;
                     float nextX = enemy.x + mx;
                     float nextY = enemy.y + my;
                     float cdx = nextX - 32.0f;
                     float cdy = nextY - 32.0f;
                     if (nextX >= 7.0f && nextX <= MAP_WIDTH - 7.0f && worldMap[(int)nextX][(int)enemy.y] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(nextX, enemy.y, 0.4f)) enemy.x = nextX;
                     cdx = enemy.x - 32.0f;
                     cdy = nextY - 32.0f;
                     if (nextY >= 7.0f && nextY <= MAP_HEIGHT - 7.0f && worldMap[(int)enemy.x][(int)nextY] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(enemy.x, nextY, 0.4f)) enemy.y = nextY;
                 }
                 
                 enemy.healTimer += deltaTime;
                 if (enemy.healTimer >= 2.0f) {
                     enemy.health++;
                     enemy.healTimer = 0;
                 }
                 
                 enemy.fireTimer -= deltaTime;
                 float rangeCheckDist = sqrtf((enemy.x - player.x)*(enemy.x - player.x) + (enemy.y - player.y)*(enemy.y - player.y));
                 if (enemy.fireTimer <= 0 && rangeCheckDist < 24.0f) {
                     enemy.fireTimer = 2.0f;
                     Rocket r;
                     r.x = enemy.x;
                     r.y = enemy.y;
                     r.dirX = 0; r.dirY = 0;
                     r.speed = 0;
                     r.active = true;
                     r.isEnemy = true;
                     r.z = 0;
                     r.verticalSpeed = 10.0f;
                     r.targetX = player.x;
                     r.targetY = player.y;
                     rockets.push_back(r);
                     PlayBazookaFireSound();
                 }

                 continue;
             } else if (enemy.state == 0) { // Seek Horde / RALLY
                 activeCommand = CMD_RALLY;
                 if (militiaFormTimer == 0) {
                     militiaFormTimer = 5.0f;
                     militiaMessageTimer = 3.0f;
                     militiaActive = true;
                 }
                 
                 militiaFormTimer -= deltaTime;
                 if (militiaFormTimer <= 0) {
                     // RALLY COMPLETE - BURST SPAWN
                     for(int i=0; i<15; i++) {
                         Enemy m;
                         m.x = enemy.x + (rand()%16 - 8);
                         m.y = enemy.y + (rand()%16 - 8);
                         
                         // Safety Checks
                         if (m.x < 5.0f) m.x = 5.0f;
                         if (m.x > MAP_WIDTH - 5.0f) m.x = MAP_WIDTH - 5.0f;
                         if (m.y < 5.0f) m.y = 5.0f;
                         if (m.y > MAP_HEIGHT - 5.0f) m.y = MAP_HEIGHT - 5.0f;
                         
                         float spdx = m.x - 32.0f;
                         float spdy = m.y - 32.0f;
                         if (sqrtf(spdx*spdx + spdy*spdy) < 6.0f) {
                              float ang = atan2f(spdy, spdx);
                              m.x = 32.0f + cosf(ang) * 6.5f;
                              m.y = 32.0f + sinf(ang) * 6.5f;
                         }

                         m.active = true;
                         m.health = 4;
                         m.speed = 4.0f + ((rand()%10)/10.0f);
                         if (worldMap[(int)m.x][(int)m.y] == 0) pendingEnemies.push_back(m);
                     }
                     for(int i=0; i<5; i++) {
                         Enemy s;
                         s.x = enemy.x + (rand()%20 - 10);
                         s.y = enemy.y + (rand()%20 - 10);
                         
                         // Safety Checks
                         if (s.x < 5.0f) s.x = 5.0f;
                         if (s.x > MAP_WIDTH - 5.0f) s.x = MAP_WIDTH - 5.0f;
                         if (s.y < 5.0f) s.y = 5.0f;
                         if (s.y > MAP_HEIGHT - 5.0f) s.y = MAP_HEIGHT - 5.0f;
                         
                         float spdx = s.x - 32.0f;
                         float spdy = s.y - 32.0f;
                         if (sqrtf(spdx*spdx + spdy*spdy) < 6.0f) {
                              float ang = atan2f(spdy, spdx);
                              s.x = 32.0f + cosf(ang) * 6.5f;
                              s.y = 32.0f + sinf(ang) * 6.5f;
                         }
                         
                         s.active = true;
                         s.isShooter = true;
                         s.health = 3;
                         s.speed = 3.0f;
                         if (worldMap[(int)s.x][(int)s.y] == 0) pendingEnemies.push_back(s);
                     }
                     enemy.state = 1; // Charge after rally
                 }
                 continue;
             } else { // Chase (State 1) - PINCER
                 activeCommand = CMD_PINCER;
                 float targetX = player.x; float targetY = player.y;
                 float closestDist = sqrtf((player.x - enemy.x)*(player.x - enemy.x) + (player.y - enemy.y)*(player.y - enemy.y));
                 for (auto& e : allies) {
                     if (e.active && e.isAllied) {
                         float d = sqrtf((e.x - enemy.x)*(e.x - enemy.x) + (e.y - enemy.y)*(e.y - enemy.y));
                         if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; }
                     }
                 }
                 float dx = targetX - enemy.x;
                 float dy = targetY - enemy.y;
                 float dist = sqrtf(dx*dx + dy*dy);
                 
                 if (dist < 3.0f && enemy.attackTimer <= 0) {
                     if (!godMode) player.health -= 20;
                     PlayMarshallAttackSound();
                     screenShakeTimer = 1.0f;
                     playerHurtTimer = 0.5f;
                     
                     float kx = (player.x - enemy.x) / dist;
                     float ky = (player.y - enemy.y) / dist;
                     player.x += kx * 2.0f;
                     player.y += ky * 2.0f;
                     
                     enemy.attackTimer = 2.0f;
                 }
                 if (enemy.attackTimer > 0) enemy.attackTimer -= deltaTime;
                 
                 if (dist > 2.5f) {
                     enemy.pathRecalcTimer -= deltaTime;
                     if (enemy.pathRecalcTimer <= 0 || enemy.path.empty()) {
                         enemy.path = Pathfinder::FindPath(enemy.x, enemy.y, player.x, player.y);
                         enemy.pathIndex = 0;
                         enemy.pathRecalcTimer = 0.3f;
                     }
                     
                     float pathTargetX, pathTargetY;
                     float chaseSpeed = 4.5f;
                     if (Pathfinder::GetNextPathPoint(enemy.x, enemy.y, enemy.path, enemy.pathIndex, pathTargetX, pathTargetY)) {
                         float pdx = pathTargetX - enemy.x;
                         float pdy = pathTargetY - enemy.y;
                         float pdist = sqrtf(pdx*pdx + pdy*pdy);
                         if (pdist > 0.1f) {
                             float mx = (pdx / pdist) * chaseSpeed * deltaTime;
                             float my = (pdy / pdist) * chaseSpeed * deltaTime;
                             float cdx = (enemy.x + mx) - 32.0f;
                             float cdy = (enemy.y + my) - 32.0f;
                             if (worldMap[(int)(enemy.x + mx)][(int)enemy.y] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding((enemy.x + mx), enemy.y, 0.4f)) {
                                 bool collision = false;
                                 for(const auto& br : bigRocks) {
                                     float dx = (enemy.x + mx) - br.x;
                                     float dy = enemy.y - br.y;
                                     if(dx*dx + dy*dy < 0.64f) { collision = true; break; }
                                 }
                                 if(!collision) enemy.x += mx;
                             }
                             cdx = enemy.x - 32.0f;
                             cdy = (enemy.y + my) - 32.0f;
                             if (worldMap[(int)enemy.x][(int)(enemy.y + my)] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(enemy.x, (enemy.y + my), 0.4f)) {
                                 bool collision = false;
                                 for(const auto& br : bigRocks) {
                                     float dx = enemy.x - br.x;
                                     float dy = (enemy.y + my) - br.y;
                                     if(dx*dx + dy*dy < 0.64f) { collision = true; break; }
                                 }
                                 if(!collision) enemy.y += my;
                             }
                         }
                     } else {
                         float mx = (dx / dist) * chaseSpeed * deltaTime;
                         float my = (dy / dist) * chaseSpeed * deltaTime;
                         float cdx = (enemy.x + mx) - 32.0f;
                         float cdy = (enemy.y + my) - 32.0f;
                          if (worldMap[(int)(enemy.x + mx)][(int)enemy.y] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding((enemy.x + mx), enemy.y, 0.4f)) {
                                 bool collision = false;
                                 for(const auto& br : bigRocks) {
                                     float dx = (enemy.x + mx) - br.x;
                                     float dy = enemy.y - br.y;
                                     if(dx*dx + dy*dy < 0.64f) { collision = true; break; }
                                 }
                                 if(!collision) enemy.x += mx;
                          }
                         cdx = enemy.x - 32.0f;
                         cdy = (enemy.y + my) - 32.0f;
                          if (worldMap[(int)enemy.x][(int)(enemy.y + my)] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(enemy.x, (enemy.y + my), 0.4f)) {
                                 bool collision = false;
                                 for(const auto& br : bigRocks) {
                                     float dx = enemy.x - br.x;
                                     float dy = (enemy.y + my) - br.y;
                                     if(dx*dx + dy*dy < 0.64f) { collision = true; break; }
                                 }
                                 if(!collision) enemy.y += my;
                          }
                     }
                 }
                 
                 enemy.summonTimer -= deltaTime;
                 if (enemy.summonTimer <= 0) {
                     enemy.summonTimer = 10.0f;
                     float spawnDist = (enemy.tacticState != 0) ? -8.0f : 0.0f;
                     float behindX = player.x - cosf(player.angle) * spawnDist;
                     float behindY = player.y - sinf(player.angle) * spawnDist;
                     for (int k=0; k<5; k++) {
                         Enemy s;
                         if (enemy.tacticState != 0) {
                             s.x = behindX + (rand()%200 - 100)/50.0f;
                             s.y = behindY + (rand()%200 - 100)/50.0f;
                         } else {
                             s.x = enemy.x + (rand()%200 - 100)/50.0f;
                             s.y = enemy.y + (rand()%200 - 100)/50.0f;
                         }
                         if (s.x > 1 && s.x < MAP_WIDTH-1 && s.y > 1 && s.y < MAP_HEIGHT-1 && worldMap[(int)s.x][(int)s.y] == 0) {
                             s.active = true; s.health = 1; s.speed = 3.0f; s.spriteIndex = rand()%4;
                             s.isShooter = false; s.isMarshall = false;
                             s.tacticState = 0; s.flankDir = 0; s.tacticTimer = 0;
                             s.path.clear(); s.pathIndex = 0; s.pathRecalcTimer = 0;
                             pendingEnemies.push_back(s);
                         }
                     }
                 }
                 continue;
             }
         }

        float targetX = player.x; float targetY = player.y;
        float closestDist = sqrtf((player.x - enemy.x)*(player.x - enemy.x) + (player.y - enemy.y)*(player.y - enemy.y));
        Enemy* spearTarget = nullptr;
        for (auto& e : allies) {
            if (e.active && e.isAllied) {
                float d = sqrtf((e.x - enemy.x)*(e.x - enemy.x) + (e.y - enemy.y)*(e.y - enemy.y));
                if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; spearTarget = &e; }
            }
        }
        
        float dx = targetX - enemy.x;
        float dy = targetY - enemy.y;
        float dist = sqrtf(dx*dx + dy*dy);
        
        float pdx = dx; float pdy = dy;
        float pdist = dist;

        if (enemy.isSpearGuy) {
            float dx = pdx; float dy = pdy; float dist = pdist;
            if (enemy.dashCooldown > 0) enemy.dashCooldown -= deltaTime;
            if (enemy.blockCooldown > 0) enemy.blockCooldown -= deltaTime;
            
            int nearbyMelee = 0;
            float closestHordeX = 0, closestHordeY = 0;
            float closestHordeDist = 99999.0f;
            
            for (auto& other : enemies) {
                if (&other == &enemy || !other.active || other.isShooter || other.isSpearGuy) continue;
                float ox = enemy.x - other.x;
                float oy = enemy.y - other.y;
                float odist = sqrtf(ox*ox + oy*oy);
                if (odist < 10.0f) {
                    nearbyMelee++;
                }
                if (odist < closestHordeDist) {
                    closestHordeDist = odist;
                    closestHordeX = other.x;
                    closestHordeY = other.y;
                }
            }
            
            bool beingShotAt = false;
            for (auto& b : bullets) {
                if (!b.active) continue;
                float bdx = b.x - enemy.x;
                float bdy = b.y - enemy.y;
                float bdist = sqrtf(bdx*bdx + bdy*bdy);
                if (bdist < 10.0f) {
                    float dot = (b.dirX * bdx) + (b.dirY * bdy);
                    if (dot < 0) {
                        beingShotAt = true;
                        break;
                    }
                }
            }
            
            if (beingShotAt) {
                if (enemy.dashCooldown <= 0) {
                    enemy.spearState = 2; // Dash
                    enemy.spearTimer = 0.3f; // 0.3 sec dash
                    enemy.dashCooldown = 1.0f;
                    enemy.dashDir = (rand() % 2 == 0) ? 1 : -1;
                } else if (enemy.blockCooldown <= 0 && enemy.spearState != 2) {
                    enemy.spearState = 3; // Block
                    enemy.spearTimer = 0.5f;
                    enemy.blockCooldown = 0.5f;
                }
            }
            
            if (enemy.spearState == 2) { // Dashing
                enemy.spearTimer -= deltaTime;
                float dashSpeed = 15.0f;
                float ang = atan2f(dy, dx) + (enemy.dashDir * 3.14159f / 2.0f);
                float moveX = cosf(ang) * dashSpeed * deltaTime;
                float moveY = sinf(ang) * dashSpeed * deltaTime;
                if (worldMap[(int)(enemy.x + moveX)][(int)enemy.y] == 0) {
                    float pdx2 = (enemy.x + moveX) - targetX;
                    float pdy2 = enemy.y - targetY;
                    if (pdx2*pdx2 + pdy2*pdy2 >= 0.64f) enemy.x += moveX;
                }
                if (worldMap[(int)enemy.x][(int)(enemy.y + moveY)] == 0) {
                    float pdx2 = enemy.x - targetX;
                    float pdy2 = (enemy.y + moveY) - targetY;
                    if (pdx2*pdx2 + pdy2*pdy2 >= 0.64f) enemy.y += moveY;
                }
                if (enemy.spearTimer <= 0) enemy.spearState = 0;
            } else if (enemy.spearState == 3) { // Blocking
                enemy.spearTimer -= deltaTime;
                if (enemy.spearTimer <= 0) enemy.spearState = 0;
            } else {
                if (dist <= 2.0f) {
                    enemy.spearState = 1; // Idle while attacking
                    if (enemy.attackTimer <= 0) {
                        if (spearTarget) {
                            spearTarget->health -= 15;
                            spearTarget->hurtTimer = 0.5f;
                            if (spearTarget->health <= 0) {
                                spearTarget->active = false;
                                if (spearTarget->isDefectedOfficer) {
                                    
                                    
                                }
                            }
                        } else {
                            if (!godMode) player.health -= 15; // Spear damage
                            PlayPlayerHurtSound();
                            playerHurtTimer = 0.5f;
                            screenShakeTimer = 0.5f;
                        }
                        enemy.attackTimer = 1.5f;
                    }
                } else if (nearbyMelee >= 3) {
                    enemy.spearState = 0; // Move with horde
                    // Always approach the player — use formation speed when close
                    float rushSpeed = (dist > 5.0f) ? enemy.speed : enemy.speed * 1.3f;
                    float moveX = (dx / dist) * rushSpeed * deltaTime;
                    float moveY = (dy / dist) * rushSpeed * deltaTime;
                    if (worldMap[(int)(enemy.x + moveX)][(int)enemy.y] == 0) enemy.x += moveX;
                    if (worldMap[(int)enemy.x][(int)(enemy.y + moveY)] == 0) enemy.y += moveY;
                } else {
                    if (dist < 8.0f) {
                        enemy.spearState = 0; // Dash towards player
                        float rushSpeed = enemy.speed * 1.5f;
                        float moveX = (dx / dist) * rushSpeed * deltaTime;
                        float moveY = (dy / dist) * rushSpeed * deltaTime;
                        if (worldMap[(int)(enemy.x + moveX)][(int)enemy.y] == 0) enemy.x += moveX;
                        if (worldMap[(int)enemy.x][(int)(enemy.y + moveY)] == 0) enemy.y += moveY;
                    } else {
                        if (closestHordeDist < 9999.0f) {
                            enemy.spearState = 0; // Move to horde
                            float hdx = closestHordeX - enemy.x;
                            float hdy = closestHordeY - enemy.y;
                            if (closestHordeDist > 1.0f) {
                                float moveX = (hdx / closestHordeDist) * enemy.speed * deltaTime;
                                float moveY = (hdy / closestHordeDist) * enemy.speed * deltaTime;
                                if (worldMap[(int)(enemy.x + moveX)][(int)enemy.y] == 0) enemy.x += moveX;
                                if (worldMap[(int)enemy.x][(int)(enemy.y + moveY)] == 0) enemy.y += moveY;
                            }
                        } else {
                            enemy.spearState = 1; // Idle
                        }
                    }
                }
            }
            if (enemy.attackTimer > 0) enemy.attackTimer -= deltaTime;

        } else if (enemy.isOfficer) {
            if (enemy.firingTimer > 0) enemy.firingTimer -= deltaTime;
            if (enemy.fireTimer > 0) enemy.fireTimer -= deltaTime;
            if (enemy.officerCooldown > 0) enemy.officerCooldown -= deltaTime;
            
            std::vector<Enemy*> gunners;
            for (auto& e : enemies) {
                if (e.active && e.isShooter && !e.isOfficer && e.isEnemy) {
                    gunners.push_back(&e);
                }
            }
            
            for (auto& e : enemies) {
                if (e.active && !e.isShooter && !e.isMarshall && !e.isOfficer && e.isEnemy) {
                    float d = sqrtf((e.x - enemy.x)*(e.x - enemy.x) + (e.y - enemy.y)*(e.y - enemy.y));
                    if (d < 8.0f) {
                        e.speed = 1.4f;
                    }
                }
            }
            
            // Don't interrupt state 2 (volley in progress) — gunners still have pending shots
            if (enemy.officerState != 2) {
                if (dist < 8.0f || gunners.size() <= (size_t)(maxShooterSpawn * 0.5f)) {
                    enemy.officerState = 3;
                } else {
                    enemy.officerState = 1;
                }
            }
            
            if (enemy.officerState == 3) {
                float rx = -dx; float ry = -dy;
                float moveSpeed = enemy.speed * 1.2f;
                enemy.x += (rx/dist)*moveSpeed*deltaTime;
                enemy.y += (ry/dist)*moveSpeed*deltaTime;
                
                // Command grunts to retreat away from player as well
                for (auto* g : gunners) {
                    float gDist = sqrtf((player.x - g->x)*(player.x - g->x) + (player.y - g->y)*(player.y - g->y));
                    if (gDist > 0.1f) {
                        float grx = g->x - player.x;
                        float gry = g->y - player.y;
                        float gSpeed = g->speed * 1.2f;
                        g->x += (grx/gDist) * gSpeed * deltaTime;
                        g->y += (gry/gDist) * gSpeed * deltaTime;
                    }
                }
                
                if (enemy.officerCooldown <= 0 && gunners.size() < 4) {
                    int lost = 4 - gunners.size();
                    for(int i=0; i<lost; i++){
                        Enemy shooter;
                        shooter.x = enemy.x + (rand()%200 - 100)/100.0f; 
                        shooter.y = enemy.y + (rand()%200 - 100)/100.0f;
                        shooter.active = true; shooter.speed = 1.2f; shooter.spriteIndex = 0; shooter.health = 2;
                        shooter.isShooter = true; shooter.fireTimer = 2.0f; shooter.hasNeuralBrain = true; NeuralAI::InheritBrain(shooter.brain);
                        pendingEnemies.push_back(shooter);
                    }
                    enemy.officerCooldown = 20.0f;
                }
            } else if (enemy.officerState == 1) {
                if (dist > 12.0f) {
                    enemy.x += (dx/dist)*enemy.speed*deltaTime;
                    enemy.y += (dy/dist)*enemy.speed*deltaTime;
                } else if (dist < 10.0f) {
                    enemy.x -= (dx/dist)*enemy.speed*deltaTime;
                    enemy.y -= (dy/dist)*enemy.speed*deltaTime;
                }
                
                float angleToPlayer = atan2f(-dy, -dx);
                float lineAngle = angleToPlayer + 3.14159f/2.0f;
                
                int i = 0;
                int count = gunners.size();
                bool allReady = true;
                for (auto* g : gunners) {
                    float offset = (i - (count-1)/2.0f) * 0.8f;
                    float tx = enemy.x + cosf(lineAngle) * offset;
                    float ty = enemy.y + sinf(lineAngle) * offset;
                    float gdx = tx - g->x; float gdy = ty - g->y;
                    float gdist = sqrtf(gdx*gdx + gdy*gdy);
                    if (gdist > 0.5f) {
                        g->x += (gdx/gdist) * g->speed * deltaTime;
                        g->y += (gdy/gdist) * g->speed * deltaTime;
                        allReady = false;
                    }
                    g->fireTimer = 2.0f; 
                    i++;
                }
                
                if (allReady && enemy.fireTimer <= 0) {
                    // Begin staggered volley: assign each gunner a negative countdown
                    // They fire sequentially 0.1s apart instead of all at once
                    const float VOLLEY_STAGGER = 0.1f;
                    int idx = 1;
                    for (auto* g : gunners) {
                        g->fireTimer = -(idx * VOLLEY_STAGGER); // negative = pending shot
                        idx++;
                    }
                    enemy.fireTimer = 3.0f;
                    enemy.officerState = 2;
                    // Whistle fires at the start of the volley command
                }
            } else if (enemy.officerState == 2) {
                // Staggered volley in progress — tick each gunner's countdown
                bool allFired = true;
                for (auto* g : gunners) {
                    if (g->fireTimer < 0.0f) {
                        g->fireTimer += deltaTime;
                        allFired = false;
                        if (g->fireTimer >= 0.0f) {
                            // This gunner's moment to fire
                            float targetX = player.x; float targetY = player.y;
                            float closestDist = sqrtf((player.x - g->x)*(player.x - g->x) + (player.y - g->y)*(player.y - g->y));
                            for (auto& e : allies) {
                                if (e.active) {
                                    float d = sqrtf((e.x - g->x)*(e.x - g->x) + (e.y - g->y)*(e.y - g->y));
                                    if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; }
                                }
                            }
                            float targetAngle = atan2f(targetY - g->y, targetX - g->x);
                            EnemyBullet eb; eb.x = g->x; eb.y = g->y;
                            eb.dirX = cosf(targetAngle); eb.dirY = sinf(targetAngle);
                            eb.speed = 8.0f; eb.active = true; eb.isLaser = false;
                            enemyBullets.push_back(eb);
                            g->firingTimer = 0.2f;
                            PlayEnemyFireSound(); // One crack per gunner
                        }
                    }
                }
                // Return to forming line once all shots have been fired and cooldown elapsed
                if (allFired && enemy.fireTimer <= 0) {
                    enemy.officerState = 1;
                }
            }

            if (enemy.officerState != enemy.prevOfficerState) {
                if (enemy.officerState == 1 && enemy.prevOfficerState != 2) PlayOfficerCommandSound();
                else if (enemy.officerState == 2) PlayOfficerWhistleSound();
                else if (enemy.officerState == 3) PlayOfficerRetreatSound();
                enemy.prevOfficerState = enemy.officerState;
            }

        } else if (enemy.isShooter) {
            if (officerSpawned) {
                if (enemy.firingTimer > 0) enemy.firingTimer -= deltaTime;
                continue;
            }
            if (enemy.firingTimer > 0) enemy.firingTimer -= deltaTime;
            
            int nearbyHordeCount = 0;
            for (auto& other : enemies) {
                if (&other == &enemy || !other.active) continue;
                float ox = enemy.x - other.x;
                float oy = enemy.y - other.y;
                if (sqrtf(ox*ox + oy*oy) < 8.0f && other.tacticState != 0) nearbyHordeCount++;
            }
            
            if (nearbyHordeCount >= 4 && enemy.tacticState == 0) {
                enemy.tacticState = 3;
                enemy.flankDir = (rand() % 2 == 0) ? 1 : -1;
                enemy.tacticTimer = 0;
            } else if (nearbyHordeCount < 2 && enemy.tacticState == 3) {
                enemy.tacticState = 0;
            }
            
            if (enemy.tacticState == 3) {
                float targetAngle = atan2f(dy, dx) + (enemy.flankDir * PI / 3.0f);
                float targetX = player.x + cosf(targetAngle) * 12.0f;
                float targetY = player.y + sinf(targetAngle) * 12.0f;
                float tdx = targetX - enemy.x;
                float tdy = targetY - enemy.y;
                float tdist = sqrtf(tdx*tdx + tdy*tdy);
                
                if (tdist > 2.0f) {
                    float moveX = (tdx / tdist) * enemy.speed * 1.5f * deltaTime;
                    float moveY = (tdy / tdist) * enemy.speed * 1.5f * deltaTime;
                    float newX = enemy.x + moveX;
                    float newY = enemy.y + moveY;
         // Check Wall Collision
        if (worldMap[(int)newX][(int)enemy.y] == 0) {
            bool collision = false;
            for(const auto& br : bigRocks) {
                float dx = newX - br.x;
                float dy = enemy.y - br.y;
                if(dx*dx + dy*dy < 2.25f) { collision = true; break; } 
            }
            if(!collision) enemy.x = newX;
        }
        if (worldMap[(int)enemy.x][(int)newY] == 0) {
             bool collision = false;
            for(const auto& br : bigRocks) {
                float dx = enemy.x - br.x;
                float dy = newY - br.y;
                if(dx*dx + dy*dy < 2.25f) { collision = true; break; } 
            }
            if(!collision) enemy.y = newY;
        }

                }
                
                if (dist <= 18.0f && dist > 1.0f) {
                    enemy.fireTimer -= deltaTime;
                    if (enemy.fireTimer <= 0) {
                        EnemyBullet eb;
                        eb.x = enemy.x;
                        eb.y = enemy.y;
                        float targetX = player.x; float targetY = player.y;
                        float closestDist = sqrtf((player.x - enemy.x)*(player.x - enemy.x) + (player.y - enemy.y)*(player.y - enemy.y));
                        for (auto& e : allies) {
                            if (e.active) {
                                float d = sqrtf((e.x - enemy.x)*(e.x - enemy.x) + (e.y - enemy.y)*(e.y - enemy.y));
                                if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; }
                            }
                        }
                        float edx = targetX - enemy.x;
                        float edy = targetY - enemy.y;
                        float edist = sqrtf(edx*edx + edy*edy);
                        eb.dirX = edx / edist;
                        eb.dirY = edy / edist;
                        eb.speed = 8.0f;
                        eb.active = true;
                        eb.isLaser = false;
                        enemyBullets.push_back(eb);
                        enemy.fireTimer = 1.5f;
                        enemy.firingTimer = 0.5f;
                        PlayEnemyFireSound();
                    }
                }
            } else if (dist <= 16.0f && dist > 1.0f) {
                enemy.fireTimer -= deltaTime;
                if (enemy.fireTimer <= 0) {
                    EnemyBullet eb;
                    eb.x = enemy.x;
                    eb.y = enemy.y;
                    float targetX = player.x; float targetY = player.y;
                    float closestDist = sqrtf((player.x - enemy.x)*(player.x - enemy.x) + (player.y - enemy.y)*(player.y - enemy.y));
                    for (auto& e : allies) {
                        if (e.active) {
                            float d = sqrtf((e.x - enemy.x)*(e.x - enemy.x) + (e.y - enemy.y)*(e.y - enemy.y));
                            if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; }
                        }
                    }
                    float edx = targetX - enemy.x;
                    float edy = targetY - enemy.y;
                    float edist = sqrtf(edx*edx + edy*edy);
                    eb.dirX = edx / edist;
                    eb.dirY = edy / edist;
                    eb.speed = 8.0f;
                    eb.active = true;
                    eb.isLaser = false;
                    enemyBullets.push_back(eb);
                    
                    enemy.fireTimer = 2.0f;
                    enemy.firingTimer = 0.5f;
                    PlayEnemyFireSound();
                }
            } else if (dist > 16.0f) {
                enemy.pathRecalcTimer -= deltaTime;
                if (enemy.pathRecalcTimer <= 0 || enemy.path.empty()) {
                    enemy.path = Pathfinder::FindPath(enemy.x, enemy.y, player.x, player.y);
                    enemy.pathIndex = 0;
                    enemy.pathRecalcTimer = 0.5f;
                }
                
                float pathTargetX, pathTargetY;
                if (Pathfinder::GetNextPathPoint(enemy.x, enemy.y, enemy.path, enemy.pathIndex, pathTargetX, pathTargetY)) {
                    float pdx = pathTargetX - enemy.x;
                    float pdy = pathTargetY - enemy.y;
                    float pdist = sqrtf(pdx*pdx + pdy*pdy);
                    if (pdist > 0.1f) {
                        float moveX = (pdx / pdist) * enemy.speed * deltaTime;
                        float moveY = (pdy / pdist) * enemy.speed * deltaTime;
                        float newX = enemy.x + moveX;
                        float newY = enemy.y + moveY;
                        float cdx = newX - 32.0f;
                        float cdy = newY - 32.0f;
                        if (worldMap[(int)newX][(int)enemy.y] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(newX, enemy.y, 0.4f)) enemy.x = newX;
                        cdx = enemy.x - 32.0f;
                        cdy = newY - 32.0f;
                        if (worldMap[(int)enemy.x][(int)newY] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(enemy.x, newY, 0.4f)) enemy.y = newY;
                    }
                } else {
                    float moveX = (dx / dist) * enemy.speed * deltaTime;
                    float moveY = (dy / dist) * enemy.speed * deltaTime;
                    float newX = enemy.x + moveX;
                    float newY = enemy.y + moveY;
                    float cdx = newX - 32.0f;
                    float cdy = newY - 32.0f;
                    if (worldMap[(int)newX][(int)enemy.y] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(newX, enemy.y, 0.4f)) {
                         bool collision = false;
                         for(const auto& br : bigRocks) {
                             float dx = newX - br.x;
                             float dy = enemy.y - br.y;
                             if(dx*dx + dy*dy < 0.64f) { collision = true; break; } 

                         }
                         if(!collision) enemy.x = newX;
                    }
                    cdx = enemy.x - 32.0f;
                    cdy = newY - 32.0f;
                    if (worldMap[(int)enemy.x][(int)newY] == 0 && (cdx*cdx + cdy*cdy >= 9.0f) && !IsPositionColliding(enemy.x, newY, 0.4f)) {
                         bool collision = false;
                         for(const auto& br : bigRocks) {
                             float dx = enemy.x - br.x;
                             float dy = newY - br.y;
                             if(dx*dx + dy*dy < 0.64f) { collision = true; break; } 

                         }
                         if(!collision) enemy.y = newY;
                    }

                }
            }
        } else {
            int nearbyCount = 0;
            float hordeCenterX = enemy.x, hordeCenterY = enemy.y;
            for (auto& other : enemies) {
                if (&other == &enemy || !other.active || other.isShooter) continue;
                float ox = enemy.x - other.x;
                float oy = enemy.y - other.y;
                float odist = sqrtf(ox*ox + oy*oy);
                if (odist < 8.0f) {
                    nearbyCount++;
                    hordeCenterX += other.x;
                    hordeCenterY += other.y;
                }
            }
            if (nearbyCount > 0) {
                hordeCenterX /= (nearbyCount + 1);
                hordeCenterY /= (nearbyCount + 1);
            }
            
            // MILITIA TACTICS OVERRIDE
            if (activeCommand == CMD_RALLY && marshallHealthBarActive) {
                // Ignore player, seek Marshall
                dx = marshallX - enemy.x;
                dy = marshallY - enemy.y;
                dist = sqrtf(dx*dx + dy*dy);
                if (dist > 3.0f) {
                     float mx = (dx/dist) * enemy.speed * 1.5f * deltaTime; // Rush
                     float my = (dy/dist) * enemy.speed * 1.5f * deltaTime;
                     if (worldMap[(int)(enemy.x + mx)][(int)enemy.y] == 0) enemy.x += mx;
                     if (worldMap[(int)enemy.x][(int)(enemy.y + my)] == 0) enemy.y += my;
                }
                continue;
            } else if (activeCommand == CMD_PHALANX && marshallHealthBarActive) {
                // Wall Formation
                float mdx = player.x - marshallX;
                float mdy = player.y - marshallY;
                float mdist = sqrtf(mdx*mdx + mdy*mdy);
                float wallDist = 6.0f; // Distance from Marshall
                
                // Position should be between Marshall and Player
                float tx = marshallX + (mdx/mdist) * wallDist;
                float ty = marshallY + (mdy/mdist) * wallDist;
                
                // Add jitter for wall spread based on enemy address/index
                int spread = ((intptr_t)&enemy % 5) - 2;
                float perpX = -mdy/mdist;
                float perpY = mdx/mdist;
                tx += perpX * spread * 1.5f;
                ty += perpY * spread * 1.5f;
                
                float tdx = tx - enemy.x;
                float tdy = ty - enemy.y;
                float tdist = sqrtf(tdx*tdx + tdy*tdy);
                
                // Attack if player is close, else hold line
                float pDist = sqrtf((player.x - enemy.x)*(player.x - enemy.x) + (player.y - enemy.y)*(player.y - enemy.y));
                if (pDist < 4.0f) {
                    // Standard Aggro
                } else if (tdist > 1.0f) {
                     float mx = (tdx/tdist) * enemy.speed * deltaTime;
                     float my = (tdy/tdist) * enemy.speed * deltaTime;
                     if (worldMap[(int)(enemy.x + mx)][(int)enemy.y] == 0) enemy.x += mx;
                     if (worldMap[(int)enemy.x][(int)(enemy.y + my)] == 0) enemy.y += my;
                     continue;
                } else {
                     continue; // Hold
                }
            } else if (activeCommand == CMD_PINCER) {
                // Forced Flanking
                int side = (enemy.spriteIndex % 2 == 0) ? 1 : -1;
                float ang = atan2f(dy, dx) + (side * PI * 0.4f); // 72 degrees offset
                dx = cosf(ang) * 10.0f;
                dy = sinf(ang) * 10.0f;
                // Continue to standard movement with modified dx/dy vector direction
            }
            
            for (auto& other : enemies) {
                if (&other == &enemy || !other.active || other.isShooter || other.isMarshall) continue;
                if (other.tacticState != 0) continue;
                float ox = other.x - hordeCenterX;
                float oy = other.y - hordeCenterY;
                float odist = sqrtf(ox*ox + oy*oy);
                if (odist < 16.0f && odist >= 6.0f && nearbyCount >= 8) {
                    other.tacticState = (rand() % 2 == 0) ? 1 : 2;
                    other.flankDir = (rand() % 2 == 0) ? 1 : -1;
                    other.tacticTimer = 2.0f;
                }
            }
            
            if (nearbyCount >= 8 && enemy.tacticState == 0) {
                enemy.tacticState = (rand() % 2 == 0) ? 1 : 2;
                enemy.flankDir = (rand() % 2 == 0) ? 1 : -1;
                enemy.tacticTimer = 2.0f;
            } else if (nearbyCount < 3 && enemy.tacticState != 0) {
                enemy.tacticState = 0;
            }
            
            if (nearbyCount >= 8 && !hordeActive) {
                hordeActive = true;
                hordeMessageTimer = 3.0f;
                if (officerSpawned) {
                    Enemy* officer = nullptr;
                    int lineCount = 0;
                    for (auto& e : enemies) {
                        if (e.active) {
                            if (e.isOfficer) officer = &e;
                            else if (e.isShooter && e.isEnemy) lineCount++;
                        }
                    }
                    if (officer && officer->officerCooldown <= 0.0f) {
                        // Spawn only the gunners lost (4 is the full firing line), capped at 4
                        int lost = 4 - lineCount;
                        int spawnCount = (lost > 0) ? lost : 0;
                        if (spawnCount > 4) spawnCount = 4; // Safety cap
                        for (int i = 0; i < spawnCount; i++) {
                            Enemy shooter;
                            shooter.x = officer->x + (rand()%200 - 100)/100.0f;
                            shooter.y = officer->y + (rand()%200 - 100)/100.0f;
                            shooter.active = true; shooter.speed = 1.2f; shooter.spriteIndex = 0; shooter.health = 2; shooter.maxHealth = 2;
                            shooter.isShooter = true; shooter.fireTimer = 2.0f; shooter.hasNeuralBrain = true; NeuralAI::InheritBrain(shooter.brain);
                            pendingEnemies.push_back(shooter);
                        }
                        if (spawnCount > 0) officer->officerCooldown = 20.0f;
                    }
                }
            }
            
            int hordeCount = 0;
            for (auto& e : enemies) {
                if (e.active && e.tacticState != 0) hordeCount++;
            }
            if (hordeCount < 4) {
                hordeActive = false;
            }
            
            if (enemy.tacticTimer > 0) enemy.tacticTimer -= deltaTime;
            
            float separationX = 0, separationY = 0;
            for (auto& other : enemies) {
                if (&other == &enemy || !other.active || other.isShooter || other.isMarshall) continue;
                float ox = enemy.x - other.x;
                float oy = enemy.y - other.y;
                float odist = sqrtf(ox*ox + oy*oy);
                if (odist < 1.5f && odist > 0.01f) {
                    separationX += (ox / odist) * (1.5f - odist);
                    separationY += (oy / odist) * (1.5f - odist);
                }
            }
            
            float moveX = 0, moveY = 0;
            float neuralMoveBias = 1.0f;
            float neuralStrafe = 0;
            float neuralAggression = 0;
            float neuralDodge = 0;
            float neuralCoverSeek = 0;
            float bulletDirX = 0, bulletDirY = 0;
            bool bulletIncoming = false;
            
            if (enemy.hasNeuralBrain) {
                float inputs[NeuralAI::INPUT_COUNT];
                inputs[0] = dist / 30.0f;
                float angleToPlayer = atan2f(dy, dx);
                inputs[1] = sinf(angleToPlayer);
                inputs[2] = cosf(angleToPlayer);
                inputs[3] = player.angle / PI;
                inputs[4] = (float)enemy.health / 4.0f;
                inputs[5] = (float)nearbyCount / 10.0f;
                inputs[6] = isMoving ? 1.0f : 0.0f;
                inputs[7] = (float)currentWeapon / 2.0f;
                inputs[8] = enemy.brain.survivalTime / 30.0f;
                
                // Bullet detection logic for neural inputs
                float closestBulletDist = 999.0f;
                float closestBulletAngle = 0.0f;
                for (auto& b : bullets) {
                    if (!b.active) continue;
                    float bdx = enemy.x - b.x;
                    float bdy = enemy.y - b.y;
                    float bdist = sqrtf(bdx*bdx + bdy*bdy);
                    // Check if bullet is moving roughly towards enemy
                    float dot = (b.dirX * bdx) + (b.dirY * bdy); 
                    if (bdist < 7.0f && dot > 0 && bdist < closestBulletDist) {
                        closestBulletDist = bdist;
                        closestBulletAngle = atan2f(b.dirY, b.dirX);
                        bulletDirX = b.dirX;
                        bulletDirY = b.dirY;
                        bulletIncoming = true;
                    }
                }
                
                inputs[9] = (closestBulletDist < 7.0f) ? (1.0f - closestBulletDist/7.0f) : 0.0f;
                inputs[10] = (closestBulletDist < 7.0f) ? (closestBulletAngle / PI) : 0.0f;
                
                float checkX = enemy.x + cosf(angleToPlayer);
                float checkY = enemy.y + sinf(angleToPlayer);
                inputs[11] = Pathfinder::IsBlocked((int)checkX, (int)checkY) ? 1.0f : -1.0f;
                inputs[12] = (float)NeuralAI::GetGeneration() / 50.0f;
                
                float outputs[NeuralAI::OUTPUT_COUNT];
                enemy.brain.Evaluate(inputs, outputs);
                
                neuralMoveBias = outputs[0];
                neuralStrafe = outputs[1];
                neuralAggression = outputs[2];
                neuralDodge = outputs[3];
                neuralCoverSeek = outputs[4];
            }
            
            if (enemy.tacticState == 2) {
                float angleToEnemy = atan2f(enemy.y - player.y, enemy.x - player.x);
                float playerFacing = player.angle;
                float angleDiff = angleToEnemy - playerFacing;
                while (angleDiff > PI) angleDiff -= 2 * PI;
                while (angleDiff < -PI) angleDiff += 2 * PI;
                
                float targetAngle;
                if (enemy.flankDir == 1) {
                    targetAngle = playerFacing + PI * 0.75f;
                } else {
                    targetAngle = playerFacing - PI * 0.75f;
                }
                
                float encircleRadius = (dist > 10.0f) ? 10.0f : 4.0f;
                float targetX = player.x + cosf(targetAngle) * encircleRadius;
                float targetY = player.y + sinf(targetAngle) * encircleRadius;
                
                float tdx = targetX - enemy.x;
                float tdy = targetY - enemy.y;
                float tdist = sqrtf(tdx*tdx + tdy*tdy);
                
                if (tdist > 0.5f) {
                    float speedMult = (dist > 12.0f) ? 2.2f : 1.8f;
                    moveX = (tdx / tdist) * (enemy.speed - 1.0f) * speedMult * deltaTime;
                    moveY = (tdy / tdist) * (enemy.speed - 1.0f) * speedMult * deltaTime;
                } else if (dist > 2.0f) {
                    moveX = (dx / dist) * (enemy.speed - 1.0f) * 1.5f * deltaTime;
                    moveY = (dy / dist) * (enemy.speed - 1.0f) * 1.5f * deltaTime;
                }
            } else if (dist > 1.2f) {
                // Always approach the player — no retreat
                // neuralMoveBias modulates approach speed:
                //   positive = aggressive faster approach
                //   negative = cautious slower approach (0.5x to 1.0x speed)
                float speedMod = 1.0f;
                if (enemy.hasNeuralBrain) {
                    if (neuralMoveBias >= 0.0f) {
                        // Aggressive: up to 1.5x speed
                        speedMod = 1.0f + neuralMoveBias * 0.5f;
                    } else {
                        // Cautious: 0.5x to 1.0x speed (never retreat)
                        speedMod = 1.0f + neuralMoveBias * 0.5f;
                        if (speedMod < 0.5f) speedMod = 0.5f;
                    }
                }
                
                enemy.pathRecalcTimer -= deltaTime;
                if (enemy.pathRecalcTimer <= 0 || enemy.path.empty()) {
                    enemy.path = Pathfinder::FindPath(enemy.x, enemy.y, player.x, player.y);
                    enemy.pathIndex = 0;
                    enemy.pathRecalcTimer = 0.5f;
                }
                
                float pathTargetX, pathTargetY;
                if (Pathfinder::GetNextPathPoint(enemy.x, enemy.y, enemy.path, enemy.pathIndex, pathTargetX, pathTargetY)) {
                    float pdx = pathTargetX - enemy.x;
                    float pdy = pathTargetY - enemy.y;
                    float pdist = sqrtf(pdx*pdx + pdy*pdy);
                    if (pdist > 0.1f) {
                        moveX = (pdx / pdist) * enemy.speed * speedMod * deltaTime;
                        moveY = (pdy / pdist) * enemy.speed * speedMod * deltaTime;
                    }
                } else {
                    moveX = (dx / dist) * enemy.speed * speedMod * deltaTime;
                    moveY = (dy / dist) * enemy.speed * speedMod * deltaTime;
                }
            }
            
            moveX += separationX * enemy.speed * 0.5f * deltaTime;
            moveY += separationY * enemy.speed * 0.5f * deltaTime;
            
            if (enemy.hasNeuralBrain && fabsf(neuralStrafe) > 0.2f) {
                float strafeX = -dy / (dist > 0.1f ? dist : 0.1f);
                float strafeY = dx / (dist > 0.1f ? dist : 0.1f);
                moveX += strafeX * neuralStrafe * enemy.speed * 0.5f * deltaTime;
                moveY += strafeY * neuralStrafe * enemy.speed * 0.5f * deltaTime;
            }
            
            if (enemy.hasNeuralBrain && bulletIncoming && fabsf(neuralDodge) > 0.2f) {
                if (enemy.dodgeDir == 0) enemy.dodgeDir = (rand() % 2 == 0) ? 1 : -1;
                // Perpendicular to incoming bullet dir
                float dodgeX = -bulletDirY * enemy.dodgeDir;
                float dodgeY = bulletDirX * enemy.dodgeDir;
                // Move out of bullet path with an enhanced burst of speed
                moveX += dodgeX * fabsf(neuralDodge) * enemy.speed * 2.0f * deltaTime;
                moveY += dodgeY * fabsf(neuralDodge) * enemy.speed * 2.0f * deltaTime;
            } else {
                enemy.dodgeDir = 0;
            }
            
            // Rock cover-seeking: when a bullet is incoming and coverSeek is active,
            // find the nearest BigRock and bias movement to get behind it
            if (enemy.hasNeuralBrain && bulletIncoming && neuralCoverSeek > 0.2f) {
                int gx = (int)(enemy.x / GRID_CELL_SIZE);
                int gy = (int)(enemy.y / GRID_CELL_SIZE);
                float bestCoverDist = 999.0f;
                float coverX = 0, coverY = 0;
                bool foundCover = false;
                
                // Search current + adjacent grid cells for nearby BigRocks
                int minCX = (gx > 0) ? gx - 1 : 0;
                int maxCX = (gx < 16) ? gx + 1 : 16;
                int minCY = (gy > 0) ? gy - 1 : 0;
                int maxCY = (gy < 16) ? gy + 1 : 16;
                
                for (int cx = minCX; cx <= maxCX; cx++) {
                    for (int cy = minCY; cy <= maxCY; cy++) {
                        for (int idx : bigRockGrid[cx][cy]) {
                            const BigRock& br = bigRocks[idx];
                            float rdx = br.x - enemy.x;
                            float rdy = br.y - enemy.y;
                            float rockDist = sqrtf(rdx*rdx + rdy*rdy);
                            
                            if (rockDist < 8.0f && rockDist > 0.5f) {
                                // Position behind rock relative to bullet direction
                                // Move to the far side of the rock from the bullet source
                                float behindX = br.x + bulletDirX * 1.5f;
                                float behindY = br.y + bulletDirY * 1.5f;
                                float cdx = behindX - enemy.x;
                                float cdy = behindY - enemy.y;
                                float coverDist = sqrtf(cdx*cdx + cdy*cdy);
                                
                                if (coverDist < bestCoverDist) {
                                    bestCoverDist = coverDist;
                                    coverX = cdx;
                                    coverY = cdy;
                                    foundCover = true;
                                }
                            }
                        }
                    }
                }
                
                if (foundCover && bestCoverDist > 0.3f) {
                    float coverNorm = sqrtf(coverX*coverX + coverY*coverY);
                    if (coverNorm > 0.1f) {
                        moveX += (coverX / coverNorm) * neuralCoverSeek * enemy.speed * 1.2f * deltaTime;
                        moveY += (coverY / coverNorm) * neuralCoverSeek * enemy.speed * 1.2f * deltaTime;
                    }
                }
            }
            
            float newX = enemy.x + moveX;
            float newY = enemy.y + moveY;
            float centerDx = newX - 32.0f;
            float centerDy = newY - 32.0f;
            bool blockedBySpire = (centerDx*centerDx + centerDy*centerDy < 9.0f);
            if (worldMap[(int)newX][(int)enemy.y] == 0 && !blockedBySpire && !IsPositionColliding(newX, enemy.y, 0.4f)) {
                 bool collision = false;
                 for(const auto& br : bigRocks) {
                     float dx = newX - br.x;
                     float dy = enemy.y - br.y;
                     if(dx*dx + dy*dy < 0.64f) { collision = true; break; }
                 }
                 if (!collision && healingTower.state != TOWER_DORMANT) {
                     float htdx = newX - healingTower.x;
                     float htdy = enemy.y - healingTower.y;
                     if(htdx*htdx + htdy*htdy < 1.0f) { collision = true; }
                 }
                 if (!collision) {
                     float pdx = newX - player.x;
                     float pdy = enemy.y - player.y;
                     if (pdx*pdx + pdy*pdy < 0.64f) { collision = true; }
                 }
                 if(!collision) enemy.x = newX;
            }
            centerDx = enemy.x - 32.0f;
            centerDy = newY - 32.0f;
            blockedBySpire = (centerDx*centerDx + centerDy*centerDy < 9.0f);
            if (worldMap[(int)enemy.x][(int)newY] == 0 && !blockedBySpire && !IsPositionColliding(enemy.x, newY, 0.4f)) {
                 bool collision = false;
                 for(const auto& br : bigRocks) {
                     float dx = enemy.x - br.x;
                     float dy = newY - br.y;
                     if(dx*dx + dy*dy < 0.64f) { collision = true; break; }
                 }
                 if (!collision && healingTower.state != TOWER_DORMANT) {
                     float htdx = enemy.x - healingTower.x;
                     float htdy = newY - healingTower.y;
                     if(htdx*htdx + htdy*htdy < 1.0f) { collision = true; }
                 }
                 if (!collision) {
                     float pdx = enemy.x - player.x;
                     float pdy = newY - player.y;
                     if (pdx*pdx + pdy*pdy < 0.64f) { collision = true; }
                 }
                 if(!collision) enemy.y = newY;
            }
            
            if (enemy.attackTimer > 0) enemy.attackTimer -= deltaTime;
            
            if (enemy.spriteIndex == 4 && !enemy.isSpearGuy && !enemy.isOfficer && enemy.isEnemy && !enemy.isMarshall) {
                int nearbyCount = 0;
                for (auto& other : enemies) {
                    if (&other == &enemy || !other.active) continue;
                    if (!other.isShooter && !other.isMarshall && !other.isOfficer && other.isEnemy) {
                        float d = sqrtf((enemy.x - other.x)*(enemy.x - other.x) + (enemy.y - other.y)*(enemy.y - other.y));
                        if (d < 8.0f) nearbyCount++;
                    }
                }
                if (nearbyCount >= 3) {
                    if (enemy.maxHealth == 4) {
                        enemy.maxHealth = 8;
                        enemy.health *= 2;
                    }
                } else {
                    if (enemy.maxHealth == 8) {
                        enemy.maxHealth = 4;
                        enemy.health /= 2;
                        if (enemy.health < 1) enemy.health = 1;
                    }
                }
            }

            float attackRange = 2.0f + (enemy.hasNeuralBrain ? neuralAggression * 0.5f : 0);
            if (dist < attackRange && enemy.attackTimer <= 0) {
                if (!godMode) {
                    if (enemy.spriteIndex == 4 && !enemy.isSpearGuy && !enemy.isOfficer && enemy.isEnemy) {
                        player.health -= 10;
                        // Apply knockback
                        playerKnockbackX = -(dx / dist) * 8.0f; // 2-4 tiles worth of velocity
                        playerKnockbackY = -(dy / dist) * 8.0f;
                    } else {
                        player.health -= 5;
                    }
                }
                if (enemy.hasNeuralBrain) {
                    enemy.brain.damageDealt += (enemy.spriteIndex == 4) ? 10.0f : 5.0f;
                }
                
                enemy.attackTimer = 1.0f;
                playerHurtTimer = 0.3f;
                PlayPlayerHurtSound();
                if (player.health <= 0) {
                    // score retained on continue
                    graves.push_back({player.x, player.y});
                    SaveGraves();
                    player.health = player.maxHealth;
                    player.level = 1; player.xp = 0; player.xpToNextLevel = 100; player.maxHealth = 100; if (johnSecret.isSpawned && johnSecret.naturalSpawn) { player.maxHealth *= 2; player.health = player.maxHealth; } g_BonusSpeed = 0.0f; g_PendingUpgrades = 0; g_LevelUpWindowOpen = false; player.x = 10.0f;
                    player.y = 32.0f;
                    gunUpgraded = false;
                    currentWeapon = 0;
                    playerDamage = 1;
                    maxAmmo = 8;
                    ammo = 8;
                    if (player.health <= 0) {
                        pendingGameReset = true;
                        if (!godMode) player.health = player.maxHealth; // Prevent further damage this frame
                        break;
                    }
                }
            }
        }
        
        if (enemy.hurtTimer > 0) enemy.hurtTimer -= deltaTime;
        
        enemy.distance = dist;
    }
    
    for (auto& eb : enemyBullets) {
        if (!eb.active) continue;
        
        eb.x += eb.dirX * eb.speed * deltaTime;
        eb.y += eb.dirY * eb.speed * deltaTime;
        
        if (eb.x < 0 || eb.x > MAP_WIDTH || eb.y < 0 || eb.y > MAP_HEIGHT) {
            eb.active = false;
            continue;
        }
        
        if (worldMap[(int)eb.x][(int)eb.y] != 0) {
            eb.active = false;
            continue;
        }
        
        for(const auto& br : bigRocks) {
             float dx = eb.x - br.x;
             float dy = eb.y - br.y;
             if(dx*dx + dy*dy < 0.49f) { 
eb.active = false; break; }
        }
        if (!eb.active) continue;
        
        bool hitDefected = false;
        for (auto& e : allies) {
            if (e.active) {
                float edx = e.x - eb.x;
                float edy = e.y - eb.y;
                if (sqrtf(edx*edx + edy*edy) < 0.5f) {
                    int dmg = eb.isLaser ? 10 : 5;
                    e.health -= dmg;
                    e.hurtTimer = 0.2f;
                    if (e.health <= 0) {
                        e.active = false;
                        if (e.isDefectedOfficer) {
                            
                            
                        }
                    }
                    eb.active = false;
                    hitDefected = true;
                    break;
                }
            }
        }
        if (hitDefected) continue;
        
        float pdx = player.x - eb.x;
        float pdy = player.y - eb.y;
        if (sqrtf(pdx*pdx + pdy*pdy) < 0.5f) {
            int dmg = eb.isLaser ? 10 : 5;
            if (!godMode) player.health -= dmg;
            playerHurtTimer = 0.3f;
            PlayPlayerHurtSound();
            eb.active = false;
            
                if (player.health <= 0) {
                    pendingGameReset = true;
                    if (!godMode) player.health = player.maxHealth; // Prevent further damage this frame
                    break;
                }
            }
        }
    
    // Prevent spawning and clear enemies during pre-boss phase
    if (preBossPhase) {
        enemies.clear();
        officerSpawned = false;
        
    } else if (!bossActive) {
        // Progressive spawn cap increase (pre-boss only)
        spawnCapTimer -= deltaTime;
        if (spawnCapTimer <= 0) {
            spawnCapTimer = 20.0f;
            if (maxMeleeSpawn < MELEE_CAP) maxMeleeSpawn += 3;
            if (maxMeleeSpawn > MELEE_CAP) maxMeleeSpawn = MELEE_CAP;
            if (maxShooterSpawn < SHOOTER_CAP) maxShooterSpawn += 1;
            if (maxShooterSpawn > SHOOTER_CAP) maxShooterSpawn = SHOOTER_CAP;
        }
        
        shooterSpawnTimer -= deltaTime;
        if (shooterSpawnTimer <= 0) {
            shooterSpawnTimer = 3.0f;
            
            int meleeCount = 0;
            int shooterCount = 0;
            for (auto& e : enemies) {
                if (e.active) {
                    if (e.isShooter) shooterCount++;
                    else meleeCount++;
                }
            }
            
            // Spawn melee up to current max
            if (meleeCount < maxMeleeSpawn) {
                int meleeToSpawn = maxMeleeSpawn - meleeCount;
                int spearGuysToSpawn = (meleeToSpawn >= 3) ? (meleeToSpawn / 3) : 0;
                for (int i = 0; i < meleeToSpawn; i++) {
                    Enemy enemy;
                    do {
                        enemy.x = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
                        enemy.y = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
                    } while (worldMap[(int)enemy.x][(int)enemy.y] != 0 || 
                             sqrtf((enemy.x - player.x)*(enemy.x - player.x) + (enemy.y - player.y)*(enemy.y - player.y)) < 15.0f);
                    enemy.active = true;
                    enemy.speed = 1.5f + (rand() % 100) / 100.0f;
                    enemy.distance = 0;
                    enemy.spriteIndex = rand() % 5;
                    if (enemy.spriteIndex == 4) { enemy.health = 4; enemy.maxHealth = 4; } else { enemy.health = 1; enemy.maxHealth = 1; }
                    enemy.hurtTimer = 0;
                    enemy.isShooter = false;
                    enemy.fireTimer = 0;
                    enemy.firingTimer = 0;
                    enemy.isMarshall = false; // Fix uninitialized
                    enemy.hasNeuralBrain = true;
                    NeuralAI::InheritBrain(enemy.brain);
                    if (i < spearGuysToSpawn) {
                        enemy.isSpearGuy = true;
                        enemy.health = 3; // Spearguy Hp: 3 bullet hits (or 15 dmg, health is per hit mostly? wait, 1 hit = 5 dmg, so maybe 3)
                        enemy.speed = 2.0f; // fast
                    }
                    pendingEnemies.push_back(enemy);
                }
            }
            
            // Spawn shooters up to current max
            if (shooterCount < maxShooterSpawn) {
                int shootersToSpawn = maxShooterSpawn - shooterCount;
                for (int i = 0; i < shootersToSpawn; i++) {
                    Enemy shooter;
                    do {
                        shooter.x = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
                        shooter.y = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
                    } while (worldMap[(int)shooter.x][(int)shooter.y] != 0 || 
                             sqrtf((shooter.x - player.x)*(shooter.x - player.x) + (shooter.y - player.y)*(shooter.y - player.y)) < 15.0f);
                    shooter.active = true;
                    shooter.speed = 1.2f;
                    shooter.distance = 0;
                    shooter.spriteIndex = 0;
                    shooter.health = 2;
                    shooter.hurtTimer = 0;
                    shooter.isShooter = true;
                    shooter.fireTimer = 2.0f;
                    shooter.firingTimer = 0;
                    shooter.isMarshall = false; // Fix uninitialized
                    shooter.hasNeuralBrain = true;
                    NeuralAI::InheritBrain(shooter.brain);
                    pendingEnemies.push_back(shooter);
                }
            }
            
            // Officer spawn logic
            if (!officerSpawned && shooterCount >= 4) {
                Enemy officer;
                do {
                    float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                    officer.x = player.x + cosf(angle) * 10.0f;
                    officer.y = player.y + sinf(angle) * 10.0f;
                    if (officer.x < 2.0f) officer.x = 2.0f; if (officer.x >= MAP_WIDTH - 2.0f) officer.x = MAP_WIDTH - 2.1f;
                    if (officer.y < 2.0f) officer.y = 2.0f; if (officer.y >= MAP_HEIGHT - 2.0f) officer.y = MAP_HEIGHT - 2.1f;
                } while (worldMap[(int)officer.x][(int)officer.y] != 0);
                officer.active = true;
                officer.speed = 1.2f;
                officer.distance = 0;
                officer.spriteIndex = 0;
                officer.health = 4;
                officer.maxHealth = 4;
                officer.hurtTimer = 0;
                officer.isShooter = true; // They can shoot
                officer.isOfficer = true;
                officer.fireTimer = 2.0f;
                officer.firingTimer = 0;
                officer.isMarshall = false;
                officer.hasNeuralBrain = true;
                officer.officerState = 0;
                officer.officerCooldown = 20.0f; // Begin with cooldown so reinforcement can't fire immediately
                NeuralAI::InheritBrain(officer.brain);
                pendingEnemies.push_back(officer);
                officerSpawned = true;
            }
        }
    }
    
    if (preBossPhase && !bossActive) {
        preBossTimer -= deltaTime;
        
        preBossPulseTimer += deltaTime;
        if (preBossPulseTimer >= 1.0f) {
            preBossPulseTimer = 0;
            preBossPulseFrame = !preBossPulseFrame;
        }
        
        if (preBossTimer <= 0) {
            preBossPhase = false; TriggerEvent("pre_boss_phase", false);
            bossActive = true;
            TriggerEvent("boss_active", true);
            bossEventTimer = 3.0f;
            
            for(int i=0; i<6; i++) {
                claws[i].state = CLAW_IDLE;
            }
            activeClawIndex = 0;
            claws[0].state = CLAW_CHASING;
            claws[0].timer = 4.0f;
            
            // Spawn 15 enemies for boss fight
            for(int i=0; i<15; i++) {
                Enemy enemy;
                enemy.x = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
                enemy.y = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
                if(sqrtf((enemy.x - player.x)*(enemy.x - player.x) + (enemy.y - player.y)*(enemy.y - player.y)) < 10.0f) {
                    enemy.x = 32; enemy.y = 5;
                }
                enemy.active = true;
                enemy.speed = 1.5f + (rand() % 100) / 100.0f;
                enemy.distance = 0;
                enemy.spriteIndex = rand() % 5;
                if (enemy.spriteIndex == 4) { enemy.health = 4; enemy.maxHealth = 4; } else { enemy.health = 1; enemy.maxHealth = 1; }
                enemy.hurtTimer = 0;
                enemy.isShooter = false;
                enemy.fireTimer = 0;
                enemy.firingTimer = 0;
                enemies.push_back(enemy);
            }
        }
    }
    
    // Boss Logic
    if (bossActive) { 
        // Trigger Phase 2
        if (!phase2Active && bossHealth <= 750) {
            phase2Active = true;
            forceFieldActive = true;
            enemies.clear(); // Kill all minions
            fireballs.clear();
            
            // Setup Claws for Phase 2
            for(int i=0; i<6; i++) {
                claws[i].state = CLAW_PH2_AWAKEN;
                claws[i].animFrame = 0;
                claws[i].animTimer = 0;
                claws[i].health = 250;
                claws[i].x = claws[i].homeX;
                claws[i].y = claws[i].homeY;
                claws[i].hurt = false;
                claws[i].hurtTimer = 0;
            }
            activeClawIndex = -1; // Reset active claw
            lastActiveClaw = 5;
        }

        if (!phase2Active) {
             // Phase 1: Fireballs
             fireballSpawnTimer -= deltaTime;
             if (fireballSpawnTimer <= 0) {
                 Fireball fb;
                 float centerX = 32.0f;
                 float centerY = 32.0f;
                 float dx = player.x - centerX;
                 float dy = player.y - centerY;
                 float dist = sqrtf(dx*dx + dy*dy);
                 
                 float dirX = 1.0f, dirY = 0.0f;
                 if(dist > 0) {
                     dirX = dx/dist;
                     dirY = dy/dist;
                 }
                 
                 // Spawn outside the boss collision radius (2.0f)
                 fb.x = centerX + dirX * 4.0f;
                 fb.y = centerY + dirY * 4.0f;
                 fb.dirX = dirX;
                 fb.dirY = dirY;
                 fb.speed = 8.0f;
                 fb.active = true;
                 fireballs.push_back(fb);
                 
                 fireballSpawnTimer = 1.5f;
             }
        }

        if (phase2Active) {
            // Phase 2 Logic
            phase2BossAnimTimer += deltaTime;
            if (phase2BossAnimTimer >= 0.5f) {
                phase2BossFrame = (phase2BossFrame + 1) % 3;
                phase2BossAnimTimer = 0;
            }
            
            int livingClaws = 0;
            for(int i=0; i<6; i++) {
                Claw& c = claws[i];
                if (c.state != CLAW_PH2_DEAD) livingClaws++;
                
                if (c.hurtTimer > 0) c.hurtTimer -= deltaTime;
                
                if (c.state == CLAW_PH2_AWAKEN) {
                    c.animTimer += deltaTime;
                    if (c.animTimer >= 0.5f) {
                        c.animFrame++;
                        c.animTimer = 0;
                        if (c.animFrame >= 4) {
                            c.animFrame = 3; // Stay on last frame
                            c.state = CLAW_IDLE; // Wait for selection
                        }
                    }
                } else if (c.state == CLAW_PH2_DROPPING) {
                    c.timer -= deltaTime;
                    if (c.timer <= 0) {
                        c.state = CLAW_PH2_ANCHORED;
                        c.timer = 10.0f; // Anchor time
                    }
                } else if (c.state == CLAW_PH2_ANCHORED) {
                    c.timer -= deltaTime;
                    laserTimer += deltaTime;
                    if (laserTimer >= 0.5f) { // Rapid burst every 0.5s
                        // Fire laser projectile at player
                        float targetX = player.x; float targetY = player.y;
                        float closestDist = sqrtf((player.x - c.x)*(player.x - c.x) + (player.y - c.y)*(player.y - c.y));
                        for (auto& e : allies) {
                            if (e.active) {
                                float d = sqrtf((e.x - c.x)*(e.x - c.x) + (e.y - c.y)*(e.y - c.y));
                                if (d < closestDist) { closestDist = d; targetX = e.x; targetY = e.y; }
                            }
                        }
                        float dx = targetX - c.x;
                        float dy = targetY - c.y;
                        float dist = sqrtf(dx*dx + dy*dy);
                        if (dist > 0.1f) {
                            EnemyBullet laser;
                            laser.x = c.x;
                            laser.y = c.y;
                            laser.dirX = dx / dist;
                            laser.dirY = dy / dist;
                            laser.speed = 15.0f; // Fast laser
                            laser.active = true;
                            laser.isLaser = true;
                            enemyBullets.push_back(laser);
                        }
                        laserTimer = 0;
                    }
                    if (c.timer <= 0) {
                        c.state = CLAW_PH2_RISING; // Rising animation
                        c.timer = 2.0f; // 2s rise
                        c.x = c.homeX;
                        c.y = c.homeY;
                        activeLaserClaw = -1;
                    }
                } else if (c.state == CLAW_PH2_RISING) {
                    c.timer -= deltaTime;
                    float progress = c.timer / 2.0f;
                    if (progress < 0) progress = 0;
                    c.x = c.homeX;
                    c.y = c.homeY;
                    if (c.timer <= 0) {
                        if (c.health <= 0) {
                            c.state = CLAW_PH2_DEAD;
                        } else {
                            c.state = CLAW_IDLE;
                        }
                    }
                }
            }
            if (livingClaws == 0 && !enragedMode) {
                enragedMode = true;
                forceFieldActive = false;
                
                for(int i = 0; i < 6; i++) {
                    claws[i].state = CLAW_IDLE;
                    claws[i].health = 999;
                    claws[i].x = claws[i].homeX;
                    claws[i].y = claws[i].homeY;
                }
            }
            
            if (enragedMode) {
                forceFieldActive = false;
                
                fireballSpawnTimer -= deltaTime;
                if (fireballSpawnTimer <= 0) {
                    Fireball fb;
                    fb.x = 32.0f;
                    fb.y = 32.0f;
                    float dx = player.x - 32.0f;
                    float dy = player.y - 32.0f;
                    float dist = sqrtf(dx*dx + dy*dy);
                    if(dist > 0) {
                        fb.dirX = dx/dist;
                        fb.dirY = dy/dist;
                    } else {
                        fb.dirX = 1; fb.dirY = 0;
                    }
                    
                    // Spawn at 4.0f radius
                    fb.x = 32.0f + fb.dirX * 4.0f;
                    fb.y = 32.0f + fb.dirY * 4.0f;
                    fb.speed = 8.0f;
                    fb.active = true;
                    fireballs.push_back(fb);
                    
                    fireballSpawnTimer = 0.8f;
                }
                
                for(int i = 0; i < 6; i++) {
                    Claw& claw = claws[i];
                    
                    if(claw.state == CLAW_IDLE) {
                        claw.state = CLAW_CHASING;
                        claw.timer = 2.0f;
                    }
                    else if(claw.state == CLAW_CHASING) {
                        float dx = player.x - claw.x;
                        float dy = player.y - claw.y;
                        float dist = sqrtf(dx*dx + dy*dy);
                        if(dist > 0.5f) {
                            claw.x += (dx/dist) * 12.0f * deltaTime;
                            claw.y += (dy/dist) * 12.0f * deltaTime;
                        }
                        claw.timer -= deltaTime;
                        if(claw.timer <= 0) {
                            claw.state = CLAW_SLAMMING;
                            claw.timer = 0.3f;
                            claw.groundY = claw.y;
                            claw.dealtDamage = false;
                        }
                    }
                    else if(claw.state == CLAW_SLAMMING) {
                        claw.timer -= deltaTime;
                        if(claw.timer <= 0 && !claw.dealtDamage) {
                            float dx = player.x - claw.x;
                            float dy = player.y - claw.y;
                            float dist = sqrtf(dx*dx + dy*dy);
                            float aoeRadius = 5.0f + (rand() % 5);
                            if(dist < aoeRadius) {
                                if (!godMode) player.health -= 15;
                                playerHurtTimer = 0.3f;
                                PlayPlayerHurtSound();
                                if(player.health <= 0) {
                                    // score retained on continue
                                    player.health = player.maxHealth;
                                    player.level = 1; player.xp = 0; player.xpToNextLevel = 100; player.maxHealth = 100; if (johnSecret.isSpawned && johnSecret.naturalSpawn) { player.maxHealth *= 2; player.health = player.maxHealth; } g_BonusSpeed = 0.0f; g_PendingUpgrades = 0; g_LevelUpWindowOpen = false; player.x = 10.0f;
                                    player.y = 32.0f;
                                    bossActive = false; TriggerEvent("boss_active", false);
                                    preBossPhase = false; TriggerEvent("pre_boss_phase", false);
                                    bossHealth = 1500;
                                    enemies.clear();
                                    fireballs.clear();
                                    InitClaws();
                                    SpawnEnemies();
                                    phase2Active = false;
                                    forceFieldActive = false;
                                    enragedMode = false;
                                }
                            }
                            claw.dealtDamage = true;
                            claw.state = CLAW_RISING;
                            claw.timer = 0.5f;
                            PlaySlamSound();
                            screenShakeTimer = 1.0f;
                            screenShakeIntensity = 60.0f;
                        }
                    }
                    else if(claw.state == CLAW_RISING) {
                        claw.timer -= deltaTime;
                        if(claw.timer <= 0) {
                            claw.state = CLAW_RETURNING;
                        }
                    }
                    else if(claw.state == CLAW_RETURNING) {
                        float dx = claw.homeX - claw.x;
                        float dy = claw.homeY - claw.y;
                        float dist = sqrtf(dx*dx + dy*dy);
                        if(dist > 0.5f) {
                            claw.x += (dx/dist) * 15.0f * deltaTime;
                            claw.y += (dy/dist) * 15.0f * deltaTime;
                        } else {
                            claw.state = CLAW_IDLE;
                        }
                    }
                }
            } else if (livingClaws > 0) {
                forceFieldActive = true;
            }
            
            // Pick new claw to anchor sequentially (only if force field still active)
            if (activeLaserClaw == -1 && livingClaws > 0 && forceFieldActive) {
                int attempts = 0;
                int idx = (lastActiveClaw + 1) % 6;
                bool found = false;
                
                // Find next living claw that is ready (CLAW_IDLE)
                for(int i=0; i<6; i++) {
                    if (claws[idx].state == CLAW_PH2_DEAD) {
                        idx = (idx + 1) % 6;
                        continue; // Skip dead claws
                    }
                    if (claws[idx].state == CLAW_IDLE) {
                        found = true;
                        break;
                    }
                    idx = (idx + 1) % 6;
                }
                
                if (found) {
                    activeLaserClaw = idx;
                    lastActiveClaw = idx;
                    claws[idx].state = CLAW_PH2_DROPPING;
                    claws[idx].timer = 2.0f; // Drop time
                    SpawnGravitals(player.x, player.y);
                }
            }

            
        } else {
            // Phase 1 Logic
            if (bossEventTimer > 0) bossEventTimer -= deltaTime;
            
            fireballSpawnTimer -= deltaTime;
            if (fireballSpawnTimer <= 0) {
                Fireball fb;
                fb.x = 32.0f;
                fb.y = 32.0f;
                float dx = player.x - 32.0f;
                float dy = player.y - 32.0f;
                float dist = sqrtf(dx*dx + dy*dy);
                if(dist > 0) {
                    fb.dirX = dx/dist;
                    fb.dirY = dy/dist;
                } else {
                    fb.dirX = 1; fb.dirY = 0;
                }
                fb.speed = 5.0f; 
                fb.active = true;
                fireballs.push_back(fb);
                
                fireballSpawnTimer = 2.0f; 
            }
            
            for(int i = 0; i < 6; i++) {
                Claw& claw = claws[i];
                
                if(claw.state == CLAW_CHASING) {
                    float dx = player.x - claw.x;
                    float dy = player.y - claw.y;
                    float dist = sqrtf(dx*dx + dy*dy);
                    if(dist > 0.5f) {
                        claw.x += (dx/dist) * 8.0f * deltaTime;
                        claw.y += (dy/dist) * 8.0f * deltaTime;
                    }
                    claw.timer -= deltaTime;
                    if(claw.timer <= 0) {
                        claw.state = CLAW_SLAMMING;
                        claw.timer = 0.5f;
                        claw.groundY = claw.y;
                        claw.dealtDamage = false;
                    }
                }
                else if(claw.state == CLAW_SLAMMING) {
                    claw.timer -= deltaTime;
                    if(claw.timer <= 0 && !claw.dealtDamage) {
                        float dx = player.x - claw.x;
                        float dy = player.y - claw.y;
                        float dist = sqrtf(dx*dx + dy*dy);
                        float aoeRadius = 4.0f + (rand() % 5);
                        if(dist < aoeRadius) {
                            if (!godMode) player.health -= 10;
                            playerHurtTimer = 0.3f;
                            PlayPlayerHurtSound();
                            if(player.health <= 0) {
                                // score retained on continue
                                graves.push_back({player.x, player.y});
                                SaveGraves();
                                player.health = player.maxHealth;
                                player.level = 1; player.xp = 0; player.xpToNextLevel = 100; player.maxHealth = 100; if (johnSecret.isSpawned && johnSecret.naturalSpawn) { player.maxHealth *= 2; player.health = player.maxHealth; } g_BonusSpeed = 0.0f; g_PendingUpgrades = 0; g_LevelUpWindowOpen = false; player.x = 10.0f;
                                player.y = 32.0f;
                                
                                bossActive = false; TriggerEvent("boss_active", false);
                                preBossPhase = false; TriggerEvent("pre_boss_phase", false);
                                bossHealth = 200;
                                enemies.clear();
                                fireballs.clear();
                                InitClaws();
                                SpawnEnemies();
                                phase2Active = false;
                                enragedMode = false;
                            }
                        }
                        claw.dealtDamage = true;
                        claw.state = CLAW_RISING;
                        claw.timer = 1.0f;
                        PlaySlamSound();
                        screenShakeTimer = 1.0f;
                        screenShakeIntensity = 50.0f;
                    }
                }
                else if(claw.state == CLAW_RISING) {
                    claw.timer -= deltaTime;
                    if(claw.timer <= 0) {
                        claw.state = CLAW_RETURNING;
                    }
                }
                else if(claw.state == CLAW_RETURNING) {
                    float dx = claw.homeX - claw.x;
                    float dy = claw.homeY - claw.y;
                    float dist = sqrtf(dx*dx + dy*dy);
                    if(dist > 0.5f) {
                        claw.x += (dx/dist) * clawReturnSpeed * deltaTime;
                        claw.y += (dy/dist) * clawReturnSpeed * deltaTime;
                    } else {
                        claw.x = claw.homeX;
                        claw.y = claw.homeY;
                        claw.state = CLAW_IDLE;
                        
                        activeClawIndex = (activeClawIndex + 1) % 6;
                        claws[activeClawIndex].state = CLAW_CHASING;
                        claws[activeClawIndex].timer = 4.0f;
                    }
                }
            }
        }
    }
    
    // Update Fireballs
    for(auto& fb : fireballs) {
        if(!fb.active) continue;
        
        fb.x += fb.dirX * fb.speed * deltaTime;
        fb.y += fb.dirY * fb.speed * deltaTime;
        
        float distToSpire = sqrtf((fb.x - 32.0f)*(fb.x - 32.0f) + (fb.y - 32.0f)*(fb.y - 32.0f));
                 if (distToSpire < 2.0f && bossActive) {
            // Hit boss
            fb.active = false;
        }
        
        for(const auto& br : bigRocks) {
             float dx = fb.x - br.x;
             float dy = fb.y - br.y;
             if(dx*dx + dy*dy < 0.49f) { 
fb.active = false; break; }
        }
        
        if(fb.x < 0 || fb.x > MAP_WIDTH || fb.y < 0 || fb.y > MAP_HEIGHT) fb.active = false;
        if (!fb.active) continue; // Check if it was deactivated by rock or spire
        
        float dx = player.x - fb.x;
        float dy = player.y - fb.y;
        if (sqrtf(dx*dx + dy*dy) < 0.5f) {
            if (!godMode) player.health -= 10;
            playerHurtTimer = 0.3f;
            fb.active = false;
            
            if (player.health <= 0) {
                // score retained on continue
                graves.push_back({player.x, player.y});
                SaveGraves();
                player.health = player.maxHealth;
                player.level = 1; player.xp = 0; player.xpToNextLevel = 100; player.maxHealth = 100; if (johnSecret.isSpawned && johnSecret.naturalSpawn) { player.maxHealth *= 2; player.health = player.maxHealth; } g_BonusSpeed = 0.0f; g_PendingUpgrades = 0; g_LevelUpWindowOpen = false; player.x = 10.0f;
                player.y = 32.0f;
                gunUpgraded = false;
                currentWeapon = 0;
                playerDamage = 1;
                maxAmmo = 8;
                ammo = 8;
                
                if (bossActive) {
                    bossActive = false; TriggerEvent("boss_active", false);
                    preBossPhase = false; TriggerEvent("pre_boss_phase", false);
                    bossHealth = 1500;
                    phase2Active = false;
                    enragedMode = false;
                    enemies.clear();
                    fireballs.clear();
                    InitClaws();
                }
                SpawnEnemies();
            }
        }
        
        if(fb.x < 0 || fb.x > MAP_WIDTH || fb.y < 0 || fb.y > MAP_HEIGHT) fb.active = false;
    }
    
    
    if (bossHurtTimer > 0) bossHurtTimer -= deltaTime;
    if (playerHurtTimer > 0) playerHurtTimer -= deltaTime;
    
    if (bossActive && !bossDead && !phase2Active) {
        bossSpawnTimer -= deltaTime;
        if (bossSpawnTimer <= 0) {
            bossSpawnTimer = 2.0f;
            
            int meleeCount = 0, shooterCount = 0;
            for (auto& e : enemies) {
                if (e.active) {
                    if (e.isShooter) shooterCount++;
                    else meleeCount++;
                }
            }
            
            const int BOSS_MELEE_CAP = 30;
            const int BOSS_SHOOTER_CAP = 10;
            
            if (meleeCount < BOSS_MELEE_CAP) {
                Enemy e;
                int attempts = 0;
                do {
                    float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                    float dist = 8.0f + (float)(rand() % 20);
                    e.x = player.x + cosf(angle) * dist;
                    e.y = player.y + sinf(angle) * dist;
                    if (e.x < 1.5f) e.x = 1.5f; if (e.x >= MAP_WIDTH - 1.5f) e.x = (float)(MAP_WIDTH - 2);
                    if (e.y < 1.5f) e.y = 1.5f; if (e.y >= MAP_HEIGHT - 1.5f) e.y = (float)(MAP_HEIGHT - 2);
                    attempts++;
                } while (worldMap[(int)e.x][(int)e.y] != 0 && attempts < 10);
                
                if (worldMap[(int)e.x][(int)e.y] == 0) {
                    e.active = true;
                    e.health = 1; // standard
                    e.speed = 3.0f; // Slower than player (4.0)
                    e.spriteIndex = rand() % 5;
                    e.hurtTimer = 0;
                    e.isShooter = false;
                    e.fireTimer = 0;
                    e.firingTimer = 0;
                    e.isMarshall = false; // Fix uninitialized
                    enemies.push_back(e);
                }
            }
            
            if (shooterCount < BOSS_SHOOTER_CAP) {
                Enemy e;
                int attempts = 0;
                do {
                    float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                    float dist = 10.0f + (float)(rand() % 15);
                    e.x = player.x + cosf(angle) * dist;
                    e.y = player.y + sinf(angle) * dist;
                    if (e.x < 1.5f) e.x = 1.5f; if (e.x >= MAP_WIDTH - 1.5f) e.x = (float)(MAP_WIDTH - 2);
                    if (e.y < 1.5f) e.y = 1.5f; if (e.y >= MAP_HEIGHT - 1.5f) e.y = (float)(MAP_HEIGHT - 2);
                    attempts++;
                } while (worldMap[(int)e.x][(int)e.y] != 0 && attempts < 10);
                
                if (worldMap[(int)e.x][(int)e.y] == 0) {
                    e.active = true;
                    e.health = 1; // standard
                    e.speed = 2.0f; 
                    e.spriteIndex = 0;
                    e.hurtTimer = 0;
                    e.isShooter = true;
                    e.fireTimer = 2.0f + (float)(rand() % 20) / 10.0f;
                    e.firingTimer = 0;
                    e.isMarshall = false; // Fix uninitialized
                    pendingEnemies.push_back(e);
                }
            }
        }
    }
    
    for (auto& pe : pendingEnemies) {
        enemies.push_back(pe);
    }
    pendingEnemies.clear();
    
    for (auto& pa : pendingAllies) {
        allies.push_back(pa);
    }
    pendingAllies.clear();
}

void UpdateGun(float deltaTime) {
    if (fireTimer > 0) fireTimer -= deltaTime;
    
    if (isMoving) {
        gunSwayPhase += deltaTime * 8.0f;
        gunSwayX = sinf(gunSwayPhase) * 15.0f;
        gunSwayY = fabsf(cosf(gunSwayPhase * 2.0f)) * 8.0f;
    } else {
        gunSwayX *= 0.9f;
        gunSwayY *= 0.9f;
        gunSwayPhase = 0;
    }
    
    if (isReloading) {
        reloadTimer += deltaTime;
        
        // Sound Logic
        if (reloadTimer > 0.1f && reloadStage == 0) { PlayReloadSound(0); reloadStage++; }
        if (reloadTimer > 1.4f && reloadStage == 1) { PlayReloadSound(1); reloadStage++; }
        if (reloadTimer > 2.2f && reloadStage == 2) { PlayReloadSound(2); reloadStage++; }
        
        if (reloadTimer < reloadDuration / 2) {
            gunReloadOffset = (reloadTimer / (reloadDuration / 2)) * 300;
        } else if (reloadTimer < reloadDuration) {
            gunReloadOffset = 300 - ((reloadTimer - reloadDuration / 2) / (reloadDuration / 2)) * 300;
        } else {
            isReloading = false;
            reloadTimer = 0;
            gunReloadOffset = 0;
            ammo = maxAmmo;
        }
    }
    
    // Recoil Decay
    if (gunRecoil > 0.1f) {
        gunRecoil *= 0.85f; // Decay recoil
    } else {
        gunRecoil = 0;
    }
}

void StartReload() {
    if (isReloading || ammo == maxAmmo) return;
    isReloading = true;
    reloadTimer = 0;
    reloadStage = 0;
}

void ShootBullet() {
    if (fireTimer > 0 || isReloading || ammo <= 0) return;
    
    ammo--;
    
    if (currentWeapon == 2) {
        Rocket r;
        r.x = player.x;
        r.y = player.y;
        r.dirX = cosf(player.angle);
        r.dirY = sinf(player.angle);
        r.speed = 25.0f; // Fast rocket
        r.active = true;
        r.isEnemy = false; 
        r.targetX = 0; r.targetY = 0;
        r.z = 0.5f; // Eye level
        r.verticalSpeed = 0;
        r.startX = r.x;
        r.startY = r.y;
        r.maxRange = 64.0f;
        r.safetyTimer = 0.2f;
        rockets.push_back(r);
        PlayGunSound(2); // Bazooka Fire Sound
        gunRecoil = 80.0f; // Strong recoil for Bazooka
    } else if (currentWeapon == 1) { // Shotgun - 5 Pellets
        for (int i = 0; i < 5; i++) {
            Bullet b;
            b.x = player.x;
            b.y = player.y;
            // Spread: -0.1 to +0.1 radians
            float spread = (i - 2) * 0.05f; 
            b.dirX = cosf(player.angle + spread);
            b.dirY = sinf(player.angle + spread);
            b.speed = 10.0f;
            b.active = true;
            b.damage = 1; // 1 damage per pellet
            b.startX = b.x;
            b.startY = b.y;
            b.maxRange = 12.0f;
            bullets.push_back(b);
        }
        PlayGunSound(0); // Standard sound for now (could pitch shift if needed)
        gunRecoil = 40.0f; // Medium recoil
    } else {
        Bullet b;
        b.x = player.x;
        b.y = player.y;
        b.dirX = cosf(player.angle);
        b.dirY = sinf(player.angle);
        b.speed = 10.0f;
        b.active = true;
        b.damage = 1; // Standard damage fixed to 1
        b.startX = b.x;
        b.startY = b.y;
        b.maxRange = 24.0f;
        bullets.push_back(b);
        PlayGunSound(0);
        gunRecoil = 20.0f; // Light recoil
    }
    
    isFiring = true;
    fireTimer = 0.30f;
    
    PlayGunSound();
}

void UpdateBullets(float deltaTime) {
    UpdateGravitals(deltaTime);
    bool shouldClearEnemies = false;
    for (auto& r : rockets) {
        if (!r.active) continue;
        
        if (r.isEnemy) {
            // Marshall Rocket Logic: Arc + Homing
            r.verticalSpeed -= 20.0f * deltaTime; // Gravity
            r.z += r.verticalSpeed * deltaTime;
            
            // Homing Movement
            float dx = r.targetX - r.x;
            float dy = r.targetY - r.y;
            float dist = sqrtf(dx*dx + dy*dy);
            
            if (dist > 0.1f) {
                // Move towards target
                float moveSpeed = 12.0f; // Horizontal speed
                r.x += (dx / dist) * moveSpeed * deltaTime;
                r.y += (dy / dist) * moveSpeed * deltaTime;
            }
            
            bool hitRock = false;
            for(const auto& br : bigRocks) {
                 float dx = r.x - br.x;
                 float dy = r.y - br.y;
                 if(dx*dx + dy*dy < 0.49f) { hitRock = true; break; }
            }
            if (hitRock) {
                r.active = false;
                Explosion ex;
                ex.x = r.x; ex.y = r.y; ex.timer = 1.0f; ex.active = true;
                explosions.push_back(ex);
                PlayBazookaExplosionSound();
                continue;
            }

            
            // Spawn Trail
            if ((int)(GetTickCount() / 50) % 2 == 0) {
                 RocketTrail t;
                 t.x = r.x; t.y = r.y; t.life = 0.5f; t.active = true;
                 rocketTrails.push_back(t); // Note: Trails don't support Z yet, but looks okay
            }
            
            if (r.z <= 0) { // Ground Impact
                r.active = false;
                Explosion ex;
                ex.x = r.x; ex.y = r.y; ex.timer = 1.0f; ex.active = true;
                explosions.push_back(ex);
                PlayBazookaExplosionSound();
                
                // Damage Player
                float pdx = player.x - r.x;
                float pdy = player.y - r.y;
                if (sqrtf(pdx*pdx + pdy*pdy) < 3.0f) {
                    if (!godMode) player.health -= 15;
                    PlayPlayerHurtSound();
                    playerHurtTimer = 0.5f;
                    screenShakeTimer = 0.5f;
                }
            }
            
        } else {
            // Player Rocket Logic (Straight)
            r.x += r.dirX * r.speed * deltaTime;
            r.y += r.dirY * r.speed * deltaTime;
            
            // Range Check
            float travelledDx = r.x - r.startX;
            float travelledDy = r.y - r.startY;
            if (sqrtf(travelledDx*travelledDx + travelledDy*travelledDy) > r.maxRange) {
                 r.active = false;
                 Explosion ex;
                 ex.x = r.x; ex.y = r.y; ex.timer = 1.0f; ex.active = true;
                 explosions.push_back(ex);
                 PlayBazookaExplosionSound();
                 continue;
            }
            
            if (r.safetyTimer > 0) {
                r.safetyTimer -= deltaTime;
                // Safety time only prevents self-damage, not collision detection
            }
            
            // Spawn Trail
            if ((int)(GetTickCount() / 50) % 2 == 0) {
                 RocketTrail t;
                 t.x = r.x; t.y = r.y; t.life = 0.5f; t.active = true;
                 rocketTrails.push_back(t);
            }
            
            bool hit = false;
            int mx = (int)r.x;
            int my = (int)r.y;
            
            if (mx < 0 || mx >= MAP_WIDTH || my < 0 || my >= MAP_HEIGHT || worldMap[mx][my] != 0) hit = true;
            
            if (!hit) {
                 for (auto& e : enemies) {
                     if (!e.active) continue;
                     float edx = r.x - e.x;
                     float edy = r.y - e.y;
                     if (sqrtf(edx*edx + edy*edy) < 1.0f) { hit = true; break; }
                 }
            }
            
            if (!hit) {
                 for(const auto& br : bigRocks) {
                     float dx = r.x - br.x;
                     float dy = r.y - br.y;
                     if(dx*dx + dy*dy < 0.49f) { hit = true; break; }
                 }
            }
            
            // Healing Tower Collision (always, regardless of state)
            if (!hit) {
                 float htdx = r.x - healingTower.x;
                 float htdy = r.y - healingTower.y;
                 if (htdx*htdx + htdy*htdy < 1.0f) { hit = true; }
            }

            
            if (!hit && bossActive) {
                 float bdx = r.x - 32.0f;
                 float bdy = r.y - 32.0f;
                 if (sqrtf(bdx*bdx + bdy*bdy) < 2.5f) hit = true;
            }
            
            if (!hit && phase2Active) {
                 for (int i = 0; i < 6; i++) {
                     if (claws[i].state == CLAW_PH2_DEAD) continue;
                     float cdx = r.x - claws[i].x;
                     float cdy = r.y - claws[i].y;
                     if (sqrtf(cdx*cdx + cdy*cdy) < 2.0f) { hit = true; break; }
                 }
            }
            
            if (hit) {
                r.active = false;
                Explosion ex;
                ex.x = r.x; ex.y = r.y; ex.timer = 1.0f; ex.active = true;
                explosions.push_back(ex);
                
                PlayBazookaExplosionSound();
                screenShakeTimer = 0.5f;
                screenShakeIntensity = 20.0f;
                
                for (auto& g : gravitals) {
                    if (!g.active) continue;
                    float dX = r.x - g.x;
                    float dY = r.y - g.y;
                    float dist = sqrtf(dX*dX + dY*dY);
                    if (dist < 8.0f) {
                        int damage = 5; // 50 * 0.1 (90% reduction)
                        g.health -= damage;
                        g.hurtTimer = 0.5f;
                        if (g.health <= 0) {
                            g.active = false;
                            score += 5;
                            GivePlayerXP(20);
                            PlayScoreSound();
                        }
                    }
                }
                
                for (auto& e : enemies) {
                    if (!e.active) continue;
                    float dX = r.x - e.x;
                    float dY = r.y - e.y;
                    float dist = sqrtf(dX*dX + dY*dY);
                    if (dist < 8.0f) {
                        int damage = 50;
                        if (e.isMarshall && activeCommand == CMD_PINCER) damage = 25;
                        e.health -= damage;
                        if (e.spriteIndex == 4 || e.isShooter || e.isMarshall) e.hurtTimer = 0.5f;
                        if (e.isMarshall) PlayMarshallHurtSound(); else PlayEnemyHurtSound();
                        
                        if (e.health <= 0) {
                            if (e.hasNeuralBrain && !e.isMarshall) {
                                NeuralAI::UpdateGlobalBest(e.brain);
                            }
                            e.active = false;
                            if (e.isMarshall) { marshallKilled = true; bazookaUnlocked = true; upgradeMessageTimer = 3.0f; }
                            if (e.isOfficer) { officerSpawned = false; } // Reset so officer can respawn
                            score++;
                            GivePlayerXP(GetEnemyXP(e));
                            PlayScoreSound();
                            if (score > highScore) { highScore = score; SaveHighScore(); }
                            if (score >= 1000 && !bossActive && !preBossPhase) { preBossPhase = true; TriggerEvent("pre_boss_phase", true); preBossTimer = 30.0f; }
                        }
                    }
                }
                
                if (bossActive) {
                    float bdx = r.x - 32.0f;
                    float bdy = r.y - 32.0f;
                    if (sqrtf(bdx*bdx + bdy*bdy) < 8.0f) {
                        if (!forceFieldActive) {
                            bossHealth -= 50;
                            bossHurtTimer = 2.0f;
                            PlayScoreSound();
                            if (bossHealth <= 0) {
                                bossActive = false; TriggerEvent("boss_active", false);
                                bossDead = true;
                                postBossPhase = true;
                                musicRunning = false;
                                score += 50;
                                GivePlayerXP(1000);
                                if (score > highScore) { highScore = score; SaveHighScore(); }
                                for (auto& e : enemies) e.active = false;
                                enemies.clear();
                                fireballs.clear();
                                for(int i = 0; i < 6; i++) {
                                    claws[i].state = CLAW_DORMANT;
                                    claws[i].x = claws[i].homeX;
                                    claws[i].y = claws[i].homeY;
                                }
                                phase2Active = false;
                                forceFieldActive = false;
                                activeLaserClaw = -1;
                                
                                wchar_t exePath[MAX_PATH];
                                GetModuleFileNameW(NULL, exePath, MAX_PATH);
                                wchar_t* lastSlash = wcsrchr(exePath, L'\\');
                                if (lastSlash) *lastSlash = L'\0';
                                wchar_t dialoguePath[MAX_PATH];
                                swprintf(dialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\leader.line", exePath);
                                
                                NPCSystem::ClearNPCs();
                                NPCSystem::SpawnNPC(32.0f, 28.0f, L"Leader", leaderIdlePixels, leaderIdleW, leaderIdleH, leaderTalkingPixels, leaderTalkingW, leaderTalkingH, dialoguePath);
                                
                                wchar_t followerDialoguePath[MAX_PATH];
                                swprintf(followerDialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\followers.line", exePath);
                                NPCSystem::SpawnNPC(29.0f, 28.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                                NPCSystem::SpawnNPC(35.0f, 28.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                                NPCSystem::SpawnNPC(27.0f, 30.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                                NPCSystem::SpawnNPC(37.0f, 30.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                                
                                wchar_t victoryMusicPath[MAX_PATH];
                                swprintf(victoryMusicPath, MAX_PATH, L"open \"%ls\\assets\\sound-effects\\victory.mp3\" type mpegvideo alias victory", exePath);
                                mciSendStringW(victoryMusicPath, NULL, 0, NULL);
                                mciSendStringW(L"play victory repeat", NULL, 0, NULL);
                            }
                        }
                    }
                }
                
                if (phase2Active) {
                    for (int i = 0; i < 6; i++) {
                        if (claws[i].state == CLAW_PH2_DEAD) continue;
                        float cdx = r.x - claws[i].x;
                        float cdy = r.y - claws[i].y;
                        if (sqrtf(cdx*cdx + cdy*cdy) < 8.0f) {
                            claws[i].health -= 50;
                            claws[i].hurtTimer = 0.2f;
                            if (claws[i].health <= 0) {
                                claws[i].state = CLAW_PH2_RISING;
                                claws[i].timer = 2.0f;
                                PlayScoreSound();
                                if (activeLaserClaw == i) activeLaserClaw = -1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Update Trails and Explosions
    for (auto& t : rocketTrails) {
        if (t.active) {
            t.life -= deltaTime;
            if (t.life <= 0) t.active = false;
        }
    }
    for (auto& ex : explosions) {
        if (ex.active) {
            ex.timer -= deltaTime;
            if (ex.timer <= 0) ex.active = false;
        }
    }

    if (isFiring && fireTimer < 0.1f) isFiring = false;
    


    for (auto& b : bullets) {
        if (!b.active) continue;
        
        b.x += b.dirX * b.speed * deltaTime;
        b.y += b.dirY * b.speed * deltaTime;
        
        // Range Check
        float travelledDx = b.x - b.startX;
        float travelledDy = b.y - b.startY;
        if (sqrtf(travelledDx*travelledDx + travelledDy*travelledDy) > b.maxRange) {
            b.active = false;
            continue;
        }
        
        int mx = (int)b.x;
        int my = (int)b.y;
        if (mx < 0 || mx >= MAP_WIDTH || my < 0 || my >= MAP_HEIGHT || worldMap[mx][my] != 0) {
            b.active = false;
            continue;
        }
        
        for (auto& g : gravitals) {
            if (!g.active) continue;
            float dx = b.x - g.x;
            float dy = b.y - g.y;
            if (dx*dx + dy*dy < 1.0f) {
                b.active = false; 
                g.health -= 1; // Standard bullet damage
                g.hurtTimer = 0.5f;
                if (g.health <= 0) {
                     g.active = false;
                     score += 5;
                     GivePlayerXP(20);
                     PlayScoreSound();
                }
                break;
            }
        }
        if (!b.active) continue;

        for(const auto& br : bigRocks) {
             float dx = b.x - br.x;
             float dy = b.y - br.y;
             if(dx*dx + dy*dy < 0.49f) { b.active = false; break; }
        }
        if (!b.active) continue;
        
        for (auto& enemy : enemies) {
            if (!enemy.active) continue;
            if (!enemy.isEnemy) continue; // Don't hit friendly defected units
            float edx = b.x - enemy.x;
            float edy = b.y - enemy.y;
            if (sqrtf(edx*edx + edy*edy) < 0.4f) {
                b.active = false;
                
                if (enemy.isSpearGuy && enemy.spearState == 3) {
                    PlayEnemyHurtSound();
                    break;
                }
                
                enemy.health -= b.damage;
                if (enemy.spriteIndex == 4 || enemy.isShooter || enemy.isSpearGuy) enemy.hurtTimer = 0.5f;
                
                if (enemy.isMarshall) {
                    enemy.hurtTimer = 0.5f;
                    PlayMarshallHurtSound();
                } else {
                    PlayEnemyHurtSound();
                }
                
                if (enemy.health <= 0) {
                    enemy.active = false;
                    if (enemy.isOfficer) {
                        officerSpawned = false;
                    }

                    if (enemy.isMarshall) {
                        marshallKilled = true;
                        bazookaUnlocked = true;
                        upgradeMessageTimer = 3.0f;
                    }
                    score++;
                    GivePlayerXP(GetEnemyXP(enemy));
                    PlayScoreSound();
                    
                    if (score == 50 && !gunUpgraded) {
                        gunUpgraded = true;
                        // playerDamage = 1; // Keep base damage at 1, handled by weapon logic now
                        maxAmmo += 2;
                        ammo = maxAmmo;
                        upgradeMessageTimer = 3.0f;
                    }
                    
                    if (score > highScore) {
                        highScore = score;
                        SaveHighScore();
                    }

                    // Check Score for Boss Trigger
                    if (score >= 1000 && !bossActive && !preBossPhase) {
                        preBossPhase = true;
                        TriggerEvent("pre_boss_phase", true);
                        preBossTimer = 30.0f;
                        
                        // Despawn all enemies
                        // for (auto& e : enemies) e.active = false;
                        // enemies.clear(); // Unsafe in loop
                        shouldClearEnemies = true;
                        
                        scoreTimer = 0; // Clear score text
                    }
                    
                    // Marshall Spawn Trigger
                    if (score >= 250 && !marshallSpawned) {
                        Enemy marshall;
                        int attempts = 0;
                        do {
                            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                            float dist = 10.0f + (float)(rand() % 15);
                            marshall.x = player.x + cosf(angle) * dist;
                            marshall.y = player.y + sinf(angle) * dist;
                            if (marshall.x < 1.5f) marshall.x = 1.5f; if (marshall.x >= MAP_WIDTH - 1.5f) marshall.x = (float)(MAP_WIDTH - 2);
                            if (marshall.y < 1.5f) marshall.y = 1.5f; if (marshall.y >= MAP_HEIGHT - 1.5f) marshall.y = (float)(MAP_HEIGHT - 2);
                            attempts++;
                        } while (worldMap[(int)marshall.x][(int)marshall.y] != 0 && attempts < 10);
                        
                        if (worldMap[(int)marshall.x][(int)marshall.y] == 0) {
                            marshall.active = true;
                            marshall.health = marshallMaxHP; 
                            marshall.speed = 2.5f;
                            marshall.spriteIndex = 4; // Use elite/red imp base but override render
                            marshall.hurtTimer = 0;
                            marshall.isShooter = false;
                            marshall.fireTimer = 0;
                            marshall.firingTimer = 0;
                            // Marshall Specifics
                            marshall.isMarshall = true;
                            marshall.state = 0; // Seek Horde
                            marshall.healTimer = 0;
                            marshall.summonTimer = 10.0f; // Initial delay
                            marshall.attackTimer = 0;
                            
                            
                            pendingEnemies.push_back(marshall);
                            marshallSpawned = true;
                            TriggerEvent("marshall_spawned", true);
                             // Trigger initial officer defection immediately after Marshall spawns
                            
                            // Spawn 10 minions to follow him
                            for (int k=0; k<10; k++) {
                                Enemy s;
                                s.x = marshall.x + (rand()%200 - 100)/50.0f; 
                                s.y = marshall.y + (rand()%200 - 100)/50.0f;
                                if (s.x < 1.5f) s.x = 1.5f; if (s.x >= MAP_WIDTH - 1.5f) s.x = (float)(MAP_WIDTH - 2);
                                if (s.y < 1.5f) s.y = 1.5f; if (s.y >= MAP_HEIGHT - 1.5f) s.y = (float)(MAP_HEIGHT - 2);
                                
                                if (worldMap[(int)s.x][(int)s.y] == 0) {
                                    s.active = true; s.health = 1; s.speed = 3.0f; s.spriteIndex = rand()%4; 
                                    s.isShooter = false; s.isMarshall = false; 
                                    pendingEnemies.push_back(s);
                                }
                            }
                        }
                    }
                    
                    scoreTimer = 3.0f;
                    int msgIndex = rand() % 3;
                    wcscpy(scoreMsg, praiseMsgs[msgIndex]);
                }
                break;
            }
        }
        
        if (bossActive && bossHealth > 0) {
            bool hitHit = false;
            
            // Check Boss Hit (if no Force Field)
            if (phase2Active && forceFieldActive) {
                float bdx = b.x - 32.0f;
                float bdy = b.y - 32.0f;
                if (sqrtf(bdx*bdx + bdy*bdy) < 3.5f) {
                     b.active = false; // Deflected
                     hitHit = true; 
                }
            } else {
                float bdx = b.x - 32.0f;
                float bdy = b.y - 32.0f;
                if (sqrtf(bdx*bdx + bdy*bdy) < 2.5f) {
                    // Check for Marshall Pincer Armor (50%)
                    bool applyDamage = true;
                    if (bossActive && marshallSpawned && activeCommand == CMD_PINCER) {
                        if (rand() % 2 == 0) applyDamage = false; // 50% chance to ignore 1 dmg
                    }
                    if (applyDamage) bossHealth -= playerDamage;
                    
                    bossHurtTimer = 2.0f;
                    b.active = false;
                    PlayScoreSound();
                    hitHit = true;
                    
                    if (bossHealth <= 0) {
                        bossActive = false; TriggerEvent("boss_active", false);
                        bossDead = true;
                        postBossPhase = true;
                        musicRunning = false;
                        score += 50;
                        GivePlayerXP(1000);
                        if (score > highScore) {
                            highScore = score;
                            SaveHighScore();
                        }
                        
                        for (auto& e : enemies) e.active = false;
                        enemies.clear();
                        fireballs.clear();
                        
                        for(int i = 0; i < 6; i++) {
                            claws[i].state = CLAW_DORMANT;
                            claws[i].x = claws[i].homeX;
                            claws[i].y = claws[i].homeY;
                        }
                        
                        phase2Active = false;
                        forceFieldActive = false;
                        activeLaserClaw = -1;
                        
                        wchar_t exePath[MAX_PATH];
                        GetModuleFileNameW(NULL, exePath, MAX_PATH);
                        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
                        if (lastSlash) *lastSlash = L'\0';
                        wchar_t dialoguePath[MAX_PATH];
                        swprintf(dialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\leader.line", exePath);
                        
                        NPCSystem::ClearNPCs();
                        NPCSystem::SpawnNPC(32.0f, 28.0f, L"Leader", leaderIdlePixels, leaderIdleW, leaderIdleH, leaderTalkingPixels, leaderTalkingW, leaderTalkingH, dialoguePath);
                        
                        wchar_t followerDialoguePath[MAX_PATH];
                        swprintf(followerDialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\followers.line", exePath);
                        NPCSystem::SpawnNPC(29.0f, 28.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        NPCSystem::SpawnNPC(35.0f, 28.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        NPCSystem::SpawnNPC(27.0f, 30.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        NPCSystem::SpawnNPC(37.0f, 30.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        
                        wchar_t victoryMusicPath[MAX_PATH];
                        swprintf(victoryMusicPath, MAX_PATH, L"open \"%ls\\assets\\sound-effects\\victory.mp3\" type mpegvideo alias victory", exePath);
                        mciSendStringW(victoryMusicPath, NULL, 0, NULL);
                        mciSendStringW(L"play victory repeat", NULL, 0, NULL);
                    }
                }
            }
            
            // Check Claws Hit (Phase 2)
            if (!hitHit && phase2Active) {
                for(int i=0; i<6; i++) {
                    if (claws[i].state == CLAW_PH2_DEAD) continue;
                    
                    float cdx = b.x - claws[i].x;
                    float cdy = b.y - claws[i].y;
                    if (sqrtf(cdx*cdx + cdy*cdy) < 2.0f) {
                        b.active = false;
                        claws[i].health -= playerDamage; 
                        claws[i].hurtTimer = 0.2f;
                        if (claws[i].health <= 0) {
                            claws[i].state = CLAW_PH2_RISING;
                            claws[i].timer = 2.0f;
                            PlayScoreSound();
                            if (activeLaserClaw == i) activeLaserClaw = -1;
                        }
                        break;
                    }
                }
            }
        }
    }
    
    if (shouldClearEnemies) {
        enemies.clear();
        officerSpawned = false;
        
        // fireballs.clear(); // Maybe? Pre-boss clears everything.
        // Logic in UpdateEnemies says: enemies.clear().
        // Here we just clear enemies.
    }
    
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
}

int GetAliveParagonCount() {
    int count = 0;
    for (auto& p : allies) {
        if (!p.isParagon) continue; if (p.active) count++; }
    return count;
}

void UpdateAllies(float deltaTime) {
    if (!paragonsUnlocked && score >= 200 && marshallKilled) {
        paragonsUnlocked = true;
        paragonMessageTimer = 3.0f;
        for (int i = 0; i < 2; i++) {
            Enemy p;
            p.isParagon = true;
            p.isAllied = true;
            p.isEnemy = false;
            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
            p.x = player.x + cosf(angle) * 1.5f;
            p.y = player.y + sinf(angle) * 1.5f;
            p.speed = 4.5f;
            p.health = 10;
            p.active = true;
            p.hurtTimer = 0;
            p.targetX = 0; p.targetY = 0;
            p.hunting = false;
            p.targetEnemyIndex = -1;
            p.targetClawIndex = -1;
            pendingAllies.push_back(p);
        }
    }
    
    if (paragonMessageTimer > 0) paragonMessageTimer -= deltaTime;
    if (paragonSummonCooldown > 0) paragonSummonCooldown -= deltaTime;
    
    for (auto& p : allies) {
        if (!p.active) continue;
        
        auto& enemy = p;
        float dx = player.x - p.x;
        float dy = player.y - p.y;
        float dist = sqrtf(dx*dx + dy*dy);
        
        if (enemy.isDefectedGunner) {
            if (defectedOfficerActive) {
                if (enemy.firingTimer > 0) enemy.firingTimer -= deltaTime;
                continue;
            }
            if (enemy.firingTimer > 0) enemy.firingTimer -= deltaTime;
            float closestDist = 9999.0f;
            Enemy* target = nullptr;
            for (auto& e : enemies) {
                if (!e.active || !e.isEnemy) continue;
                float edx = e.x - enemy.x; float edy = e.y - enemy.y;
                float d = sqrtf(edx*edx + edy*edy);
                if (d < closestDist) { closestDist = d; target = &e; }
            }
            if (target && closestDist < 15.0f) {
                float edx = target->x - enemy.x; float edy = target->y - enemy.y;
                if (closestDist > 6.0f) {
                    enemy.x += (edx/closestDist)*enemy.speed*deltaTime;
                    enemy.y += (edy/closestDist)*enemy.speed*deltaTime;
                }
                if (closestDist <= 12.0f && enemy.fireTimer <= 0) {
                    float targetAngle = atan2f(edy, edx);
                    Bullet b; b.x = enemy.x; b.y = enemy.y;
                    b.dirX = cosf(targetAngle); b.dirY = sinf(targetAngle);
                    b.active = true; b.speed = 10.0f; b.damage = 15;
                    b.startX = enemy.x; b.startY = enemy.y; b.maxRange = 25.0f;
                    bullets.push_back(b);
                    enemy.fireTimer = 2.0f;
                    enemy.firingTimer = 0.2f;
                    PlayEnemyFireSound();
                }
            } else {
                if (dist > 5.0f) {
                    enemy.x += (dx/dist)*enemy.speed*deltaTime;
                    enemy.y += (dy/dist)*enemy.speed*deltaTime;
                }
            }
            if (enemy.fireTimer > 0) enemy.fireTimer -= deltaTime;

        } else if (enemy.isDefectedOfficer) {
            if (enemy.firingTimer > 0) enemy.firingTimer -= deltaTime;
            if (enemy.fireTimer > 0) enemy.fireTimer -= deltaTime;
            if (enemy.officerCooldown > 0) enemy.officerCooldown -= deltaTime;

            if (dist < 8.0f) {
                if (enemy.healTimer <= 0 && player.health < player.maxHealth) {
                    player.health += 1;
                    enemy.healTimer = 1.0f;
                } else if (enemy.healTimer > 0) {
                    enemy.healTimer -= deltaTime;
                }
            }

            std::vector<Enemy*> defectedGunners;
            for (auto& e : allies) {
                if (e.active && e.isDefectedGunner) defectedGunners.push_back(&e);
            }

            float closestEnemyDist = 9999.0f;
            Enemy* hostileTarget = nullptr;
            for (auto& e : enemies) {
                if (!e.active || !e.isEnemy) continue;
                float edx = e.x - enemy.x; float edy = e.y - enemy.y;
                float d = sqrtf(edx*edx + edy*edy);
                if (d < closestEnemyDist) { closestEnemyDist = d; hostileTarget = &e; }
            }

            if (enemy.officerState != 2) {
                if (defectedGunners.size() <= 2) {
                    enemy.officerState = 3; // Squad depleted -> fall back / regroup
                } else {
                    enemy.officerState = 1; // Squad replenished -> engage
                }
            }

            if (enemy.officerState == 3) {
                if (dist > 5.0f) {
                    enemy.x += (dx/dist)*enemy.speed*1.2f*deltaTime;
                    enemy.y += (dy/dist)*enemy.speed*1.2f*deltaTime;
                }

                for (auto* g : defectedGunners) {
                    float gdx = player.x - g->x;
                    float gdy = player.y - g->y;
                    float gDist = sqrtf(gdx*gdx + gdy*gdy);
                    float gSpeed = g->speed * 1.2f;
                    if (gDist > 5.0f) {
                        g->x += (gdx/gDist) * gSpeed * deltaTime;
                        g->y += (gdy/gDist) * gSpeed * deltaTime;
                    }
                }
                
                if (enemy.officerCooldown <= 0 && defectedGunners.size() < 4) {
                    int needed = 4 - (int)defectedGunners.size();
                    for (int i = 0; i < needed; i++) {
                        Enemy dg;
                        dg.x = enemy.x + (rand()%200 - 100)/100.0f;
                        dg.y = enemy.y + (rand()%200 - 100)/100.0f;
                        dg.active = true; dg.speed = 1.2f; dg.spriteIndex = 0; dg.health = 2; dg.maxHealth = 2;
                        dg.isShooter = true; dg.isDefectedGunner = true;
                        dg.isAllied = true; dg.isEnemy = false;
                        dg.fireTimer = 2.0f; dg.firingTimer = 0; dg.hurtTimer = 0;
                        dg.hasNeuralBrain = true;
                        NeuralAI::InheritBrain(dg.brain);
                        pendingAllies.push_back(dg);
                    }
                    enemy.officerCooldown = 20.0f;
                }
            } else if (enemy.officerState == 1) {
                if (dist > 5.0f) {
                    enemy.x += (dx/dist)*enemy.speed*deltaTime;
                    enemy.y += (dy/dist)*enemy.speed*deltaTime;
                }

                if (hostileTarget) {
                    float aTgt = atan2f(hostileTarget->y - enemy.y, hostileTarget->x - enemy.x);
                    float lineAngle = aTgt + 3.14159f/2.0f;
                    int gi = 0; int gc = (int)defectedGunners.size(); bool allReady = true;
                    for (auto* g : defectedGunners) {
                        float offset = (gi - (gc-1)/2.0f) * 0.8f;
                        float tx = enemy.x + cosf(lineAngle) * offset;
                        float ty = enemy.y + sinf(lineAngle) * offset;
                        float gdx = tx - g->x; float gdy = ty - g->y;
                        float gdist = sqrtf(gdx*gdx + gdy*gdy);
                        if (gdist > 1.2f) {
                            g->x += (gdx/gdist)*g->speed*2.0f*deltaTime;
                            g->y += (gdy/gdist)*g->speed*2.0f*deltaTime;
                            allReady = false;
                        }
                        g->fireTimer = 2.0f;
                        gi++;
                    }
                    if (allReady && enemy.fireTimer <= 0) {
                        const float VOLLEY_STAGGER = 0.05f;
                        int idx = 1;
                        for (auto* g : defectedGunners) {
                            g->fireTimer = -(idx * VOLLEY_STAGGER);
                            idx++;
                        }
                        enemy.fireTimer = 1.5f;
                        enemy.officerState = 2;
                    }
                }
            } else if (enemy.officerState == 2) {
                bool allFired = true;
                for (auto* g : defectedGunners) {
                    if (g->fireTimer < 0.0f) {
                        g->fireTimer += deltaTime;
                        allFired = false;
                        if (g->fireTimer >= 0.0f) {
                            Enemy* gTarget = nullptr; float gClosest = 9999.0f;
                            for (auto& e : enemies) {
                                if (!e.active || !e.isEnemy) continue;
                                float gdx = e.x - g->x; float gdy = e.y - g->y;
                                float gd = sqrtf(gdx*gdx + gdy*gdy);
                                if (gd < gClosest) { gClosest = gd; gTarget = &e; }
                            }
                            if (gTarget) {
                                float shotAngle = atan2f(gTarget->y - g->y, gTarget->x - g->x);
                                Bullet eb; eb.x = g->x; eb.y = g->y;
                                eb.dirX = cosf(shotAngle); eb.dirY = sinf(shotAngle);
                                eb.active = true; eb.speed = 10.0f; eb.damage = 1;
                                eb.startX = g->x; eb.startY = g->y; eb.maxRange = 25.0f;
                                bullets.push_back(eb);
                                g->firingTimer = 0.2f;
                                PlayEnemyFireSound();
                            }
                        }
                    }
                }
                if (allFired && enemy.fireTimer <= 0) {
                    enemy.officerState = 1;
                }
            }

            if (enemy.officerState != enemy.prevOfficerState) {
                enemy.prevOfficerState = enemy.officerState;
            }
            
        } else if (enemy.isParagon) {
            if (p.hurtTimer > 0) p.hurtTimer -= deltaTime;
        
        float distToPlayer = sqrtf((p.x - player.x)*(p.x - player.x) + (p.y - player.y)*(p.y - player.y));
        
        int nearestEnemyIdx = -1;
        float nearestEnemyDist = 6.0f;
        for (size_t i = 0; i < enemies.size(); i++) {
            if (!enemies[i].active) continue;
            float dx = enemies[i].x - p.x;
            float dy = enemies[i].y - p.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < nearestEnemyDist) {
                nearestEnemyDist = dist;
                nearestEnemyIdx = (int)i;
            }
        }
        
        int nearestClawIdx = -1;
        float nearestClawDist = 6.0f;
        if (phase2Active) {
            for (int i = 0; i < 6; i++) {
                if (claws[i].state == CLAW_PH2_DEAD) continue;
                float dx = claws[i].x - p.x;
                float dy = claws[i].y - p.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < nearestClawDist) {
                    nearestClawDist = dist;
                    nearestClawIdx = i;
                }
            }
        }
        
        float evadeX = 0, evadeY = 0;
        
        for (auto& eb : enemyBullets) {
            if (!eb.active) continue;
            float dx = p.x - eb.x;
            float dy = p.y - eb.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 4.0f && dist > 0.1f) {
                float dotProduct = dx * eb.dirX + dy * eb.dirY;
                if (dotProduct > 0) {
                    float perpX = -eb.dirY;
                    float perpY = eb.dirX;
                    evadeX += perpX * (4.0f - dist);
                    evadeY += perpY * (4.0f - dist);
                }
            }
        }
        
        for (auto& fb : fireballs) {
            if (!fb.active) continue;
            float dx = p.x - fb.x;
            float dy = p.y - fb.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 4.0f && dist > 0.1f) {
                float dotProduct = dx * fb.dirX + dy * fb.dirY;
                if (dotProduct > 0) {
                    float perpX = -fb.dirY;
                    float perpY = fb.dirX;
                    evadeX += perpX * (4.0f - dist);
                    evadeY += perpY * (4.0f - dist);
                }
            }
        }
        
        for (int i = 0; i < 6; i++) {
            if (claws[i].state == CLAW_SLAMMING || claws[i].state == CLAW_CHASING) {
                float dx = p.x - claws[i].x;
                float dy = p.y - claws[i].y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 6.0f && dist > 0.1f) {
                    evadeX += (dx / dist) * (6.0f - dist) * 2.0f;
                    evadeY += (dy / dist) * (6.0f - dist) * 2.0f;
                }
            }
            if (claws[i].state == CLAW_PH2_ANCHORED && activeLaserClaw == i) {
                float dx = p.x - claws[i].x;
                float dy = p.y - claws[i].y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 5.0f && dist > 0.1f) {
                    float perpX = -dy / dist;
                    float perpY = dx / dist;
                    evadeX += perpX * (5.0f - dist);
                    evadeY += perpY * (5.0f - dist);
                }
            }
        }
        
        if (evadeX != 0 || evadeY != 0) {
            float evadeDist = sqrtf(evadeX*evadeX + evadeY*evadeY);
            if (evadeDist > 0.1f) {
                p.x += (evadeX / evadeDist) * p.speed * 1.5f * deltaTime;
                p.y += (evadeY / evadeDist) * p.speed * 1.5f * deltaTime;
            }
        }
        
        if (distToPlayer > 16.0f) {
            float dx = player.x - p.x;
            float dy = player.y - p.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist > 0.1f) {
                p.x += (dx / dist) * p.speed * deltaTime;
                p.y += (dy / dist) * p.speed * deltaTime;
            }
            p.hunting = false;
        } else if (nearestEnemyIdx != -1) {
            p.hunting = true;
            p.targetEnemyIndex = nearestEnemyIdx;
            float dx = enemies[nearestEnemyIdx].x - p.x;
            float dy = enemies[nearestEnemyIdx].y - p.y;
            float dist = sqrtf(dx*dx + dy*dy);
            
            float repelX = 0, repelY = 0;
            for (auto& other : allies) {
                if (!other.isParagon) continue;
                if (&other == &p || !other.active) continue;
                float ox = p.x - other.x;
                float oy = p.y - other.y;
                float odist = sqrtf(ox*ox + oy*oy);
                if (odist < 1.2f && odist > 0.01f) {
                    repelX += (ox / odist) * (1.2f - odist);
                    repelY += (oy / odist) * (1.2f - odist);
                }
            }
            
            if (dist > 0.5f) {
                p.x += (dx / dist) * p.speed * deltaTime;
                p.y += (dy / dist) * p.speed * deltaTime;
            }
            if (repelX != 0 || repelY != 0) {
                float repelDist = sqrtf(repelX*repelX + repelY*repelY);
                if (repelDist > 0.01f) {
                    p.x += (repelX / repelDist) * p.speed * 0.3f * deltaTime;
                    p.y += (repelY / repelDist) * p.speed * 0.3f * deltaTime;
                }
            }
            if (dist < 1.0f) {
                enemies[nearestEnemyIdx].health -= 2;
                if (enemies[nearestEnemyIdx].health <= 0) {
                    enemies[nearestEnemyIdx].active = false;
                    if (enemies[nearestEnemyIdx].isMarshall) marshallKilled = true;
                    if (enemies[nearestEnemyIdx].isOfficer) officerSpawned = false;
                    score++;
                    GivePlayerXP(GetEnemyXP(enemies[nearestEnemyIdx]));
                    PlayScoreSound();
                    if (score > highScore) { highScore = score; SaveHighScore(); }
                }
            }
        } else if (nearestClawIdx != -1) {
            p.hunting = true;
            p.targetClawIndex = nearestClawIdx;
            float dx = claws[nearestClawIdx].x - p.x;
            float dy = claws[nearestClawIdx].y - p.y;
            float dist = sqrtf(dx*dx + dy*dy);
            
            float repelX = 0, repelY = 0;
            for (auto& other : allies) {
                if (!other.isParagon) continue;
                if (&other == &p || !other.active) continue;
                float ox = p.x - other.x;
                float oy = p.y - other.y;
                float odist = sqrtf(ox*ox + oy*oy);
                if (odist < 1.2f && odist > 0.01f) {
                    repelX += (ox / odist) * (1.2f - odist);
                    repelY += (oy / odist) * (1.2f - odist);
                }
            }
            
            if (dist > 0.5f) {
                p.x += (dx / dist) * p.speed * deltaTime;
                p.y += (dy / dist) * p.speed * deltaTime;
            }
            if (repelX != 0 || repelY != 0) {
                float repelDist = sqrtf(repelX*repelX + repelY*repelY);
                if (repelDist > 0.01f) {
                    p.x += (repelX / repelDist) * p.speed * 0.3f * deltaTime;
                    p.y += (repelY / repelDist) * p.speed * 0.3f * deltaTime;
                }
            }
            if (dist < 1.5f) {
                claws[nearestClawIdx].health -= 2;
                claws[nearestClawIdx].hurtTimer = 0.2f;
                if (claws[nearestClawIdx].health <= 0) {
                    claws[nearestClawIdx].state = CLAW_PH2_RISING;
                    claws[nearestClawIdx].timer = 2.0f;
                    PlayScoreSound();
                    if (activeLaserClaw == nearestClawIdx) activeLaserClaw = -1;
                }
            }
        } else {
            p.hunting = false;
            if (distToPlayer > 2.0f) {
                float dx = player.x - p.x;
                float dy = player.y - p.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist > 0.1f) {
                    p.x += (dx / dist) * p.speed * deltaTime;
                    p.y += (dy / dist) * p.speed * deltaTime;
                }
            } else {
                float repelX = 0, repelY = 0;
                for (auto& other : allies) {
                if (!other.isParagon) continue;
                    if (&other == &p || !other.active) continue;
                    float ox = p.x - other.x;
                    float oy = p.y - other.y;
                    float odist = sqrtf(ox*ox + oy*oy);
                    if (odist < 1.5f && odist > 0.01f) {
                        repelX += (ox / odist) * (1.5f - odist);
                        repelY += (oy / odist) * (1.5f - odist);
                    }
                }
                if (repelX != 0 || repelY != 0) {
                    float repelDist = sqrtf(repelX*repelX + repelY*repelY);
                    if (repelDist > 0.01f) {
                        p.x += (repelX / repelDist) * p.speed * 0.5f * deltaTime;
                        p.y += (repelY / repelDist) * p.speed * 0.5f * deltaTime;
                    }
                }
            }
        }
        }
        
        if (p.x < 1.5f) p.x = 1.5f; if (p.x >= MAP_WIDTH - 1.5f) p.x = (float)(MAP_WIDTH - 2);
        if (p.y < 1.5f) p.y = 1.5f; if (p.y >= MAP_HEIGHT - 1.5f) p.y = (float)(MAP_HEIGHT - 2);
    }
    
    for (auto& eb : enemyBullets) {
        if (!eb.active) continue;
        for (auto& p : allies) {

            if (!p.active) continue;
            float dx = eb.x - p.x;
            float dy = eb.y - p.y;
            if (sqrtf(dx*dx + dy*dy) < 1.0f) {
                eb.active = false;
                if (p.hurtTimer <= 0.0f) {
                    p.health -= (eb.isLaser ? 10 : 5);
                    p.hurtTimer = 1.0f;
                    if (p.health <= 0) p.active = false;
                }
                break;
            }
        }
    }
    
    for (auto& fb : fireballs) {
        if (!fb.active) continue;
        for (auto& p : allies) {

            if (!p.active) continue;
            float dx = fb.x - p.x;
            float dy = fb.y - p.y;
            if (sqrtf(dx*dx + dy*dy) < 0.5f) {
                fb.active = false;
                if (p.hurtTimer <= 0.0f) {
                    p.health -= 10;
                    p.hurtTimer = 1.0f;
                    if (p.health <= 0) p.active = false;
                }
                break;
            }
        }
    }
    
    for (auto& e : enemies) {
        if (!e.active || e.isShooter || e.isMarshall) continue;
        for (auto& p : allies) {

            if (!p.active) continue;
            float dx = p.x - e.x;
            float dy = p.y - e.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 1.0f && p.hurtTimer <= 0.0f) {
                p.health -= 1;
                p.hurtTimer = 1.0f;
                if (p.health <= 0) p.active = false;
                break;
            }
        }
    }
}

void DrawCompass(HDC hdc) {
    if (!compassPixels || compassW <= 0 || compassH <= 0) return;
    
    int compassScale = 3;
    int compassDrawW = compassW * compassScale;
    int compassDrawH = compassH * compassScale;
    int compassX = (SCREEN_WIDTH - compassDrawW) / 2;
    int compassY = 50;
    
    int centerDrawX = compassX + compassDrawW / 2;
    int centerDrawY = compassY + compassDrawH / 2;
    int srcCenterX = compassW / 2;
    int srcCenterY = compassH / 2;
    
    float rotAngle = player.angle + PI / 2.0f;
    float cosA = cosf(rotAngle);
    float sinA = sinf(rotAngle);
    
    for (int y = 0; y < compassDrawH; y++) {
        int screenY = compassY + y;
        if (screenY < 0 || screenY >= SCREEN_HEIGHT) continue;
        
        for (int x = 0; x < compassDrawW; x++) {
            int screenX = compassX + x;
            if (screenX < 0 || screenX >= SCREEN_WIDTH) continue;
            
            float dx = (float)(x - compassDrawW / 2) / compassScale;
            float dy = (float)(y - compassDrawH / 2) / compassScale;
            
            float srcXf = srcCenterX + dx * cosA + dy * sinA;
            float srcYf = srcCenterY - dx * sinA + dy * cosA;
            int srcX = (int)floorf(srcXf + 0.5f);
            int srcY = (int)floorf(srcYf + 0.5f);
            
            if (srcX < 0 || srcX >= compassW || srcY < 0 || srcY >= compassH) continue;
            
            DWORD col = compassPixels[srcY * compassW + srcX];
            int b = (col >> 0) & 0xFF;
            int g = (col >> 8) & 0xFF;
            int r = (col >> 16) & 0xFF;
            
            if (r == 255 && g == 0 && b == 255) continue;
            if (r == 0 && g == 0 && b == 0) continue;
            
            renderBuffer[screenY * SCREEN_WIDTH + screenX] = MakeColor(r, g, b);
        }
    }
}

void DrawMinimapToBuffer() {
    if (!renderBuffer) return;
    
    int cellSize = 3;
    int mapDrawWidth = MAP_WIDTH * cellSize;
    int mapDrawHeight = MAP_HEIGHT * cellSize;
    
    int offsetX = SCREEN_WIDTH - mapDrawWidth - 60;
    int offsetY = 30;
    
    DWORD bgColor = MakeColor(30, 30, 30);
    for (int y = offsetY - 3; y < offsetY + mapDrawHeight + 3; y++) {
        if (y < 0 || y >= SCREEN_HEIGHT) continue;
        for (int x = offsetX - 3; x < offsetX + mapDrawWidth + 3; x++) {
            if (x < 0 || x >= SCREEN_WIDTH) continue;
            renderBuffer[y * SCREEN_WIDTH + x] = bgColor;
        }
    }
    
    DWORD wall1Color = MakeColor(100, 80, 60);
    DWORD wall2Color = MakeColor(80, 100, 80);
    DWORD wall3Color = MakeColor(60, 60, 100);
    
    for (int my = 0; my < MAP_HEIGHT; my++) {
        for (int mx = 0; mx < MAP_WIDTH; mx++) {
            if (worldMap[mx][my] > 0) {
                DWORD col = wall3Color;
                if (worldMap[mx][my] == 2) col = wall1Color;
                else if (worldMap[mx][my] == 1) col = wall2Color;
                
                for (int py = 0; py < cellSize; py++) {
                    int sy = offsetY + my * cellSize + py;
                    if (sy < 0 || sy >= SCREEN_HEIGHT) continue;
                    for (int px = 0; px < cellSize; px++) {
                        int sx = offsetX + mx * cellSize + px;
                        if (sx < 0 || sx >= SCREEN_WIDTH) continue;
                        renderBuffer[sy * SCREEN_WIDTH + sx] = col;
                    }
                }
            }
        }
    }
    
    int playerX = offsetX + (int)(player.x * cellSize);
    int playerY = offsetY + (int)(player.y * cellSize);
    DWORD playerColor = MakeColor(0, 255, 0);
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int sx = playerX + dx;
            int sy = playerY + dy;
            if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT) {
                renderBuffer[sy * SCREEN_WIDTH + sx] = playerColor;
            }
        }
    }
    
    DWORD enemyColor = MakeColor(255, 0, 0);
    DWORD smartColor = MakeColor(200, 50, 200);
    DWORD allyColor = MakeColor(50, 200, 50); // Light green for allies
    for (auto& enemy : enemies) {
        if (enemy.active) {
            int ex = offsetX + (int)(enemy.x * cellSize);
            int ey = offsetY + (int)(enemy.y * cellSize);
            DWORD col = (enemy.tacticState != 0) ? smartColor : enemyColor;
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    int sx = ex + dx;
                    int sy = ey + dy;
                    if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
                        renderBuffer[sy * SCREEN_WIDTH + sx] = col;
                    }
                }
            }
        }
    }
    
    for (auto& ally : allies) {
        if (ally.active) {
            int ex = offsetX + (int)(ally.x * cellSize);
            int ey = offsetY + (int)(ally.y * cellSize);
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    int sx = ex + dx;
                    int sy = ey + dy;
                    if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
                        renderBuffer[sy * SCREEN_WIDTH + sx] = allyColor;
                    }
                }
            }
        }
    }
    
    DWORD medkitColor = MakeColor(0, 200, 255);
    for (int i = 0; i < 3; i++) {
        if (medkits[i].active) {
            int mx = offsetX + (int)(medkits[i].x * cellSize);
            int my = offsetY + (int)(medkits[i].y * cellSize);
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    int sx = mx + dx;
                    int sy = my + dy;
                    if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
                        renderBuffer[sy * SCREEN_WIDTH + sx] = medkitColor;
                    }
                }
            }
        }
    }
    
    DWORD spireColor = MakeColor(255, 165, 0);
    int spireX = offsetX + 32 * cellSize;
    int spireY = offsetY + 32 * cellSize;
    for (int dy = -4; dy <= 4; dy++) {
        for (int dx = -4; dx <= 4; dx++) {
            int sx = spireX + dx;
            int sy = spireY + dy;
            if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
                renderBuffer[sy * SCREEN_WIDTH + sx] = spireColor;
            }
        }
    }
    
    // Gravitals (Red dots)
    DWORD gravitalColor = MakeColor(255, 50, 50);
    for (auto& g : gravitals) {
        if (g.active) {
            int gx = offsetX + (int)(g.x * cellSize);
            int gy = offsetY + (int)(g.y * cellSize);
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    int sx = gx + dx;
                    int sy = gy + dy;
                    if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
                        renderBuffer[sy * SCREEN_WIDTH + sx] = gravitalColor;
                    }
                }
            }
        }
    }
    
    // Healing Tower (Yellow Plus Sign)
    DWORD towerColor = MakeColor(255, 255, 0);
    int htX = offsetX + (int)(healingTower.x * cellSize);
    int htY = offsetY + (int)(healingTower.y * cellSize);
    // Draw plus sign: vertical bar
    for (int dy = -4; dy <= 4; dy++) {
        int sx = htX;
        int sy = htY + dy;
        if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
            renderBuffer[sy * SCREEN_WIDTH + sx] = towerColor;
        }
    }
    // Draw plus sign: horizontal bar
    for (int dx = -4; dx <= 4; dx++) {
        int sx = htX + dx;
        int sy = htY;
        if (sx >= offsetX && sx < offsetX + mapDrawWidth && sy >= offsetY && sy < offsetY + mapDrawHeight) {
            renderBuffer[sy * SCREEN_WIDTH + sx] = towerColor;
        }
    }
}

void DrawMinimap(HDC hdc) {
    // Save original GDI objects once at the start
    HGDIOBJ origPen = GetCurrentObject(hdc, OBJ_PEN);
    HGDIOBJ origBrush = GetCurrentObject(hdc, OBJ_BRUSH);
    
    int cellSize = 3;
    int mapDrawWidth = MAP_WIDTH * cellSize;
    int mapDrawHeight = MAP_HEIGHT * cellSize;
    
    int offsetX = SCREEN_WIDTH - mapDrawWidth - 60;
    int offsetY = 30;
    
    RECT bgRect = {offsetX - 3, offsetY - 3, offsetX + mapDrawWidth + 3, offsetY + mapDrawHeight + 3};
    FillRect(hdc, &bgRect, hBrushMapBG);
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (worldMap[x][y] > 0) {
                RECT cell = {
                    offsetX + x * cellSize, 
                    offsetY + y * cellSize,
                    offsetX + (x + 1) * cellSize, 
                    offsetY + (y + 1) * cellSize
                };
                HBRUSH brush = hBrushWall3;
                if (worldMap[x][y] == 2) brush = hBrushWall1;
                else if (worldMap[x][y] == 1) brush = hBrushWall2;
                
                FillRect(hdc, &cell, brush);
            }
        }
    }
    
    int playerScreenX = offsetX + (int)(player.x * cellSize);
    int playerScreenY = offsetY + (int)(player.y * cellSize);
    
    float triSize = 10.0f;
    POINT tri[3];
    tri[0].x = playerScreenX + (int)(cosf(player.angle) * triSize);
    tri[0].y = playerScreenY + (int)(sinf(player.angle) * triSize);
    tri[1].x = playerScreenX + (int)(cosf(player.angle + 2.4f) * triSize * 0.5f);
    tri[1].y = playerScreenY + (int)(sinf(player.angle + 2.4f) * triSize * 0.5f);
    tri[2].x = playerScreenX + (int)(cosf(player.angle - 2.4f) * triSize * 0.5f);
    tri[2].y = playerScreenY + (int)(sinf(player.angle - 2.4f) * triSize * 0.5f);
    
    SelectObject(hdc, hPenPlayer);
    SelectObject(hdc, hBrushPlayer);
    Polygon(hdc, tri, 3);
    
    if (viewRange) {
        float range = 0;
        if (currentWeapon == 0) range = 24.0f;
        else if (currentWeapon == 1) range = 12.0f;
        else if (currentWeapon == 2) range = 64.0f;
        
        int rangePx = (int)(range * cellSize);
        
        SelectObject(hdc, hPenRange);
        SelectObject(hdc, hBrushHollow);
        
        Ellipse(hdc, playerScreenX - rangePx, playerScreenY - rangePx, 
                     playerScreenX + rangePx, playerScreenY + rangePx);
    }
    
    MoveToEx(hdc, playerScreenX, playerScreenY, NULL);
    SelectObject(hdc, hPenFOV);
    int fovLen = 20;
    LineTo(hdc, playerScreenX + (int)(cosf(player.angle) * fovLen), playerScreenY + (int)(sinf(player.angle) * fovLen));
    
    // Switch to NULL_PEN for filled shapes (dots) to avoid borders
    SelectObject(hdc, GetStockObject(NULL_PEN));
    
    int spireScreenX = offsetX + (int)(32 * cellSize);
    int spireScreenY = offsetY + (int)(32 * cellSize);
    SelectObject(hdc, hBrushSpire);
    Ellipse(hdc, spireScreenX - 6, spireScreenY - 6, spireScreenX + 6, spireScreenY + 6);
    
    SelectObject(hdc, hBrushMedkit);
    for (int i = 0; i < 3; i++) {
        if (medkits[i].active) {
            int medkitScreenX = offsetX + (int)(medkits[i].x * cellSize);
            int medkitScreenY = offsetY + (int)(medkits[i].y * cellSize);
            Ellipse(hdc, medkitScreenX - 4, medkitScreenY - 4, medkitScreenX + 4, medkitScreenY + 4);
        }
    }
    
    for (auto& enemy : enemies) {
        if (enemy.active) {
            int ex = offsetX + (int)(enemy.x * cellSize);
            int ey = offsetY + (int)(enemy.y * cellSize);
            
            if (ex >= offsetX && ex < offsetX + mapDrawWidth && ey >= offsetY && ey < offsetY + mapDrawHeight) {
                // Use Purple for tactical(smart) enemies, Red for normal
                HBRUSH enemyBrush = (enemy.tacticState != 0) ? hBrushPurple : hBrushRed; 
                SelectObject(hdc, enemyBrush);
                Ellipse(hdc, ex - 3, ey - 3, ex + 3, ey + 3);
            }
        }
    }
    
    SelectObject(hdc, hBrushPlayer);
    for (auto& ally : allies) {
        if (ally.active) {
            int ex = offsetX + (int)(ally.x * cellSize);
            int ey = offsetY + (int)(ally.y * cellSize);
            
            if (ex >= offsetX && ex < offsetX + mapDrawWidth && ey >= offsetY && ey < offsetY + mapDrawHeight) {
                Ellipse(hdc, ex - 3, ey - 3, ex + 3, ey + 3);
            }
        }
    }
    
    SelectObject(hdc, hBrushMagenta);
    for (int i = 0; i < 6; i++) {
        int cx = offsetX + (int)(claws[i].x * cellSize);
        int cy = offsetY + (int)(claws[i].y * cellSize);
        
        Rectangle(hdc, cx - 4, cy - 4, cx + 4, cy + 4);
    }
    
    // Marshall Health Bar
    if (marshallHealthBarActive) {
        int barW = 300;
        int barH = 15;
        // Move bar down to 50 to ensure large text fits on top
        int barX = (SCREEN_WIDTH - barW) / 2;
        int barY = 50; 
        
        RECT border = {barX - 2, barY - 2, barX + barW + 2, barY + barH + 2};
        FillRect(hdc, &border, hBrushDarkRed); 
        
        if (marshallHP > 0) {
            int fillW = (int)((float)marshallHP / marshallMaxHP * barW);
            RECT fill = {barX, barY, barX + fillW, barY + barH};
            FillRect(hdc, &fill, hBrushRed); 
        }
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFontMedium);
        
        SIZE sz;
        const char* name = "MARSHALL";
        GetTextExtentPoint32A(hdc, name, 8, &sz);
        // Draw text above bar with dynamic height
        TextOutA(hdc, barX + (barW - sz.cx) / 2, barY - sz.cy - 2, name, 8);
        
        SelectObject(hdc, hOldFont);
    }
    
    // Deserialize global reset
    if (pendingGameReset) {
        pendingGameReset = false;
        // score retained on continue
        player.health = player.maxHealth;
        player.level = 1; player.xp = 0; player.xpToNextLevel = 100; player.maxHealth = 100; if (johnSecret.isSpawned && johnSecret.naturalSpawn) { player.maxHealth *= 2; player.health = player.maxHealth; } g_BonusSpeed = 0.0f; g_PendingUpgrades = 0; g_LevelUpWindowOpen = false; player.x = 10.0f;
        player.y = 32.0f;
        
        // Reset weapon ammo to defaults
        weaponAmmo[0] = 8; weaponAmmo[1] = 5; weaponAmmo[2] = 4;
        weaponMaxAmmo[0] = 8; weaponMaxAmmo[1] = 5; weaponMaxAmmo[2] = 4;
        
        // Reset Boss & Game State
        bossActive = false; TriggerEvent("boss_active", false);
        preBossPhase = false; TriggerEvent("pre_boss_phase", false);
        preBossTimer = 0;
        preBossPulseTimer = 0;
        bossHealth = 1500;
        phase2Active = false;
        enragedMode = false;
        enemies.clear();
        fireballs.clear();
        enemyBullets.clear();
        gravitals.clear();
        InitClaws();
        marshallSpawned = false; 
        marshallKilled = false;
        officerSpawned = false;
        
        defectedRespawnTimer = 0.0f;
        militiaBarActive = false;
        SpawnEnemies();
    }

    // Restore original objects
    SelectObject(hdc, origPen);
    SelectObject(hdc, origBrush);
}

void ManageAlliedOfficer(float deltaTime) {
    if (!HasEvent("marshall_spawned") || HasEvent("pre_boss_phase") || HasEvent("boss_active")) {
        return; 
    }

    int alliedOfficerCount = 0;
    for (const auto& e : allies) {
        if (e.active && e.isDefectedOfficer) {
            alliedOfficerCount++;
        }
    }
    for (const auto& e : pendingAllies) {
        if (e.active && e.isDefectedOfficer) {
            alliedOfficerCount++;
        }
    }

    defectedOfficerActive = (alliedOfficerCount > 0);

    if (alliedOfficerCount < 1) {
        if (defectedRespawnTimer > 0) {
            defectedRespawnTimer -= deltaTime;
        } else {
            Enemy defected;
            do {
                float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                defected.x = player.x + cosf(angle) * 3.0f;
                defected.y = player.y + sinf(angle) * 3.0f;
                if (defected.x < 2.0f) defected.x = 2.0f; if (defected.x >= MAP_WIDTH - 2.0f) defected.x = MAP_WIDTH - 2.1f;
                if (defected.y < 2.0f) defected.y = 2.0f; if (defected.y >= MAP_HEIGHT - 2.0f) defected.y = MAP_HEIGHT - 2.1f;
            } while (worldMap[(int)defected.x][(int)defected.y] != 0);
            
            defected.active = true;
            defected.speed = 1.2f;
            defected.distance = 0;
            defected.spriteIndex = 0;
            defected.health = 4;
            defected.maxHealth = 4;
            defected.hurtTimer = 0;
            defected.isShooter = true;
            defected.isOfficer = false;
            defected.isDefectedOfficer = true;
            defected.isAllied = true; defected.isEnemy = false;
            defected.fireTimer = 2.0f;
            defected.firingTimer = 0;
            defected.isMarshall = false;
            defected.hasNeuralBrain = true;
            defected.officerState = 0;
            defected.officerCooldown = 0.0f;
            NeuralAI::InheritBrain(defected.brain);
            pendingAllies.push_back(defected);
            
            defectedOfficerActive = true;
            defectedRespawnTimer = 5.0f;
        }
    }
}

void UpdatePlayer(float deltaTime) {
    bool isSprinting = keys[VK_LSHIFT] || keys[VK_SHIFT];
    float sprintSpeed = enragedMode ? 13.0f : 6.5f;
    float baseSpeed = (isSprinting ? sprintSpeed : 4.0f) + g_BonusSpeed;
    float moveSpeed = baseSpeed * deltaTime;
    float rotSpeed = 2.5f * deltaTime;
    
    isMoving = false;
    
    if (dialogueController.IsActive()) {
        return;
    }
    
    auto CheckPlayerCollision = [&](float nx, float ny) -> bool {
        if (worldMap[(int)nx][(int)ny] != 0) return true;
        for(const auto& br : bigRocks) {
            float dx = nx - br.x;
            float dy = ny - br.y;
            if(dx*dx + dy*dy < 0.64f) return true; 
        }
        for (const auto& e : enemies) {
            if (!e.active) continue;
            float edx = nx - e.x;
            float edy = ny - e.y;
            if (edx*edx + edy*edy < 0.64f) return true; // Respect enemy hitbox
        }
        float htdx = nx - healingTower.x;
        float htdy = ny - healingTower.y;
        if (healingTower.state != TOWER_DORMANT && htdx*htdx + htdy*htdy < 1.0f) return true;
        return false;
    };

    // Process input (movement, rotation)
    if (g_EnableMouseLook && !spectatorMode) {
        if (keys['W'] || keys[VK_UP]) {
            float newX = player.x + cosf(player.angle) * moveSpeed;
            float newY = player.y + sinf(player.angle) * moveSpeed;
            if ((newX-32)*(newX-32) + (player.y-32)*(player.y-32) < 4.0f) newX = player.x;
            if (!CheckPlayerCollision(newX, player.y)) player.x = newX;
            
            if ((player.x-32)*(player.x-32) + (newY-32)*(newY-32) < 4.0f) newY = player.y;
            if (!CheckPlayerCollision(player.x, newY)) player.y = newY;
            isMoving = true;
        }

        if (keys['S'] || keys[VK_DOWN]) {
            float newX = player.x - cosf(player.angle) * moveSpeed;
            float newY = player.y - sinf(player.angle) * moveSpeed;
            if ((newX-32)*(newX-32) + (player.y-32)*(player.y-32) < 4.0f) newX = player.x;
            if (!CheckPlayerCollision(newX, player.y)) player.x = newX;
            
            if ((player.x-32)*(player.x-32) + (newY-32)*(newY-32) < 4.0f) newY = player.y;
            if (!CheckPlayerCollision(player.x, newY)) player.y = newY;
            isMoving = true;
        }

        if (keys['A']) {
            float strafeAngle = player.angle - PI / 2;
            float newX = player.x + cosf(strafeAngle) * moveSpeed;
            float newY = player.y + sinf(strafeAngle) * moveSpeed;
            if ((newX-32)*(newX-32) + (player.y-32)*(player.y-32) < 4.0f) newX = player.x;
            if (!CheckPlayerCollision(newX, player.y)) player.x = newX;
            
            if ((player.x-32)*(player.x-32) + (newY-32)*(newY-32) < 4.0f) newY = player.y;
            if (!CheckPlayerCollision(player.x, newY)) player.y = newY;
            isMoving = true;
        }

        if (keys['D']) {
            float strafeAngle = player.angle + PI / 2;
            float newX = player.x + cosf(strafeAngle) * moveSpeed;
            float newY = player.y + sinf(strafeAngle) * moveSpeed;
            if ((newX-32)*(newX-32) + (player.y-32)*(player.y-32) < 4.0f) newX = player.x;
            if (!CheckPlayerCollision(newX, player.y)) player.x = newX;
            
            if ((player.x-32)*(player.x-32) + (newY-32)*(newY-32) < 4.0f) newY = player.y;
            if (!CheckPlayerCollision(player.x, newY)) player.y = newY;
            isMoving = true;
        }
    }
    
    if (spectatorMode) {
        float speed = 10.0f * deltaTime;
        if (keys[VK_SHIFT]) speed *= 2.0f;
        
        if (keys['W']) {
            spectatorX += cosf(spectatorAngle) * speed;
            spectatorY += sinf(spectatorAngle) * speed;
        }
        if (keys['S']) {
            spectatorX -= cosf(spectatorAngle) * speed;
            spectatorY -= sinf(spectatorAngle) * speed;
        }
        if (keys['A']) {
            spectatorX -= cosf(spectatorAngle + PI/2) * speed;
            spectatorY -= sinf(spectatorAngle + PI/2) * speed;
        }
        if (keys['D']) {
            spectatorX += cosf(spectatorAngle + PI/2) * speed;
            spectatorY += sinf(spectatorAngle + PI/2) * speed;
        }
        if (keys['Q']) spectatorAngle -= 2.0f * deltaTime;
        if (keys['E']) spectatorAngle += 2.0f * deltaTime;
        
        // Sync player camera to spectator for rendering
        player.x = spectatorX;
        player.y = spectatorY;
        player.angle = spectatorAngle;
        player.pitch = spectatorPitch;
        
        return; // Skip rest of player updates (reload, steps, healing)
    }
    
    if (keys['R']) StartReload();
    
    static float stepTimer = 0;
    if (isMoving) {
        stepTimer -= deltaTime;
        if (stepTimer <= 0) {
            PlayStepSound();
            stepTimer = 0.4f;
        }
    } else {
        stepTimer = 0;
    }
    
    if (healFlashTimer > 0) healFlashTimer -= deltaTime;
    
    for (int i = 0; i < 3; i++) {
        if (medkits[i].active) {
            float dx = player.x - medkits[i].x;
            float dy = player.y - medkits[i].y;
            if (sqrtf(dx*dx + dy*dy) < 1.5f) {
                player.health += Medkit::HEAL_AMOUNT;
                if (player.health > player.maxHealth) player.health = player.maxHealth;
                medkits[i].active = false;
                medkits[i].respawnTimer = Medkit::RESPAWN_TIME;
                healFlashTimer = 1.0f;
                PlayHealSound();
            }
        } else {
            medkits[i].respawnTimer -= deltaTime;
            if (medkits[i].respawnTimer <= 0) {
                do {
                    medkits[i].x = 5.0f + (rand() % ((MAP_WIDTH - 10) * 10)) / 10.0f;
                    medkits[i].y = 5.0f + (rand() % ((MAP_HEIGHT - 10) * 10)) / 10.0f;
                } while (worldMap[(int)medkits[i].x][(int)medkits[i].y] != 0 || 
                         sqrtf((medkits[i].x - 32)*(medkits[i].x - 32) + (medkits[i].y - 32)*(medkits[i].y - 32)) < 5.0f);
                medkits[i].active = true;
            }
        }
    }
    
    playerNearGate = false;
    int px = (int)player.x;
    int py = (int)player.y;
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            int cx = px + dx;
            int cy = py + dy;
            if (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
                if (worldMap[cx][cy] == 4) {
                    float dist = sqrtf((float)(dx*dx + dy*dy));
                    if (dist < 2.0f) {
                        playerNearGate = true;
                        break;
                    }
                }
            }
        }
        if (playerNearGate) break;
    }
    
    static bool ePressed = false;
    if (playerNearGate && keys['E'] && !gateDialogueActive && !dialogueController.IsActive()) {
        if (!ePressed) {
            ePressed = true;
            wchar_t exePath[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            wchar_t* lastBackSlash = wcsrchr(exePath, L'\\');
            wchar_t* lastForwardSlash = wcsrchr(exePath, L'/');
            wchar_t* lastSlash = lastBackSlash;
            if (lastForwardSlash && (!lastSlash || lastForwardSlash > lastSlash)) lastSlash = lastForwardSlash;
            if (lastSlash) *lastSlash = L'\0';
            
            wchar_t dialoguePath[MAX_PATH];
            swprintf(dialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\player.line", exePath);
            if (playerDialogueController.LoadFromLine(dialoguePath)) {
                playerDialogueController.Start();
                gateDialogueActive = true;
            }
        }
    } else if (!keys['E']) {
        ePressed = false;
    }
}

void RenderGun() {
    DWORD* baseGunPixels = gunUpgraded ? (currentWeapon == 1 ? gunUpgrade1Pixels : (currentWeapon == 2 ? gunUpgrade2Pixels : gunPixels)) : gunPixels;
    DWORD* baseFirePixels = gunUpgraded ? (currentWeapon == 1 ? gunfire1Pixels : (currentWeapon == 2 ? gunfire2Pixels : gunfirePixels)) : gunfirePixels;
    int baseGunW = gunUpgraded ? (currentWeapon == 1 ? gunUpgrade1W : (currentWeapon == 2 ? gunUpgrade2W : gunW)) : gunW;
    int baseGunH = gunUpgraded ? (currentWeapon == 1 ? gunUpgrade1H : (currentWeapon == 2 ? gunUpgrade2H : gunH)) : gunH;
    int baseFireW = gunUpgraded ? (currentWeapon == 1 ? gunfire1W : (currentWeapon == 2 ? gunfire2W : gunfireW)) : gunfireW;
    int baseFireH = gunUpgraded ? (currentWeapon == 1 ? gunfire1H : (currentWeapon == 2 ? gunfire2H : gunfireH)) : gunfireH;
    
    if (!baseGunPixels || baseGunW <= 0 || baseGunH <= 0) return;
    
    int gunScale = 10;
    int gunDrawW = baseGunW * gunScale;
    int gunDrawH = baseGunH * gunScale;
    int gunX = SCREEN_WIDTH - gunDrawW + 20 + (int)gunSwayX;
    int gunY = SCREEN_HEIGHT - gunDrawH - 0 + (int)gunSwayY + (int)gunReloadOffset;
    
    DWORD* pixels = baseGunPixels;
    int srcW = baseGunW, srcH = baseGunH;
    
    if (isFiring && baseFirePixels && baseFireW > 0 && !isReloading) {
        pixels = baseFirePixels;
        srcW = baseFireW;
        srcH = baseFireH;
        gunDrawW = srcW * gunScale;
        gunDrawH = srcH * gunScale;
        gunX = SCREEN_WIDTH - gunDrawW + 20 + (int)gunSwayX;
        gunY = SCREEN_HEIGHT - gunDrawH - 0 + (int)gunSwayY + (int)gunReloadOffset + (int)gunRecoil;
    }
    
    // Apply recoil to idle gun too if recoil persists
    gunY += (int)gunRecoil;
    
    for (int y = 0; y < gunDrawH; y++) {
        int screenY = gunY + y;
        if (screenY < 0 || screenY >= SCREEN_HEIGHT) continue;
        int srcY = y * srcH / gunDrawH;
        
        for (int x = 0; x < gunDrawW; x++) {
            int screenX = gunX + x;
            if (screenX < 0 || screenX >= SCREEN_WIDTH) continue;
            int srcX = x * srcW / gunDrawW;
            
            DWORD col = pixels[srcY * srcW + srcX];
            int a = (col >> 24) & 0xFF;
            if (a == 0) continue;
            
            int b = (col >> 0) & 0xFF;
            int g = (col >> 8) & 0xFF;
            int r = (col >> 16) & 0xFF;
            renderBuffer[screenY * SCREEN_WIDTH + screenX] = MakeColor(r, g, b);
        }
    }
}

void SaveGame();
void RenderGame(HDC hdc) {
    CastRays();
    // Render3DScene(); // Disabled
    RenderClouds();
    RenderSprites();
    RenderGun();
    
    if (playerHurtTimer > 0) {
        float intensity = playerHurtTimer / 0.3f;
        if (intensity > 1.0f) intensity = 1.0f;
        ApplyHurtFlash_Fast(renderBuffer, SCREEN_WIDTH * SCREEN_HEIGHT, intensity);
    }
    
    int hbIndex = player.health / 10;
    if (hbIndex > 10) hbIndex = 10;
    if (hbIndex < 0) hbIndex = 0;
    if (healthbarPixels[hbIndex] && healthbarW > 0 && healthbarH > 0) {
        int hbScale = 8;
        int hbDrawW = healthbarW * hbScale;
        int hbDrawH = healthbarH * hbScale;
        int hbX = 70; 
        int hbY = SCREEN_HEIGHT - 160; 
        
        for (int y = 0; y < hbDrawH; y++) {
            int screenY = hbY + y;
            if (screenY < 0 || screenY >= SCREEN_HEIGHT) continue;
            int srcY = y * healthbarH / hbDrawH;
            
            for (int x = 0; x < hbDrawW; x++) {
                int screenX = hbX + x;
                if (screenX < 0 || screenX >= SCREEN_WIDTH) continue;
                int srcX = x * healthbarW / hbDrawW;
                
                DWORD col = healthbarPixels[hbIndex][srcY * healthbarW + srcX];
                int a = (col >> 24) & 0xFF;

                if (a == 0) continue;
                
                int b = (col >> 0) & 0xFF;
                int g = (col >> 8) & 0xFF;
                int r = (col >> 16) & 0xFF;
                renderBuffer[screenY * SCREEN_WIDTH + screenX] = MakeColor(r, g, b);
            }
        }
    }
    
    int xpIndex = (player.xp * 10) / player.xpToNextLevel;
    if (xpIndex > 10) xpIndex = 10;
    if (xpIndex < 0) xpIndex = 0;
    if (xpBarPixels[xpIndex] && xpBarW > 0 && xpBarH > 0) {
        int xpScale = 8;
        int xpDrawW = xpBarW * xpScale;
        int xpDrawH = xpBarH * xpScale;
        int xpX = 70; 
        int xpY = SCREEN_HEIGHT - 160 - xpDrawH + 65; 
        
        for (int y = 0; y < xpDrawH; y++) {
            int screenY = xpY + y;
            if (screenY < 0 || screenY >= SCREEN_HEIGHT) continue;
            int srcY = y * xpBarH / xpDrawH;
            
            for (int x = 0; x < xpDrawW; x++) {
                int screenX = xpX + x;
                if (screenX < 0 || screenX >= SCREEN_WIDTH) continue;
                int srcX = x * xpBarW / xpDrawW;
                
                DWORD col = xpBarPixels[xpIndex][srcY * xpBarW + srcX];
                int a = (col >> 24) & 0xFF;

                if (a == 0) continue;
                
                int b = (col >> 0) & 0xFF;
                int g = (col >> 8) & 0xFF;
                int r = (col >> 16) & 0xFF;
                renderBuffer[screenY * SCREEN_WIDTH + screenX] = MakeColor(r, g, b);
            }
        }
    }
    
    DrawCompass(hdc);
    
    memcpy(backBufferPixels, renderBuffer, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(DWORD));
    DrawMinimap(backBufferDC);
    
    // Draw Dialogue into the scene (distorted by VCR) -> Using backBufferDC
    if (dialogueController.IsActive()) {
        std::wstring name = dialogueController.GetSpeakerName();
        std::wstring text = dialogueController.GetCurrentText();
        bool showOpts = dialogueController.IsShowingOptions();
        std::vector<std::wstring> opts = dialogueController.GetCurrentOptions();
        int selectedOpt = dialogueController.GetSelectedOptionIndex();
        DialogueSystem::RenderDialogueBox(backBufferDC, SCREEN_WIDTH, SCREEN_HEIGHT, name, text, showOpts, opts, selectedOpt, 
            dialogueController.GetNameColor(), dialogueController.GetDialogueColor());
    }
    
    if (playerNearGate && !gateDialogueActive && !dialogueController.IsActive()) {
        SetBkMode(backBufferDC, TRANSPARENT);
        HFONT promptFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
        HFONT oldFont = (HFONT)SelectObject(backBufferDC, promptFont);
        SetTextColor(backBufferDC, RGB(255, 255, 100));
        const wchar_t* promptText = L"[E] Open Gate";
        SIZE sz;
        GetTextExtentPoint32W(backBufferDC, promptText, (int)wcslen(promptText), &sz);
        int promptX = (SCREEN_WIDTH - sz.cx) / 2;
        int promptY = SCREEN_HEIGHT / 2 + 50;
        TextOutW(backBufferDC, promptX, promptY, promptText, (int)wcslen(promptText));
        SelectObject(backBufferDC, oldFont);
        DeleteObject(promptFont);
    }
    
    if (gateDialogueActive && playerDialogueController.IsActive()) {
        std::wstring name = playerDialogueController.GetSpeakerName();
        std::wstring text = playerDialogueController.GetCurrentText();
        bool showOpts = playerDialogueController.IsShowingOptions();
        std::vector<std::wstring> opts = playerDialogueController.GetCurrentOptions();
        int selectedOpt = playerDialogueController.GetSelectedOptionIndex();
        DialogueSystem::RenderDialogueBox(backBufferDC, SCREEN_WIDTH, SCREEN_HEIGHT, name, text, showOpts, opts, selectedOpt,
            playerDialogueController.GetNameColor(), playerDialogueController.GetDialogueColor());
    }
    
    if (gateDialogueActive && !playerDialogueController.IsActive()) {
        gateDialogueActive = false;
    }

    if (scoreTimer > 0) {
        HFONT hOldFont = (HFONT)SelectObject(backBufferDC, hFontPixel);
        SetBkMode(backBufferDC, TRANSPARENT);
        
        wchar_t pointText[] = L"+1";
        SIZE size;
        GetTextExtentPoint32W(backBufferDC, pointText, 2, &size);
        int px = (SCREEN_WIDTH - size.cx) / 2;
        int py = (SCREEN_HEIGHT - size.cy) / 2 - 40;
        
        // Outline Pass
        SetTextColor(backBufferDC, RGB(0, 0, 0));
        for (int oy = -2; oy <= 2; oy+=2) {
            for (int ox = -2; ox <= 2; ox+=2) {
                 if (ox == 0 && oy == 0) continue;
                 TextOutW(backBufferDC, px + ox, py + oy, pointText, 2);
            }
        }
        
        // Main Pass
        SetTextColor(backBufferDC, RGB(255, 255, 255));
        TextOutW(backBufferDC, px, py, pointText, 2);

        GetTextExtentPoint32W(backBufferDC, scoreMsg, (int)wcslen(scoreMsg), &size);
        int sx = (SCREEN_WIDTH - size.cx) / 2;
        int sy = (SCREEN_HEIGHT - size.cy) / 2 + 30; // Shifted down slightly to accommodate running larger font
        
        // Outline Pass
        SetTextColor(backBufferDC, RGB(0, 0, 0));
        for (int oy = -2; oy <= 2; oy+=2) {
            for (int ox = -2; ox <= 2; ox+=2) {
                 if (ox == 0 && oy == 0) continue;
                 TextOutW(backBufferDC, sx + ox, sy + oy, scoreMsg, (int)wcslen(scoreMsg));
            }
        }
        
        // Main Pass
        SetTextColor(backBufferDC, RGB(255, 255, 255));
        TextOutW(backBufferDC, sx, sy, scoreMsg, (int)wcslen(scoreMsg));
        
        SelectObject(backBufferDC, hOldFont);
    }
    
    {
        wchar_t levelText[64];
        swprintf(levelText, 64, L"Level: %d", player.level);
        HFONT hLvlFont = CreateFontW(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Fixedsys");
        HFONT hOldFont = (HFONT)SelectObject(backBufferDC, hLvlFont);
        SetBkMode(backBufferDC, TRANSPARENT);
        SetTextColor(backBufferDC, RGB(255, 255, 0));
        SIZE size;
        GetTextExtentPoint32W(backBufferDC, levelText, (int)wcslen(levelText), &size);
        TextOutW(backBufferDC, (SCREEN_WIDTH - size.cx) / 2, SCREEN_HEIGHT - 60, levelText, (int)wcslen(levelText));
        SelectObject(backBufferDC, hOldFont);
        DeleteObject(hLvlFont);
    }
    
    if (g_LevelUpWindowOpen) {
        int boxW = 150;
        int boxH = 60;
        int gap = 20;
        int totalW = 3 * boxW + 2 * gap;
        int startX = SCREEN_WIDTH / 2 - totalW / 2;
        int y = SCREEN_HEIGHT / 2 - boxH / 2;
        
        HBRUSH bgBrush = CreateSolidBrush(RGB(50, 50, 50));
        HBRUSH btnBrush = CreateSolidBrush(RGB(100, 100, 100));
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        
        HBRUSH oldBrush = (HBRUSH)SelectObject(backBufferDC, bgBrush);
        HPEN oldPen = (HPEN)SelectObject(backBufferDC, borderPen);
        
        RECT windowRect = {startX - 20, y - 60, startX + totalW + 20, y + boxH + 20};
        FillRect(backBufferDC, &windowRect, bgBrush);
        
        SelectObject(backBufferDC, btnBrush);
        
        RECT speedBtn = {startX, y, startX + boxW, y + boxH};
        FillRect(backBufferDC, &speedBtn, btnBrush);
        
        RECT healthBtn = {startX + boxW + gap, y, startX + 2*boxW + gap, y + boxH};
        FillRect(backBufferDC, &healthBtn, btnBrush);
        
        RECT damageBtn = {startX + 2*boxW + 2*gap, y, startX + 3*boxW + 2*gap, y + boxH};
        FillRect(backBufferDC, &damageBtn, btnBrush);
        
        HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Fixedsys");
        HFONT oldFont = (HFONT)SelectObject(backBufferDC, hFont);
        SetBkMode(backBufferDC, TRANSPARENT);
        SetTextColor(backBufferDC, RGB(255, 255, 255));
        
        const wchar_t* title = L"LEVEL UP! CHOOSE AN UPGRADE";
        SIZE tSize;
        GetTextExtentPoint32W(backBufferDC, title, (int)wcslen(title), &tSize);
        TextOutW(backBufferDC, (SCREEN_WIDTH - tSize.cx) / 2, y - 45, title, (int)wcslen(title));
        
        const wchar_t* spd = L"+1 Speed";
        const wchar_t* hp = L"+10 Health";
        const wchar_t* dmg = L"+1 Damage";
        
        SIZE sSize, hSize, dSize;
        GetTextExtentPoint32W(backBufferDC, spd, (int)wcslen(spd), &sSize);
        GetTextExtentPoint32W(backBufferDC, hp, (int)wcslen(hp), &hSize);
        GetTextExtentPoint32W(backBufferDC, dmg, (int)wcslen(dmg), &dSize);
        
        TextOutW(backBufferDC, speedBtn.left + (boxW - sSize.cx)/2, speedBtn.top + (boxH - sSize.cy)/2, spd, (int)wcslen(spd));
        TextOutW(backBufferDC, healthBtn.left + (boxW - hSize.cx)/2, healthBtn.top + (boxH - hSize.cy)/2, hp, (int)wcslen(hp));
        TextOutW(backBufferDC, damageBtn.left + (boxW - dSize.cx)/2, damageBtn.top + (boxH - dSize.cy)/2, dmg, (int)wcslen(dmg));
        
        SelectObject(backBufferDC, oldFont);
        DeleteObject(hFont);
        SelectObject(backBufferDC, oldBrush);
        SelectObject(backBufferDC, oldPen);
        DeleteObject(bgBrush);
        DeleteObject(btnBrush);
        DeleteObject(borderPen);
    }
    if (g_LevelUpWindowOpen || g_PauseMenuOpen || victoryScreen) {
        POINT pt;
        if (GetCursorPos(&pt) && ScreenToClient(hMainWnd, &pt)) {
            g_ui.mouseX = pt.x;
            g_ui.mouseY = pt.y;
        }
    }

    if (g_PauseMenuOpen) {
        DrawUI9Slice(backBufferPixels, SCREEN_WIDTH, SCREEN_HEIGHT, uiWindowPixels, uiWindowW, uiWindowH, SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 300, 250, 8);
        
        SetBkMode(backBufferDC, TRANSPARENT);
        SetTextColor(backBufferDC, RGB(255, 255, 255));
        HFONT hOldFont = (HFONT)SelectObject(backBufferDC, hFontHUD);
        
        const wchar_t* title = L"PAUSED";
        SIZE sz;
        GetTextExtentPoint32W(backBufferDC, title, wcslen(title), &sz);
        TextOutW(backBufferDC, SCREEN_WIDTH/2 - sz.cx/2, SCREEN_HEIGHT/2 - 80, title, wcslen(title));
        
        if (DoUIButton(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 200, 40)) {
            PostQuitMessage(0);
        }
        const wchar_t* quitText = L"Quit";
        GetTextExtentPoint32W(backBufferDC, quitText, wcslen(quitText), &sz);
        TextOutW(backBufferDC, SCREEN_WIDTH/2 - sz.cx/2, SCREEN_HEIGHT/2 - 20 + 20 - sz.cy/2, quitText, wcslen(quitText));
        
        if (DoUIButton(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 40, 200, 40)) {
            SaveGame();
            PostQuitMessage(0);
        }
        const wchar_t* saveText = L"Save and Exit";
        GetTextExtentPoint32W(backBufferDC, saveText, wcslen(saveText), &sz);
        TextOutW(backBufferDC, SCREEN_WIDTH/2 - sz.cx/2, SCREEN_HEIGHT/2 + 40 + 20 - sz.cy/2, saveText, wcslen(saveText));
        
        SelectObject(backBufferDC, hOldFont);
    }
if (g_LevelUpWindowOpen || g_PauseMenuOpen || victoryScreen) {
        // Draw a custom software cursor
        POINT pt;
        if (GetCursorPos(&pt) && ScreenToClient(hMainWnd, &pt)) {
            HPEN cursorPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
            HPEN oldCPen = (HPEN)SelectObject(backBufferDC, cursorPen);
            MoveToEx(backBufferDC, pt.x, pt.y, NULL);
            LineTo(backBufferDC, pt.x + 15, pt.y + 10);
            MoveToEx(backBufferDC, pt.x, pt.y, NULL);
            LineTo(backBufferDC, pt.x + 10, pt.y + 15);
            MoveToEx(backBufferDC, pt.x, pt.y, NULL);
            LineTo(backBufferDC, pt.x + 10, pt.y + 10);
            SelectObject(backBufferDC, oldCPen);
            DeleteObject(cursorPen);
        }
    }

    memcpy(renderBuffer, backBufferPixels, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(DWORD));
    
    ApplyPostProcess();
    
    int shakeX = 0, shakeY = 0;
    if (screenShakeTimer > 0) {
        float shakeFactor = screenShakeTimer / 1.0f;
        shakeX = (int)((rand() % (int)(screenShakeIntensity * 2 + 1) - screenShakeIntensity) * shakeFactor);
        shakeY = (int)((rand() % (int)(screenShakeIntensity * 2 + 1) - screenShakeIntensity) * shakeFactor);
    }
    
    BitBlt(g_renderDC, shakeX, shakeY, SCREEN_WIDTH, SCREEN_HEIGHT, backBufferDC, 0, 0, SRCCOPY);
    
    // Phase 2 Visuals (Force Field + Lasers)
    if (phase2Active) {
        // Force Field
        if (forceFieldActive) {
             float dx = 32.0f - player.x; // Boss X
             float dy = 32.0f - player.y; // Boss Y
             float dist = sqrtf(dx*dx + dy*dy);
             
             float spriteAngle = atan2f(dy, dx) - player.angle;
             while (spriteAngle > PI) spriteAngle -= 2 * PI;
             while (spriteAngle < -PI) spriteAngle += 2 * PI;
             
             if (fabsf(spriteAngle) < FOV && dist > 0.5f) {
                 float screenX = (0.5f + spriteAngle / FOV) * SCREEN_WIDTH;
                 float spriteHeight = (SCREEN_HEIGHT / dist) * 8.0f; // Boss Scale 8.0
                 int radius = (int)(spriteHeight / 2.0f * 0.8f); // Slightly smaller than full sprite width
                 
                 // Center Y calculation matching RenderSprite
                 int centerY = (SCREEN_HEIGHT / 2 + (int)((SCREEN_HEIGHT / 2.0f) / dist) + (int)player.pitch) - (int)(spriteHeight / 2.0f);

                 HPEN oldPen = (HPEN)SelectObject(g_renderDC, hPenRed); 
                 HBRUSH oldBrush = (HBRUSH)SelectObject(g_renderDC, hBrushHollow);
                 Ellipse(g_renderDC, (int)(screenX - radius), centerY - radius, (int)(screenX + radius), centerY + radius);
                 SelectObject(g_renderDC, oldPen);
                 SelectObject(g_renderDC, oldBrush);
             }
        }
        
        // Laser
        if (activeLaserClaw != -1 && claws[activeLaserClaw].state == CLAW_PH2_ANCHORED) {
             float dx = claws[activeLaserClaw].x - player.x;
             float dy = claws[activeLaserClaw].y - player.y;
             float dist = sqrtf(dx*dx + dy*dy);
             
             float spriteAngle = atan2f(dy, dx) - player.angle;
             while (spriteAngle > PI) spriteAngle -= 2 * PI;
             while (spriteAngle < -PI) spriteAngle += 2 * PI;
             
             if (fabsf(spriteAngle) < FOV && dist > 0.5f) {
                 float screenX = (0.5f + spriteAngle / FOV) * SCREEN_WIDTH;
                 float spriteHeight = (SCREEN_HEIGHT / dist) * 8.0f; // Claw Scale 8.0
                 
                 int centerY = (SCREEN_HEIGHT / 2 + (int)((SCREEN_HEIGHT / 2.0f) / dist) + (int)player.pitch) - (int)(spriteHeight / 2.0f);
                 
                 HPEN oldPen = (HPEN)SelectObject(g_renderDC, hPenLaser);
                 MoveToEx(g_renderDC, (int)screenX, centerY, NULL);
                 LineTo(g_renderDC, SCREEN_WIDTH / 2, SCREEN_HEIGHT); // To weapon
                 SelectObject(g_renderDC, oldPen);
             }
        }
    }
    
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;
    int reticleSize = 12;
    int reticleGap = 4;
    HPEN oldPen = (HPEN)SelectObject(g_renderDC, hPenWhite);
    MoveToEx(g_renderDC, cx - reticleSize, cy, NULL);
    LineTo(g_renderDC, cx - reticleGap, cy);
    MoveToEx(g_renderDC, cx + reticleGap, cy, NULL);
    LineTo(g_renderDC, cx + reticleSize, cy);
    MoveToEx(g_renderDC, cx, cy - reticleSize, NULL);
    LineTo(g_renderDC, cx, cy - reticleGap);
    MoveToEx(g_renderDC, cx, cy + reticleGap, NULL);
    LineTo(g_renderDC, cx, cy + reticleSize);
    SelectObject(g_renderDC, oldPen);
    
    
    SetBkMode(g_renderDC, TRANSPARENT);
    SetTextColor(g_renderDC, RGB(255, 255, 0));
    TextOutW(g_renderDC, 60, 30, loadStatus, (int)wcslen(loadStatus));
    
    if (!missingAssets.empty()) {
        SetTextColor(g_renderDC, RGB(255, 80, 80));
        int yPos = 50;
        TextOutA(g_renderDC, 60, yPos, "MISSING ASSETS:", 15);
        yPos += 15;
        for (size_t i = 0; i < missingAssets.size() && i < 10; i++) {
            TextOutW(g_renderDC, 70, yPos, missingAssets[i].c_str(), (int)missingAssets[i].length());
            yPos += 15;
        }
        if (missingAssets.size() > 10) {
            wchar_t moreText[64];
            swprintf(moreText, 64, L"... and %zu more", missingAssets.size() - 10);
            TextOutW(g_renderDC, 70, yPos, moreText, (int)wcslen(moreText));
        }
    }
    
    wchar_t ammoText[64];
    if (isReloading) {
        swprintf(ammoText, 64, L"RELOADING...");
        SetTextColor(g_renderDC, RGB(255, 255, 0));
    } else {
        swprintf(ammoText, 64, L"Ammo: %d/%d", ammo, maxAmmo);
        SetTextColor(g_renderDC, ammo == 0 ? RGB(255, 0, 0) : RGB(255, 255, 255));
    }
    TextOutW(g_renderDC, 60, 70, ammoText, (int)wcslen(ammoText));
    
    wchar_t scoreText[128];
    swprintf(scoreText, 128, L"Score: %d  High Score: %d", score, highScore);
    SetTextColor(g_renderDC, RGB(255, 255, 255));
    TextOutW(g_renderDC, 60, 110, scoreText, (int)wcslen(scoreText));
    
    if (paragonsUnlocked && paragonSummonCooldown > 0) {
        wchar_t cdText[64];
        swprintf(cdText, 64, L"Summon: %.1fs", paragonSummonCooldown);
        SetTextColor(g_renderDC, RGB(147, 112, 219));
        TextOutW(g_renderDC, 60, 150, cdText, (int)wcslen(cdText));
        
        int barW = 100;
        int barH = 8;
        int barX = 60;
        int barY = 175;
        RECT bgRect = {barX, barY, barX + barW, barY + barH};
        FillRect(g_renderDC, &bgRect, hBrushDarkGray);
        
        float pct = paragonSummonCooldown / 3.0f;
        if (pct > 1.0f) pct = 1.0f;
        int fillW = (int)(barW * (1.0f - pct));
        RECT fillRect = {barX, barY, barX + fillW, barY + barH};
        FillRect(g_renderDC, &fillRect, hBrushMagenta);
    }
    

    
    if (hordeMessageTimer > 0) {
        HFONT hOldHFont = (HFONT)SelectObject(g_renderDC, hFontMedium);
        
        SetTextColor(g_renderDC, RGB(255, 0, 0));
        SetBkMode(g_renderDC, TRANSPARENT);
        
        const wchar_t* hordeMsg = L"The Towns Folk has rallied!";
        SIZE hsz;
        GetTextExtentPoint32W(g_renderDC, hordeMsg, (int)wcslen(hordeMsg), &hsz);
        TextOutW(g_renderDC, (SCREEN_WIDTH - hsz.cx) / 2, SCREEN_HEIGHT / 4, hordeMsg, (int)wcslen(hordeMsg));
        
        SelectObject(g_renderDC, hOldHFont);
    }
    
    if (paragonMessageTimer > 0) {
        HFONT hOldPFont = (HFONT)SelectObject(g_renderDC, hFontMedium);
        
        int alpha = (int)((paragonMessageTimer / 3.0f) * 255.0f);
        if (alpha > 255) alpha = 255;
        if (alpha < 0) alpha = 0;
        SetTextColor(g_renderDC, RGB(147, 112, 219));
        SetBkMode(g_renderDC, TRANSPARENT);
        
        const wchar_t* msg = L"The Brotherhood has deemed you worthy";
        SIZE sz;
        GetTextExtentPoint32W(g_renderDC, msg, (int)wcslen(msg), &sz);
        TextOutW(g_renderDC, (SCREEN_WIDTH - sz.cx) / 2, SCREEN_HEIGHT / 3, msg, (int)wcslen(msg));
        
        SelectObject(g_renderDC, hOldPFont);
    }
    
    if (upgradeMessageTimer > 0) {
        HFONT hOldUFont = (HFONT)SelectObject(g_renderDC, hFontMedium);
        
        SetTextColor(g_renderDC, RGB(255, 215, 0));
        SetBkMode(g_renderDC, TRANSPARENT);
        
        const wchar_t* upMsg = L"Gun Upgraded! Damage: 5, Ammo +2";
        SIZE usz;
        GetTextExtentPoint32W(g_renderDC, upMsg, (int)wcslen(upMsg), &usz);
        TextOutW(g_renderDC, (SCREEN_WIDTH - usz.cx) / 2, SCREEN_HEIGHT / 5, upMsg, (int)wcslen(upMsg));
        
        const wchar_t* upMsg2 = L"Press 1 or 2 to switch weapons";
        SIZE usz2;
        GetTextExtentPoint32W(g_renderDC, upMsg2, (int)wcslen(upMsg2), &usz2);
        TextOutW(g_renderDC, (SCREEN_WIDTH - usz2.cx) / 2, SCREEN_HEIGHT / 5 + 40, upMsg2, (int)wcslen(upMsg2));
        
        SelectObject(g_renderDC, hOldUFont);
    }
    
    // Boss Health Bar
   
    if (militiaMessageTimer > 0) {
        HFONT hOldMFont = (HFONT)SelectObject(g_renderDC, hFontMedium);
        SetTextColor(g_renderDC, RGB(255, 0, 0));
        SetBkMode(g_renderDC, TRANSPARENT);
        const wchar_t* msg = L"A militia is forming...";
        SIZE sz;
        GetTextExtentPoint32W(g_renderDC, msg, (int)wcslen(msg), &sz);
        TextOutW(g_renderDC, (SCREEN_WIDTH - sz.cx) / 2, SCREEN_HEIGHT / 4 + 40, msg, (int)wcslen(msg));
        SelectObject(g_renderDC, hOldMFont);
        militiaMessageTimer -= 0.016f; // Approx frame time dec
    }

    // Boss Bar (The Spire) - Text above, centered
    if (bossActive) {
        int barW = 400;
        int barH = 20;
        int barX = (SCREEN_WIDTH - barW) / 2;
        int barY = 40;
        
        RECT bgRect = {barX, barY, barX + barW, barY + barH};
        FillRect(g_renderDC, &bgRect, hBrushDarkRed);
        
        int hp = bossHealth;
        int max = 1500; 
        int hpW = (int)((float)hp / max * barW);
        if (hpW < 0) hpW = 0;
        if (hpW > barW) hpW = barW;
        
        RECT hpRect = {barX, barY, barX + hpW, barY + barH};
        FillRect(g_renderDC, &hpRect, hBrushRed);
        
        HFONT hOldSpireFont = (HFONT)SelectObject(g_renderDC, hFontMedium);
        const wchar_t* name = L"THE SPIRE";
        SIZE sz;
        GetTextExtentPoint32W(g_renderDC, name, (int)wcslen(name), &sz);
        SetTextColor(g_renderDC, RGB(255, 255, 255));
        TextOutW(g_renderDC, barX + (barW - sz.cx) / 2, barY - sz.cy - 5, name, (int)wcslen(name));
        SelectObject(g_renderDC, hOldSpireFont);
    }

    // Militia Bar - Placed below the Marshall bar, shows count
    if (militiaBarActive) {
         int barW = 300;
         int barH = 15;
         int barX = (SCREEN_WIDTH - barW) / 2;
         int barY = 85; 
         
         RECT mBgRect = {barX - 2, barY - 2, barX + barW + 2, barY + barH + 2}; 
         FillRect(g_renderDC, &mBgRect, hBrushDarkGray);
         
         int maxRef = (militiaMaxCount < 1) ? 1 : militiaMaxCount;
         int mW = (int)((float)militiaCount / (float)maxRef * barW);
         if (mW > barW) mW = barW;
         if (mW < 0) mW = 0;
         RECT mHpRect = {barX, barY, barX + mW, barY + barH};
         FillRect(g_renderDC, &mHpRect, hBrushGold);
         
         wchar_t mText[64];
         swprintf(mText, 64, L"THE MILITIA  %d / %d", militiaCount, militiaMaxCount);
         SetTextColor(g_renderDC, RGB(255, 255, 255));
         TextOutW(g_renderDC, barX, barY - 15, mText, (int)wcslen(mText));
    }
    
    // Claw Health Bars (Phase 2 only)
    if (phase2Active && !enragedMode) {
        int clawBarW = 80;
        int clawBarH = 8;
        int startX = (SCREEN_WIDTH - (clawBarW * 6 + 10 * 5)) / 2;
        int clawBarY = 110;
        
        for (int i = 0; i < 6; i++) {
            int barX = startX + i * (clawBarW + 10);
            
            RECT bgRect = {barX, clawBarY, barX + clawBarW, clawBarY + clawBarH};
            FillRect(g_renderDC, &bgRect, hBrushDarkGray);
            
            if (claws[i].state != CLAW_PH2_DEAD) {
                int hp = claws[i].health;
                if (hp < 0) hp = 0;
                if (hp > 250) hp = 250;
                int hpW = (int)((float)hp / 250.0f * clawBarW);
                
                RECT hpRect = {barX, clawBarY, barX + hpW, clawBarY + clawBarH};
                FillRect(g_renderDC, &hpRect, hBrushMagenta);
            }
            
            wchar_t clawLabel[16];
            swprintf(clawLabel, 16, L"C%d", i + 1);
            SetTextColor(g_renderDC, claws[i].state == CLAW_PH2_DEAD ? RGB(100, 100, 100) : RGB(255, 255, 255));
            TextOutA(g_renderDC, barX + clawBarW / 2 - 8, clawBarY - 12, (claws[i].state == CLAW_PH2_DEAD ? "X" : ""), 1);
            SetTextColor(g_renderDC, RGB(255, 255, 255));
            TextOutW(g_renderDC, barX, clawBarY + clawBarH + 2, clawLabel, (int)wcslen(clawLabel));
        }
    }
    
    // Countdown Timer during Pre-Boss Phase
    if (preBossPhase) {
        wchar_t bossTimerMsg[64];
        swprintf(bossTimerMsg, 64, L"BOSS IN: %.0f", preBossTimer);
        
        HFONT hOldFont = (HFONT)SelectObject(g_renderDC, hFontTitle);
        
        SetTextColor(g_renderDC, RGB(255, 0, 0));
        SetBkMode(g_renderDC, TRANSPARENT);
        
        SIZE size;
        GetTextExtentPoint32W(g_renderDC, bossTimerMsg, (int)wcslen(bossTimerMsg), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, SCREEN_HEIGHT / 2 - 50, bossTimerMsg, (int)wcslen(bossTimerMsg));
        
        SelectObject(g_renderDC, hOldFont);
    }

    // Pre-Boss Phase: Shaking "God has awoken" text
    if (bossActive && bossEventTimer > 0) {
        HFONT hOldFont = (HFONT)SelectObject(g_renderDC, hFontBig);
        SetTextColor(g_renderDC, RGB(255, 0, 0));
        SetBkMode(g_renderDC, TRANSPARENT);
        
        int shakeX = (rand() % 10) - 5;
        int shakeY = (rand() % 10) - 5;
        
        TextOutW(g_renderDC, SCREEN_WIDTH/2 - 200 + shakeX, SCREEN_HEIGHT/2 - 100 + shakeY, L"God has awoken", 14);
        SelectObject(g_renderDC, hOldFont);
        
        SetTextColor(g_renderDC, RGB(255, 255, 255));
    }

    
    SetTextColor(g_renderDC, RGB(255, 255, 255));
    wchar_t info[128];
    swprintf(info, 128, L"WASD=Move | Mouse=Look | LClick=Shoot | R=Reload | ESC=Quit");
    TextOutW(g_renderDC, 10, SCREEN_HEIGHT - 25, info, (int)wcslen(info));
    
    if (postBossPhase && !dialogueController.IsActive()) {
        NPCSystem::NPC* nearNPC = NPCSystem::GetNearestInteractableNPC(player.x, player.y, 3.0f);
        if (nearNPC && !nearNPC->dialoguePath.empty()) {
            HFONT hOldPromptFont = (HFONT)SelectObject(g_renderDC, hFontHUD);
            SetTextColor(g_renderDC, RGB(255, 255, 0));
            SetBkMode(g_renderDC, TRANSPARENT);
            const wchar_t* prompt = L"Press E to interact";
            SIZE sz;
            GetTextExtentPoint32W(g_renderDC, prompt, (int)wcslen(prompt), &sz);
            TextOutW(g_renderDC, (SCREEN_WIDTH - sz.cx) / 2, SCREEN_HEIGHT / 2 + 100, prompt, (int)wcslen(prompt));
            SelectObject(g_renderDC, hOldPromptFont);
        }
    }
    

    
    if (whiteFadeToVictory && whiteFadeTimer > 0) {
        float fadeProgress = 1.0f - (whiteFadeTimer / 2.0f);
        if (fadeProgress > 1.0f) fadeProgress = 1.0f;
        if (fadeProgress < 0) fadeProgress = 0;
        int fadeAmount = (int)(fadeProgress * 256);
        ApplyBrightFade_Fast(backBufferPixels, SCREEN_WIDTH * SCREEN_HEIGHT, fadeAmount);
        
        BITMAPINFO biFade = {};
        biFade.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        biFade.bmiHeader.biWidth = SCREEN_WIDTH;
        biFade.bmiHeader.biHeight = -SCREEN_HEIGHT;
        biFade.bmiHeader.biPlanes = 1;
        biFade.bmiHeader.biBitCount = 32;
        SetDIBitsToDevice(g_renderDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, SCREEN_HEIGHT, backBufferPixels, &biFade, DIB_RGB_COLORS);
    }
    
    if (healFlashTimer > 0) {
        float intensity = healFlashTimer / 1.0f;
        if (intensity > 1.0f) intensity = 1.0f;
        int alpha = (int)(intensity * 80);
        ApplyHealFlash_Fast(backBufferPixels, SCREEN_WIDTH * SCREEN_HEIGHT, alpha);
        
        BITMAPINFO biHeal = {};
        biHeal.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        biHeal.bmiHeader.biWidth = SCREEN_WIDTH;
        biHeal.bmiHeader.biHeight = -SCREEN_HEIGHT;
        biHeal.bmiHeader.biPlanes = 1;
        biHeal.bmiHeader.biBitCount = 32;
        SetDIBitsToDevice(g_renderDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, SCREEN_HEIGHT, 
            backBufferPixels, &biHeal, DIB_RGB_COLORS);
    }
    
    if (victoryScreen) {
        ApplyVictoryBright_Fast(backBufferPixels, SCREEN_WIDTH * SCREEN_HEIGHT);
        
        BITMAPINFO bi2 = {};
        bi2.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi2.bmiHeader.biWidth = SCREEN_WIDTH;
        bi2.bmiHeader.biHeight = -SCREEN_HEIGHT;
        bi2.bmiHeader.biPlanes = 1;
        bi2.bmiHeader.biBitCount = 32;
        SetDIBitsToDevice(g_renderDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, SCREEN_HEIGHT, 
            backBufferPixels, &bi2, DIB_RGB_COLORS);
        
        static bool cursorShownForVictory = false;
        if (!cursorShownForVictory) {
            ShowCursor(TRUE);
            cursorShownForVictory = true;
        }
        
        SetBkMode(g_renderDC, TRANSPARENT);
        
        HFONT oldFont = (HFONT)SelectObject(g_renderDC, hFontBig);
        SetTextColor(g_renderDC, RGB(0, 150, 0));
        const wchar_t* wonText = L"You Won!";
        SIZE size;
        GetTextExtentPoint32W(g_renderDC, wonText, (int)wcslen(wonText), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, 150, wonText, (int)wcslen(wonText));
        
        SelectObject(g_renderDC, hFontMedium);
        SetTextColor(g_renderDC, RGB(50, 50, 50));
        wchar_t hsText[128];
        swprintf(hsText, 128, L"Final Score: %d", score);
        GetTextExtentPoint32W(g_renderDC, hsText, (int)wcslen(hsText), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, 240, hsText, (int)wcslen(hsText));
        
        swprintf(hsText, 128, L"High Score: %d", highScore);
        GetTextExtentPoint32W(g_renderDC, hsText, (int)wcslen(hsText), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, 290, hsText, (int)wcslen(hsText));
        
        SelectObject(g_renderDC, hFontHUD);
        
        RECT playAgainBtn = {SCREEN_WIDTH/2 - 120, 380, SCREEN_WIDTH/2 + 120, 430};
        RECT exitBtn = {SCREEN_WIDTH/2 - 120, 450, SCREEN_WIDTH/2 + 120, 500};
        
        FillRect(g_renderDC, &playAgainBtn, hBrushGreen);
        FillRect(g_renderDC, &exitBtn, hBrushRed); // Using standard red (200,0,0) vs old (180,0,0) - acceptable
        
        SetTextColor(g_renderDC, RGB(255, 255, 255));
        const wchar_t* playText = L"Play Again";
        GetTextExtentPoint32W(g_renderDC, playText, (int)wcslen(playText), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, 392, playText, (int)wcslen(playText));
        
        const wchar_t* exitText = L"Exit";
        GetTextExtentPoint32W(g_renderDC, exitText, (int)wcslen(exitText), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, 462, exitText, (int)wcslen(exitText));
        
        SelectObject(g_renderDC, oldFont);
    }
    
    // Debug Console
    if (consoleActive) {
        RECT consoleRect = {0, 0, SCREEN_WIDTH, 200};
        FillRect(g_renderDC, &consoleRect, hBrushDarkGray);
        
        SetBkMode(g_renderDC, TRANSPARENT);
        SetTextColor(g_renderDC, RGB(255, 255, 255));
        
        HFONT oldFont = (HFONT)SelectObject(g_renderDC, hFontDebug);
        
        TextOutW(g_renderDC, 10, 10, L"DEBUG CONSOLE (type 'exit' to close)", 36);
        TextOutW(g_renderDC, 10, 35, L">", 1);
        TextOutW(g_renderDC, 25, 35, consoleBuffer.c_str(), (int)consoleBuffer.length());
        
        // Cursor
        if ((int)(GetTickCount() / 500) % 2 == 0) {
            SIZE size;
            GetTextExtentPoint32W(g_renderDC, consoleBuffer.c_str(), (int)consoleBuffer.length(), &size);
            TextOutW(g_renderDC, 25 + size.cx, 35, L"_", 1);
        }
        
        if (wcslen(consoleError) > 0) {
            SetTextColor(g_renderDC, RGB(255, 80, 80));
            TextOutW(g_renderDC, 10, 60, consoleError, (int)wcslen(consoleError));
            SetTextColor(g_renderDC, RGB(255, 255, 255));
        }
        
        SelectObject(g_renderDC, oldFont);
    }
    
    // Stats Display
    if (showStats) {
        int meleeCount = 0, shooterCount = 0;
        for (auto& e : enemies) {
            if (e.active) {
                if (e.isShooter) shooterCount++;
                else meleeCount++;
            }
        }
        int totalEnemies = meleeCount + shooterCount;
        int paragonCount = GetAliveParagonCount();
        
        float degAngle = player.angle * 180.0f / PI;
        while (degAngle < 0) degAngle += 360.0f;
        while (degAngle >= 360.0f) degAngle -= 360.0f;
        
        const wchar_t* dirName = L"E";
        if (degAngle >= 337.5f || degAngle < 22.5f) dirName = L"E";
        else if (degAngle >= 22.5f && degAngle < 67.5f) dirName = L"SE";
        else if (degAngle >= 67.5f && degAngle < 112.5f) dirName = L"S";
        else if (degAngle >= 112.5f && degAngle < 157.5f) dirName = L"SW";
        else if (degAngle >= 157.5f && degAngle < 202.5f) dirName = L"W";
        else if (degAngle >= 202.5f && degAngle < 247.5f) dirName = L"NW";
        else if (degAngle >= 247.5f && degAngle < 292.5f) dirName = L"N";
        else if (degAngle >= 292.5f && degAngle < 337.5f) dirName = L"NE";
        
        SetBkMode(g_renderDC, TRANSPARENT);
        SetTextColor(g_renderDC, RGB(255, 255, 255));
        wchar_t statText[512];
        swprintf(statText, 512, L"FPS: %d  |  Enemies: %d (Melee: %d/%d, Shooters: %d/%d)  |  Paragons: %d/8  |  Pos: (%.1f, %.1f)  |  Cap Timer: %.1f  |  Dir: %.1f° %ls\nHT State: %d | Timer: %.1f | CD: %.1f", 
                 currentFPS, totalEnemies, meleeCount, maxMeleeSpawn, shooterCount, maxShooterSpawn, paragonCount, player.x, player.y, spawnCapTimer, degAngle, dirName,
                 healingTower.state, healingTower.timer, healingTower.cooldownTimer);
        TextOutW(g_renderDC, 60, SCREEN_HEIGHT - 90, statText, (int)wcslen(statText));
    }
    
    if (errorTimer > 0 && wcslen(errorMessage) > 0) {
        HFONT hErrFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
        HFONT hOldErrFont = (HFONT)SelectObject(g_renderDC, hErrFont);
        SetBkMode(g_renderDC, TRANSPARENT);
        SetTextColor(g_renderDC, RGB(255, 50, 50));
        SIZE size;
        GetTextExtentPoint32W(g_renderDC, errorMessage, (int)wcslen(errorMessage), &size);
        TextOutW(g_renderDC, (SCREEN_WIDTH - size.cx) / 2, SCREEN_HEIGHT - 150, errorMessage, (int)wcslen(errorMessage));
        SelectObject(g_renderDC, hOldErrFont);
        DeleteObject(hErrFont);
    }
    
    // UI drawing moved to backBufferDC
    
    BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, g_renderDC, 0, 0, SRCCOPY);
}

void SaveGame() {
    CreateDirectoryW(L"configs", NULL);
    FILE* f = _wfopen(L"configs/savegame.dat", L"wb");
    if (!f) return;
    
    try {
        fwrite(&player, sizeof(Player), 1, f);
        fwrite(&score, sizeof(int), 1, f);
        fwrite(&phase2Active, sizeof(bool), 1, f);
        fwrite(&forceFieldActive, sizeof(bool), 1, f);
        fwrite(&enragedMode, sizeof(bool), 1, f);
        fwrite(&hordeActive, sizeof(bool), 1, f);
        fwrite(&bossActive, sizeof(bool), 1, f);
        fwrite(&preBossPhase, sizeof(bool), 1, f);
        fwrite(&bossHealth, sizeof(int), 1, f);
        
        fwrite(&currentWeapon, sizeof(int), 1, f);
        fwrite(&ammo, sizeof(int), 1, f);
        fwrite(weaponAmmo, sizeof(int), 3, f);
        fwrite(&gunUpgraded, sizeof(bool), 1, f);
        fwrite(&bazookaUnlocked, sizeof(bool), 1, f);
        
        int enemyCount = enemies.size();
        fwrite(&enemyCount, sizeof(int), 1, f);
        for (auto& e : enemies) {
            fwrite(&e.x, sizeof(float), 1, f);
            fwrite(&e.y, sizeof(float), 1, f);
            fwrite(&e.active, sizeof(bool), 1, f);
            fwrite(&e.spriteIndex, sizeof(int), 1, f);
            fwrite(&e.health, sizeof(int), 1, f);
            fwrite(&e.maxHealth, sizeof(int), 1, f);
            fwrite(&e.isShooter, sizeof(bool), 1, f);
            fwrite(&e.isMarshall, sizeof(bool), 1, f);
            fwrite(&e.state, sizeof(int), 1, f);
            fwrite(&e.tacticState, sizeof(int), 1, f);
            fwrite(&e.flankDir, sizeof(int), 1, f);
            fwrite(&e.brain, sizeof(NeuralAI::NeuralNet), 1, f);
            fwrite(&e.hasNeuralBrain, sizeof(bool), 1, f);
            fwrite(&e.dodgeDir, sizeof(int), 1, f);
            fwrite(&e.isPhalanx, sizeof(bool), 1, f);
            fwrite(&e.isSpearGuy, sizeof(bool), 1, f);
            fwrite(&e.spearState, sizeof(int), 1, f);
            fwrite(&e.isOfficer, sizeof(bool), 1, f);
            fwrite(&e.isDefectedOfficer, sizeof(bool), 1, f);
            fwrite(&e.isDefectedGunner, sizeof(bool), 1, f);
            fwrite(&e.officerState, sizeof(int), 1, f);
            fwrite(&e.speed, sizeof(float), 1, f);
        }
        

        
        for (int i = 0; i < 6; i++) {
            fwrite(&claws[i], sizeof(Claw), 1, f);
        }

        // Append missing player variables
        fwrite(&playerDamage, sizeof(int), 1, f);
        fwrite(&g_BonusSpeed, sizeof(float), 1, f);
        fwrite(&g_PendingUpgrades, sizeof(int), 1, f);
        
        // Append missing boss state variables
        fwrite(&bossDead, sizeof(bool), 1, f);
        fwrite(&preBossTimer, sizeof(float), 1, f);
        fwrite(&bossEventTimer, sizeof(float), 1, f);
        fwrite(&fireballSpawnTimer, sizeof(float), 1, f);
        fwrite(&bossHurtTimer, sizeof(float), 1, f);
        fwrite(&bossSpawnTimer, sizeof(float), 1, f);
        fwrite(&postBossPhase, sizeof(bool), 1, f);
        fwrite(&phase2BossFrame, sizeof(int), 1, f);
        fwrite(&phase2BossAnimTimer, sizeof(float), 1, f);
        
        int allyCount = allies.size();
        fwrite(&allyCount, sizeof(int), 1, f);
        for (auto& e : allies) {
            fwrite(&e.x, sizeof(float), 1, f);
            fwrite(&e.y, sizeof(float), 1, f);
            fwrite(&e.active, sizeof(bool), 1, f);
            fwrite(&e.spriteIndex, sizeof(int), 1, f);
            fwrite(&e.health, sizeof(int), 1, f);
            fwrite(&e.maxHealth, sizeof(int), 1, f);
            fwrite(&e.isShooter, sizeof(bool), 1, f);
            fwrite(&e.isMarshall, sizeof(bool), 1, f);
            fwrite(&e.state, sizeof(int), 1, f);
            fwrite(&e.tacticState, sizeof(int), 1, f);
            fwrite(&e.flankDir, sizeof(int), 1, f);
            fwrite(&e.brain, sizeof(NeuralAI::NeuralNet), 1, f);
            fwrite(&e.hasNeuralBrain, sizeof(bool), 1, f);
            fwrite(&e.dodgeDir, sizeof(int), 1, f);
            fwrite(&e.isPhalanx, sizeof(bool), 1, f);
            fwrite(&e.isSpearGuy, sizeof(bool), 1, f);
            fwrite(&e.spearState, sizeof(int), 1, f);
            fwrite(&e.isOfficer, sizeof(bool), 1, f);
            fwrite(&e.isDefectedOfficer, sizeof(bool), 1, f);
            fwrite(&e.isDefectedGunner, sizeof(bool), 1, f);
            fwrite(&e.officerState, sizeof(int), 1, f);
            fwrite(&e.speed, sizeof(float), 1, f);
            fwrite(&e.isParagon, sizeof(bool), 1, f);
            fwrite(&e.targetX, sizeof(float), 1, f);
            fwrite(&e.targetY, sizeof(float), 1, f);
            fwrite(&e.hunting, sizeof(bool), 1, f);
            fwrite(&e.targetEnemyIndex, sizeof(int), 1, f);
            fwrite(&e.targetClawIndex, sizeof(int), 1, f);
        }
        
    } catch (...) {
        // Handle unexpected errors during save
    }

    fclose(f);
}

bool LoadGame() {
    FILE* f = _wfopen(L"configs/savegame.dat", L"rb");
    if (!f) return false;
    
    try {
        fread(&player, sizeof(Player), 1, f);
        fread(&score, sizeof(int), 1, f);
        fread(&phase2Active, sizeof(bool), 1, f);
        fread(&forceFieldActive, sizeof(bool), 1, f);
        fread(&enragedMode, sizeof(bool), 1, f);
        fread(&hordeActive, sizeof(bool), 1, f);
        fread(&bossActive, sizeof(bool), 1, f);
        fread(&preBossPhase, sizeof(bool), 1, f);
        fread(&bossHealth, sizeof(int), 1, f);
        
        fread(&currentWeapon, sizeof(int), 1, f);
        fread(&ammo, sizeof(int), 1, f);
        fread(weaponAmmo, sizeof(int), 3, f);
        fread(&gunUpgraded, sizeof(bool), 1, f);
        fread(&bazookaUnlocked, sizeof(bool), 1, f);
        
        int enemyCount = 0;
        fread(&enemyCount, sizeof(int), 1, f);
        enemies.clear();
        for (int i = 0; i < enemyCount; i++) {
            Enemy e;
            fread(&e.x, sizeof(float), 1, f);
            fread(&e.y, sizeof(float), 1, f);
            fread(&e.active, sizeof(bool), 1, f);
            fread(&e.spriteIndex, sizeof(int), 1, f);
            fread(&e.health, sizeof(int), 1, f);
            fread(&e.maxHealth, sizeof(int), 1, f);
            fread(&e.isShooter, sizeof(bool), 1, f);
            fread(&e.isMarshall, sizeof(bool), 1, f);
            fread(&e.state, sizeof(int), 1, f);
            fread(&e.tacticState, sizeof(int), 1, f);
            fread(&e.flankDir, sizeof(int), 1, f);
            fread(&e.brain, sizeof(NeuralAI::NeuralNet), 1, f);
            fread(&e.hasNeuralBrain, sizeof(bool), 1, f);
            fread(&e.dodgeDir, sizeof(int), 1, f);
            fread(&e.isPhalanx, sizeof(bool), 1, f);
            fread(&e.isSpearGuy, sizeof(bool), 1, f);
            fread(&e.spearState, sizeof(int), 1, f);
            fread(&e.isOfficer, sizeof(bool), 1, f);
            fread(&e.isDefectedOfficer, sizeof(bool), 1, f);
            fread(&e.isDefectedGunner, sizeof(bool), 1, f);
            fread(&e.officerState, sizeof(int), 1, f);
            fread(&e.speed, sizeof(float), 1, f);
            enemies.push_back(e);
        }
        
        for (int i = 0; i < 6; i++) {
            fread(&claws[i], sizeof(Claw), 1, f);
        }
        
        // Read appended player variables (with backward compatibility)
        if (fread(&playerDamage, sizeof(int), 1, f) != 1) playerDamage = 1;
        if (fread(&g_BonusSpeed, sizeof(float), 1, f) != 1) g_BonusSpeed = 0.0f;
        if (fread(&g_PendingUpgrades, sizeof(int), 1, f) != 1) g_PendingUpgrades = 0;
        
        // Read appended boss state variables
        if (fread(&bossDead, sizeof(bool), 1, f) != 1) bossDead = false;
        if (fread(&preBossTimer, sizeof(float), 1, f) != 1) preBossTimer = 0.0f;
        if (fread(&bossEventTimer, sizeof(float), 1, f) != 1) bossEventTimer = 0.0f;
        if (fread(&fireballSpawnTimer, sizeof(float), 1, f) != 1) fireballSpawnTimer = 0.0f;
        if (fread(&bossHurtTimer, sizeof(float), 1, f) != 1) bossHurtTimer = 0.0f;
        if (fread(&bossSpawnTimer, sizeof(float), 1, f) != 1) bossSpawnTimer = 0.0f;
        if (fread(&postBossPhase, sizeof(bool), 1, f) != 1) postBossPhase = false;
        if (fread(&phase2BossFrame, sizeof(int), 1, f) != 1) phase2BossFrame = 0;
        if (fread(&phase2BossAnimTimer, sizeof(float), 1, f) != 1) phase2BossAnimTimer = 0.0f;
        
        int allyCount = 0;
        if (fread(&allyCount, sizeof(int), 1, f) == 1) {
            allies.clear();
            for (int i = 0; i < allyCount; i++) {
                Enemy e;
                fread(&e.x, sizeof(float), 1, f);
                fread(&e.y, sizeof(float), 1, f);
                fread(&e.active, sizeof(bool), 1, f);
                fread(&e.spriteIndex, sizeof(int), 1, f);
                fread(&e.health, sizeof(int), 1, f);
                fread(&e.maxHealth, sizeof(int), 1, f);
                fread(&e.isShooter, sizeof(bool), 1, f);
                fread(&e.isMarshall, sizeof(bool), 1, f);
                fread(&e.state, sizeof(int), 1, f);
                fread(&e.tacticState, sizeof(int), 1, f);
                fread(&e.flankDir, sizeof(int), 1, f);
                fread(&e.brain, sizeof(NeuralAI::NeuralNet), 1, f);
                fread(&e.hasNeuralBrain, sizeof(bool), 1, f);
                fread(&e.dodgeDir, sizeof(int), 1, f);
                fread(&e.isPhalanx, sizeof(bool), 1, f);
                fread(&e.isSpearGuy, sizeof(bool), 1, f);
                fread(&e.spearState, sizeof(int), 1, f);
                fread(&e.isOfficer, sizeof(bool), 1, f);
                fread(&e.isDefectedOfficer, sizeof(bool), 1, f);
                fread(&e.isDefectedGunner, sizeof(bool), 1, f);
                fread(&e.officerState, sizeof(int), 1, f);
                fread(&e.speed, sizeof(float), 1, f);
                fread(&e.isParagon, sizeof(bool), 1, f);
                fread(&e.targetX, sizeof(float), 1, f);
                fread(&e.targetY, sizeof(float), 1, f);
                fread(&e.hunting, sizeof(bool), 1, f);
                fread(&e.targetEnemyIndex, sizeof(int), 1, f);
                fread(&e.targetClawIndex, sizeof(int), 1, f);
                allies.push_back(e);
            }
        }
        
    } catch (...) {
        fclose(f);
        return false;
    }
    
    fclose(f);
    return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HDC screenDC = GetDC(hwnd);
            backBufferDC = CreateCompatibleDC(screenDC);
            
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = SCREEN_WIDTH;
            bmi.bmiHeader.biHeight = -SCREEN_HEIGHT;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            
            backBufferDIB = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, 
                (void**)&backBufferPixels, NULL, 0);
            SelectObject(backBufferDC, backBufferDIB);
            
            g_renderDC = CreateCompatibleDC(screenDC);
            g_renderBitmap = CreateCompatibleBitmap(screenDC, SCREEN_WIDTH, SCREEN_HEIGHT);
            g_renderOldBitmap = (HBITMAP)SelectObject(g_renderDC, g_renderBitmap);
            
            ReleaseDC(hwnd, screenDC);
            
            zBuffer = new float[SCREEN_WIDTH * SCREEN_HEIGHT];
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RenderGame(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CHAR: {
            if (consoleActive) {
                if (wParam == VK_BACK) {
                    if (consoleBuffer.length() > 0) consoleBuffer.pop_back();
                } else if (wParam == VK_RETURN) {
                    if (consoleBuffer == L"exit") {
                        consoleActive = false;
                        consoleBuffer = L"";
                    } else if (consoleBuffer.find(L"score") == 0) {
                        // Parse "score = 100" or "score 100"
                        size_t eqPos = consoleBuffer.find(L'=');
                        if (eqPos != std::wstring::npos) {
                            std::wstring numStr = consoleBuffer.substr(eqPos + 1);
                            score = _wtoi(numStr.c_str());
                        } else {
                            // Try "score 100" format
                            size_t spacePos = consoleBuffer.find(L' ');
                            if (spacePos != std::wstring::npos) {
                                std::wstring numStr = consoleBuffer.substr(spacePos + 1);
                                score = _wtoi(numStr.c_str());
                            }
                        }
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"stat on") {
                        showStats = true;
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"stat off") {
                        showStats = false;
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"reset cam") {
                        gunSwayX = 0; gunSwayY = 0;
                        consoleBuffer = L"";
                    } else if (consoleBuffer.find(L"view-range") == 0) {
                        if (consoleBuffer.find(L" on") != std::wstring::npos) viewRange = true;
                        else if (consoleBuffer.find(L" off") != std::wstring::npos) viewRange = false;
                        else viewRange = !viewRange;
                        consoleBuffer = L"";
                    } else if (consoleBuffer.find(L"player.dmg") == 0) {
                        size_t eqPos = consoleBuffer.find(L'=');
                        if (eqPos != std::wstring::npos) {
                            std::wstring numStr = consoleBuffer.substr(eqPos + 1);
                            playerDamage = _wtoi(numStr.c_str());
                        } else {
                            size_t spacePos = consoleBuffer.find(L' ');
                             if (spacePos != std::wstring::npos) {
                                std::wstring numStr = consoleBuffer.substr(spacePos + 1);
                                playerDamage = _wtoi(numStr.c_str());
                            }
                        }
                        if (playerDamage < 1) playerDamage = 1;
                        consoleBuffer = L"";
                    } else if (consoleBuffer.find(L"player.gmode") == 0) {
                        if (consoleBuffer.find(L"true") != std::wstring::npos) godMode = true;
                        else if (consoleBuffer.find(L"false") != std::wstring::npos) godMode = false;
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"spec on") {
                        if (!spectatorMode) {
                            spectatorMode = true;
                            spectatorX = player.x;
                            spectatorY = player.y;
                            spectatorAngle = player.angle;
                            spectatorPitch = player.pitch;
                            savedPlayerX = player.x;
                            savedPlayerY = player.y;
                            savedPlayerAngle = player.angle;
                        }
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"spec off") {
                        if (spectatorMode) {
                            spectatorMode = false;
                            player.x = savedPlayerX;
                            player.y = savedPlayerY;
                            player.angle = savedPlayerAngle;
                            // Pitch is shared or reset? Let's keep current pitch or reset
                        }
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"skip") {
                        bossDead = true;
                        bossActive = false; TriggerEvent("boss_active", false);
                        preBossPhase = false; TriggerEvent("pre_boss_phase", false);
                        phase2Active = false;
                        forceFieldActive = false;
                        activeLaserClaw = -1;
                        postBossPhase = true;
                        enemies.clear();
                        pendingEnemies.clear();
                        
                        wchar_t exePath[MAX_PATH];
                        GetModuleFileNameW(NULL, exePath, MAX_PATH);
                        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
                        if (lastSlash) *lastSlash = L'\0';
                        wchar_t dialoguePath[MAX_PATH];
                        swprintf(dialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\leader.line", exePath);
                        
                        NPCSystem::ClearNPCs();
                        NPCSystem::SpawnNPC(32.0f, 28.0f, L"Leader", leaderIdlePixels, leaderIdleW, leaderIdleH, leaderTalkingPixels, leaderTalkingW, leaderTalkingH, dialoguePath);
                        
                        wchar_t followerDialoguePath[MAX_PATH];
                        swprintf(followerDialoguePath, MAX_PATH, L"%ls\\assets\\dialogues\\followers.line", exePath);
                        NPCSystem::SpawnNPC(29.0f, 28.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        NPCSystem::SpawnNPC(35.0f, 28.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        NPCSystem::SpawnNPC(27.0f, 30.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        NPCSystem::SpawnNPC(37.0f, 30.0f, L"Follower", followerPixels, followerW, followerH, followerPixels, followerW, followerH, followerDialoguePath);
                        
                        wchar_t victoryMusicPath[MAX_PATH];
                        swprintf(victoryMusicPath, MAX_PATH, L"open \"%ls\\assets\\sound-effects\\victory.mp3\" type mpegvideo alias victory", exePath);
                        mciSendStringW(victoryMusicPath, NULL, 0, NULL);
                        mciSendStringW(L"play victory repeat", NULL, 0, NULL);
                        
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"unlockall") {
                        gunUpgraded = true;
                        bazookaUnlocked = true;
                        // Give ammo too?
                        weaponMaxAmmo[0] = 999; weaponAmmo[0] = 999;
                        weaponMaxAmmo[1] = 999; weaponAmmo[1] = 999;
                        weaponMaxAmmo[2] = 999; weaponAmmo[2] = 999;
                        ammo = 999;
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"spawn gravital") {
                        Gravital g;
                        g.x = player.x;
                        g.y = player.y;
                        g.z = 0.75f; 
                        g.targetX = player.x; 
                        g.targetY = player.y;
                        g.active = true;
                        g.health = 30;
                        g.hurtTimer = 0;
                        g.state = GRAVITAL_CHASE; // Start in chase or idle?
                        g.slamTimer = 0;
                        g.animTimer = 0;
                        g.animFrame = 0;
                        gravitals.push_back(g);
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"spawn officer") {
                        TriggerEvent("marshall_spawned", true);
                        defectedRespawnTimer = 0.0f; // Force immediate spawn
                    } else if (consoleBuffer == L"spawn john") {
                        johnSecret.isSpawned = true;
                        johnSecret.naturalSpawn = false;
                        johnSecret.x = healingTower.x + 2.0f;
                        johnSecret.y = healingTower.y;
                        johnSecret.angle = 0.0f;
                        consoleBuffer = L"";
                    } else if (consoleBuffer == L"help") {
                        wcscpy(consoleError, L"Cmds: score=N, stat on/off, reset cam, view-range on/off, player.dmg=N, player.gmode true/false, spec on/off, skip, unlockall, spawn gravital, spawn officer, spawn john, exit");
                        consoleBuffer = L"";
                    } else {
                        wcscpy(consoleError, L"Unknown command");
                        consoleBuffer = L"";
                    }
                } else {
                    consoleError[0] = L'\0';
                    if (wParam != L'`' && wParam != L'~') {
                        consoleBuffer += (wchar_t)wParam;
                    }
                }
            }
            return 0;
        }
        case WM_KEYDOWN:
            if (wParam == VK_OEM_3 && g_DevConsole) {
                consoleActive = !consoleActive;
                return 0;
            }
            if (consoleActive) return 0; // Block game input
            
            if (victoryScreen) return 0;
            keys[wParam & 0xFF] = true;
            if (wParam == VK_ESCAPE) g_PauseMenuOpen = !g_PauseMenuOpen;
            if (gunUpgraded && !consoleActive) {
                int nextWeapon = currentWeapon;
                if (wParam == '1' && !dialogueController.IsShowingOptions()) nextWeapon = 0;
                else if (wParam == '2' && !dialogueController.IsShowingOptions()) nextWeapon = 1;
                else if (wParam == '3' && bazookaUnlocked) nextWeapon = 2;
                
                if (nextWeapon != currentWeapon && !dialogueController.IsShowingOptions()) {
                    weaponAmmo[currentWeapon] = ammo;
                    currentWeapon = nextWeapon;
                    ammo = weaponAmmo[currentWeapon];
                    maxAmmo = weaponMaxAmmo[currentWeapon];
                    isReloading = false;
                    reloadTimer = 0;
                    gunReloadOffset = 0;
                }
            }
            
            if (wParam == 'E' && postBossPhase && !consoleActive) {
                if (!dialogueController.IsActive()) {
                    NPCSystem::NPC* nearNPC = NPCSystem::GetNearestInteractableNPC(player.x, player.y, 3.0f);
                    if (nearNPC && !nearNPC->dialoguePath.empty()) {
                        currentTalkingNPC = nearNPC;
                        nearNPC->isTalking = true;
                        if (dialogueController.LoadFromLine(nearNPC->dialoguePath.c_str())) {
                            if (nearNPC->name == L"Follower") {
                                dialogueController.StartRandom();
                            } else {
                                dialogueController.Start();
                            }
                        }
                    }
                } else if (!dialogueController.IsShowingOptions()) {
                    dialogueController.AdvanceLine();
                    if (!dialogueController.IsActive()) {
                        if (currentTalkingNPC) currentTalkingNPC->isTalking = false;
                        currentTalkingNPC = nullptr;
                    }
                }
            }
            
            if (wParam == 'F' && !consoleActive && !victoryScreen) {
                if (paragonsUnlocked && GetAliveParagonCount() < 8 && paragonSummonCooldown <= 0) {
                    Enemy p;
            p.isParagon = true;
            p.isAllied = true;
            p.isEnemy = false;
                    p.x = player.x;
                    p.y = player.y;
                    p.speed = 4.5f;
                    p.health = 10;
                    p.active = true;
                    p.hurtTimer = 0;
                    p.targetX = 0; p.targetY = 0;
                    p.hunting = false;
                    p.targetEnemyIndex = -1;
                    p.targetClawIndex = -1;
                    p.isAllied = true;
                    pendingAllies.push_back(p);
                    paragonSummonCooldown = 3.0f;
                }
            }
            if (wParam == 'E' && gateDialogueActive && !consoleActive) {
                if (playerDialogueController.IsActive() && !playerDialogueController.IsShowingOptions()) {
                    playerDialogueController.AdvanceLine();
                }
            }
            
            if (dialogueController.IsShowingOptions() && !consoleActive) {
                if (wParam == VK_UP || wParam == VK_LEFT || wParam == 'W' || wParam == 'A') {
                    dialogueController.MoveSelectionLeft();
                } else if (wParam == VK_DOWN || wParam == VK_RIGHT || wParam == 'S' || wParam == 'D') {
                    dialogueController.MoveSelectionRight();
                } else if (wParam == VK_RETURN || wParam == VK_SPACE) {
                    int selectedIdx = dialogueController.GetSelectedOptionIndex();
                    dialogueController.ConfirmSelection();
                    if (dialogueController.IsFinished()) {
                        if (selectedIdx == 0) {
                            whiteFadeToVictory = true;
                            whiteFadeTimer = 2.0f;
                        }
                        if (currentTalkingNPC) currentTalkingNPC->isTalking = false;
                        currentTalkingNPC = nullptr;
                    }
                }
            }
            return 0;
        case WM_KEYUP:
            if (victoryScreen) return 0;
            keys[wParam & 0xFF] = false;
            return 0;
        case WM_LBUTTONDOWN: {
            if (g_PauseMenuOpen) {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);
                if (mx >= SCREEN_WIDTH/2 - 100 && mx <= SCREEN_WIDTH/2 + 100) {
                    if (my >= SCREEN_HEIGHT/2 - 20 && my <= SCREEN_HEIGHT/2 + 20) {
                        PostQuitMessage(0); // Quit
                    } else if (my >= SCREEN_HEIGHT/2 + 40 && my <= SCREEN_HEIGHT/2 + 80) {
                        SaveGame();
                        PostQuitMessage(0); // Save and Exit
                    }
                }
                return 0;
            }
            if (consoleActive) return 0;
            if (g_LevelUpWindowOpen) {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);
                
                int boxW = 150;
                int boxH = 60;
                int gap = 20;
                int totalW = 3 * boxW + 2 * gap;
                int startX = SCREEN_WIDTH / 2 - totalW / 2;
                int y = SCREEN_HEIGHT / 2 - boxH / 2;
                
                RECT speedBtn = {startX, y, startX + boxW, y + boxH};
                RECT healthBtn = {startX + boxW + gap, y, startX + 2*boxW + gap, y + boxH};
                RECT damageBtn = {startX + 2*boxW + 2*gap, y, startX + 3*boxW + 2*gap, y + boxH};
                
                bool clicked = false;
                if (mx >= speedBtn.left && mx <= speedBtn.right && my >= speedBtn.top && my <= speedBtn.bottom) {
                    g_BonusSpeed += 1.0f;
                    clicked = true;
                } else if (mx >= healthBtn.left && mx <= healthBtn.right && my >= healthBtn.top && my <= healthBtn.bottom) {
                    player.maxHealth += 1;
                    player.health += 1;
                    clicked = true;
                } else if (mx >= damageBtn.left && mx <= damageBtn.right && my >= damageBtn.top && my <= damageBtn.bottom) {
                    extern int playerDamage;
                    playerDamage += 1;
                    clicked = true;
                }
                
                if (clicked) {
                    g_PendingUpgrades--;
                    if (g_PendingUpgrades <= 0) {
                        g_LevelUpWindowOpen = false;
                        POINT center = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
                        ClientToScreen(hwnd, &center);
                        SetCursorPos(center.x, center.y);
                    }
                }
                return 0;
            }
            if (victoryScreen) {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);
                RECT playAgainBtn = {SCREEN_WIDTH/2 - 120, 380, SCREEN_WIDTH/2 + 120, 430};
                RECT exitBtn = {SCREEN_WIDTH/2 - 120, 450, SCREEN_WIDTH/2 + 120, 500};
                
                if (mx >= playAgainBtn.left && mx <= playAgainBtn.right && my >= playAgainBtn.top && my <= playAgainBtn.bottom) {
                    victoryScreen = false;
                    bossDead = false;
                    bossActive = false; TriggerEvent("boss_active", false);
                    preBossPhase = false; TriggerEvent("pre_boss_phase", false);
                    bossHealth = 200;
                    phase2Active = false;
                    enragedMode = false;
                    // score retained on continue
                    player.health = player.maxHealth;
                    player.level = 1; player.xp = 0; player.xpToNextLevel = 100; player.maxHealth = 100; if (johnSecret.isSpawned && johnSecret.naturalSpawn) { player.maxHealth *= 2; player.health = player.maxHealth; } g_BonusSpeed = 0.0f; g_PendingUpgrades = 0; g_LevelUpWindowOpen = false; player.x = 10.0f;
                    player.y = 32.0f;
                    player.angle = 0.0f;
                    // Reset weapon ammo to defaults
                    weaponAmmo[0] = 8; weaponAmmo[1] = 5; weaponAmmo[2] = 4;
                    weaponMaxAmmo[0] = 8; weaponMaxAmmo[1] = 5; weaponMaxAmmo[2] = 4;
                    currentWeapon = 0;
                    ammo = weaponAmmo[0];
                    maxAmmo = weaponMaxAmmo[0];
                    enemies.clear();
                    fireballs.clear();
                    bullets.clear();
                    SpawnEnemies();
                    SpawnMedkit();
                    InitClaws();
                    musicRunning = true;
                    _beginthread(BackgroundMusic, 0, NULL);
                }
                
                if (mx >= exitBtn.left && mx <= exitBtn.right && my >= exitBtn.top && my <= exitBtn.bottom) {
                    PostQuitMessage(0);
                }
            } else {
                ShootBullet();
            }
            return 0;
        }
        case WM_RBUTTONDOWN: {
            // Right click logic (if any)
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (consoleActive || victoryScreen || g_LevelUpWindowOpen || g_PauseMenuOpen) return 0;
            
            static int lastMouseX = SCREEN_WIDTH / 2;
            int mx = LOWORD(lParam);
            int deltaX = mx - lastMouseX;
            
            float sensitivity = 0.003f * g_MouseSensitivity;
            if (spectatorMode) {
                spectatorAngle += deltaX * sensitivity;
                // Keep player.angle synced for immediate feedback if needed, 
                // though UpdatePlayer also syncs it. Doing it here prevents 'lag' frame.
                player.angle = spectatorAngle; 
            } else {
                player.angle += deltaX * sensitivity;
            }
            
            POINT center = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
            ClientToScreen(hwnd, &center);
            SetCursorPos(center.x, center.y);
            lastMouseX = SCREEN_WIDTH / 2;
            
            return 0;
        }
        case WM_DESTROY:
            musicRunning = false;
            CleanupAudio();
            KillTimer(hwnd, 1);
            if (backBufferDIB) DeleteObject(backBufferDIB);
            if (backBufferDC) DeleteDC(backBufferDC);
            DialogueSystem::CleanupDialogueAssets();
            delete[] zBuffer;
            if (grassPixels) delete[] grassPixels;
            for(int i=0; i<5; i++) if (enemyPixels[i]) delete[] enemyPixels[i];
            if (enemy5HurtPixels) delete[] enemy5HurtPixels;
            
            for(int i=0; i<3; i++) if(spirePhase2Pixels[i] && spirePhase2Pixels[i] != spirePixels) delete[] spirePhase2Pixels[i];
            for(int i=0; i<4; i++) if(clawPhase2Pixels[i] && clawPhase2Pixels[i] != clawDormantPixels) delete[] clawPhase2Pixels[i];
            if(clawHurtPixels) delete[] clawHurtPixels;
            if (treePixels) delete[] treePixels;
            if (cloudPixels) delete[] cloudPixels;
            if (gunPixels) delete[] gunPixels;
            if (gunfirePixels) delete[] gunfirePixels;
            if (bulletPixels) delete[] bulletPixels;
            if (medkitPixels) delete[] medkitPixels;
            if (clawDormantPixels) delete[] clawDormantPixels;
            if (clawActivePixels) delete[] clawActivePixels;
            if (gunnerPixels) delete[] gunnerPixels;
            if (gunnerFiringPixels) delete[] gunnerFiringPixels;
            if (officerMovePixels) delete[] officerMovePixels;
            if (officerIdlePixels) delete[] officerIdlePixels;
            if (officerHurtPixels) delete[] officerHurtPixels;
            if (officerFirePixels) delete[] officerFirePixels;
            if (defectedMovingPixels) delete[] defectedMovingPixels;
            if (defectedIdlePixels) delete[] defectedIdlePixels;
            if (defectedFiringPixels) delete[] defectedFiringPixels;
            if (defectedGunnerPixels) delete[] defectedGunnerPixels;
            if (defectedGunnerFiringPixels) delete[] defectedGunnerFiringPixels;
            for (int i = 0; i < 11; i++) if (healthbarPixels[i]) delete[] healthbarPixels[i];
            CleanupThreadPool();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LONG WINAPI CrashHandler(EXCEPTION_POINTERS* pExceptionInfo) {
    wchar_t crashMsg[512];
    swprintf(crashMsg, 512, L"LoneShooter crashed!\n\nException Code: 0x%08X\nAddress: 0x%p\n\nThe game will now close.",
        pExceptionInfo->ExceptionRecord->ExceptionCode,
        pExceptionInfo->ExceptionRecord->ExceptionAddress);
    MessageBoxW(NULL, crashMsg, L"LoneShooter - Crash", MB_OK | MB_ICONERROR);
    
    wchar_t logPath[MAX_PATH];
    GetModuleFileNameW(NULL, logPath, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(logPath, L'\\');
    if (!lastSlash) lastSlash = wcsrchr(logPath, L'/');
    if (lastSlash) *lastSlash = L'\0';
    wcscat(logPath, L"\\crash.log");
    
    FILE* f = _wfopen(logPath, L"a");
    if (f) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fwprintf(f, L"[%04d-%02d-%02d %02d:%02d:%02d] Exception 0x%08X at 0x%p\n",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
            pExceptionInfo->ExceptionRecord->ExceptionCode,
            pExceptionInfo->ExceptionRecord->ExceptionAddress);
        fclose(f);
    }
    
    return EXCEPTION_EXECUTE_HANDLER;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SetUnhandledExceptionFilter(CrashHandler);
    (void)hPrevInstance; (void)lpCmdLine;
    
    if (!ShowSettingsMenu(hInstance)) {
        return 0;
    }
    
    if (g_FullscreenMode) {
        SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
        SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
    }
    
    if (g_FullscreenMode) {
        SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
        SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
    }
    
    LoadHighScore();
    LoadGraves();
    InitTrigTables();
    InitGraphics();
    TryLoadAssets();
    GenerateWorld();
    Pathfinder::Init(worldMap, IsPositionColliding);
    SpawnEnemies();
    SpawnMedkit();
    InitClaws();
    InitThreadPool();
    
    if (g_LoadGameRequested) {
        LoadGame();
    }
    
    enemies.reserve(64);
    bullets.reserve(32);
    fireballs.reserve(32);
    enemyBullets.reserve(64);
    allies.reserve(16);
    g_allSprites.reserve(2048);
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"LoneShooterClass";
    RegisterClassExW(&wc);
    
    InitAudio();
    
    if (g_FullscreenMode) {
        hMainWnd = CreateWindowExW(WS_EX_TOPMOST, L"LoneShooterClass", L"Lone Shooter",
            WS_POPUP,
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            NULL, NULL, hInstance, NULL);
    } else {
        RECT windowRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);
        
        hMainWnd = CreateWindowExW(0, L"LoneShooterClass", L"Lone Shooter",
            (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX),
            CW_USEDEFAULT, CW_USEDEFAULT, 
            windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
            NULL, NULL, hInstance, NULL);
    }
    
    InitOpenGL(hMainWnd);
    
    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);
    ShowCursor(FALSE);
    
    if (assetsFolderMissing) {
        MessageBoxW(hMainWnd, L"CRITICAL ERROR: Assets folder is missing or empty!\n\nThe game cannot start without assets.\nPlease ensure the 'assets' folder exists and contains the required files.", L"LoneShooter - Asset Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    _beginthread(BackgroundMusic, 0, NULL);
    
    LARGE_INTEGER perfFreq, prevCount, currentCount;
    QueryPerformanceFrequency(&perfFreq);
    QueryPerformanceCounter(&prevCount);
    double targetFrameTime = 1.0 / 75.0; // 75 FPS Cap
    timeBeginPeriod(1); // High resolution sleep

    MSG msg;
    static double timeAccum = 0;

    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                timeEndPeriod(1);
                CleanupGraphics();
                return (int)msg.wParam;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        QueryPerformanceCounter(&currentCount);
        double elapsed = (double)(currentCount.QuadPart - prevCount.QuadPart) / (double)perfFreq.QuadPart;
        
        if (elapsed < targetFrameTime) {
             DWORD sleepMs = (DWORD)((targetFrameTime - elapsed) * 1000.0);
             if (sleepMs > 0) Sleep(sleepMs);
             continue; 
        }
        prevCount = currentCount;
        
        if (elapsed > 0.1) elapsed = 0.1; // Cap delta time
        float deltaTime = (float)elapsed;
        
        if (!spectatorMode) {
            ManageAlliedOfficer(deltaTime);
            if (scoreTimer > 0) scoreTimer -= deltaTime;
            if (screenShakeTimer > 0) screenShakeTimer -= deltaTime;
            if (errorTimer > 0) errorTimer -= deltaTime;
            if (hordeMessageTimer > 0) hordeMessageTimer -= deltaTime;
            if (upgradeMessageTimer > 0) upgradeMessageTimer -= deltaTime;
            
            if (whiteFadeToVictory && whiteFadeTimer > 0) {
                whiteFadeTimer -= deltaTime;
                if (whiteFadeTimer <= 0) {
                    whiteFadeToVictory = false;
                    postBossPhase = false;
                    victoryScreen = true;
                    NPCSystem::ClearNPCs();
                }
            }
        }
        
        fpsCounter++;
        timeAccum += deltaTime;
        if (timeAccum >= 1.0) {
            currentFPS = fpsCounter;
            fpsCounter = 0;
            timeAccum = 0;
        }
        
        if (!g_LevelUpWindowOpen && !g_PauseMenuOpen) {
            UpdatePlayer(deltaTime);
            UpdateHealingTower(deltaTime);
            UpdateJohn(deltaTime);
            
            if (!spectatorMode) {
                UpdateEnemies(deltaTime);
                UpdateClouds(deltaTime);
                UpdateGun(deltaTime);
                UpdateBullets(deltaTime);
                UpdateAllies(deltaTime);
            }
        }
        
        HDC hdc = GetDC(hMainWnd);
        RenderGame(hdc);
        ReleaseDC(hMainWnd, hdc);
        g_ui.mouseReleased = false;
    }
    return 0;
}

