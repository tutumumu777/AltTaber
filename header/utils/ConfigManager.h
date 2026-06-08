#ifndef WIN_SWITCHER_CONFIGMANAGER_H
#define WIN_SWITCHER_CONFIGMANAGER_H

#include <QSettings>
#include <QApplication>
#include <QColor>
#include <QMargins>
#include "ConfigManagerBase.h"

// 注意：对于大量使用的类，header-only 模式会导致编译时间过长
#define cfg ConfigManager::instance()

enum DisplayMonitor {
    PrimaryMonitor, // 0 主显示器
    MouseMonitor, // 1 跟随鼠标
    EnumCount // Just for count
};

/// Alt+Tab 跨应用切换器（横向图标网格）样式
struct AppSwitcherStyle {
    int iconSize = 64;                                  // app_switcher/icon_size
    QMargins iconPadding{8, 8, 8, 8};                   // app_switcher/icon_padding "L,T,R,B"
    QColor backgroundColor{25, 25, 25, 100};            // app_switcher/background_color
    QColor selectedColor{80, 80, 80, 200};              // app_switcher/selected_bg
    QColor hoverColor{50, 50, 50, 100};                 // app_switcher/hover_bg
    int borderRadius = 8;                               // app_switcher/border_radius
    QColor labelColor{240, 240, 240};                   // app_switcher/label_color
    int labelFontSize = 10;                             // app_switcher/label_font_size (fallback: label/font_size)
    int labelTopGap = 0;                                // app_switcher/label_top_gap
    int labelBottomGap = 0;                             // app_switcher/label_bottom_gap
    QString labelFontFamily{"Microsoft YaHei UI"};      // app_switcher/label_font_family (fallback: label/font_family)
    // Badge
    QColor badgeBg{133, 114, 97};                       // app_switcher/badge_bg
    QColor badgeBorder{200, 200, 200, 50};              // app_switcher/badge_border
    QColor badgeText{214, 192, 171};                    // app_switcher/badge_text
    qreal badgeFontSize = 12.8;                         // app_switcher/badge_font_size
};

/// Alt+` 同应用多窗口切换器（顶部大图标 + 竖向标题列表）样式
struct GroupSwitcherStyle {
    int iconSize = 96;                                  // group_switcher/icon_size
    QMargins iconPadding{12, 12, 12, 8};                // group_switcher/icon_padding "L,T,R,B"
    int itemSpacing = 4;                                // group_switcher/item_spacing
    QMargins itemPadding{12, 6, 12, 6};                 // group_switcher/item_padding "L,T,R,B"
    int itemMinWidth = 320;                             // group_switcher/item_min_width
    int itemHeight = 28;                                // group_switcher/item_height
    QColor backgroundColor{25, 25, 25, 100};            // group_switcher/background_color
    QColor itemBackground{0, 0, 0, 0};                  // group_switcher/item_bg (transparent)
    QColor selectedItemBg{80, 80, 80, 200};             // group_switcher/selected_bg
    QColor textColor{240, 240, 240};                    // group_switcher/text_color
    QColor selectedTextColor{255, 255, 255};            // group_switcher/selected_text
    int fontSize = 10;                                  // group_switcher/font_size
    QString fontFamily{"Microsoft YaHei UI"};           // group_switcher/font_family
    int borderRadius = 6;                               // group_switcher/border_radius
    int maxVisibleItems = 15;                           // group_switcher/max_visible_items：超过该数量则启用滚动
};

class ConfigManager : public ConfigManagerBase {
    inline static const QString FileName = "config.ini";
    inline static const QString TemplateFileName = "config.template.ini";

public:
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    static ConfigManager& instance() {
        static const auto dir = QApplication::applicationDirPath();
        static const auto filePath = dir + "/" + FileName;
        // 首次访问时准备：
        //   1) config.ini 不存在或为空时，写入默认内容（仅初次，不覆盖用户配置）
        //   2) config.template.ini 每次启动覆盖写出以保持与代码同步
        static const bool _prep = (
            ensureDefaultConfig(filePath),
            writeTemplateFile(dir + "/" + TemplateFileName), true);
        Q_UNUSED(_prep);
        static ConfigManager instance{filePath}; // multiple threads safe
        return instance;
    }

public:
    DisplayMonitor getDisplayMonitor() {
        auto monitor = get("DisplayMonitor", DisplayMonitor::MouseMonitor).toInt();
        if (monitor < 0 || monitor >= DisplayMonitor::EnumCount) {
            qWarning() << "Invalid DisplayMonitor enum" << monitor;
            monitor = DisplayMonitor::MouseMonitor;
        }
        return static_cast<DisplayMonitor>(monitor);
    }

