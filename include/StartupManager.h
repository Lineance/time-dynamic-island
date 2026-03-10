// 开机自启动管理器头文件
#pragma once

#include <windows.h>
#include <string>

class StartupManager {
public:
    StartupManager();
    
    // 设置开机自启动
    bool SetAutoStartup(bool enable);
    
    // 检查是否已设置自启动
    bool IsAutoStartupEnabled() const;
    
    // 获取当前可执行文件路径
    static std::wstring GetCurrentExecutablePath();
    
private:
    // 注册表键路径
    static constexpr const wchar_t* RUN_KEY_PATH = 
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    // 应用名称
    static constexpr const wchar_t* APP_NAME = L"DynamicIsland";
};
