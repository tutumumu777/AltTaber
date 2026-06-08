#ifndef WIN_SWITCHER_ICONONLYDELEGATE_H
#define WIN_SWITCHER_ICONONLYDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>
#include "ConfigManager.h"

/// Icon Only Mode for QListWidget (Alt+Tab 横向应用切换器)
class IconOnlyDelegate : public QStyledItemDelegate {
    AppSwitcherStyle m_style; // 默认值见 AppSwitcherStyle 内部初始化器

public:
    explicit IconOnlyDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /// 应用样式（首次以及配置热重载时调用）
    void applyStyle(const AppSwitcherStyle& style) { m_style = style; }
    const AppSwitcherStyle& style() const { return m_style; }
};


#endif //WIN_SWITCHER_ICONONLYDELEGATE_H