    void setDisplayMonitor(DisplayMonitor monitor) {
        set("DisplayMonitor", monitor);
    }

    /// 加载 Alt+Tab 切换器样式（缺失项使用默认值）
    AppSwitcherStyle loadAppStyle() {
        AppSwitcherStyle s;
        s.iconSize         = get("app_switcher/icon_size", s.iconSize).toInt();
        s.iconPadding      = parseMargins(get("app_switcher/icon_padding"), s.iconPadding);
        s.backgroundColor  = parseColor(get("app_switcher/background_color"), s.backgroundColor);
        s.selectedColor    = parseColor(get("app_switcher/selected_bg"), s.selectedColor);
        s.hoverColor       = parseColor(get("app_switcher/hover_bg"), s.hoverColor);
        s.borderRadius     = get("app_switcher/border_radius", s.borderRadius).toInt();
        s.labelColor       = parseColor(get("app_switcher/label_color"), s.labelColor);
        // labelFontSize / labelFontFamily 兼容旧 key label/font_size, label/font_family
        s.labelFontSize    = get("app_switcher/label_font_size", get("label/font_size", s.labelFontSize)).toInt();
        s.labelFontFamily  = get("app_switcher/label_font_family", get("label/font_family", s.labelFontFamily)).toString();
        s.labelTopGap      = get("app_switcher/label_top_gap", s.labelTopGap).toInt();
        s.labelBottomGap   = get("app_switcher/label_bottom_gap", s.labelBottomGap).toInt();
        s.badgeBg          = parseColor(get("app_switcher/badge_bg"), s.badgeBg);
        s.badgeBorder      = parseColor(get("app_switcher/badge_border"), s.badgeBorder);
        s.badgeText        = parseColor(get("app_switcher/badge_text"), s.badgeText);
        s.badgeFontSize    = get("app_switcher/badge_font_size", s.badgeFontSize).toReal();
        return s;
    }

    /// 加载 Alt+` 组内切换器样式（缺失项使用默认值）
    GroupSwitcherStyle loadGroupStyle() {
        GroupSwitcherStyle s;
        s.iconSize          = get("group_switcher/icon_size", s.iconSize).toInt();
        s.iconPadding       = parseMargins(get("group_switcher/icon_padding"), s.iconPadding);
        s.itemSpacing       = get("group_switcher/item_spacing", s.itemSpacing).toInt();
        s.itemPadding       = parseMargins(get("group_switcher/item_padding"), s.itemPadding);
        s.itemMinWidth      = get("group_switcher/item_min_width", s.itemMinWidth).toInt();
        s.itemHeight        = get("group_switcher/item_height", s.itemHeight).toInt();
        s.backgroundColor   = parseColor(get("group_switcher/background_color"), s.backgroundColor);
        s.itemBackground    = parseColor(get("group_switcher/item_bg"), s.itemBackground);
        s.selectedItemBg    = parseColor(get("group_switcher/selected_bg"), s.selectedItemBg);
        s.textColor         = parseColor(get("group_switcher/text_color"), s.textColor);
        s.selectedTextColor = parseColor(get("group_switcher/selected_text"), s.selectedTextColor);
        s.fontSize          = get("group_switcher/font_size", s.fontSize).toInt();
        s.fontFamily        = get("group_switcher/font_family", s.fontFamily).toString();
        s.borderRadius      = get("group_switcher/border_radius", s.borderRadius).toInt();
        s.maxVisibleItems   = get("group_switcher/max_visible_items", s.maxVisibleItems).toInt();
        if (s.maxVisibleItems < 1) s.maxVisibleItems = 1;
        return s;
    }

private:
    explicit ConfigManager(const QString& filename) : ConfigManagerBase(filename) {}

