// 时间服务头文件
#pragma once

#include <string>
#include <chrono>

class TimeService {
public:
    TimeService();
    
    // 获取当前时间字符串
    std::wstring GetCurrentTimeString() const;
    std::wstring GetCurrentTimeString12Hour() const;
    std::wstring GetCurrentTimeString24Hour() const;
    
    // 格式设置
    void Set24HourFormat(bool use24Hour);
    bool Is24HourFormat() const { return use24HourFormat_; }
    
    // 切换格式
    void ToggleFormat();
    
private:
    bool use24HourFormat_ = true;
};
