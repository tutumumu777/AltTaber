#ifndef WIN_SWITCHER_ICONONLYDELEGATE_H
#define WIN_SWITCHER_ICONONLYDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>
#include <QColor>

/// Icon Only Mode for QListWidget
class IconOnlyDelegate : public QStyledItemDelegate {
    QColor selectedColor;
    QColor hoverColor;
    int radius;
    bool m_groupSwitcherMode = false; // Alt+` 组内切换模式

public:
    explicit IconOnlyDelegate(QObject* parent = nullptr,
                              QColor selectedColor = QColor(80, 80, 80, 200),
                              QColor hoverColor = QColor(50, 50, 50, 100),
                              int radius = 8)
        : QStyledItemDelegate(parent), selectedColor(selectedColor), hoverColor(hoverColor),
          radius(radius) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /// 切换到组内切换模式：每格上方显示"窗口 N"，下方显示窗口标题
    void setGroupSwitcherMode(bool enabled) { m_groupSwitcherMode = enabled; }
    bool isGroupSwitcherMode() const { return m_groupSwitcherMode; }
};


#endif //WIN_SWITCHER_ICONONLYDELEGATE_H
