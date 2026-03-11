// Dynamic Island - Capsule shape with settings dialog and auto-hide
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <objbase.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "resource.h"

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "advapi32.lib")

// Global variables
HWND g_hwnd = nullptr;
HWND g_settingsDlg = nullptr;

// Settings
bool g_use24Hour = true;
bool g_showDate = true;
bool g_showYear = false;
bool g_showSeconds = true;
int g_theme = 0; // 0=Dark, 1=Light, 2=System
bool g_autoHide = false; // Auto-hide feature
bool g_isHovering = false;

// Window size settings - 0=Small, 1=Medium, 2=Large
int g_windowSize = 1;

// Position settings - 0=Left, 1=Center, 2=Right
int g_position = 1;

// Hide delay in minutes and seconds
int g_hideDelayMinutes = 0;
int g_hideDelaySeconds = 1;

// Font settings
int g_fontSize = 14;
bool g_boldFont = true;

// Window size definitions for different sizes
const int SIZE_WIDTH[3] = {120, 144, 168};
const int SIZE_HEIGHT[3] = {20, 25, 30};

// Position tracking
int g_currentX = 0;
int g_currentY = 10;
int g_hiddenY = -50; // Position when hidden (above screen)

// Auto-hide timer
UINT_PTR g_hideTimer = 0;
bool g_hideTimerActive = false;

// Mouse check timer for auto-hide
const int MOUSE_CHECK_INTERVAL = 100;  // 100ms

// Animation state
bool g_isAnimating = false;
bool g_isHiding = false;  // true=hide, false=show
int g_animationY = 10;    // Current Y position during animation
const int ANIMATION_STEP = 3;      // Pixels per frame
const int ANIMATION_INTERVAL = 15; // Milliseconds
const int ANIMATION_TIMER_ID = 4;

// Hidden state for auto-hide
bool g_isHidden = false;           // Whether the island is currently hidden
bool g_waitingForHide = false;     // Whether waiting to hide (during delay)

// Forward declarations
void RenderWindow();
void UpdateWindowPosition(bool expanded);
void ShowWindowAtPosition(int x, int y, int width, int height);
std::wstring GetTimeString();
std::wstring GetDateString();
void OpenTimeSettings();
void ShowSettingsDialog();
COLORREF GetBackgroundColor();
COLORREF GetTextColor();
bool IsLightTheme();
void SaveSettings();
void LoadSettings();
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void CheckMousePositionAndShowHide();
void StartHideAnimation();
void StartShowAnimation();
void UpdateAnimation();
void UpdateWindowPositionWithY(int y);

// Check system theme
bool IsLightTheme() {
    DWORD value = 0;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, 
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(value);
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, 
                        reinterpret_cast<LPBYTE>(&value), &size);
        RegCloseKey(hKey);
    }
    return value == 1;
}