    /// 将默认配置模板写入指定路径（覆盖），供用户参考复粘。
    /// 模板不会被 QSettings 改动，可以安全携带中文注释。
    static void writeTemplateFile(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "#Failed to write config template:" << path;
            return;
        }
        f.write(templateContent().toUtf8());
    }

    /// 首次启动时：若 config.ini 不存在或为空，写入带注释的默认配置。
    /// 已有用户配置时不动，避免覆盖丢失。
    static void ensureDefaultConfig(const QString& path) {
        QFile f(path);
        if (f.exists() && f.size() > 0) return;
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "#Failed to write default config:" << path;
            return;
        }
        f.write(defaultConfigContent().toUtf8());
    }

    /// 模板文件内容：头部说明 + 共用配置主体（中英双语）
    static QString templateContent() {
        return QStringLiteral(
R"(; ============================================================
;                   AltTaber 配置模板 (config.template.ini)
; ------------------------------------------------------------
;  [使用说明]
;    本文件仅作为参考，不会被程序读取。每次启动会被覆盖为最新版本。
;    需要调整设置时，请复制需要的项到同目录的 config.ini，
;    然后在系统托盘 [Settings] 菜单中打开编辑（关闭 notepad 后自动热重载）。
;
;  [Usage]
;    This file is for reference only and is NEVER read by the program.
;    It will be overwritten on every launch to stay in sync with the latest defaults.
;    To change settings, copy the keys you need into config.ini in the same directory,
;    then open it via the tray menu [Settings] (auto-reloaded on close).
;
;  [格式约定 / Conventions]
;    颜色 / Color  : #AARRGGBB  (alpha-prefixed; A=ff opaque, A=00 transparent)
;    边距 / Margin : L,T,R,B    (left, top, right, bottom in pixels)
;    字体名错误时会回退到 / Font fallback chain:
;        Microsoft YaHei UI → Microsoft YaHei → Consolas
; ============================================================
)")
               + commonBody();
    }

    /// 首次生成的 config.ini 内容：头部说明 + 共用配置主体（中英双语）
    static QString defaultConfigContent() {
        return QStringLiteral(
R"(; ============================================================
;                AltTaber 配置文件 (config.ini)
; ------------------------------------------------------------
;  [中文]
;    本文件由程序读取并应用。你可以随意修改以下项：
;      1）从托盘菜单 [Settings] 打开编辑，关闭 notepad 后自动热重载。
;      2）或直接编辑本文件，修改后重启程序生效。
;    本文件不会被程序自动覆盖。如需查看最新默认值与详细说明，请参考同目录的
;    config.template.ini（每次启动自动更新）。
;
;  [English]
;    This file is read and applied by the program. You may freely edit any entry below:
;      1) Open it via the tray menu [Settings]; auto-reload after closing notepad.
;      2) Or edit this file directly and restart the program.
;    This file is NEVER auto-overwritten by the program. For up-to-date defaults and
;    full documentation, see config.template.ini in the same directory (refreshed on
;    every launch).
;
;  [格式约定 / Conventions]
;    颜色 / Color  : #AARRGGBB  (alpha-prefixed; A=ff opaque, A=00 transparent)
;    边距 / Margin : L,T,R,B    (left, top, right, bottom in pixels)
;    字体名错误时会回退到 / Font fallback chain:
;        Microsoft YaHei UI → Microsoft YaHei → Consolas
; ============================================================
)")
               + commonBody();
    }

    /// 共用的配置主体：DisplayMonitor + [app_switcher] + [group_switcher]。
    /// 所有项采用中英双行注释。
    static QString commonBody() {
        return QStringLiteral(
R"(
; 浮窗在哪个屏幕显示：0=主显示器  1=跟随鼠标（默认）
; Which monitor to display the popup on: 0=primary, 1=follow mouse (default)
DisplayMonitor=1


; ============================================================
; [app_switcher] Alt+Tab 跨应用切换器（横向图标网格）
; [app_switcher] Alt+Tab cross-app switcher (horizontal icon grid)
; ============================================================
[app_switcher]

; 应用图标边长（像素）。默认 64
; App icon size in pixels. Default: 64
icon_size=64

; 图标四周内边距 L,T,R,B。决定图标与单元格边界的间隔，从而决定整个网格单元的大小。默认 8,8,8,8
; Inner padding around the icon (defines the grid cell size). Default: 8,8,8,8
icon_padding=8,8,8,8

; 浮窗整体背景色。默认 #64191919（半透明深灰）
; Popup background color. Default: #64191919 (semi-transparent dark gray)
background_color=#64191919

; 选中条目背景色。默认 #c8505050
; Selected item background color. Default: #c8505050
selected_bg=#c8505050

; 鼠标悬停条目背景色。默认 #64323232
; Hover item background color. Default: #64323232
hover_bg=#64323232

; 选中/悬停背景的圆角半径。默认 8
; Corner radius for selected/hover background. Default: 8
border_radius=8

; 底部应用描述文字颜色。默认 #fff0f0f0
; Bottom description text color. Default: #fff0f0f0
label_color=#fff0f0f0

; 底部描述字号（pt）。默认 10。兼容旧 key label/font_size
; Bottom description font size (pt). Default: 10. (Legacy key label/font_size still works)
label_font_size=10

; 底部描述字体。默认 Microsoft YaHei UI。兼容旧 key label/font_family
; Bottom description font family. Default: Microsoft YaHei UI. (Legacy key label/font_family still works)
label_font_family=Microsoft YaHei UI

; 描述与图标之间的额外上间距（可为负使描述靠近图标）。默认 0
; Extra gap between label and icon (negative pulls the label closer). Default: 0
label_top_gap=0

; 描述与浮窗下边之间的额外下间距。默认 0
; Extra gap between label and the popup bottom edge. Default: 0
label_bottom_gap=0

; 多窗口数 Badge（右上角小圆点）背景色。默认 #ff857261
; Window-count badge background (small dot at top-right). Default: #ff857261
badge_bg=#ff857261

; Badge 边框色。默认 #32c8c8c8
; Badge border color. Default: #32c8c8c8
badge_border=#32c8c8c8

; Badge 文字色。默认 #ffd6c0ab
; Badge text color. Default: #ffd6c0ab
badge_text=#ffd6c0ab

; Badge 文字字号（pt，支持小数）。默认 12.8
; Badge text size (pt, fractional supported). Default: 12.8
badge_font_size=12.8


; ============================================================
; [group_switcher] Alt+\` 同应用多窗口切换器
; 布局：顶部一个应用大图标 + 下方竖向窗口标题列表。
; [group_switcher] Alt+\` same-app multi-window switcher.
; Layout: a large app icon at the top + vertical list of window titles below.
; ============================================================
[group_switcher]

; 顶部应用大图标边长。默认 96
; Top app icon size. Default: 96
icon_size=96

; 顶部图标四周内边距 L,T,R,B。默认 12,12,12,8
; Inner padding around the top icon. Default: 12,12,12,8
icon_padding=12,12,12,8

; 标题条目之间的纵向间距。默认 4
; Vertical gap between title items. Default: 4
item_spacing=4

; 条目内文字与四边的内边距 L,T,R,B。默认 12,6,12,6
; Inner padding within an item. Default: 12,6,12,6
item_padding=12,6,12,6

; 条目最小宽度（像素）。默认 320
; Minimum item width in pixels. Default: 320
item_min_width=320

; 条目固定高度。默认 28
; Fixed item height. Default: 28
item_height=28

; 浮窗整体背景色。默认 #64191919
; Popup background color. Default: #64191919
background_color=#64191919

; 默认条目背景色（透明=让浮窗背景透出）。默认 #00000000
; Default item background color (transparent lets popup background show through). Default: #00000000
item_bg=#00000000

; 选中条目背景色。默认 #c8505050
; Selected item background color. Default: #c8505050
selected_bg=#c8505050

; 默认文字色。默认 #fff0f0f0
; Default text color. Default: #fff0f0f0
text_color=#fff0f0f0

; 选中条目文字色。默认 #ffffffff
; Selected item text color. Default: #ffffffff
selected_text=#ffffffff

; 标题字号（pt）。默认 10
; Title font size (pt). Default: 10
font_size=10

; 标题字体。默认 Microsoft YaHei UI
; Title font family. Default: Microsoft YaHei UI
font_family=Microsoft YaHei UI

; 选中条目背景的圆角半径。默认 6
; Corner radius for the selected item background. Default: 6
border_radius=6

; 条目数超过该阈值时启用滚动（防止浮窗超出屏幕）。默认 15
; Enable scrolling when items exceed this threshold (prevents popup from overflowing screen). Default: 15
max_visible_items=15
)");
    }

    /// 解析 "#AARRGGBB" / "#RRGGBB" / 命名色；非法则返回 def
    static QColor parseColor(const QVariant& v, const QColor& def) {
        if (!v.isValid()) return def;
        const auto s = v.toString().trimmed();
        if (s.isEmpty()) return def;
        QColor c(s);
        return c.isValid() ? c : def;
    }

    /// 解析 "L,T,R,B"；非法则返回 def
    static QMargins parseMargins(const QVariant& v, const QMargins& def) {
        if (!v.isValid()) return def;
        const auto parts = v.toString().split(',', Qt::SkipEmptyParts);
        if (parts.size() != 4) return def;
        bool ok[4]{};
        const int l = parts[0].trimmed().toInt(&ok[0]);
        const int t = parts[1].trimmed().toInt(&ok[1]);
        const int r = parts[2].trimmed().toInt(&ok[2]);
        const int b = parts[3].trimmed().toInt(&ok[3]);
        return (ok[0] && ok[1] && ok[2] && ok[3]) ? QMargins(l, t, r, b) : def;
    }
};


#endif //WIN_SWITCHER_CONFIGMANAGER_H
