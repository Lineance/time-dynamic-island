# 灵动岛 (Dynamic Island) - Windows 11 时间显示程序

一个轻量级的 Windows 11 后台程序，在屏幕上方显示时间，具有类似 macOS 灵动岛的效果。

## ✨ 功能特性

- 🕐 **实时时间显示**：格式为 `21:21:21` 的椭圆形时间显示
- 🎨 **自适应主题**：自动跟随系统深色/浅色主题
- 🖱️ **鼠标交互**：
  - 悬停时放大效果
  - 左键点击：打开系统时间设置
  - 右键点击：切换 24/12 小时制
- 🛡️ **反作弊规避**：专为游戏优化，避免被 ACE 等反作弊系统检测
- 🚀 **开机自启**：自动随系统启动
- 💾 **极低内存占用**：仅 2-5MB

## 🛠️ 编译要求

### 必需
- **Visual Studio 2022**（包含 C++ 桌面开发组件）
- **CMake 3.15+**（可选，但强烈推荐）

### 可选
- Git（用于克隆仓库）

## 📦 编译步骤

### 方法一：使用 CMake（推荐）

1. **打开开发者命令提示符**
   ```
   开始菜单 → Visual Studio 2022 → 开发人员命令提示符
   ```

2. **导航到项目目录**
   ```cmd
   cd "d:\Trae CN\Code\灵动岛"
   ```

3. **创建并进入构建目录**
   ```cmd
   mkdir build
   cd build
   ```

4. **配置 CMake**
   ```cmd
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

5. **编译项目**
   ```cmd
   cmake --build . --config Release
   ```

6. **获取可执行文件**
   - 编译后的文件位于：`build\bin\Release\DynamicIsland.exe`

### 方法二：使用 Visual Studio IDE

1. **打开解决方案**
   - 双击 `build\DynamicIsland.sln`（需要先运行 CMake 配置）

2. **设置配置**
   - 选择 `Release` 配置
   - 选择 `x64` 平台

3. **生成解决方案**
   - 按 `Ctrl+Shift+B` 或选择 菜单 → 生成 → 生成解决方案

### 方法三：使用批处理脚本

```cmd
build.bat Release
```

## 🚀 使用方法

1. **运行程序**
   ```
   双击 DynamicIsland.exe
   ```

2. **设置开机自启**
   - 程序会自动设置开机自启动
   - 如需禁用，请手动删除注册表项：
     ```
     HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
     删除 "DynamicIsland" 值
     ```

3. **操作说明**
   - **鼠标悬停**：灵动岛会放大
   - **左键点击**：打开系统时间和日期设置
   - **右键点击**：在 24 小时制和 12 小时制之间切换

4. **退出程序**
   - 在任务管理器中结束 `DynamicIsland.exe` 进程
   - 或使用任务栏右下角的系统托盘图标退出（如果实现）

## 🎯 技术细节

### 内存占用
- **空闲时**：< 2MB
- **渲染时**：< 5MB
- **优化措施**：
  - 单线程消息循环
  - 仅在时间变化时重绘（每秒 1 次）
  - GDI+ 资源及时释放

### 反作弊规避
程序采用以下措施避免被游戏反作弊系统（如 ACE）检测：

1. **窗口样式伪装**：使用 `WS_EX_TOOLWINDOW` 样式，不在 Alt+Tab 中显示
2. **无注入行为**：不注入任何 DLL 到其他进程
3. **无钩子技术**：不使用 `SetWindowsHookEx` 等钩子 API
4. **游戏检测**：检测到全屏游戏时自动隐藏
5. **进程列表检测**：检测已知游戏进程（可配置）

### 支持的游戏（已优化）
- 英雄联盟 (League of Legends)
- 瓦罗兰特 (Valorant)
- CS:GO / CS2
- Apex Legends
- Fortnite
- Overwatch
- 魔兽世界
- Minecraft

## 📁 项目结构

```
灵动岛/
├── CMakeLists.txt              # CMake 配置
├── build.bat                   # Windows 批处理构建脚本
├── compile.ps1                 # PowerShell 构建脚本
├── README.md                   # 项目说明文档
├── .gitignore                  # Git 忽略文件
├── include/                    # 头文件
│   ├── WindowManager.h
│   ├── Renderer.h
│   ├── TimeService.h
│   ├── ThemeMonitor.h
│   ├── InteractionHandler.h
│   ├── AntiCheatEvasion.h
│   ├── StartupManager.h
│   └── utils.h
├── src/                        # 源文件
│   ├── main.cpp
│   ├── WindowManager.cpp
│   ├── Renderer.cpp
│   ├── TimeService.cpp
│   ├── ThemeMonitor.cpp
│   ├── InteractionHandler.cpp
│   ├── AntiCheatEvasion.cpp
│   ├── StartupManager.cpp
│   └── utils.cpp
├── resources/                  # 资源文件
│   ├── app.rc
│   └── app.manifest
└── build/                      # 构建输出目录
```

## ⚙️ 配置（可选）

目前程序使用默认配置，未来版本将支持配置文件 `config.ini`：

```ini
[General]
Use24HourFormat=true
AutoStartup=true
StartMinimized=false