// Load settings from registry
void LoadSettings() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\DynamicIsland", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size;
        size = sizeof(g_use24Hour);
        RegQueryValueExW(hKey, L"Use24Hour", NULL, NULL, reinterpret_cast<LPBYTE>(&g_use24Hour), &size);
        size = sizeof(g_showDate);
        RegQueryValueExW(hKey, L"ShowDate", NULL, NULL, reinterpret_cast<LPBYTE>(&g_showDate), &size);
        size = sizeof(g_showYear);
        RegQueryValueExW(hKey, L"ShowYear", NULL, NULL, reinterpret_cast<LPBYTE>(&g_showYear), &size);
        size = sizeof(g_showSeconds);
        RegQueryValueExW(hKey, L"ShowSeconds", NULL, NULL, reinterpret_cast<LPBYTE>(&g_showSeconds), &size);
        size = sizeof(g_theme);
        RegQueryValueExW(hKey, L"Theme", NULL, NULL, reinterpret_cast<LPBYTE>(&g_theme), &size);
        size = sizeof(g_autoHide);
        RegQueryValueExW(hKey, L"AutoHide", NULL, NULL, reinterpret_cast<LPBYTE>(&g_autoHide), &size);
        size = sizeof(g_windowSize);
        RegQueryValueExW(hKey, L"WindowSize", NULL, NULL, reinterpret_cast<LPBYTE>(&g_windowSize), &size);
        size = sizeof(g_position);
        RegQueryValueExW(hKey, L"Position", NULL, NULL, reinterpret_cast<LPBYTE>(&g_position), &size);
        size = sizeof(g_hideDelayMinutes);
        RegQueryValueExW(hKey, L"HideDelayMinutes", NULL, NULL, reinterpret_cast<LPBYTE>(&g_hideDelayMinutes), &size);
        size = sizeof(g_hideDelaySeconds);
        RegQueryValueExW(hKey, L"HideDelaySeconds", NULL, NULL, reinterpret_cast<LPBYTE>(&g_hideDelaySeconds), &size);
        size = sizeof(g_fontSize);
        RegQueryValueExW(hKey, L"FontSize", NULL, NULL, reinterpret_cast<LPBYTE>(&g_fontSize), &size);
        size = sizeof(g_boldFont);
        RegQueryValueExW(hKey, L"BoldFont", NULL, NULL, reinterpret_cast<LPBYTE>(&g_boldFont), &size);
        RegCloseKey(hKey);
    }
}

// Save settings to registry
void SaveSettings() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\DynamicIsland", 0, NULL, 
            REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"Use24Hour", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_use24Hour), sizeof(g_use24Hour));
        RegSetValueExW(hKey, L"ShowDate", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_showDate), sizeof(g_showDate));
        RegSetValueExW(hKey, L"ShowYear", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_showYear), sizeof(g_showYear));
        RegSetValueExW(hKey, L"ShowSeconds", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_showSeconds), sizeof(g_showSeconds));
        RegSetValueExW(hKey, L"Theme", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_theme), sizeof(g_theme));
        RegSetValueExW(hKey, L"AutoHide", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_autoHide), sizeof(g_autoHide));
        RegSetValueExW(hKey, L"WindowSize", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_windowSize), sizeof(g_windowSize));
        RegSetValueExW(hKey, L"Position", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_position), sizeof(g_position));
        RegSetValueExW(hKey, L"HideDelayMinutes", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_hideDelayMinutes), sizeof(g_hideDelayMinutes));
        RegSetValueExW(hKey, L"HideDelaySeconds", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_hideDelaySeconds), sizeof(g_hideDelaySeconds));
        RegSetValueExW(hKey, L"FontSize", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_fontSize), sizeof(g_fontSize));
        RegSetValueExW(hKey, L"BoldFont", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&g_boldFont), sizeof(g_boldFont));
        RegCloseKey(hKey);
    }
}

// Get time string
std::wstring GetTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm tm;
    localtime_s(&tm, &time);
    
    std::wstringstream wss;
    int hour = g_use24Hour ? tm.tm_hour : (tm.tm_hour % 12);
    if (hour == 0 && !g_use24Hour) hour = 12;
    
    wss << std::setfill(L'0') << std::setw(2) << hour << L":"
        << std::setw(2) << tm.tm_min;
    
    if (g_showSeconds) {
        wss << L":" << std::setw(2) << tm.tm_sec;
    }
    
    return wss.str();
}

// Get date string - Format: 01/02 or 2026/01/02
std::wstring GetDateString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm tm;
    localtime_s(&tm, &time);
    
    std::wstringstream wss;
    
    if (g_showYear) {
        // Show year: 2026/01/02
        wss << (tm.tm_year + 1900) << L"/"
            << std::setfill(L'0') << std::setw(2) << (tm.tm_mon + 1) << L"/"
            << std::setw(2) << tm.tm_mday;
    } else {
        // Don't show year: 01/02
        wss << std::setfill(L'0') << std::setw(2) << (tm.tm_mon + 1) << L"/"
            << std::setw(2) << tm.tm_mday;
    }
    
    return wss.str();
}

