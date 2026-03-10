// 窗口管理器头文件
#pragma once

#include <windows.h>
#include <functional>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // 初始化窗口
    bool Initialize(HWND hInstance, int showCmd);
    
    // 获取窗口句柄
    HWND GetHwnd() const { return hwnd_; }
    
    // 更新窗口位置
    void UpdatePosition();
    
    // 设置窗口大小（支持动画）
    void SetWindowSize(int width, int height, bool animated = false);
    
    // 获取当前大小
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    
    // 设置渲染回调
    using RenderCallback = std::function<void(HDC, int, int)>;
    void SetRenderCallback(RenderCallback callback) { renderCallback_ = callback; }
    
    // 触发重绘
    void Invalidate();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
    HWND hwnd_ = nullptr;
    int width_ = 200;
    int height_ = 60;
    RenderCallback renderCallback_;
    bool isInitialized_ = false;
};
