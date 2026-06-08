# AltTaber

![Language](https://img.shields.io/badge/language-C++20-239120)
![Framework](https://img.shields.io/badge/framework-Qt%206-41cd52)
![OS](https://img.shields.io/badge/OS-Windows%2010%2F11-0078D4)
![License](https://img.shields.io/badge/license-MIT-blue)

> A macOS-style `Alt-Tab` window/application switcher built for **Windows** 💻.

🔗 **GitHub**: <https://github.com/tutumumu777/AltTaber>
🌐 **Languages**: **English** · [简体中文](doc/README.zh-CN.md)
📚 **Docs**: [`doc/README.en.md`](doc/README.en.md) · [`doc/README.zh-CN.md`](doc/README.zh-CN.md)

![ui](img/ui.png)

---

## 📑 Table of Contents

- [⚡ Features](#-features)
- [⌨️ Keyboard Shortcuts](#️-keyboard-shortcuts)
- [🌟 Visual Highlights](#-visual-highlights)
- [🖱 System Tray](#-system-tray)
- [⚙ Configuration](#-configuration)
- [🔑 Runtime & Permissions](#-runtime--permissions)
- [🔧 Building](#-building)
- [🧐 Credits](#-credits)

---

## ⚡ Features

### 1. `Alt+Tab` — Switch Across Applications

- Horizontal icon grid; **most recently used (MRU)** apps come first
- Hold `Shift` to reverse
- Releasing `Alt` switches to the most recently active window of the selected app
- A **badge** at the top-right of each icon shows the number of open windows for that app

![switch apps](img/Alt_tab.gif)

### 2. ``Alt+` `` — Switch Between Windows of the Same App

- **New vertical layout**: a large app icon at the top, list of window titles below
- Hold `Shift` to reverse
- Most recently active window first
- A thin scrollbar appears automatically when items exceed the threshold (default `15`, configurable)

![switch windows](img/Alt_%60.gif)

> **Mode mutual switching** (v0.5+): pressing ``Alt+` `` while the Alt+Tab popup is shown switches you into the same-app window list (based on the currently selected app); and vice versa. No need to release `Alt` in between.

### 3. Mouse Wheel on the Switcher — Switch / Minimize Windows

With the `Alt+Tab` popup visible, hover over an app icon and scroll:

- **Wheel up** ⬆️: bring the next window of that app to front (without taking focus)
- **Wheel down** ⬇️: **minimize** the current window

![wheel](img/Alt_Wheel.gif)

### 4. Wheel on Taskbar Icons 🚧 `Beta`

> [!CAUTION]
> Experimental. May not work for some applications.

Just scroll the wheel on a taskbar icon to switch windows of that app:

- **Wheel up** ⬆️: switch to the previous window
- **Wheel down** ⬇️: minimize the current window

![taskbar wheel](img/Taskbar_Wheel.gif)

### 5. Keyboard Operations (When the Switcher Is Visible)

- Arrow keys:
  - ⬅️ ➡️: select previous / next app
  - ⬆️ ⬇️: mapped to wheel up / down (switch / minimize windows)
- Vim-style:
  - `h` / `l`: select previous / next app
  - `j` / `k`: switch / minimize windows
- `Tab` / `Shift+Tab`: equivalent to ⬅️ / ➡️

---

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|:---:|:---|
| `Alt+Tab` | Show the cross-app switcher; keep pressing `Tab` to cycle |
| `Alt+Shift+Tab` | Reverse cycle |
| ``Alt+` `` | Show the same-app window switcher |
| ``Alt+Shift+` `` | Reverse cycle |
| `←` `→` / `h` `l` | Select prev / next app within the switcher |
| `↑` `↓` / `j` `k` | Switch / minimize windows (same as wheel) |
| Release `Alt` | Commit selection and switch to the target window |
| Wheel on a switcher icon | Switch / minimize windows of that app |
| Wheel on a taskbar icon | Same (Beta) |

---

## 🌟 Visual Highlights

- **Acrylic / frosted-glass** background blur
  ![bg blur](img/bg-blur.png)
- Adapts to **Win11 rounded corners**
- **Window-count badge** at the top-right of each app icon
  ![app badge](img/app%20badge.png)
- Custom **QQ chat-partner avatar** overlaid on the QQ icon
  ![qq avatar](img/app%20qq%20avatar.png)
- Full **HiDPI / multi-monitor** support
- Configurable display monitor: **mouse-tracked** / **primary** (toggle via tray menu)
- Every visual property (icon size, font, color, padding, radius, scroll threshold...) is configurable through INI with hot-reload

---

## 🖱 System Tray

Right-click the tray icon to access:

| Menu | Description |
|:---|:---|
| **Check for Updates** | Check the latest GitHub release |
| **Settings** | Open `config.ini` in `notepad.exe`; **auto-reload after closing notepad** |
| **Start with Windows** | Toggle auto-start; user-mode → registry, admin-mode → scheduled task (🔑) |
| **Display Monitor** | Choose `Primary Monitor` / `Mouse Monitor` |
| **Quit** | Exit |

---

## ⚙ Configuration

### Files

| File | Path | Purpose | Auto-overwritten? |
|:---|:---|:---|:---:|
| `config.ini` | `<exe dir>/config.ini` | Active configuration; default content with **bilingual (CN/EN)** comments is generated on first launch | ❌ Never overwritten by program |
| `config.template.ini` | `<exe dir>/config.template.ini` | Up-to-date reference doc with full bilingual comments | ✅ Overwritten on every launch |

> Hot reload: edit via tray `Settings`, save and close notepad. Or edit and restart the program.

**Conventions**:

- Color: `#AARRGGBB` (alpha-prefixed; `A=ff` opaque, `A=00` transparent)
- Margin: `L,T,R,B` (left, top, right, bottom in pixels)
- Font fallback chain: `Microsoft YaHei UI` → `Microsoft YaHei` → `Consolas`

### `[app_switcher]` — Alt+Tab Cross-App Switcher

| Key | Default | Description |
|:---|:---:|:---|
| `icon_size` | `64` | App icon size (px) |
| `icon_padding` | `8,8,8,8` | Inner padding around the icon (defines the grid cell size) |
| `background_color` | `#64191919` | Popup background |
| `selected_bg` | `#c8505050` | Selected item background |
| `hover_bg` | `#64323232` | Hover item background |
| `border_radius` | `8` | Corner radius for selected / hover |
| `label_color` | `#fff0f0f0` | Description text color |
| `label_font_size` | `10` | Description font size (pt) |
| `label_font_family` | `Microsoft YaHei UI` | Description font family |
| `label_top_gap` | `0` | Extra gap between label and icon (can be negative) |
| `label_bottom_gap` | `0` | Extra gap between label and popup bottom |
| `badge_bg` | `#ff857261` | Badge background |
| `badge_border` | `#32c8c8c8` | Badge border |
| `badge_text` | `#ffd6c0ab` | Badge text color |
| `badge_font_size` | `12.8` | Badge text size (pt) |

### `[group_switcher]` — Alt+\` Same-App Multi-Window Switcher

| Key | Default | Description |
|:---|:---:|:---|
| `icon_size` | `96` | Top app icon size |
| `icon_padding` | `12,12,12,8` | Inner padding around the top icon |
| `item_spacing` | `4` | Vertical gap between items |
| `item_padding` | `12,6,12,6` | Inner padding within an item |
| `item_min_width` | `320` | Minimum item width |
| `item_height` | `28` | Fixed item height |
| `background_color` | `#64191919` | Popup background |
| `item_bg` | `#00000000` | Default item background (transparent → popup background shows through) |
| `selected_bg` | `#c8505050` | Selected item background |
| `text_color` | `#fff0f0f0` | Default text color |
| `selected_text` | `#ffffffff` | Selected text color |
| `font_size` | `10` | Title font size (pt) |
| `font_family` | `Microsoft YaHei UI` | Title font family |
| `border_radius` | `6` | Corner radius for the selected item |
| `max_visible_items` | `15` | Enable scrolling when items exceed this threshold |

### Global

| Key | Default | Description |
|:---|:---:|:---|
| `DisplayMonitor` | `1` | Monitor for the popup: `0` = primary, `1` = follow mouse |

---

## 🔑 Runtime & Permissions

- **OS**: Windows 10 / 11 only (frosted glass requires DWM)
- **Privileges**:
  - **Standard user** is enough for all switching features
  - **Administrator** required to manage:
    - System windows (e.g. Task Manager)
    - Apps running as administrator (e.g. some game accelerators)
- **Auto-start**: Tray menu → `Start with Windows`
  - Standard: writes to `HKCU\...\Run` registry
  - Admin: creates a scheduled task (🔑); auto-start also runs as administrator

---

## 🔧 Building

### Dependencies

- Visual Studio 2022 with MSVC v143 toolchain
- Qt 6.11+ for MSVC 2022 64-bit (default `C:/Qt/6.11.0/msvc2022_64`; adjust `CMAKE_PREFIX_PATH` in [CMakeLists.txt](CMakeLists.txt))
- CMake 3.29+

### Release Build

```bash
cmake -S . -B build
cmake --build build --config Release
```

Outputs go to `build/Release/`:

- `AltTaber.exe` — main executable
- Qt runtime DLLs (auto-deployed via `windeployqt`)
- `README.md`, `doc/`, `img/` — copied as part of the release package
- `config.ini`, `config.template.ini` — generated on first launch

Or run [`build_release.sh`](build_release.sh).

### Disabling Qt Auto-Deploy

`CMakeLists.txt` exposes `ALTTABER_AUTO_DEPLOY_QT` (default `ON`). After building, runtime DLLs are copied with `copy_if_different` and Qt is deployed via `windeployqt --no-compiler-runtime`. To disable:

```bash
cmake -S . -B build -DALTTABER_AUTO_DEPLOY_QT=OFF
```

---

## 🧐 Credits

References & inspiration:

- [MrBeanCpp/AltTaber](https://github.com/MrBeanCpp/AltTaber) — upstream project
- [window-switcher](https://github.com/sigoden/window-switcher)
- [cmdtab](https://github.com/stianhoiland/cmdtab)

---

## 📜 License

MIT
