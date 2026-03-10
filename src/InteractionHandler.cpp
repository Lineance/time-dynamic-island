#include "InteractionHandler.h"

InteractionHandler::InteractionHandler() = default;

InteractionHandler::~InteractionHandler() = default;

LRESULT InteractionHandler::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEMOVE: {
        if (!isHovering_) {
            isHovering_ = true;
            
            // 触发放大
            if (isExpanded_) {
                // 已经放大，不做处理
            } else {
                isExpanded_ = true;
                // 这里需要通知窗口管理器进行放大
                // 实际实现在主程序中处理
            }
            
            TrackMouseLeave(hwnd);
        }
        return 0;
    }
    
    case WM_MOUSELEAVE: {
        isHovering_ = false;
        isExpanded_ = false;
        isTrackingMouse_ = false;
        // 这里需要通知窗口管理器进行缩小
        // 实际实现在主程序中处理
        return 0;
    }
    
    case WM_LBUTTONDOWN: {
        if (leftClickCallback_) {
            leftClickCallback_();
        }
        return 0;
    }
    
    case WM_RBUTTONDOWN: {
        if (rightClickCallback_) {
            rightClickCallback_();
        }
        return 0;
    }
    
    case WM_SETCURSOR: {
        // 设置鼠标指针为手型
        if (LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursor(nullptr, IDC_HAND));
            return TRUE;
        }
        break;
    }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void InteractionHandler::TrackMouseLeave(HWND hwnd) {
    if (!isTrackingMouse_) {
        TRACKMOUSEEVENT tme = {};
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd;
        
        if (TrackMouseEvent(&tme)) {
            isTrackingMouse_ = true;
        }
    }
}
