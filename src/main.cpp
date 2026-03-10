#include <windows.h>
#include <shellapi.h>
#include "WindowManager.h"
#include "Renderer.h"
#include "TimeService.h"
#include "ThemeMonitor.h"
#include "InteractionHandler.h"
#include "AntiCheatEvasion.h"
#include "StartupManager.h"
#include "utils.h"

#pragma comment(lib, "shell32.lib")

// 全局变量
WindowManager* g_windowManager = nullptr;
Renderer* g_renderer = nullptr;
TimeService* g_timeService = nullptr;
ThemeMonitor* g_themeMonitor = nullptr;
InteractionHandler* g_interactionHandler = nullptr;
AntiCheatEvasion* g_antiCheatEvasion = nullptr;

// 定时器 ID
constexpr UINT_PTR UPDATE_TIMER_ID = 1;
constexpr UINT UPDATE_INTERVAL_MS = 1000; // 1 秒更新一次

// 窗口尺寸
constexpr int NORMAL_WIDTH = 200;
constexpr int NORMAL_HEIGHT = 60;
constexpr int EXPANDED_WIDTH = 240;
constexpr int EXPANDED_HEIGHT = 72;

// 前向声明
void UpdateDisplay();
void Cleanup();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                    LPWSTR lpCmdLine, int nCmdShow) {
    // 防止多实例运行
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"DynamicIsland_Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }
    
    // 初始化 COM（用于 ShellExecute 等）
    CoInitialize(nullptr);
    
    // 设置开机自启动
    StartupManager startupManager;
    startupManager.SetAutoStartup(true);
    
    // 初始化各模块
    g_windowManager = new WindowManager();
    g_renderer = new Renderer();
    g_timeService = new TimeService();
    g_themeMonitor = new ThemeMonitor();
    g_interactionHandler = new InteractionHandler();
    g_antiCheatEvasion = new AntiCheatEvasion();
    
    // 初始化 GDI+
    if (!g_renderer->Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize GDI+", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // 初始化窗口
    if (!g_windowManager->Initialize(hInstance, nCmdShow)) {
        MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // 初始化反作弊规避
    g_antiCheatEvasion->Initialize(g_windowManager->GetHwnd());
    
    // 初始化主题
    Renderer::Theme currentTheme = static_cast<Renderer::Theme>(g_themeMonitor->GetCurrentTheme());
    g_renderer->SetTheme(currentTheme);
    
    // 设置主题变化回调
    g_themeMonitor->StartMonitoring([currentTheme](ThemeMonitor::Theme newTheme) {
        if (g_renderer) {
            g_renderer->SetTheme(static_cast<Renderer::Theme>(newTheme));
            if (g_windowManager) {
                g_windowManager->Invalidate();
            }
        }
    });
    
    // 设置交互回调
    g_interactionHandler->SetLeftClickCallback([]() {
        // 左键点击：打开系统时间设置
        ShellExecuteW(nullptr, L"open", L"control.exe", L"timedate.cpl", nullptr, SW_SHOW);
    });
    
    g_interactionHandler->SetRightClickCallback([]() {
        // 右键点击：切换时间格式
        if (g_timeService) {
            g_timeService->ToggleFormat();
            if (g_windowManager) {
                g_windowManager->Invalidate();
            }
        }
    });
    
    // 设置渲染回调
    g_windowManager->SetRenderCallback([](HDC hdc, int width, int height) {
        if (g_renderer && g_timeService) {
            std::wstring timeStr = g_timeService->GetCurrentTimeString();
            g_renderer->Render(hdc, width, height, timeStr);
        }
    });
    
    // 启动定时器
    SetTimer(g_windowManager->GetHwnd(), UPDATE_TIMER_ID, UPDATE_INTERVAL_MS, nullptr);
    
    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        // 处理交互消息
        if (g_interactionHandler) {
            LRESULT result = g_interactionHandler->HandleMessage(
                g_windowManager->GetHwnd(), msg.message, msg.wParam, msg.lParam);
            
            // 如果消息被处理，跳过默认处理
            if (result != 0) {
                continue;
            }
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // 清理
    Cleanup();
    CoUninitialize();
    
    return static_cast<int>(msg.wParam);
}

void UpdateDisplay() {
    if (g_windowManager && g_renderer && g_timeService) {
        // 检查是否有游戏运行
        if (g_antiCheatEvasion && g_antiCheatEvasion->IsGameRunning()) {
            // 游戏运行时隐藏窗口
            ShowWindow(g_windowManager->GetHwnd(), SW_HIDE);
        } else {
            // 显示窗口
            ShowWindow(g_windowManager->GetHwnd(), SW_SHOW);
            g_windowManager->Invalidate();
        }
    }
}

void Cleanup() {
    if (g_themeMonitor) {
        g_themeMonitor->StopMonitoring();
    }
    
    delete g_interactionHandler;
    delete g_antiCheatEvasion;
    delete g_themeMonitor;
    delete g_timeService;
    delete g_renderer;
    delete g_windowManager;
}

// Windows 入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}
