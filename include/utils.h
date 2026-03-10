// 工具函数头文件
#pragma once

#include <windows.h>
#include <string>

namespace utils {

// 获取屏幕工作区域
RECT GetWorkArea();

// 居中计算
int CenterX(int width);
int CenterY(int height);

// 颜色工具
COLORREF InvertColor(COLORREF color);
bool IsColorDark(COLORREF color);

// 字符串转换
std::wstring ToWideString(const std::string& str);
std::string ToString(const std::wstring& wstr);

// 注册表操作
bool GetRegistryValue(HKEY hKey, const wchar_t* subKey, const wchar_t* valueName, DWORD& value);
bool SetRegistryValue(HKEY hKey, const wchar_t* subKey, const wchar_t* valueName, DWORD value);

// 进程检测
bool IsProcessRunning(const wchar_t* processName);
bool IsForegroundWindowFullscreen();

} // namespace utils
