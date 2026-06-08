#ifndef WIN_SWITCHER_GROUPSWITCHERWIDGET_H
#define WIN_SWITCHER_GROUPSWITCHERWIDGET_H

#include <QWidget>
#include <QList>
#include <QIcon>
#include <Windows.h>
#include "utils/ConfigManager.h"

class QLabel;
class QListWidget;
class GroupItemDelegate;

/// Alt+` 同应用多窗口切换浮窗：
///   ┌────────────────────────────┐
///   │         [App Icon]         │   <- 顶部居中单个大图标
///   │                            │
///   │  window title 1            │   <- 竖向窗口标题列表（仅文字，无编号）
///   │  window title 2  (selected)│
///   │  window title 3            │
///   └────────────────────────────┘
///
/// 仅做被动展示：键盘事件由 Widget（KeyboardHooker 的固定 receiver）统一处理。
/// 该窗口不抢占焦点，避免破坏 Widget 与全局键盘钩子的协作。
class GroupSwitcherWidget : public QWidget {
    Q_OBJECT

public:
    explicit GroupSwitcherWidget(QWidget* parent = nullptr);

    /// 首次显示或窗口列表变化时调用：重建条目并显示
    void showFor(const QList<HWND>& windows, int currentIndex,
                 const QIcon& appIcon, const QString& exePath);

    /// 已显示状态下旋转选中项（不重建列表）
    void setCurrentIndex(int idx);

    int currentIndex() const;
    HWND currentHwnd() const;
    int count() const { return m_windows.size(); }

    HWND hWnd() { return reinterpret_cast<HWND>(winId()); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void reloadStyle();        // 读取最新 GroupSwitcherStyle 并应用到子控件 + 触发重排
    void rebuildItems();       // 用 m_windows 重建 m_listWidget 的条目
    void recomputeGeometry();  // 根据条目内容 + 样式计算并应用窗口/子控件尺寸位置
    void positionOnScreen();   // 居中到目标屏幕（沿用 cfg.getDisplayMonitor() 策略）

    QLabel* m_iconLabel = nullptr;
    QListWidget* m_listWidget = nullptr;
    GroupItemDelegate* m_delegate = nullptr;

    QList<HWND> m_windows;
    QString m_exePath;
    QIcon m_appIcon;
    GroupSwitcherStyle m_style;
};


#endif //WIN_SWITCHER_GROUPSWITCHERWIDGET_H