// Open time settings
void OpenTimeSettings() {
    ShellExecuteW(NULL, L"open", L"control.exe", L"timedate.cpl", NULL, SW_SHOW);
}

// Get background color based on theme
COLORREF GetBackgroundColor() {
    if (g_theme == 0) {
        // Dark theme - dark gray background
        return RGB(30, 30, 30);
    } else if (g_theme == 1) {
        // Light theme - white background
        return RGB(255, 255, 255);
    } else {
        // System theme
        if (IsLightTheme()) {
            return RGB(255, 255, 255);
        } else {
            return RGB(30, 30, 30);
        }
    }
}

// Get text color based on theme
COLORREF GetTextColor() {
    if (g_theme == 0) {
        // Dark theme - white text
        return RGB(255, 255, 255);
    } else if (g_theme == 1) {
        // Light theme - black text
        return RGB(0, 0, 0);
    } else {
        // System theme
        if (IsLightTheme()) {
            return RGB(0, 0, 0);
        } else {
            return RGB(255, 255, 255);
        }
    }
}

// Show window at specific position
void ShowWindowAtPosition(int x, int y, int width, int height) {
    g_currentX = x;
    g_currentY = y;
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, width, height, 
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
}

// Update window position
void UpdateWindowPosition(bool expanded) {
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int screenWidth = workArea.right - workArea.left;
    
    // Get current size based on window size setting
    int baseWidth = SIZE_WIDTH[g_windowSize];
    int baseHeight = SIZE_HEIGHT[g_windowSize];
    int width = expanded ? (int)(baseWidth * 1.3) : baseWidth;
    int height = expanded ? (int)(baseHeight * 1.3) : baseHeight;
    
    // Calculate X position based on position setting
    int x;
    switch (g_position) {
        case 0: // Left - near left edge
            x = workArea.left + 10;
            break;
        case 2: // Right - near right edge
            x = workArea.right - width - 10;
            break;
        default: // Center
            x = workArea.left + (screenWidth - width) / 2;
            break;
    }
    
    int y = workArea.top + 10;
    
    if (g_autoHide) {
        y = g_isHovering ? y : g_hiddenY;
    }
    
    // Update window position and size
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, width, height, 
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
    
    // Recreate the capsule region for the new size
    int radius = height / 2 * 15 / 10;  // 1.5x radius
    HRGN hRgn = CreateRoundRectRgn(0, 0, width, height, radius, radius);
    SetWindowRgn(g_hwnd, hRgn, TRUE);
}

// Render window - SIMPLE DIRECT DRAWING
void RenderWindow() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(g_hwnd, &ps);
    
    if (hdc == NULL) return;
    
    RECT clientRect;
    GetClientRect(g_hwnd, &clientRect);
    
    // Get colors
    COLORREF bgColor = GetBackgroundColor();
    COLORREF textColor = GetTextColor();
    
    // Fill background directly
    HBRUSH bgBrush = CreateSolidBrush(bgColor);
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Build display string
    std::wstring dateStr = g_showDate ? GetDateString() : L"";
    std::wstring timeStr = GetTimeString();
    std::wstring displayStr = dateStr + L"  " + timeStr;
    
    // Create font with user-defined size and weight
    int fontWeight = g_boldFont ? FW_BOLD : FW_NORMAL;
    HFONT font = CreateFontW(
        -g_fontSize,    // Font size in points
        0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Microsoft YaHei UI"
    );
    
    // Draw text with perfect vertical centering
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, textColor);
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER | TA_BASELINE);
    
    // Perfect vertical centering using text metrics
    TEXTMETRICW tm;
    GetTextMetricsW(hdc, &tm);
    int centerX = clientRect.right / 2;
    int centerY = (clientRect.bottom + tm.tmAscent - tm.tmDescent) / 2;
    TextOutW(hdc, centerX, centerY, displayStr.c_str(), (int)displayStr.length());
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
    
    EndPaint(g_hwnd, &ps);
}

