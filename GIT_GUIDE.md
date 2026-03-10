# Git 仓库使用指南

## 📦 仓库信息

- **仓库名称**: 灵动岛 (Dynamic Island)
- **初始提交**: 已完成
- **文件数量**: 28 个文件
- **代码量**: ~2589 行

## 🚀 快速开始

### 1. 克隆仓库（如果是从远程克隆）

```bash
git clone <repository-url>
cd 灵动岛
```

### 2. 查看当前状态

```bash
git status
```

### 3. 查看提交历史

```bash
git log --oneline
```

## 📝 常用 Git 命令

### 日常开发

```bash
# 查看修改的文件
git status

# 添加所有修改到暂存区
git add .

# 提交更改
git commit -m "描述你的更改"

# 推送到远程仓库
git push origin main
```

### 分支管理

```bash
# 创建新分支
git branch feature-name

# 切换到新分支
git checkout feature-name

# 或者创建并切换（一条命令）
git checkout -b feature-name

# 合并分支到 main
git checkout main
git merge feature-name

# 删除分支
git branch -d feature-name
```

### 查看差异

```bash
# 查看未暂存的更改
git diff

# 查看已暂存的更改
git diff --cached

# 查看特定文件的更改
git diff filename.cpp
```

### 撤销操作

```bash
# 撤销工作区的修改
git checkout -- filename

# 撤销暂存区的修改
git reset HEAD filename

# 撤销最后一次提交（保留更改）
git reset --soft HEAD~1

# 撤销最后一次提交（丢弃更改）
git reset --hard HEAD~1
```

## 🌿 推荐分支策略

### 分支命名

- `main` - 主分支，稳定版本
- `develop` - 开发分支
- `feature/xxx` - 新功能分支
- `bugfix/xxx` - Bug 修复分支
- `hotfix/xxx` - 紧急修复分支

### 工作流示例

```bash
# 1. 从 develop 创建新功能分支
git checkout develop
git checkout -b feature/time-format

# 2. 开发功能并提交
# ... 编写代码 ...
git add .
git commit -m "feat: 添加自定义时间格式支持"

# 3. 推送到远程
git push origin feature/time-format

# 4. 创建 Pull Request (在 GitHub/GitLab 上)

# 5. 合并后删除分支
git checkout develop
git pull origin develop
git branch -d feature/time-format
```

## 📋 提交消息规范

### 格式

```
<type>: <subject>

<body>

<footer>
```

### Type 类型

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式（不影响代码运行）
- `refactor`: 重构（既不是新功能也不是 Bug 修复）
- `perf`: 性能优化
- `test`: 测试相关
- `chore`: 构建过程或辅助工具变动

### 示例

```bash
# 新功能
git commit -m "feat: 添加日期显示功能"

# Bug 修复
git commit -m "fix: 修复时间格式切换错误"

# 文档更新
git commit -m "docs: 更新编译指南"

# 性能优化
git commit -m "perf: 优化 GDI+ 资源管理"

# 详细提交
git commit -m "feat: 添加主题自定义功能

- 支持用户自定义背景颜色
- 支持用户自定义文字颜色
- 添加配置文件支持

Closes #123"
```

## 🔧 Git 配置（可选）

### 全局配置（如果未配置）

```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
git config --global core.autocrlf true  # Windows 推荐
git config --global core.editor "notepad"  # 设置编辑器
```

### 查看配置

```bash
git config --list
```

### 设置别名

```bash
git config --global alias.st status
git config --global alias.co checkout
git config --global alias.ci commit
git config --global alias.br branch
git config --global alias.last "log -1 HEAD"
```

## 📊 项目统计

### 查看代码统计

```bash
# 查看每行代码的贡献者
git blame filename.cpp

# 查看提交统计
git shortlog -sn

# 查看代码行数变化
git diff --stat
```

### 使用 gitinspector（可选）

```bash
# 安装
pip install gitinspector

# 生成报告
gitinspector --format=html .
```

## 🌐 远程仓库

### 添加远程仓库

```bash
# 添加 GitHub 远程仓库
git remote add origin https://github.com/username/DynamicIsland.git

# 添加 Gitee 远程仓库
git remote add origin https://gitee.com/username/DynamicIsland.git
```

### 查看远程仓库

```bash
git remote -v
```

### 推送代码

```bash
# 首次推送（设置上游分支）
git push -u origin main

# 后续推送
git push
```

### 拉取代码

```bash
git pull origin main
```

## ⚠️ 注意事项

### .gitignore

本项目已配置 `.gitignore` 文件，以下文件不会被提交：

- `build/` - 编译输出目录
- `*.exe` - 可执行文件
- `*.obj`, `*.pdb` - 编译中间文件
- `.vscode/`, `.vs/` - IDE 配置
- `DynamicIsland.exe` - 编译产物

### 大文件

避免提交大文件（>10MB），如需要管理大文件，使用 Git LFS：

```bash
git lfs install
git lfs track "*.png"
git lfs track "*.ico"
```

### 敏感信息

**切勿提交**以下敏感信息：

- API 密钥
- 密码
- 数据库连接字符串
- 个人身份信息

## 🔍 故障排除

### 问题：提交时提示 LF 将被替换为 CRLF

**解决方案**：这是正常的 Windows 行为。如需禁用：

```bash
git config core.autocrlf false
```

### 问题：误提交了大文件

**解决方案**：

```bash
# 撤销最后一次提交
git reset --soft HEAD~1

# 从暂存区移除大文件
git reset HEAD largefile.zip

# 重新提交（不包含大文件）
git commit -m "提交消息"
```

### 问题：需要修改最后一次提交消息

**解决方案**：

```bash
git commit --amend -m "新的提交消息"
```

## 📚 更多资源

- [Git 官方文档](https://git-scm.com/doc)
- [Git 教程 - 菜鸟教程](https://www.runoob.com/git/git-tutorial.html)
- [Pro Git 电子书](https://git-scm.com/book/zh/v2)
- [Git 工作流指南](https://www.atlassian.com/git/tutorials/comparing-workflows)

---

**提示**: 定期提交代码，保持提交历史清晰，使用有意义的提交消息！
