# 🚀 灵动岛 - 快速使用卡片

## 1️⃣ 编译（3 步）

### PowerShell（推荐）
```powershell
cd "d:\Trae CN\Code\灵动岛"
.\compile_only.ps1
```

**✅ 完成**：生成 `obj/main.obj`

---

## 2️⃣ 创建可执行文件（可选）

创建 `link.bat`：
```batch
@echo off
link /nologo /OUT:DynamicIsland.exe /SUBSYSTEM:WINDOWS ^
    obj\main.obj ^
    user32.lib gdi32.lib gdiplus.lib shell32.lib ^
    ole32.lib comctl32.lib dwmapi.lib advapi32.lib
```

运行：
```cmd
link.bat
```

**✅ 完成**：生成 `DynamicIsland.exe`

---

## 3️⃣ 运行

### 方法 1：命令行
```cmd
.\DynamicIsland.exe
```

### 方法 2：双击
在文件资源管理器中双击 `DynamicIsland.exe`

---

## 4️⃣ 使用

### 屏幕显示
```
┌─────────────────┐
│    21:21:21     │  ← 屏幕顶部中央
└─────────────────┘
```

### 鼠标操作

| 操作 | 效果 |
|------|------|
| **悬停** | 灵动岛放大 |
| **左键点击** | 打开时间设置 |
| **右键点击** | 切换 24/12 小时制 |

### 主题
- 自动跟随系统（深色/浅色）
- 无需手动设置

---

## 5️⃣ 验证

### 检查运行状态
1. 按 `Ctrl + Shift + Esc` 打开任务管理器
2. 查找 `DynamicIsland.exe`
3. 内存应 < 5MB

### 检查功能
- [ ] 时间显示正确
- [ ] 悬停时放大
- [ ] 左键打开时间设置
- [ ] 右键切换格式
- [ ] 主题自动切换

---

## ⚡ 常见问题速查

### 无法启动？
- 安装 Visual C++ Redistributable
- 右键 → 以管理员身份运行

### 时间不对？
- 右键切换 24/12 小时制
- 检查系统时间

### 游戏中被检测？
- 关闭程序
- 避免在竞技游戏使用

### 开机不自启？
- 检查注册表：`HKEY_CURRENT_USER\...\Run`
- 手动添加启动项

---

## 📊 性能指标

| 指标 | 预期值 |
|------|--------|
| 内存 | < 5MB |
| CPU | < 1% |
| 启动时间 | < 2 秒 |

---

## 🎯 快速命令

```powershell
# 编译
.\compile_only.ps1

# 验证无.exe
.\verify_build.ps1

# 运行
.\DynamicIsland.exe
```

---

## 📁 文件位置

- **源代码**：`src/main.cpp`
- **对象文件**：`obj/main.obj`
- **可执行文件**：`DynamicIsland.exe`（如果已链接）
- **文档**：`USAGE_GUIDE.md`（详细指南）

---

**使用愉快！** 🎉

**提示**：详细说明请查看 `USAGE_GUIDE.md`