// Timer callback
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (idEvent == 1) {
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
    }
}

// Settings Dialog Procedure - Win11 style
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG: {
        // Center dialog on screen (not on parent)
        RECT rcDlg, rcScreen;
        GetWindowRect(hDlg, &rcDlg);
        
        // Get screen work area
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
        
        // Calculate center position
        int x = rcScreen.left + (rcScreen.right - rcScreen.left - (rcDlg.right - rcDlg.left)) / 2;
        int y = rcScreen.top + (rcScreen.bottom - rcScreen.top - (rcDlg.bottom - rcDlg.top)) / 2;
        
        SetWindowPos(hDlg, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
        
        // Ensure g_theme has valid default value
        if (g_theme < 0 || g_theme > 2) g_theme = 0;
        
        // Initialize controls with current settings
        CheckRadioButton(hDlg, IDC_RADIO_24HOUR, IDC_RADIO_12HOUR, 
                        g_use24Hour ? IDC_RADIO_24HOUR : IDC_RADIO_12HOUR);
        CheckDlgButton(hDlg, IDC_CHECK_DATE, g_showDate ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHECK_SECONDS, g_showSeconds ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHECK_YEAR, g_showYear ? BST_CHECKED : BST_UNCHECKED);
        
        CheckRadioButton(hDlg, IDC_RADIO_DARK, IDC_RADIO_SYSTEM,
                        g_theme == 0 ? IDC_RADIO_DARK : (g_theme == 1 ? IDC_RADIO_LIGHT : IDC_RADIO_SYSTEM));
        
        CheckRadioButton(hDlg, IDC_RADIO_SMALL, IDC_RADIO_LARGE,
                        g_windowSize == 0 ? IDC_RADIO_SMALL : (g_windowSize == 1 ? IDC_RADIO_MEDIUM : IDC_RADIO_LARGE));
        
        CheckRadioButton(hDlg, IDC_RADIO_LEFT, IDC_RADIO_RIGHT,
                        g_position == 0 ? IDC_RADIO_LEFT : (g_position == 1 ? IDC_RADIO_CENTER : IDC_RADIO_RIGHT));
        
        CheckDlgButton(hDlg, IDC_CHECK_AUTOHIDE, g_autoHide ? BST_CHECKED : BST_UNCHECKED);
        
        // Set hide delay (minutes:seconds)
        HWND hMinEdit = GetDlgItem(hDlg, IDC_EDIT_HIDEDELAY_MIN);
        wchar_t minText[32];
        swprintf_s(minText, 32, L"%d", g_hideDelayMinutes);
        SetWindowTextW(hMinEdit, minText);
        
        HWND hSecEdit = GetDlgItem(hDlg, IDC_EDIT_HIDEDELAY_SEC);
        wchar_t secText[32];
        swprintf_s(secText, 32, L"%d", g_hideDelaySeconds);
        SetWindowTextW(hSecEdit, secText);
        
        // Set font size
        HWND hFontEdit = GetDlgItem(hDlg, IDC_EDIT_FONTSIZE);
        wchar_t fontSizeText[32];
        swprintf_s(fontSizeText, 32, L"%d", g_fontSize);
        SetWindowTextW(hFontEdit, fontSizeText);
        
        CheckDlgButton(hDlg, IDC_CHECK_BOLD, g_boldFont ? BST_CHECKED : BST_UNCHECKED);
        
        return TRUE;
    }
    
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            // Get settings from dialog controls
            g_use24Hour = IsDlgButtonChecked(hDlg, IDC_RADIO_24HOUR) == BST_CHECKED;
            g_showDate = IsDlgButtonChecked(hDlg, IDC_CHECK_DATE) == BST_CHECKED;
            g_showSeconds = IsDlgButtonChecked(hDlg, IDC_CHECK_SECONDS) == BST_CHECKED;
            g_showYear = IsDlgButtonChecked(hDlg, IDC_CHECK_YEAR) == BST_CHECKED;
            g_theme = IsDlgButtonChecked(hDlg, IDC_RADIO_DARK) == BST_CHECKED ? 0 :
                     (IsDlgButtonChecked(hDlg, IDC_RADIO_LIGHT) == BST_CHECKED ? 1 : 2);
            
            g_windowSize = IsDlgButtonChecked(hDlg, IDC_RADIO_SMALL) == BST_CHECKED ? 0 :
                          (IsDlgButtonChecked(hDlg, IDC_RADIO_MEDIUM) == BST_CHECKED ? 1 : 2);
            
            g_position = IsDlgButtonChecked(hDlg, IDC_RADIO_LEFT) == BST_CHECKED ? 0 :
                        (IsDlgButtonChecked(hDlg, IDC_RADIO_CENTER) == BST_CHECKED ? 1 : 2);
            
            g_autoHide = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTOHIDE) == BST_CHECKED;
            
            // Get hide delay (minutes:seconds)
            {
                wchar_t minText[32];
                GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_HIDEDELAY_MIN), minText, 32);
                g_hideDelayMinutes = _wtoi(minText);
                
                wchar_t secText[32];
                GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_HIDEDELAY_SEC), secText, 32);
                g_hideDelaySeconds = _wtoi(secText);
            }
            
            // Get font size
            wchar_t fontSizeText[32];
            GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_FONTSIZE), fontSizeText, 32);
            g_fontSize = _wtoi(fontSizeText);
            
            g_boldFont = IsDlgButtonChecked(hDlg, IDC_CHECK_BOLD) == BST_CHECKED;
            
            // Save to registry
            SaveSettings();
            
            // Refresh window - force recreate region for new size
            UpdateWindowPosition(g_isHovering);
            InvalidateRect(g_hwnd, NULL, TRUE);
            EndDialog(hDlg, IDOK);
            return TRUE;
        
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        
        // Handle Radio Button clicks to ensure mutual exclusion
        case IDC_RADIO_DARK:
        case IDC_RADIO_LIGHT:
        case IDC_RADIO_SYSTEM:
            CheckRadioButton(hDlg, IDC_RADIO_DARK, IDC_RADIO_SYSTEM, LOWORD(wParam));
            return TRUE;
        
        case IDC_RADIO_24HOUR:
        case IDC_RADIO_12HOUR:
            CheckRadioButton(hDlg, IDC_RADIO_24HOUR, IDC_RADIO_12HOUR, LOWORD(wParam));
            return TRUE;
        
        case IDC_RADIO_SMALL:
        case IDC_RADIO_MEDIUM:
        case IDC_RADIO_LARGE:
            CheckRadioButton(hDlg, IDC_RADIO_SMALL, IDC_RADIO_LARGE, LOWORD(wParam));
            return TRUE;
        
        case IDC_RADIO_LEFT:
        case IDC_RADIO_CENTER:
        case IDC_RADIO_RIGHT:
            CheckRadioButton(hDlg, IDC_RADIO_LEFT, IDC_RADIO_RIGHT, LOWORD(wParam));
            return TRUE;
        
        case IDC_BTN_APPLY:
            // Apply settings without closing dialog
            g_use24Hour = IsDlgButtonChecked(hDlg, IDC_RADIO_24HOUR) == BST_CHECKED;
            g_showDate = IsDlgButtonChecked(hDlg, IDC_CHECK_DATE) == BST_CHECKED;
            g_showSeconds = IsDlgButtonChecked(hDlg, IDC_CHECK_SECONDS) == BST_CHECKED;
            g_showYear = IsDlgButtonChecked(hDlg, IDC_CHECK_YEAR) == BST_CHECKED;
            g_theme = IsDlgButtonChecked(hDlg, IDC_RADIO_DARK) == BST_CHECKED ? 0 :
                     (IsDlgButtonChecked(hDlg, IDC_RADIO_LIGHT) == BST_CHECKED ? 1 : 2);
            
            g_windowSize = IsDlgButtonChecked(hDlg, IDC_RADIO_SMALL) == BST_CHECKED ? 0 :
                          (IsDlgButtonChecked(hDlg, IDC_RADIO_MEDIUM) == BST_CHECKED ? 1 : 2);
            
            g_position = IsDlgButtonChecked(hDlg, IDC_RADIO_LEFT) == BST_CHECKED ? 0 :
                        (IsDlgButtonChecked(hDlg, IDC_RADIO_CENTER) == BST_CHECKED ? 1 : 2);
            
            g_autoHide = IsDlgButtonChecked(hDlg, IDC_CHECK_AUTOHIDE) == BST_CHECKED;
            
            // Get hide delay (minutes:seconds)
            {
                wchar_t minText2[32];
                GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_HIDEDELAY_MIN), minText2, 32);
                g_hideDelayMinutes = _wtoi(minText2);
                
                wchar_t secText2[32];
                GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_HIDEDELAY_SEC), secText2, 32);
                g_hideDelaySeconds = _wtoi(secText2);
            }
            
            // Get font size
            wchar_t fontSizeText2[32];
            GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_FONTSIZE), fontSizeText2, 32);
            g_fontSize = _wtoi(fontSizeText2);
            
            g_boldFont = IsDlgButtonChecked(hDlg, IDC_CHECK_BOLD) == BST_CHECKED;
            
            // Save to registry
            SaveSettings();
            
            // Refresh window - force recreate region for new size
            UpdateWindowPosition(g_isHovering);
            InvalidateRect(g_hwnd, NULL, TRUE);
            
            // Show confirmation
            MessageBoxW(hDlg, L"Settings applied successfully!", L"Applied", MB_OK | MB_ICONINFORMATION);
            return TRUE;
        }
        return TRUE;
    
    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }
    return FALSE;
}

