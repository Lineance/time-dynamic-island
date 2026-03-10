#include "WindowManager.h"
#include "utils.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

WindowManager::WindowManager() = default;

WindowManager::~WindowManager() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
    }
}

bool WindowManager::Initialize(HWND hInstance, int showCmd) {
    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // 透明背景
    wc.lpszClassName = L"DynamicIslandWindowClass";
    
    if (!RegisterClassExW(&wc)) {
        return false;
    }
    
    // 获取屏幕工作区域
    RECT workArea = utils::GetWorkArea();
    
    // 计算窗口位置（顶部居中）
    int x = workArea.left + (workArea.right - workArea.left - width_) / 2;
    int y = workArea.top + 10; // 距离顶部 10 像素
    
    // 创建窗口
    hwnd_ = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        L"DynamicIslandWindowClass",
        L"DynamicIsland",
        WS_POPUP,
        x, y, width_, height_,
        nullptr,
        nullptr,
        hInstance,
        this
    );
    
    if (!hwnd_) {
        return false;
    }
    
    // 设置窗口透明
    SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), 0, LWA_ALPHA);
    
    // 存储 this 指针以便在 WndProc 中访问
    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    
    // 显示窗口
    ShowWindow(hwnd_, showCmd);
    UpdateWindow(hwnd_);
    
    isInitialized_ = true;
    return true;
}

void WindowManager::UpdatePosition() {
    if (!hwnd_) return;
    
    RECT workArea = utils::GetWorkArea();
    int x = workArea.left + (workArea.right - workArea.left - width_) / 2;
    int y = workArea.top + 10;
    
    SetWindowPos(hwnd_, HWND_TOPMOST, x, y, width_, height_, 
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void WindowManager::SetWindowSize(int width, int height, bool animated) {
    if (!hwnd_) return;
    
    if (animated) {
        // 简单的动画效果
        int stepCount = 10;
        int widthStep = (width - width_) / stepCount;
        int heightStep = (height - height_) / stepCount;
        
        for (int i = 1; i <= stepCount; ++i) {
            int currentWidth = width_ + widthStep * i;
            int currentHeight = height_ + heightStep * i;
            
            SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, currentWidth, currentHeight,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
            
            Sleep(5); // 短暂延迟实现动画效果
        }
    }
    
    width_ = width;
    height_ = height;
    
    // 更新位置保持居中
    UpdatePosition();
}

void WindowManager::Invalidate() {
    if (hwnd_) {
        InvalidateRect(hwnd_, nullptr, TRUE);
    }
}

LRESULT CALLBACK WindowManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowManager* self = nullptr;
    
    if (msg == WM_NCCREATE) {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<WindowManager*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->hwnd_ = hwnd;
    } else {
        self = reinterpret_cast<WindowManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (self) {
        return self->HandleMessage(msg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WindowManager::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd_, &ps);
        
        if (renderCallback_) {
            renderCallback_(hdc, width_, height_);
        }
        
        EndPaint(hwnd_, &ps);
        return 0;
    }
    
    case WM_ERASEBKGND:
        return TRUE; // 防止背景闪烁
        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProc(hwnd_, msg, wParam, lParam);
}
