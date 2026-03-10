#include "ThemeMonitor.h"
#include "utils.h"

ThemeMonitor::ThemeMonitor() = default;

ThemeMonitor::~ThemeMonitor() {
    StopMonitoring();
}

ThemeMonitor::Theme ThemeMonitor::GetCurrentTheme() const {
    DWORD value = 1; // 默认为浅色主题
    
    if (utils::GetRegistryValue(
            HKEY_CURRENT_USER,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            value)) {
        
        return (value == 1) ? Theme::Light : Theme::Dark;
    }
    
    return Theme::Light;
}

void ThemeMonitor::StartMonitoring(ThemeChangeCallback callback) {
    if (isMonitoring_) {
        return;
    }
    
    callback_ = callback;
    currentTheme_ = GetCurrentTheme();
    shouldStop_ = false;
    
    // 创建监控线程
    monitoringThread_ = CreateThread(nullptr, 0, MonitoringThread, this, 0, nullptr);
    
    if (monitoringThread_) {
        isMonitoring_ = true;
    }
}

void ThemeMonitor::StopMonitoring() {
    if (!isMonitoring_) {
        return;
    }
    
    shouldStop_ = true;
    
    if (monitoringThread_) {
        WaitForSingleObject(monitoringThread_, 2000);
        CloseHandle(monitoringThread_);
        monitoringThread_ = nullptr;
    }
    
    isMonitoring_ = false;
}

DWORD WINAPI ThemeMonitor::MonitoringThread(LPVOID lpParameter) {
    ThemeMonitor* self = reinterpret_cast<ThemeMonitor*>(lpParameter);
    self->MonitorLoop();
    return 0;
}

void ThemeMonitor::MonitorLoop() {
    while (!shouldStop_) {
        // 每秒检查一次主题变化
        Sleep(1000);
        
        Theme newTheme = GetCurrentTheme();
        
        if (newTheme != currentTheme_ && callback_) {
            currentTheme_ = newTheme;
            callback_(newTheme);
        }
    }
}
