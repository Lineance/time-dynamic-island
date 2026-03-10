// 反作弊规避头文件
#pragma once

#include <windows.h>
#include <string>
#include <vector>

class AntiCheatEvasion {
public:
    AntiCheatEvasion();
    ~AntiCheatEvasion();

    // 初始化（设置窗口样式等）
    void Initialize(HWND hwnd);
    
    // 检测是否有游戏运行
    bool IsGameRunning() const;
    
    // 获取游戏进程列表
    std::vector<std::wstring> GetGameProcessList() const;
    
    // 添加/移除游戏进程名
    void AddGameProcess(const std::wstring& processName);
    void RemoveGameProcess(const std::wstring& processName);
    
    // 设置进程名称（用于伪装）
    void SetProcessName(const std::wstring& name);
    
private:
    bool IsProcessInList(const std::wstring& processName) const;
    bool CheckFullscreenGame() const;
    
    std::vector<std::wstring> gameProcessList_;
    std::wstring processName_;
};
