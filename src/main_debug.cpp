// Dynamic Island - Debug Version
// 修复窗口可见性问题

#include <windows.h>
#include <shellapi.h>
#include <objbase.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "advapi32.lib")

// Global variables
HWND g_hwnd = nullptr;
bool g_use24Hour = true;
bool g_isHovering = false;
int g_width = 200;
int g_height = 60;
const int NORMAL_WIDTH = 200;
const int NORMAL_HEIGHT = 60;
const int EXPANDED_WIDTH = 240;
const int EXPANDED_HEIGHT = 72;

// Forward declarations
void RenderWindow();
void UpdateWindowSize(int width, int height);
std::wstring GetTimeString();
void ToggleTimeFormat();
void OpenTimeSettings();
COLORREF GetBackgroundColor();
COLORREF GetTextColor();

// Check system theme
bool IsLightTheme() {
    DWORD value = 1;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, 
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(value);
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, 
                        reinterpret_cast<LPBYTE>(&value), &size);
        RegCloseKey(hKey);
    }
    return value == 1;
}

// Get time string
std::wstring GetTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm tm;
    localtime_s(&tm, &time);
    
    std::wstringstream wss;
    int hour = g_use24Hour ? tm.tm_hour : (tm.tm_hour % 12);
    if (hour == 0 && !g_use24Hour) hour = 12;
    
    wss << std::setfill(L'0') 
        << std::setw(2) << hour << L":"
        << std::setw(2) << tm.tm_min << L":"
        << std::setw(2) << tm.tm_sec;
    
    return wss.str();
}

// Toggle time format
void ToggleTimeFormat() {
    g_use24Hour = !g_use24Hour;
    InvalidateRect(g_hwnd, NULL, TRUE);
}

// Open time settings
void OpenTimeSettings() {
    ShellExecuteW(NULL, L"open", L"control.exe", L"timedate.cpl", NULL, SW_SHOW);
}

// Get background color
COLORREF GetBackgroundColor() {
    return IsLightTheme() ? RGB(255, 255, 255) : RGB(0, 0, 0);
}

// Get text color
COLORREF GetTextColor() {
    return IsLightTheme() ? RGB(0, 0, 0) : RGB(255, 255, 255);
}

// Update window size
void UpdateWindowSize(int width, int height) {
    g_width = width;
    g_height = height;
    
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int x = workArea.left + (workArea.right - workArea.left - g_width) / 2;
    int y = workArea.top + 10;
    
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, g_width, g_height, 
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

// Render window
void RenderWindow() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(g_hwnd, &ps);
    
    if (hdc == NULL) {
        return;
    }
    
    // Get client area
    RECT clientRect;
    GetClientRect(g_hwnd, &clientRect);
    
    // Create memory DC for double buffering
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    // Clear background
    HBRUSH bgBrush = CreateSolidBrush(GetBackgroundColor());
    FillRect(memDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Draw time text
    std::wstring timeStr = GetTimeString();
    SetTextColor(memDC, GetTextColor());
    SetBkMode(memDC, TRANSPARENT);
    
    // Create font
    HFONT font = CreateFontW(
        g_height / 2, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
    
    HFONT oldFont = (HFONT)SelectObject(memDC, font);
    
    // Draw centered text
    DrawTextW(memDC, timeStr.c_str(), -1, &clientRect, 
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // Copy from memory DC to screen DC
    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
    
    // Cleanup
    SelectObject(memDC, oldFont);
    DeleteObject(font);
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    
    EndPaint(g_hwnd, &ps);
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
        RenderWindow();
        return 0;
    
    case WM_ERASEBKGND:
        return TRUE;
    
    case WM_MOUSEMOVE: {
        if (!g_isHovering) {
            g_isHovering = true;
            UpdateWindowSize(EXPANDED_WIDTH, EXPANDED_HEIGHT);
            
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }
        return 0;
    }
    
    case WM_MOUSELEAVE: {
        g_isHovering = false;
        UpdateWindowSize(NORMAL_WIDTH, NORMAL_HEIGHT);
        return 0;
    }
    
    case WM_LBUTTONDOWN: {
        OpenTimeSettings();
        return 0;
    }
    
    case WM_RBUTTONDOWN: {
        ToggleTimeFormat();
        return 0;
    }
    
    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
        break;
    }
    
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Set auto startup
void SetAutoStartup() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, 
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::wstring fullPath = L"\"";
        fullPath += path;
        fullPath += L"\"";
        
        RegSetValueExW(hKey, L"DynamicIsland", 0, REG_SZ,
                      reinterpret_cast<const BYTE*>(fullPath.c_str()),
                      (DWORD)((fullPath.length() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}

// Main function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {
    // Prevent multiple instances
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"DynamicIsland_Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(NULL, L"Dynamic Island is already running!", L"Info", MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    
    // Initialize COM
    CoInitialize(NULL);
    
    // Set auto startup
    SetAutoStartup();
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"DynamicIslandClass";
    wc.hIcon = NULL;
    wc.hIconSm = NULL;
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Failed to register window class!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Calculate window position
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int x = workArea.left + (workArea.right - workArea.left - NORMAL_WIDTH) / 2;
    int y = workArea.top + 10;
    
    // Create window with visible style
    g_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,  // Removed LAYERED and TRANSPARENT for now
        L"DynamicIslandClass",
        L"DynamicIsland",
        WS_POPUP | WS_VISIBLE,  // Added WS_VISIBLE
        x, y, NORMAL_WIDTH, NORMAL_HEIGHT,
        NULL, NULL, hInstance, NULL
    );
    
    if (!g_hwnd) {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Failed to create window! Error code: %d", error);
        MessageBoxW(NULL, errorMsg, L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Show window
    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    
    // Bring to front
    SetForegroundWindow(g_hwnd);
    
    // Timer for updating time
    SetTimer(g_hwnd, 1, 1000, NULL);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    CoUninitialize();
    
    return (int)msg.wParam;
}

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}
