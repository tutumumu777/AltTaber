#include "utils/IconOnlyDelegate.h"
#include "widget.h"
#include <QFontMetrics>

void IconOnlyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen); //取消边框

    // 选中 / 悬停背景
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(m_style.selectedColor);
        painter->drawRoundedRect(option.rect, m_style.borderRadius, m_style.borderRadius);
    } else if (option.state & QStyle::State_MouseOver) {
        painter->setBrush(m_style.hoverColor);
        painter->drawRoundedRect(option.rect, m_style.borderRadius, m_style.borderRadius);
    }

    // 图标：按 iconPadding 定位（支持非对称 padding）
    auto icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    if (!icon.isNull()) {
        const QRect iconRect(
            option.rect.left() + m_style.iconPadding.left(),
            option.rect.top() + m_style.iconPadding.top(),
            m_style.iconSize,
            m_style.iconSize
        );
        icon.paint(painter, iconRect);
    }

    // Badge: 当应用有多个窗口时，右上角显示窗口数
    auto num = qvariant_cast<WindowGroup>(index.data(WindowGroupRole)).windows.size();
    if (num > 1) {
        const auto text = QString::number(num);
        const auto extraWidth = 8 * (text.size() - 1);
        constexpr auto R = 12;
        const auto badgeCenter = option.rect.topRight() + QPoint(-(R + 3), R + 3);
        const auto badgeRect = QRect(badgeCenter + QPoint(-R - extraWidth, -R), QSize(2 * R + extraWidth, 2 * R));
        painter->setPen(m_style.badgeBorder);
        painter->setBrush(m_style.badgeBg);
        painter->drawRoundedRect(badgeRect, R, R);

        QFont font{"Microsoft YaHei"};
        font.setPointSizeF(m_style.badgeFontSize);
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(m_style.badgeText);
        painter->drawText(badgeRect, Qt::AlignCenter, text);
    }
}
