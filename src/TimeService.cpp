#include "TimeService.h"
#include <sstream>
#include <iomanip>

TimeService::TimeService() = default;

std::wstring TimeService::GetCurrentTimeString() const {
    if (use24HourFormat_) {
        return GetCurrentTimeString24Hour();
    } else {
        return GetCurrentTimeString12Hour();
    }
}

std::wstring TimeService::GetCurrentTimeString24Hour() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm;
    localtime_s(&tm, &time);
    
    std::wstringstream wss;
    wss << std::setfill(L'0') 
        << std::setw(2) << tm.tm_hour << L":"
        << std::setw(2) << tm.tm_min << L":"
        << std::setw(2) << tm.tm_sec;
    
    return wss.str();
}

std::wstring TimeService::GetCurrentTimeString12Hour() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm;
    localtime_s(&tm, &time);
    
    int hour = tm.tm_hour % 12;
    if (hour == 0) hour = 12; // 12 小时制中 0 点显示为 12
    
    std::wstringstream wss;
    wss << std::setfill(L'0') 
        << std::setw(2) << hour << L":"
        << std::setw(2) << tm.tm_min << L":"
        << std::setw(2) << tm.tm_sec;
    
    return wss.str();
}

void TimeService::Set24HourFormat(bool use24Hour) {
    use24HourFormat_ = use24Hour;
}

void TimeService::ToggleFormat() {
    use24HourFormat_ = !use24HourFormat_;
}
