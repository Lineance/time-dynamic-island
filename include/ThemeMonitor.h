// 主题监听器头文件
#pragma once

#include <windows.h>
#include <functional>

class ThemeMonitor {
public:
    ThemeMonitor();
    ~ThemeMonitor();

    // 主题类型
    enum class Theme {
        Light,
        Dark
    };
    
    // 获取当前主题
    Theme GetCurrentTheme() const;
    
    // 开始监听（返回线程 ID）
    using ThemeChangeCallback = std::function<void(Theme)>;
    void StartMonitoring(ThemeChangeCallback callback);
    
    // 停止监听
    void StopMonitoring();
    
    // 检查是否正在监听
    bool IsMonitoring() const { return isMonitoring_; }
    
private:
    static DWORD WINAPI MonitoringThread(LPVOID lpParameter);
    void MonitorLoop();
    
    Theme currentTheme_ = Theme::Light;
    ThemeChangeCallback callback_;
    HANDLE monitoringThread_ = nullptr;
    bool isMonitoring_ = false;
    bool shouldStop_ = false;
};
