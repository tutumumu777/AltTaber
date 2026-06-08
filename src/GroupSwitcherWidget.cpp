#include "GroupSwitcherWidget.h"
#include "utils/Util.h"
#include "utils/setWindowBlur.h"
#include "utils/QtWin.h"
#include <QLabel>
#include <QListWidget>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFontMetrics>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QCursor>

// ── GroupItemDelegate：仅绘制文字 + 选中态背景 ────────────────────────────
class GroupItemDelegate : public QStyledItemDelegate {
public:
    GroupSwitcherStyle style;

    explicit GroupItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);

        // 除最后一项外，其他 item 的 sizeHint 包含了一个 itemSpacing 的尾部预留，
        // 需要在可见区域（itemHeight）内绘制背景与文字，避免选中态背景染色到间隔槽。
        const int rows = index.model() ? index.model()->rowCount() : 0;
        const bool isLast = (index.row() == rows - 1);
        const int trailing = isLast ? 0 : style.itemSpacing;
        const QRect visibleRect = option.rect.adjusted(0, 0, 0, -trailing);

        const bool selected = option.state & QStyle::State_Selected;
        const QColor bg = selected ? style.selectedItemBg : style.itemBackground;
        if (bg.alpha() > 0) {
            painter->setBrush(bg);
            painter->drawRoundedRect(visibleRect, style.borderRadius, style.borderRadius);
        }

        const QRect textRect = visibleRect.adjusted(
            style.itemPadding.left(), style.itemPadding.top(),
            -style.itemPadding.right(), -style.itemPadding.bottom()
        );

        QFont font(style.fontFamily);
        font.setPointSize(style.fontSize);
        painter->setFont(font);
        painter->setPen(selected ? style.selectedTextColor : style.textColor);

        const QFontMetrics fm(font);
        const auto raw = index.data(Qt::DisplayRole).toString();
        const auto elided = fm.elidedText(raw, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override {
        return {0, style.itemHeight}; // 高度仅作 fallback；GroupSwitcherWidget 会逐项 setSizeHint 覆盖
    }
};

// ── GroupSwitcherWidget ──────────────────────────────────────────────────
GroupSwitcherWidget::GroupSwitcherWidget(QWidget* parent) : QWidget(parent) {
    // 不抢占焦点：键盘事件由 Widget（KeyboardHooker receiver）统一处理
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::Tool);
    setWindowFlag(Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::NoFocus);
    setWindowTitle("AltTaber GroupSwitcher");
    QtWin::taskbarDeleteTab(this); // 删除任务栏图标

    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_iconLabel->setStyleSheet("background: transparent;");

    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setUniformItemSizes(false); // 最后一项 sizeHint 与其他不同，必须禁用 uniform
    m_listWidget->setSpacing(0);              // spacing 直接放到每项 sizeHint 尾部，以便精确控制总高度
    // 是否启用滚动、是否启用自动滚动与垂直滚动条策略，在 recomputeGeometry 中根据 maxVisibleItems 动态切换
    m_listWidget->setStyleSheet(R"(
        QListWidget {
            background-color: transparent;
            border: none;
            outline: none;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 2px 0;
        }
        QScrollBar::handle:vertical {
            background: rgba(255, 255, 255, 80);
            border-radius: 3px;
            min-height: 24px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255, 255, 255, 140);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
            background: transparent;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
    )");

    m_delegate = new GroupItemDelegate(m_listWidget);
    m_listWidget->setItemDelegate(m_delegate);

    // 配置热重载：notepad 关闭后重新加载样式并重排
    connect(&cfg, &ConfigManager::configEdited, this, [this] {
        reloadStyle();
        if (isVisible()) recomputeGeometry();
    });

    reloadStyle();
}

void GroupSwitcherWidget::reloadStyle() {
    m_style = cfg.loadGroupStyle();
    m_delegate->style = m_style;
    if (m_listWidget) m_listWidget->viewport()->update();
}

void GroupSwitcherWidget::showFor(const QList<HWND>& windows, int currentIndex,
                                  const QIcon& appIcon, const QString& exePath) {
    m_windows = windows;
    m_exePath = exePath;
    m_appIcon = appIcon;

    rebuildItems();
    setCurrentIndex(currentIndex);
    recomputeGeometry();
    positionOnScreen();

    if (!isVisible()) show();
    raise(); // 保持在最上层
}

void GroupSwitcherWidget::rebuildItems() {
    m_listWidget->clear();
    for (HWND hwnd: m_windows) {
        auto title = Util::getWindowTitle(hwnd);
        if (title.isEmpty()) title = QStringLiteral("(无标题窗口)");
        m_listWidget->addItem(title);
    }
}

void GroupSwitcherWidget::setCurrentIndex(int idx) {
    if (idx < 0 || idx >= m_windows.size()) return;
    m_listWidget->setCurrentRow(idx);
}

