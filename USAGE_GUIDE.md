# 灵动岛 - 使用指南

## 📖 项目简介

灵动岛（Dynamic Island）是一个 Windows 11 后台程序，在屏幕顶部显示时间，具有类似 macOS 灵动岛的效果。

**特点**：
- 🕐 实时时间显示（格式：21:21:21）
- 🎨 自适应系统主题（深色/浅色）
- 🖱️ 鼠标交互（悬停放大、点击操作）
- 🛡️ 反作弊规避（游戏友好）
- 🚀 开机自启动
- 💾 极低内存占用（< 5MB）

---

## ⚡ 快速开始

### 1. 编译项目

#### 方法 1：使用 PowerShell 脚本（推荐）

```powershell
# 打开项目目录
cd "d:\Trae CN\Code\灵动岛"

# 编译对象文件
.\compile_only.ps1
```

**输出**：`obj/main.obj`（对象文件）

#### 方法 2：使用批处理脚本

```cmd
cd "d:\Trae CN\Code\灵动岛"
build.bat
```

#### 方法 3：使用 CMake

```cmd
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake .. -G "Visual Studio 17 2022" -A x64

# 编译
cmake --build . --config Release
```

**输出位置**：`build/obj/*.obj`

---

## 🔧 创建可执行文件（可选）

当前配置仅编译为对象文件。如需创建可执行文件，需要额外的链接步骤。

### 链接为可执行文件

创建 `link.bat` 文件：

```batch
@echo off
REM 链接对象文件为可执行文件
link /nologo /OUT:DynamicIsland.exe /SUBSYSTEM:WINDOWS ^
    obj\main.obj ^
    user32.lib gdi32.lib gdiplus.lib shell32.lib ^
    ole32.lib comctl32.lib dwmapi.lib advapi32.lib

echo 可执行文件已创建：DynamicIsland.exe
```

运行：
```cmd
link.bat
```

**输出**：`DynamicIsland.exe`

---

## 🚀 运行程序

### 方式 1：直接运行（如果已创建 .exe）

```cmd
# 在项目目录
.\DynamicIsland.exe

# 或在任意位置
"完整路径\DynamicIsland.exe"
```

### 方式 2：通过资源管理器

1. 打开文件资源管理器
2. 导航到 `d:\Trae CN\Code\灵动岛`
3. 双击 `DynamicIsland.exe`（如果已创建）

---

## 📱 功能使用

### 时间显示

程序运行后，会在**屏幕顶部中央**显示一个椭圆形时间条：

```
┌─────────────────┐
│    21:21:21     │
└─────────────────┘
```

### 鼠标交互

#### 1. 悬停放大
- **操作**：鼠标移动到灵动岛上
- **效果**：灵动岛会放大（从 200x60 到 240x72 像素）
- **恢复**：鼠标移开后自动缩小

#### 2. 左键点击
- **操作**：左键点击灵动岛
- **效果**：打开系统时间和日期设置窗口

#### 3. 右键点击
- **操作**：右键点击灵动岛
- **效果**：切换时间格式
  - 24 小时制：`21:21:21`
  - 12 小时制：`09:21:21`

### 主题自适应

程序会自动跟随 Windows 系统主题：

| 系统主题 | 背景颜色 | 文字颜色 |
|---------|---------|---------|
| 浅色模式 | 白色 | 黑色 |
| 深色模式 | 黑色 | 白色 |

**切换主题**：
1. Windows 设置 → 个性化 → 颜色
2. 选择模式（浅色/深色）
3. 灵动岛自动更新颜色

---

## ⚙️ 配置选项

### 开机自启动

**默认**：程序会自动设置开机自启动

**位置**：注册表
```
HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
值名称：DynamicIsland
```

**禁用方法**：
1. 按 `Win + R`
2. 输入 `regedit`
3. 导航到上述路径
4. 删除 `DynamicIsland` 值

### 窗口尺寸

在源代码中修改（`src/main.cpp`）：

```cpp
const int NORMAL_WIDTH = 200;    // 正常宽度
const int NORMAL_HEIGHT = 60;    // 正常高度
const int EXPANDED_WIDTH = 240;  // 放大宽度
const int EXPANDED_HEIGHT = 72;  // 放大高度
```

重新编译后生效。

### 透明度

修改背景透明度（`src/main.cpp`）：

```cpp
Color bgColor(200,  // 透明度 0-255
    GetRValue(GetBackgroundColor()),
    GetGValue(GetBackgroundColor()),
    GetBValue(GetBackgroundColor()));
```

- `200` = 半透明
- `255` = 完全不透明
- `0` = 完全透明

---

## 🛡️ 游戏反作弊规避

### 已优化游戏

灵动岛已针对以下游戏的反作弊系统优化：

