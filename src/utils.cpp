#include "utils.h"
#include <algorithm>
#include <cctype>

namespace utils {

RECT GetWorkArea() {
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    return workArea;
}

int CenterX(int width) {
    RECT workArea = GetWorkArea();
    return (workArea.right - workArea.left - width) / 2;
}

int CenterY(int height) {
    RECT workArea = GetWorkArea();
    return (workArea.bottom - workArea.top - height) / 2;
}

COLORREF InvertColor(COLORREF color) {
    return RGB(
        255 - GetRValue(color),
        255 - GetGValue(color),
        255 - GetBValue(color)
    );
}

bool IsColorDark(COLORREF color) {
    // 计算亮度
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    
    // 使用亮度公式
    double luminance = 0.299 * r + 0.587 * g + 0.114 * b;
    return luminance < 128;
}

std::wstring ToWideString(const std::string& str) {
    if (str.empty()) return L"";
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    
    return wstr;
}

std::string ToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    
    return str;
}

bool GetRegistryValue(HKEY hKey, const wchar_t* subKey, const wchar_t* valueName, DWORD& value) {
    HKEY hKeySub;
    LONG result = RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hKeySub);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    DWORD type = REG_DWORD;
    DWORD size = sizeof(value);
    result = RegQueryValueEx(hKeySub, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
    
    RegCloseKey(hKeySub);
    
    return (result == ERROR_SUCCESS && type == REG_DWORD);
}

bool SetRegistryValue(HKEY hKey, const wchar_t* subKey, const wchar_t* valueName, DWORD value) {
    HKEY hKeySub;
    LONG result = RegCreateKeyEx(hKey, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, 
                                  KEY_SET_VALUE, nullptr, &hKeySub, nullptr);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    result = RegSetValueEx(hKeySub, valueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
    
    RegCloseKey(hKeySub);
    
    return (result == ERROR_SUCCESS);
}

bool IsProcessRunning(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    bool found = false;
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                found = true;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return found;
}

bool IsForegroundWindowFullscreen() {
    HWND hwndForeground = GetForegroundWindow();
    if (!hwndForeground) {
        return false;
    }
    
    RECT windowRect;
    GetWindowRect(hwndForeground, &windowRect);
    
    RECT screenRect;
    screenRect.left = 0;
    screenRect.top = 0;
    screenRect.right = GetSystemMetrics(SM_CXSCREEN);
    screenRect.bottom = GetSystemMetrics(SM_CYSCREEN);
    
    // 检查窗口是否覆盖整个屏幕
    return (windowRect.left <= screenRect.left &&
            windowRect.top <= screenRect.top &&
            windowRect.right >= screenRect.right &&
            windowRect.bottom >= screenRect.bottom);
}

} // namespace utils
