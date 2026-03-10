# 灵动岛 - 快速编译指南

## 前提条件

请确保已安装以下软件：

### 1. Visual Studio 2022
- 下载地址：https://visualstudio.microsoft.com/zh-hans/downloads/
- 安装时必须勾选：**"使用 C++ 的桌面开发"** 工作负载

### 2. CMake（推荐）
- 下载地址：https://cmake.org/download/
- 选择 `cmake-3.x.x-windows-x86_64.msi` 安装
- 安装时勾选 "Add CMake to the system PATH"

## 快速编译（3 步）

### 步骤 1：打开开发者命令提示符

```
方式 1：开始菜单 → Visual Studio 2022 → x64 Native Tools Command Prompt for VS 2022
方式 2：Win + R → 输入 "Developer Command Prompt for VS 2022" → 回车
```

### 步骤 2：导航到项目目录

```cmd
cd "d:\Trae CN\Code\灵动岛"
```

### 步骤 3：执行编译命令

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## 编译完成

编译成功后，可执行文件位置：

```
d:\Trae CN\Code\灵动岛\build\bin\Release\DynamicIsland.exe
```

## 常见问题

### Q1: 提示 "cmake 不是可识别的命令"
**A**: 未安装 CMake 或未添加到 PATH。可以：
- 重新安装 CMake 并勾选 "Add to PATH"
- 或使用 Visual Studio IDE 打开 `build/DynamicIsland.sln` 进行编译

### Q2: 提示找不到 Visual Studio
**A**: 请确保使用 **"x64 Native Tools Command Prompt for VS 2022"**，而不是普通的命令提示符

### Q3: 编译时提示缺少 Windows SDK
**A**: 重新运行 Visual Studio Installer，添加 "Windows 10/11 SDK" 组件

### Q4: 链接错误，找不到库文件
**A**: 确保在 CMakeLists.txt 中正确配置了 Windows 库路径（通常不需要手动配置）

## 使用 Visual Studio IDE 编译（备选方法）

如果命令行编译失败，可以使用 IDE：

1. **生成解决方案文件**
   ```cmd
   cd "d:\Trae CN\Code\灵动岛"
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

2. **打开解决方案**
   - 双击 `build/DynamicIsland.sln`

3. **设置编译配置**
   - 工具栏选择：`Release` | `x64`

4. **生成**
   - 菜单：生成 → 生成解决方案 (或按 Ctrl+Shift+B)

## 性能优化选项

如果需要更小的可执行文件，可以修改 CMakeLists.txt：

```cmake
# 添加以下选项
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
```

## 调试版本编译

如需编译调试版本（用于开发调试）：

```cmd
cmake --build . --config Debug
```

调试版本文件位置：`build\bin\Debug\DynamicIsland.exe`

## 验证编译结果

编译完成后，检查以下项目：

- ✅ 文件大小：约 50-200KB（Release 版本）
- ✅ 内存占用：运行后 < 5MB
- ✅ 窗口显示：屏幕顶部出现椭圆形时间显示
- ✅ 主题切换：切换系统主题后颜色自动变化
- ✅ 鼠标交互：悬停放大、左右键点击正常

## 下一步

编译成功后，请阅读 [README.md](README.md) 了解使用方法和配置说明。

---

**提示**: 如果编译过程中遇到任何问题，请检查 Visual Studio 和 CMake 版本是否符合要求。