// Show settings dialog
void ShowSettingsDialog() {
    // Test if dialog resource exists
    HRSRC hRsrc = FindResourceW(GetModuleHandle(NULL), MAKEINTRESOURCEW(IDD_SETTINGS_DIALOG), (LPCWSTR)RT_DIALOG);
    if (!hRsrc) {
        MessageBoxW(g_hwnd, L"Dialog resource not found!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    // Create and show the settings dialog
    INT_PTR result = DialogBoxParamW(
        GetModuleHandle(NULL),
        MAKEINTRESOURCEW(IDD_SETTINGS_DIALOG),
        g_hwnd,
        SettingsDlgProc,
        0
    );
    
    if (result == -1) {
        DWORD error = GetLastError();
        wchar_t msg[256];
        swprintf_s(msg, 256, L"DialogBoxParamW failed! Error: %d", error);
        MessageBoxW(g_hwnd, msg, L"Error", MB_OK | MB_ICONERROR);
    }
}

// Check mouse position and show/hide island
void CheckMousePositionAndShowHide() {
    if (!g_autoHide || g_isAnimating) return;  // Don't check during animation
    
    POINT pt;
    GetCursorPos(&pt);
    
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    
    int baseWidth = SIZE_WIDTH[g_windowSize];
    int baseHeight = SIZE_HEIGHT[g_windowSize];
    int width = (int)(baseWidth * 1.3);
    int height = (int)(baseHeight * 1.3);
    
    // Calculate island position when visible
    int x, visibleY;
    switch (g_position) {
        case 0: // Left - near left edge
            x = workArea.left + 10;
            break;
        case 2: // Right - near right edge
            x = workArea.right - width - 10;
            break;
        default: // Center
            x = workArea.left + (workArea.right - workArea.left - width) / 2;
            break;
    }
    visibleY = workArea.top + 10;
    
    // Check if mouse is in the trigger area (smaller than island area)
    int triggerPadding = 10;
    bool mouseInTriggerArea = (pt.x >= x - triggerPadding && pt.x <= x + width + triggerPadding &&
                               pt.y >= visibleY - triggerPadding && pt.y <= visibleY + height + triggerPadding);
    
    if (g_isHidden) {
        // Currently hidden - ONLY show if mouse enters trigger area
        if (mouseInTriggerArea) {
            g_isHidden = false;
            g_isHovering = true;
            StartShowAnimation();
        }
        // IMPORTANT: Don't do anything else when hidden
        return;
    }
    
    // Currently visible
    if (mouseInTriggerArea) {
        // Mouse is in area - stay visible, cancel hide timer
        if (g_waitingForHide) {
            KillTimer(g_hwnd, 2);
            g_waitingForHide = false;
        }
    } else {
        // Mouse left area - start hide timer if not started
        if (!g_waitingForHide) {
            int hideDelayMs = (g_hideDelayMinutes * 60 + g_hideDelaySeconds) * 1000;
            SetTimer(g_hwnd, 2, hideDelayMs, NULL);
            g_waitingForHide = true;
        }
    }
}

// Start hide animation (slide up)
void StartHideAnimation() {
    g_isAnimating = true;
    g_isHiding = true;
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    g_animationY = workArea.top + 10;  // Start from visible position
    SetTimer(g_hwnd, ANIMATION_TIMER_ID, ANIMATION_INTERVAL, NULL);
}

// Start show animation (slide down)
void StartShowAnimation() {
    g_isAnimating = true;
    g_isHiding = false;
    g_animationY = g_hiddenY;  // Start from hidden position
    SetTimer(g_hwnd, ANIMATION_TIMER_ID, ANIMATION_INTERVAL, NULL);
}

// Update animation position
void UpdateAnimation() {
    if (!g_isAnimating) return;
    
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    
    if (g_isHiding) {
        // Moving up (hide)
        g_animationY -= ANIMATION_STEP;
        if (g_animationY <= g_hiddenY) {
            g_animationY = g_hiddenY;
            g_isAnimating = false;
            KillTimer(g_hwnd, ANIMATION_TIMER_ID);
        }
    } else {
        // Moving down (show)
        g_animationY += ANIMATION_STEP;
        if (g_animationY >= workArea.top + 10) {
            g_animationY = workArea.top + 10;
            g_isAnimating = false;
            g_isHovering = true;
            KillTimer(g_hwnd, ANIMATION_TIMER_ID);
        }
    }
    
    // Update window position with current animation Y
    UpdateWindowPositionWithY(g_animationY);
}

// Update window position with specific Y coordinate
void UpdateWindowPositionWithY(int y) {
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int screenWidth = workArea.right - workArea.left;
    
    int baseWidth = SIZE_WIDTH[g_windowSize];
    int baseHeight = SIZE_HEIGHT[g_windowSize];
    int width = (int)(baseWidth * 1.3);
    int height = (int)(baseHeight * 1.3);
    
    int x;
    switch (g_position) {
        case 0: // Left - near left edge
            x = workArea.left + 10;
            break;
        case 2: // Right - near right edge
            x = workArea.right - width - 10;
            break;
        default: // Center
            x = workArea.left + (screenWidth - width) / 2;
            break;
    }
    
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, width, height, 
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
    
    int radius = height / 2 * 15 / 10;
    HRGN hRgn = CreateRoundRectRgn(0, 0, width, height, radius, radius);
    SetWindowRgn(g_hwnd, hRgn, TRUE);
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
        RenderWindow();
        return 0;
    
    case WM_TIMER:
        if (wParam == 1) {
            // Timer 1: Update time display (every second)
            InvalidateRect(hwnd, NULL, FALSE);
        } else if (wParam == 2) {
            // Timer 2: Auto-hide delay timer
            if (g_autoHide && g_waitingForHide && !g_isAnimating) {
                g_isHidden = true;  // Mark as hidden
                g_waitingForHide = false;
                StartHideAnimation();
            }
        } else if (wParam == 3) {
            // Timer 3: Mouse position check (every 100ms)
            CheckMousePositionAndShowHide();
        } else if (wParam == 4) {
            // Timer 4: Animation timer
            UpdateAnimation();
        }
        return 0;
    
    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH bgBrush = CreateSolidBrush(GetBackgroundColor());
        FillRect(hdc, &rect, bgBrush);
        DeleteObject(bgBrush);
        return TRUE;
    }
    
    case WM_MOUSEMOVE: {
        // Always handle hover expansion (regardless of auto-hide setting)
        if (!g_isHovering) {
            g_isHovering = true;
            UpdateWindowPosition(true); // Expand on hover
            
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }
        return 0;
    }
    
    case WM_MOUSELEAVE: {
        g_isHovering = false;
        UpdateWindowPosition(false); // Shrink when not hovering
        // Timer 3 handles auto-hide logic
        return 0;
    }
    
    case WM_LBUTTONDOWN:
        OpenTimeSettings();
        return 0;
    
    case WM_RBUTTONDOWN:
        ShowSettingsDialog();
        return 0;
    
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
        break;
    
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        KillTimer(hwnd, 3);
        KillTimer(hwnd, 4);
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Set auto startup
void SetAutoStartup() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, 
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::wstring fullPath = L"\"";
        fullPath += path;
        fullPath += L"\"";
        
        RegSetValueExW(hKey, L"DynamicIsland", 0, REG_SZ,
                      reinterpret_cast<const BYTE*>(fullPath.c_str()),
                      (DWORD)((fullPath.length() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}

// Main function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {
    // Temporarily disable mutex check for testing
    // HANDLE hMutex = CreateMutexW(NULL, TRUE, L"DynamicIsland_Mutex");
    // if (GetLastError() == ERROR_ALREADY_EXISTS) {
    //     MessageBoxW(NULL, L"Dynamic Island is already running!", L"Info", MB_OK | MB_ICONINFORMATION);
    //     return 0;
    // }
    
    CoInitialize(NULL);
    SetAutoStartup();
    
    InitCommonControls();
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"DynamicIslandClass";
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Failed to register window class!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    
    // Load settings first to get initial size and position
    LoadSettings();
    
    // Calculate initial position based on settings
    int baseWidth = SIZE_WIDTH[g_windowSize];
    int baseHeight = SIZE_HEIGHT[g_windowSize];
    int x;
    switch (g_position) {
        case 0: // Left
            x = workArea.left + (workArea.right - workArea.left - baseWidth) / 4;
            break;
        case 2: // Right
            x = workArea.left + (workArea.right - workArea.left - baseWidth) * 3 / 4;
            break;
        default: // Center
            x = workArea.left + (workArea.right - workArea.left - baseWidth) / 2;
            break;
    }
    int y = workArea.top + 10;
    
    g_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"DynamicIslandClass",
        L"DynamicIsland",
        WS_POPUP | WS_VISIBLE,
        x, y, baseWidth, baseHeight,
        NULL, NULL, hInstance, NULL
    );
    
    if (!g_hwnd) {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Failed to create window! Error code: %d", error);
        MessageBoxW(NULL, errorMsg, L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Create capsule shape region - 1.5x radius for more rounded corners
    int radius = (baseHeight / 2) * 15 / 10;  // 1.5x radius
    HRGN hRgn = CreateRoundRectRgn(0, 0, baseWidth, baseHeight, radius, radius);
    SetWindowRgn(g_hwnd, hRgn, TRUE);
    
    // Show window
    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    SetForegroundWindow(g_hwnd);
    
    // Timer for auto-update
    SetTimer(g_hwnd, 1, 1000, TimerProc);
    
    // Timer for mouse position check (auto-hide)
    SetTimer(g_hwnd, 3, MOUSE_CHECK_INTERVAL, NULL);
    
    // Force initial render
    InvalidateRect(g_hwnd, NULL, TRUE);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    CoUninitialize();
    
    return (int)msg.wParam;
}

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}
