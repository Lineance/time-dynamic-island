#include "StartupManager.h"
#include <shlobj.h>
#include <shobjidl.h>
#include <filesystem>

#pragma comment(lib, "shell32.lib")

StartupManager::StartupManager() = default;

bool StartupManager::SetAutoStartup(bool enable) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY_PATH, 0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    if (enable) {
        // 设置自启动
        std::wstring exePath = GetCurrentExecutablePath();
        
        result = RegSetValueExW(
            hKey,
            APP_NAME,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(exePath.c_str()),
            static_cast<DWORD>((exePath.length() + 1) * sizeof(wchar_t))
        );
    } else {
        // 删除自启动项
        result = RegDeleteValueW(hKey, APP_NAME);
    }
    
    RegCloseKey(hKey);
    
    return (result == ERROR_SUCCESS);
}

bool StartupManager::IsAutoStartupEnabled() const {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY_PATH, 0, KEY_READ, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    // 检查是否存在该值
    DWORD type = 0;
    DWORD size = 0;
    result = RegQueryValueExW(hKey, APP_NAME, nullptr, &type, nullptr, &size);
    
    RegCloseKey(hKey);
    
    return (result == ERROR_SUCCESS && type == REG_SZ);
}

std::wstring StartupManager::GetCurrentExecutablePath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    
    // 确保路径被引号包围（防止路径中有空格）
    std::wstring fullPath = L"\"";
    fullPath += path;
    fullPath += L"\"";
    
    return fullPath;
}