int GroupSwitcherWidget::currentIndex() const {
    return m_listWidget ? m_listWidget->currentRow() : -1;
}

HWND GroupSwitcherWidget::currentHwnd() const {
    const int i = currentIndex();
    return (i >= 0 && i < m_windows.size()) ? m_windows.at(i) : nullptr;
}

void GroupSwitcherWidget::recomputeGeometry() {
    const int N = m_windows.size();
    if (N == 0) return;

    // 顶部图标区
    const int iconAreaW = m_style.iconSize + m_style.iconPadding.left() + m_style.iconPadding.right();
    const int iconAreaH = m_style.iconSize + m_style.iconPadding.top() + m_style.iconPadding.bottom();

    // 列表内容宽度 = max(itemMinWidth, 最长标题 + itemPadding)
    QFont measureFont(m_style.fontFamily);
    measureFont.setPointSize(m_style.fontSize);
    const QFontMetrics fm(measureFont);
    int maxTextW = 0;
    for (HWND hwnd: m_windows) {
        auto t = Util::getWindowTitle(hwnd);
        if (t.isEmpty()) t = QStringLiteral("(无标题窗口)");
        maxTextW = std::max(maxTextW, fm.horizontalAdvance(t));
    }
    const int itemHorizPad = m_style.itemPadding.left() + m_style.itemPadding.right();
    int contentW = std::max({m_style.itemMinWidth, maxTextW + itemHorizPad, iconAreaW});

    // 是否需要滚动：当条目数 > maxVisibleItems 时，列表高度限制为 maxVisibleItems 行，滚动查看其余
    // 注：使用 qMin/qMax 避免与 windows.h 的 min/max 宏冲突
    const int visibleRows = qMin(N, m_style.maxVisibleItems);
    const bool needsScroll = (N > m_style.maxVisibleItems);
    const int listInnerH = visibleRows * m_style.itemHeight + qMax(0, visibleRows - 1) * m_style.itemSpacing;

    // 动态切换滚动策略：仅在需要时启用 autoScroll + 垂直滚动条
    m_listWidget->setAutoScroll(needsScroll);
    m_listWidget->setVerticalScrollBarPolicy(
        needsScroll ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
    if (needsScroll) {
        // 为滚动条预留宽度（与 stylesheet 中 width: 8px + margin 保持一致）
        const int scrollBarW = m_listWidget->verticalScrollBar()->sizeHint().width();
        contentW += qMax(scrollBarW, 10);
    }

    // 外边距：复用 itemPadding 的 LR/B 作为列表整体边距，顶部紧贴图标区
    const int outerL = m_style.itemPadding.left();
    const int outerR = m_style.itemPadding.right();
    const int outerB = m_style.itemPadding.bottom();

    const int totalW = std::max(contentW + outerL + outerR, iconAreaW);
    const int totalH = iconAreaH + listInnerH + outerB;

    // 应用尺寸
    setFixedSize(totalW, totalH);

    // 顶部图标 Label
    const QPixmap iconPix = m_appIcon.pixmap({m_style.iconSize, m_style.iconSize});
    m_iconLabel->setPixmap(iconPix);
    m_iconLabel->setGeometry((totalW - iconAreaW) / 2, 0, iconAreaW, iconAreaH);

    // 列表
    const int listX = (totalW - contentW) / 2;
    const int listY = iconAreaH;
    m_listWidget->setGeometry(listX, listY, contentW, listInnerH);
    // 将 itemSpacing 直接包含在除最后一项外的 sizeHint 中，
    // 让 QListWidget 不需依赖其 spacing 机制，总高度与实际占用严格匹配。
    // 需要插入滚动条的列表项 宽度要扣除滚动条宽以避免水平溶出
    const int rows = m_listWidget->count();
    const int itemContentW = needsScroll
                                 ? contentW - m_listWidget->verticalScrollBar()->sizeHint().width()
                                 : contentW;
    for (int i = 0; i < rows; i++) {
        const int h = (i < rows - 1) ? (m_style.itemHeight + m_style.itemSpacing) : m_style.itemHeight;
        m_listWidget->item(i)->setSizeHint({itemContentW, h});
    }
    m_listWidget->viewport()->update();
}

void GroupSwitcherWidget::positionOnScreen() {
    const bool displayOnPrimary = (cfg.getDisplayMonitor() == PrimaryMonitor);
    auto* screen = displayOnPrimary
                       ? QGuiApplication::primaryScreen()
                       : QGuiApplication::screenAt(QCursor::pos());
    if (!screen) screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    auto rect = this->rect();
    rect.moveCenter(screen->geometry().center());
    setGeometry(rect);
}

void GroupSwitcherWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    // 这两个 API 需要 native window，必须在 winId() 之后调用
    Util::setWindowRoundCorner(hWnd());
    setWindowBlur(hWnd());
}

void GroupSwitcherWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_style.backgroundColor);
    painter.drawRect(this->rect());
}
