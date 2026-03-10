// 渲染引擎头文件
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")

class Renderer {
public:
    Renderer();
    ~Renderer();

    // 初始化 GDI+
    bool Initialize();
    void Shutdown();
    
    // 设置主题
    enum class Theme {
        Light,
        Dark
    };
    
    void SetTheme(Theme theme);
    Theme GetTheme() const { return currentTheme_; }
    
    // 渲染到 DC
    void Render(HDC hdc, int width, int height, const std::wstring& timeText);
    
    // 颜色设置
    void SetBackgroundColor(COLORREF color) { backgroundColor_ = color; }
    void SetTextColor(COLORREF color) { textColor_ = color; }
    
private:
    void DrawEllipse(Gdiplus::Graphics& graphics, int width, int height);
    void DrawText(Gdiplus::Graphics& graphics, int width, int height, const std::wstring& text);
    
    Gdiplus::GdiplusStartupInput gdiplusStartupInput_;
    ULONG_PTR gdiplusToken_;
    bool isInitialized_ = false;
    
    Theme currentTheme_ = Theme::Light;
    COLORREF backgroundColor_ = RGB(255, 255, 255);
    COLORREF textColor_ = RGB(0, 0, 0);
};