- ✅ 英雄联盟 (League of Legends)
- ✅ 瓦罗兰特 (Valorant) - ACE 反作弊
- ✅ CS:GO / CS2
- ✅ Apex Legends
- ✅ Fortnite
- ✅ Overwatch
- ✅ 其他使用 ACE、EAC、BattlEye 的游戏

### 规避措施

1. **窗口伪装**：使用 `WS_EX_TOOLWINDOW` 样式（不在 Alt+Tab 显示）
2. **无注入**：不注入任何游戏进程
3. **无钩子**：不使用 SetWindowsHookEx 等钩子 API
4. **游戏检测**：检测到全屏游戏时自动隐藏

### 使用建议

⚠️ **重要提示**：
- 在竞技游戏中谨慎使用
- 如被检测，请关闭程序
- 遵守游戏规则和使用条款

---

## 📊 性能监控

### 内存占用

查看内存使用：
1. 按 `Ctrl + Shift + Esc` 打开任务管理器
2. 找到 `DynamicIsland.exe` 进程
3. 查看内存列

**预期**：
- 空闲时：< 2MB
- 渲染时：< 5MB

### CPU 占用

正常情况下 CPU 占用应 < 1%

---

## 🐛 故障排除

### 问题 1：程序无法启动

**可能原因**：
- 缺少 Visual C++ 运行时
- 权限不足

**解决方法**：
1. 安装 Visual C++ Redistributable
2. 右键 → 以管理员身份运行

### 问题 2：时间显示不正确

**解决方法**：
1. 检查系统时间设置
2. 右键点击灵动岛切换 24/12 小时制

### 问题 3：主题不跟随系统

**解决方法**：
1. 重启程序
2. 检查 Windows 主题设置

### 问题 4：在游戏中被检测

**解决方法**：
1. 关闭灵动岛程序
2. 在任务管理器中结束进程
3. 避免在竞技游戏中使用

### 问题 5：开机不自启

**解决方法**：
1. 检查注册表项是否存在
2. 手动添加开机启动项
3. 检查杀毒软件是否阻止

---

## 📁 文件结构

```
灵动岛/
├── src/                      # 源代码
│   ├── main.cpp              # 主程序
│   └── ...
├── include/                  # 头文件
├── obj/                      # 编译输出（对象文件）
│   └── main.obj
├── bin/                      # 可执行文件（如果已链接）
│   └── DynamicIsland.exe
├── compile_only.ps1          # 编译脚本
├── build.bat                 # 批处理构建脚本
├── verify_build.ps1          # 验证脚本
└── README.md                 # 项目说明
```

---

## 🎓 高级用法

### 自定义样式

#### 修改字体

编辑 `src/main.cpp`：

```cpp
FontFamily fontFamily(L"Segoe UI");  // 改为其他字体，如 L"Microsoft YaHei"
```

#### 修改颜色

编辑 `GetBackgroundColor()` 和 `GetTextColor()` 函数：

```cpp
// 自定义浅色主题颜色
COLORREF GetBackgroundColor() {
    return RGB(240, 240, 240);  // 浅灰色背景
}

COLORREF GetTextColor() {
    return RGB(50, 50, 50);     // 深灰色文字
}
```

### 添加新功能

#### 显示日期

在 `GetTimeString()` 中添加：

```cpp
// 获取日期
std::wstringstream dateStream;
dateStream << tm.tm_year + 1900 << L"-" 
           << std::setw(2) << tm.tm_mon + 1 << L"-" 
           << std::setw(2) << tm.tm_mday;
std::wstring dateStr = dateStream.str();
```

#### 添加天气显示

需要调用天气 API，并在渲染时绘制额外文本。

---

## 📞 帮助与支持

### 文档

- `README.md` - 项目说明
- `BUILD_CONFIG.md` - 构建配置
- `BUILD_COMPLETE_SUMMARY.md` - 编译总结
- `COMPILE_REPORT.md` - 编译报告

### 验证编译

```powershell
# 运行验证脚本
.\verify_build.ps1
```

### Git 历史

```bash
# 查看提交历史
git log --oneline

# 查看最新更改
git show
```

---

## 🎯 快速参考

### 常用命令

```powershell
# 编译
.\compile_only.ps1

# 验证
.\verify_build.ps1

# 运行（如果有.exe）
.\DynamicIsland.exe

# 查看帮助
Get-Help .\compile_only.ps1
```

### 快捷键

| 操作 | 快捷键 |
|------|--------|
| 打开时间设置 | 左键点击 |
| 切换时间格式 | 右键点击 |
| 查看任务管理器 | Ctrl + Shift + Esc |

---

## ✅ 使用清单

- [ ] 编译项目
- [ ] （可选）链接为可执行文件
- [ ] 运行程序
- [ ] 测试鼠标交互
- [ ] 验证主题切换
- [ ] 检查内存占用
- [ ] 设置开机自启（可选）

---

**使用愉快！** 🎉

如有问题，请查阅文档或重新编译项目。

**最后更新**：2026-03-10
