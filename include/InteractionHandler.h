// 交互处理器头文件
#pragma once

#include <windows.h>
#include <functional>

class InteractionHandler {
public:
    InteractionHandler();
    ~InteractionHandler();

    // 回调类型
    using ClickCallback = std::function<void()>;
    using FormatToggleCallback = std::function<void()>;
    
    // 设置回调
    void SetLeftClickCallback(ClickCallback callback) { leftClickCallback_ = callback; }
    void SetRightClickCallback(FormatToggleCallback callback) { rightClickCallback_ = callback; }
    
    // 处理窗口消息
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    // 悬停状态
    bool IsHovering() const { return isHovering_; }
    void SetHovering(bool hovering) { isHovering_ = hovering; }
    
    // 放大状态
    bool IsExpanded() const { return isExpanded_; }
    void SetExpanded(bool expanded) { isExpanded_ = expanded; }
    
private:
    void TrackMouseLeave(HWND hwnd);
    
    ClickCallback leftClickCallback_;
    FormatToggleCallback rightClickCallback_;
    
    bool isHovering_ = false;
    bool isExpanded_ = false;
    bool isTrackingMouse_ = false;
};