[Appearance]
Width=200
Height=60
ExpandedWidth=240
ExpandedHeight=72

[Games]
HideWhenGameRunning=true
GameList=valorant.exe,leagueoflegends.exe,csgo.exe
```

## 🐛 故障排除

### 问题：程序无法启动
**解决方案**：
1. 确保已安装 Visual Studio 2022 C++ 运行时
2. 以管理员身份运行
3. 检查 Windows 事件查看器获取详细错误

### 问题：时间显示不正确
**解决方案**：
1. 检查系统时间设置
2. 右键点击切换时间格式

### 问题：在游戏中被反作弊检测
**解决方案**：
1. 确保程序已更新到最新版本
2. 在游戏设置中禁用覆盖显示
3. 如仍被检测，请提交 Issue 并提供游戏名称

### 问题：开机不自启
**解决方案**：
1. 检查注册表项是否存在
2. 手动添加开机启动项
3. 检查杀毒软件是否阻止

## 📝 开发说明

### 添加新的游戏进程检测

编辑 `src/AntiCheatEvasion.cpp`，在 `gameProcessList_` 初始化中添加：

```cpp
gameProcessList_ = {
    L"valorant.exe",
    L"leagueoflegends.exe",
    L"your_game.exe",  // 添加这里
    // ...
};
```

### 修改窗口尺寸

编辑 `src/main.cpp`：

```cpp
constexpr int NORMAL_WIDTH = 200;     // 修改正常宽度
constexpr int NORMAL_HEIGHT = 60;     // 修改正常高度
constexpr int EXPANDED_WIDTH = 240;   // 修改放大宽度
constexpr int EXPANDED_HEIGHT = 72;   // 修改放大高度
```

### 修改颜色方案

编辑 `src/Renderer.cpp` 的 `SetTheme` 方法：

```cpp
void Renderer::SetTheme(Theme theme) {
    currentTheme_ = theme;
    
    if (theme == Theme::Light) {
        backgroundColor_ = RGB(255, 255, 255);  // 浅色主题背景色
        textColor_ = RGB(0, 0, 0);              // 浅色主题文字色
    } else {
        backgroundColor_ = RGB(0, 0, 0);        // 深色主题背景色
        textColor_ = RGB(255, 255, 255);        // 深色主题文字色
    }
}
```

## 📄 许可证

本项目仅供学习和研究使用。

## ⚠️ 免责声明

1. 本程序仅供个人使用，请勿在禁止使用覆盖显示的游戏服务器上使用
2. 使用本程序导致的任何封号或其他后果，开发者不承担任何责任
3. 请在游戏允许的规则范围内使用本程序

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📧 联系方式

如有问题或建议，请提交 Issue。

---

**编译时间**: 约 2-5 分钟（取决于硬件配置）
**代码量**: ~1000 行 C++ 代码
**最后更新**: 2026-03-10
