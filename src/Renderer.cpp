#include "Renderer.h"

Renderer::Renderer() = default;

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    if (isInitialized_) {
        return true;
    }
    
    Gdiplus::GdiplusStartupOutput gdiplusStartupOutput;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken_, &gdiplusStartupInput_, &gdiplusStartupOutput);
    
    if (status != Gdiplus::Ok) {
        return false;
    }
    
    isInitialized_ = true;
    return true;
}

void Renderer::Shutdown() {
    if (isInitialized_) {
        Gdiplus::GdiplusShutdown(gdiplusToken_);
        isInitialized_ = false;
    }
}

void Renderer::SetTheme(Theme theme) {
    currentTheme_ = theme;
    
    if (theme == Theme::Light) {
        backgroundColor_ = RGB(255, 255, 255);
        textColor_ = RGB(0, 0, 0);
    } else {
        backgroundColor_ = RGB(0, 0, 0);
        textColor_ = RGB(255, 255, 255);
    }
}

void Renderer::Render(HDC hdc, int width, int height, const std::wstring& timeText) {
    if (!isInitialized_) return;
    
    // 创建内存 DC 用于双缓冲
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = reinterpret_cast<HBITMAP>(SelectObject(memDC, memBitmap));
    
    // 创建 GDI+ Graphics 对象
    Gdiplus::Graphics graphics(memDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    
    // 清除背景
    Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
        200, // 透明度
        GetRValue(backgroundColor_),
        GetGValue(backgroundColor_),
        GetBValue(backgroundColor_)
    ));
    
    // 绘制椭圆形背景
    DrawEllipse(graphics, width, height);
    
    // 绘制时间文本
    DrawText(graphics, width, height, timeText);
    
    // 将内存 DC 复制到屏幕 DC
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    
    // 清理资源
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

void Renderer::DrawEllipse(Gdiplus::Graphics& graphics, int width, int height) {
    Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(
        200, // 透明度 (0-255)
        GetRValue(backgroundColor_),
        GetGValue(backgroundColor_),
        GetBValue(backgroundColor_)
    ));
    
    // 绘制椭圆（留一点边距）
    int margin = 2;
    Gdiplus::RectF rect(
        static_cast<Gdiplus::REAL>(margin),
        static_cast<Gdiplus::REAL>(margin),
        static_cast<Gdiplus::REAL>(width - 2 * margin),
        static_cast<Gdiplus::REAL>(height - 2 * margin)
    );
    
    graphics.FillEllipse(&backgroundBrush, rect);
}

void Renderer::DrawText(Gdiplus::Graphics& graphics, int width, int height, const std::wstring& text) {
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(
        255,
        GetRValue(textColor_),
        GetGValue(textColor_),
        GetBValue(textColor_)
    ));
    
    // 创建字体
    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::REAL fontSize = static_cast<Gdiplus::REAL>(height * 0.5);
    Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    
    // 测量文本大小
    Gdiplus::RectF textBounds;
    graphics.MeasureString(text.c_str(), -1, &font, Gdiplus::PointF(0, 0), &textBounds);
    
    // 计算居中位置
    Gdiplus::REAL x = (width - textBounds.Width) / 2.0f;
    Gdiplus::REAL y = (height - textBounds.Height) / 2.0f;
    
    // 绘制文本
    graphics.DrawString(text.c_str(), -1, &font, Gdiplus::PointF(x, y), &textBrush);
}
