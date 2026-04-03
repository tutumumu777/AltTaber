#ifndef WIN_SWITCHER_WIDGET_H
#define WIN_SWITCHER_WIDGET_H

#include <QWidget>
#include <Windows.h>
#include <QListWidget>
#include <QDebug>

class IconOnlyDelegate; // 前向声明，避免 IconOnlyDelegate.cpp 与 widget.h 循环包含

struct WindowGroup;

struct WindowInfo {
    QString title;
    QString className;
    HWND hwnd = nullptr;
};

// C++17 inline: 防止重定义
inline QDebug operator<<(QDebug dbg, const WindowInfo& info) {
    dbg.nospace() << "WindowInfo(" << info.title << ", " << info.className << ", " << info.hwnd << ")";
    return dbg.space();
}

Q_DECLARE_METATYPE(WindowInfo) // for QVariant
struct WindowGroup {
    WindowGroup() = default;

    void addWindow(const WindowInfo& window) {
        windows.append(window);
    }

    QString exePath;
    QIcon icon;
    QList<WindowInfo> windows;
};

Q_DECLARE_METATYPE(WindowGroup) // for QVariant

QT_BEGIN_NAMESPACE

namespace Ui {
    class Widget;
}

QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public:
    enum ForegroundChangeSource {
        WinEvent,
        Inner,
    };

    Q_ENUM(ForegroundChangeSource) // for QMetaEnum, to QString

public:
    explicit Widget(QWidget* parent = nullptr);
    QList<WindowGroup> prepareWindowGroupList();
    bool prepareListWidget();
    Q_INVOKABLE bool requestShow();
    void notifyForegroundChanged(HWND hwnd, ForegroundChangeSource source);

    HWND hWnd() { return (HWND) winId(); }

    bool isForeground() { return GetForegroundWindow() == hWnd(); }

    ~Widget() override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void rotateTaskbarWindowInGroup(const QString& exePath, bool forward, int windows);
    void clearGroupWindowOrder();

private:
    bool forceShow();
    void showLabelForItem(QListWidgetItem* item, QString text = QString());
    void setupLabelFont();
    void showGroupSwitcherOverlay(); // Alt+` 组内切换浮窗
    auto getLastActiveGroupWindow(const QString& exePath) -> QPair<HWND, QDateTime>;
    auto getLastValidActiveGroupWindow(const WindowGroup& group) -> QPair<HWND, QDateTime>;
    void sortGroupWindows(QList<HWND>& windows, const QString& exePath);
    QList<HWND> buildGroupWindowOrder(const QString& exePath);
    static HWND rotateWindowInGroup(const QList<HWND>& windows, HWND current, bool forward = true);
    static HWND rotateNormalWindowInGroup(const QList<HWND>& windows, HWND current, bool forward = true);

private:
    Ui::Widget* ui;
    QListWidget* lw = nullptr;
    IconOnlyDelegate* itemDelegate = nullptr; // 存储指针以便动态切换模式
    const QMargins ListWidgetMargin{24, 24, 24, 24};
    /// exePath -> (HWND, time)
    QHash<QString, QHash<HWND, QDateTime>> winActiveOrder;
    QList<HWND> groupWindowOrder;  // for Alt+` 同组窗口切换
    int groupCurrentIndex = 0;     // Alt+` 当前选中窗口索引
    bool isGroupSwitcherMode = false; // true 表示当前浮窗是 Alt+` 组内切换模式
};


#endif //WIN_SWITCHER_WIDGET_H
