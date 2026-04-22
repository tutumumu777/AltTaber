#include "utils/IconOnlyDelegate.h"
#include "widget.h"
#include <QFontMetrics>

void IconOnlyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen); //取消边框

    // 选中 / 悬停背景（两种模式共用）
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(selectedColor);
        painter->drawRoundedRect(option.rect, radius, radius);
    } else if (option.state & QStyle::State_MouseOver) {
        painter->setBrush(hoverColor);
        painter->drawRoundedRect(option.rect, radius, radius);
    }

    if (m_groupSwitcherMode) {
        // ── 组内切换模式 ──────────────────────────────────────
        // 布局：[上 20px "窗口 N"] [中 图标] [下 18px 窗口标题]
        constexpr int topH = 20;
        constexpr int botH = 18;
        constexpr int textMargin = 6;
        const int iconAreaH = option.rect.height() - topH - botH;

        // 上方：显示"窗口 N"
        QFont numFont("Microsoft YaHei UI");
        numFont.setPointSizeF(8.5);
        numFont.setBold(true);
        painter->setFont(numFont);
        painter->setPen(Qt::white);
        QRect topRect(option.rect.left() + textMargin, option.rect.top(),
                      option.rect.width() - 2 * textMargin, topH);
        painter->drawText(topRect, Qt::AlignCenter, index.data(GroupWindowLabelRole).toString());

        // 中间：图标
        auto icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            QRect iconArea(option.rect.left(), option.rect.top() + topH,
                           option.rect.width(), iconAreaH);
            QRect iconRect{{}, option.decorationSize};
            iconRect.moveCenter(iconArea.center());
            icon.paint(painter, iconRect);
        }

        // 下方：窗口标题（无标题则不显示）
        auto group = qvariant_cast<WindowGroup>(index.data(WindowGroupRole));
        auto title = group.windows.isEmpty() ? QString() : group.windows.first().title;
        if (!title.isEmpty()) {
            QFont titleFont("Microsoft YaHei UI");
            titleFont.setPointSizeF(8.0);
            painter->setFont(titleFont);
            painter->setPen(Qt::white);
            QRect botRect(option.rect.left() + textMargin, option.rect.top() + option.rect.height() - botH,
                          option.rect.width() - 2 * textMargin, botH);
            QFontMetrics fm(titleFont);
            auto elidedText = fm.elidedText(title, Qt::ElideRight, botRect.width());
            painter->drawText(botRect, Qt::AlignCenter, elidedText);
        }
        return;
    }

    // ── 普通模式（Alt+Tab）────────────────────────────────────
    // 居中绘制图标
    auto icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    if (!icon.isNull()) {
        QRect iconRect{{}, option.decorationSize}; // QListWidget::iconSize()
        iconRect.moveCenter(option.rect.center());
        icon.paint(painter, iconRect);
    }

    // draw badge
    auto num = qvariant_cast<WindowGroup>(index.data(WindowGroupRole)).windows.size();
    if (num > 1) {
        auto text = QString::number(num);
        const auto extraWidth = 8 * (text.size() - 1);
        constexpr auto R = 12;
        auto badgeCenter = option.rect.topRight() + QPoint(-(R + 3), R + 3);
        // extra Width for extra number
        auto badgeRect = QRect(badgeCenter + QPoint(-R - extraWidth, -R), QSize(2 * R + extraWidth, 2 * R));
        painter->setPen(QColor(200, 200, 200, 50));
        painter->setBrush(QColor(133, 114, 97)); // learn from iOS
        painter->drawRoundedRect(badgeRect, R, R);

        QFont font{"Microsoft YaHei"};
        font.setPointSizeF(12.8);
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(QColor(214, 192, 171));
        painter->drawText(badgeRect, Qt::AlignCenter, text);
    }
}
