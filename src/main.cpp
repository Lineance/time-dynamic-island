#include <windows.h>
#include <shellapi.h>
#include <gdiplus.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <functional>
#include <vector>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")

using namespace Gdiplus;

// 全局变量
HWND g_hwnd = nullptr;
ULONG_PTR g_gdiplusToken = 0;
bool g_use24Hour = true;
bool g_isHovering = false;
int g_width = 200;
int g_height = 60;
const int NORMAL_WIDTH = 200;
const int NORMAL_HEIGHT = 60;
const int EXPANDED_WIDTH = 240;
const int EXPANDED_HEIGHT = 72;

// 前向声明
void RenderWindow();
void UpdateWindowSize(int width, int height);
std::wstring GetTimeString();
void ToggleTimeFormat();
void OpenTimeSettings();
COLORREF GetBackgroundColor();
COLORREF GetTextColor();

// 获取系统主题
bool IsLightTheme() {
    DWORD value = 1;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, 
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(value);
        RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, 
                        reinterpret_cast<LPBYTE>(&value), &size);
        RegCloseKey(hKey);
    }
    return value == 1;
}

// 获取时间字符串
std::wstring GetTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
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

// 切换时间格式
void ToggleTimeFormat() {
    g_use24Hour = !g_use24Hour;
    RenderWindow();
}

// 打开时间设置
void OpenTimeSettings() {
    ShellExecuteW(nullptr, L"open", L"control.exe", L"timedate.cpl", nullptr, SW_SHOW);
}

// 获取背景颜色
COLORREF GetBackgroundColor() {
    return IsLightTheme() ? RGB(255, 255, 255) : RGB(0, 0, 0);
}

// 获取文字颜色
COLORREF GetTextColor() {
    return IsLightTheme() ? RGB(0, 0, 0) : RGB(255, 255, 255);
}

// 更新窗口大小
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

// 渲染窗口
void RenderWindow() {
    HDC hdc = GetDC(g_hwnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, g_width, g_height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    // GDI+ 渲染
    Graphics graphics(memDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    
    // 绘制椭圆背景
    Color bgColor(200, 
        GetRValue(GetBackgroundColor()),
        GetGValue(GetBackgroundColor()),
        GetBValue(GetBackgroundColor()));
    SolidBrush bgBrush(bgColor);
    
    RectF rect(2.0f, 2.0f, 
               (REAL)(g_width - 4), 
               (REAL)(g_height - 4));
    graphics.FillEllipse(&bgBrush, rect);
    
    // 绘制时间文本
    std::wstring timeStr = GetTimeString();
    Color textColor(255,
        GetRValue(GetTextColor()),
        GetGValue(GetTextColor()),
        GetBValue(GetTextColor()));
    SolidBrush textBrush(textColor);
    
    FontFamily fontFamily(L"Segoe UI");
    REAL fontSize = (REAL)(g_height * 0.5f);
    Font font(&fontFamily, fontSize, FontStyleRegular, UnitPixel);
    
    RectF textBounds;
    graphics.MeasureString(timeStr.c_str(), -1, &font, PointF(0, 0), &textBounds);
    
    REAL x = ((REAL)g_width - textBounds.Width) / 2.0f;
    REAL y = ((REAL)g_height - textBounds.Height) / 2.0f;
    
    graphics.DrawString(timeStr.c_str(), -1, &font, PointF(x, y), &textBrush);
    
    // 复制到屏幕
    BitBlt(hdc, 0, 0, g_width, g_height, memDC, 0, 0, SRCCOPY);
    
    // 清理
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    ReleaseDC(g_hwnd, hdc);
}

// 窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        RenderWindow();
        EndPaint(hwnd, &ps);
        return 0;
    }
    
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
            SetCursor(LoadCursor(nullptr, IDC_HAND));
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

// 设置开机自启动
void SetAutoStartup() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, 
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        std::wstring fullPath = L"\"";
        fullPath += path;
        fullPath += L"\"";
        
        RegSetValueExW(hKey, L"DynamicIsland", 0, REG_SZ,
                      reinterpret_cast<const BYTE*>(fullPath.c_str()),
                      (DWORD)((fullPath.length() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}

// 主函数
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {
    // 防止多实例
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"DynamicIsland_Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }
    
    // 初始化 COM
    CoInitialize(nullptr);
    
    // 设置开机自启
    SetAutoStartup();
    
    // 初始化 GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
    
    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"DynamicIslandClass";
    
    RegisterClassExW(&wc);
    
    // 创建窗口
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int x = workArea.left + (workArea.right - workArea.left - NORMAL_WIDTH) / 2;
    int y = workArea.top + 10;
    
    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        L"DynamicIslandClass",
        L"DynamicIsland",
        WS_POPUP,
        x, y, NORMAL_WIDTH, NORMAL_HEIGHT,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!g_hwnd) {
        return 1;
    }
    
    // 设置透明
    SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, LWA_ALPHA);
    
    // 显示窗口
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    // 定时器
    SetTimer(g_hwnd, 1, 1000, nullptr);
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // 清理
    GdiplusShutdown(g_gdiplusToken);
    CoUninitialize();
    
    return (int)msg.wParam;
}

// Windows 入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}
