# AltTaber 中文文档

![Language](https://img.shields.io/badge/language-C++20-239120)
![Framework](https://img.shields.io/badge/framework-Qt%206-41cd52)
![OS](https://img.shields.io/badge/OS-Windows%2010%2F11-0078D4)
![License](https://img.shields.io/badge/license-MIT-blue)

> 一款 macOS 风格的 `Alt-Tab` 窗口/应用切换器，专为 **Windows** 💻 打造。

🔗 **GitHub**: <https://github.com/tutumumu777/AltTaber>
🌐 **Languages**: [English](README.en.md) · **简体中文**

![ui](../img/ui.png)

---

## 📑 目录

- [⚡ 功能总览](#-功能总览)
- [⌨️ 快捷键速查](#️-快捷键速查)
- [🌟 视觉特色](#-视觉特色)
- [🖱 系统托盘](#-系统托盘)
- [⚙ 配置文件](#-配置文件)
- [🔑 运行环境与权限](#-运行环境与权限)
- [🔧 构建](#-构建)
- [🧐 致谢](#-致谢)

---

## ⚡ 功能总览

### 1. `Alt+Tab` —— 在应用程序之间切换

- 横向图标网格，**最近使用 (MRU)** 的应用排在最左
- 按住 `Shift` 反向切换
- 松开 `Alt` 自动切换到选中应用的「最近活跃」窗口
- 应用图标右上角 **Badge** 显示该应用打开的窗口数量

![switch apps](../img/Alt_tab.gif)

### 2. ``Alt+` `` —— 在同一应用的不同窗口之间切换

- **新版竖向布局**：浮窗顶部显示应用大图标，下方为该应用所有窗口的标题列表
- 按住 `Shift` 反向切换
- 「最近活跃」窗口优先
- 条目较多时自动启用细滚动条（默认 > 15 时滚动）

![switch windows](../img/Alt_%60.gif)

> **模式互斥**（v0.5+）：在 `Alt+Tab` 浮窗显示时按 ``Alt+` `` 会切换到同应用窗口列表（基于当前选中应用）；反之亦然。无需先松 `Alt`。

### 3. 切换器内的鼠标滚轮 —— 切换/最小化指定应用的窗口

`Alt+Tab` 呼出切换器后，鼠标悬停某个应用图标时：

- **向上滚** ⬆️：将该应用的下一个窗口浮到最前（不切焦点）
- **向下滚** ⬇️：**最小化**当前对应窗口

![wheel](../img/Alt_Wheel.gif)

### 4. 任务栏滚轮切换窗口 🚧 `Beta`

> [!CAUTION]
> 实验性功能，仍在测试中。某些应用可能不生效。

直接在任务栏的应用图标上滚轮即可切换该应用的窗口：

- **向上滚** ⬆️：切换到上一个窗口
- **向下滚** ⬇️：最小化当前窗口

![taskbar wheel](../img/Taskbar_Wheel.gif)

### 5. 键盘操作（在切换器显示时）

- 方向键：
  - ⬅️ ➡️：左/右切换当前选中应用
  - ⬆️ ⬇️：映射为滚轮上下，切换/最小化窗口
- Vim 风格：
  - `h` / `l`：左右切换当前选中应用
  - `j` / `k`：上下切换/最小化窗口
- `Tab` / `Shift+Tab`：等价于左右方向键

---

## ⌨️ 快捷键速查

| 快捷键 | 功能 |
|:---:|:---|
| `Alt+Tab` | 显示跨应用切换器；持续按 `Tab` 循环切换 |
| `Alt+Shift+Tab` | 反向循环 |
| ``Alt+` `` | 显示同应用多窗口切换器；持续按 `` ` `` 循环切换 |
| ``Alt+Shift+` `` | 反向循环 |
| `←` `→` / `h` `l` | 在切换器内左右选择应用 |
| `↑` `↓` / `j` `k` | 在切换器内切换/最小化窗口（等同滚轮） |
| 松开 `Alt` | 提交选择并切换到目标窗口 |
| 鼠标在切换器图标上滚动 | 切换/最小化该应用的窗口 |
| 鼠标在任务栏图标上滚动 | 同上（Beta） |

---

## 🌟 视觉特色

- **亚克力 / 毛玻璃** 背景模糊
  ![bg blur](../img/bg-blur.png)
- 适配 **Win11 圆角**
- 应用图标右上角显示窗口数 **Badge**
  ![app badge](../img/app%20badge.png)
- **QQ 🐧** 图标右下角显示当前聊天好友头像
  ![qq avatar](../img/app%20qq%20avatar.png)
- 完整支持 **高 DPI 缩放、多显示器**
- 浮窗显示位置：**跟随鼠标所在屏幕** / **主屏幕** 可切换（托盘菜单）
- 全部样式（图标尺寸、字体、颜色、间距、圆角、滚动阈值等）均可通过 INI 配置 + 热重载

---

## 🖱 系统托盘

右键托盘图标可看到以下菜单：

| 菜单项 | 说明 |
|:---|:---|
| **Check for Updates** | 检查 GitHub 最新版本并提示更新 |
| **Settings** | 用 `notepad.exe` 打开 `config.ini`，**关闭 notepad 后自动热重载** |
| **Start with Windows** | 开机自启动开关；普通权限走注册表，管理员权限走计划任务（🔑） |
| **Display Monitor** | 切换浮窗显示屏：`Primary Monitor` / `Mouse Monitor` |
| **Quit** | 退出 |

---

## ⚙ 配置文件

### 配置文件位置

| 文件 | 路径 | 用途 | 是否会被覆盖 |
|:---|:---|:---|:---:|
| `config.ini` | `<exe目录>/config.ini` | 实际生效的配置；首次启动自动生成默认带**中英双语注释**版本 | ❌ 不会被程序覆盖 |
| `config.template.ini` | `<exe目录>/config.template.ini` | 与代码同步的最新参考文档（含完整中英双语注释） | ✅ **每次启动覆盖**为最新 |

> 修改 `config.ini` 后，从托盘菜单 `Settings` 打开关闭即可热重载；或直接编辑后重启程序。

**格式约定**：

- 颜色：`#AARRGGBB`（含 alpha 通道，`A=ff` 不透明，`A=00` 全透明）
- 边距：`L,T,R,B`（左、上、右、下，单位像素）
- 字体名错误时回退链：`Microsoft YaHei UI` → `Microsoft YaHei` → `Consolas`

### Alt+Tab 跨应用切换样式 `[app_switcher]`

| Key | 默认值 | 说明 |
|:---|:---:|:---|
| `icon_size` | `64` | 应用图标边长（像素） |
| `icon_padding` | `8,8,8,8` | 图标四周内边距 L,T,R,B（决定网格单元大小） |
| `background_color` | `#64191919` | 浮窗整体背景色 |
| `selected_bg` | `#c8505050` | 选中条目背景色 |
| `hover_bg` | `#64323232` | 鼠标悬停背景色 |
| `border_radius` | `8` | 选中/悬停背景圆角半径 |
| `label_color` | `#fff0f0f0` | 底部应用描述文字色 |
| `label_font_size` | `10` | 底部描述字号（pt） |
| `label_font_family` | `Microsoft YaHei UI` | 底部描述字体 |
| `label_top_gap` | `0` | 描述与图标的额外上间距（可负） |
| `label_bottom_gap` | `0` | 描述与浮窗下边的额外下间距 |
| `badge_bg` | `#ff857261` | 多窗口数 Badge 背景色 |
| `badge_border` | `#32c8c8c8` | Badge 边框色 |
| `badge_text` | `#ffd6c0ab` | Badge 文字色 |
| `badge_font_size` | `12.8` | Badge 文字字号（pt） |

### Alt+\` 同应用多窗口切换样式 `[group_switcher]`

| Key | 默认值 | 说明 |
|:---|:---:|:---|
| `icon_size` | `96` | 顶部应用大图标边长 |
| `icon_padding` | `12,12,12,8` | 顶部图标四周内边距 |
| `item_spacing` | `4` | 标题条目之间的纵向间距 |
| `item_padding` | `12,6,12,6` | 条目内文字与四边的内边距 |
| `item_min_width` | `320` | 条目最小宽度 |
| `item_height` | `28` | 条目固定高度 |
| `background_color` | `#64191919` | 浮窗整体背景色 |
| `item_bg` | `#00000000` | 默认条目背景色（透明 = 让浮窗背景透出） |
| `selected_bg` | `#c8505050` | 选中条目背景色 |
| `text_color` | `#fff0f0f0` | 默认文字色 |
| `selected_text` | `#ffffffff` | 选中条目文字色 |
| `font_size` | `10` | 标题字号（pt） |
| `font_family` | `Microsoft YaHei UI` | 标题字体 |
| `border_radius` | `6` | 选中条目背景圆角半径 |
| `max_visible_items` | `15` | 条目数超过此阈值时启用滚动 |

### 其他全局项

| Key | 默认值 | 说明 |
|:---|:---:|:---|
| `DisplayMonitor` | `1` | 浮窗显示在哪个屏幕：`0`=主显示器，`1`=跟随鼠标 |

---

## 🔑 运行环境与权限

- **OS**：仅支持 Windows 10 / 11（毛玻璃需要 DWM）
- **权限**：
  - **普通用户** 即可使用所有切换功能
  - 需要 **管理员权限** 才能管理：
    - 系统窗口（如任务管理器）
    - 管理员权限运行的应用（如某些游戏加速器）
- **开机自启**：托盘菜单 `Start with Windows`
  - 普通权限：写入用户注册表的 `Run` 项
  - 管理员权限：创建计划任务（🔑），自启时也以管理员权限运行

---

## 🔧 构建

### 依赖

- Visual Studio 2022（含 MSVC v143 工具链）
- Qt 6.11+ for MSVC 2022 64-bit（默认路径 `C:/Qt/6.11.0/msvc2022_64`，可在 [CMakeLists.txt](../CMakeLists.txt) 中调整 `CMAKE_PREFIX_PATH`）
- CMake 3.29+

### Release 构建

```bash
cmake -S . -B build
cmake --build build --config Release
```

构建产物会输出到 `build/Release/`：

- `AltTaber.exe` —— 主程序
- 自动通过 `windeployqt` 部署所需 Qt DLL
- `README.md`、`doc/`、`img/` 会随构建一同放到该目录
- `config.ini`、`config.template.ini` 在程序首次启动时生成

或者直接执行项目根目录下的 [`build_release.sh`](../build_release.sh)。

### 控制 Qt 自动部署

`CMakeLists.txt` 提供选项 `ALTTABER_AUTO_DEPLOY_QT`（默认 `ON`），构建后会自动 `copy_if_different` 运行时 DLL 并调用 `windeployqt --no-compiler-runtime` 部署 Qt。如果你已把 Qt DLL 放进 PATH 或想自行部署，可关掉：

```bash
cmake -S . -B build -DALTTABER_AUTO_DEPLOY_QT=OFF
```

---

## 🧐 致谢

参考与启发：

- [MrBeanCpp/AltTaber](https://github.com/MrBeanCpp/AltTaber) —— 上游项目
- [window-switcher](https://github.com/sigoden/window-switcher)
- [cmdtab](https://github.com/stianhoiland/cmdtab)

---

## 📜 License

MIT
