#include "AntiCheatEvasion.h"
#include "utils.h"
#include <tlhelp32.h>

AntiCheatEvasion::AntiCheatEvasion() {
    // 初始化常见游戏进程列表
    gameProcessList_ = {
        L"valorant.exe",
        L"leagueoflegends.exe",
        L"csgo.exe",
        L"cs2.exe",
        L"apexlegends.exe",
        L"fortnite.exe",
        L"overwatch.exe",
        L"wow.exe",
        L"minecraft.exe",
    };
    
    // 设置中性进程名称
    processName_ = L"SystemTrayHelper";
}

AntiCheatEvasion::~AntiCheatEvasion() = default;

void AntiCheatEvasion::Initialize(HWND hwnd) {
    if (!hwnd) return;
    
    // 设置窗口样式为工具窗口（不在 Alt+Tab 显示）
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOOLWINDOW;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
    
    // 设置为透明且不被某些反作弊检测
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
}

bool AntiCheatEvasion::IsGameRunning() const {
    // 检查是否有全屏游戏
    if (CheckFullscreenGame()) {
        return true;
    }
    
    // 检查游戏进程列表
    for (const auto& processName : gameProcessList_) {
        if (utils::IsProcessRunning(processName.c_str())) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::wstring> AntiCheatEvasion::GetGameProcessList() const {
    return gameProcessList_;
}

void AntiCheatEvasion::AddGameProcess(const std::wstring& processName) {
    if (!IsProcessInList(processName)) {
        gameProcessList_.push_back(processName);
    }
}

void AntiCheatEvasion::RemoveGameProcess(const std::wstring& processName) {
    gameProcessList_.erase(
        std::remove(gameProcessList_.begin(), gameProcessList_.end(), processName),
        gameProcessList_.end()
    );
}

void AntiCheatEvasion::SetProcessName(const std::wstring& name) {
    processName_ = name;
}

bool AntiCheatEvasion::IsProcessInList(const std::wstring& processName) const {
    return std::find(gameProcessList_.begin(), gameProcessList_.end(), processName) 
           != gameProcessList_.end();
}

bool AntiCheatEvasion::CheckFullscreenGame() const {
    return utils::IsForegroundWindowFullscreen();
}
