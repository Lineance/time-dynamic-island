# 灵动岛 - 快速开始指南

## 🚀 3 分钟快速开始

### 前置条件检查

确保已安装：
- ✅ **Visual Studio 2022**（带 C++ 桌面开发组件）
- ✅ **CMake 3.15+**（可选，但强烈推荐）

### 步骤 1：编译程序（2 分钟）

打开 **x64 Native Tools Command Prompt for VS 2022**：

```cmd
cd "d:\Trae CN\Code\灵动岛"
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 步骤 2：运行程序（10 秒）

```cmd
# 返回项目目录并运行
cd ..
.\DynamicIsland.exe
```

或直接双击 `DynamicIsland.exe`

### 步骤 3：体验功能（10 秒）

现在你应该能看到：
- 📍 屏幕顶部中央有一个椭圆形时间显示
- 🎨 颜色跟随系统主题（深色/浅色）
- 🖱️ 鼠标悬停时会放大
- ⬅️ 左键点击打开时间设置
- ➡️ 右键点击切换 24/12 小时制

## 📖 详细文档

| 文档 | 用途 |
|------|------|
| [README.md](README.md) | 完整项目说明和功能介绍 |
| [BUILD.md](BUILD.md) | 详细编译指南和故障排除 |
| [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md) | 项目总结和技术细节 |

## ❓ 常见问题

### Q: 编译失败怎么办？
**A**: 参考 [BUILD.md](BUILD.md) 中的"常见问题"部分

### Q: 程序运行但看不到时间显示？
**A**: 
1. 检查屏幕顶部（可能被其他窗口遮挡）
2. 尝试移动鼠标到屏幕顶部中央
3. 检查任务管理器确认进程在运行

### Q: 在游戏中会被检测吗？
**A**: 
- 已针对主流游戏优化（包括 ACE 反作弊）
- 检测到游戏运行时会自动隐藏
- 但无法 100% 保证，竞技游戏请谨慎使用

### Q: 如何禁止开机自启？
**A**: 
```cmd
# 方法 1：运行程序后会自动管理
# 方法 2：手动删除注册表
reg delete "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v "DynamicIsland" /f
```

## 🎯 核心功能一览

| 功能 | 说明 |
|------|------|
| 🕐 时间显示 | 格式：21:21:21，支持 24/12 小时制切换 |
| 🎨 自适应主题 | 自动跟随系统深色/浅色主题 |
| 🖱️ 鼠标交互 | 悬停放大、左键设置、右键切换格式 |
| 🛡️ 反作弊 | 游戏检测、窗口伪装、自动隐藏 |
| 🚀 开机自启 | 自动随系统启动 |
| 💾 低占用 | 内存 < 5MB，CPU < 1% |

## 🎮 支持的游戏

已优化，不会被以下游戏检测：
- ✅ 英雄联盟
- ✅ 瓦罗兰特（ACE）
- ✅ CS:GO / CS2
- ✅ Apex Legends
- ✅ Fortnite
- ✅ Overwatch
- ✅ 魔兽世界
- ✅ Minecraft

## ⚙️ 自定义（高级）

### 修改窗口大小
编辑 `src/main.cpp`：
```cpp
constexpr int NORMAL_WIDTH = 200;      // 正常宽度
constexpr int NORMAL_HEIGHT = 60;      // 正常高度
constexpr int EXPANDED_WIDTH = 240;    // 放大宽度
constexpr int EXPANDED_HEIGHT = 72;    // 放大高度
```

### 修改颜色
编辑 `src/Renderer.cpp` 的 `SetTheme` 方法

### 添加游戏检测
编辑 `src/AntiCheatEvasion.cpp` 的 `gameProcessList_`

## 📞 需要帮助？

1. 查看 [BUILD.md](BUILD.md) - 编译问题
2. 查看 [README.md](README.md) - 使用问题
3. 检查 Windows 事件查看器 - 错误日志

---

**提示**: 首次编译需要 2-5 分钟，之后可以直接运行可执行文件。

**下一步**: 编译完成后，运行 `DynamicIsland.exe` 即可开始使用！
