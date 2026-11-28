#include <crucible/gui/util/standard_tree.hpp>

#include <crucible/gui/util/icons.hpp>

#include <QApplication>
#include <QPainter>

namespace crucible::gui
{

void StandardTree::TreeStyle::drawPrimitive(
    PrimitiveElement element,
    const QStyleOption* option,
    QPainter* painter,
    const QWidget* widget) const
{
    if (element != PE_IndicatorBranch)
    {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        return;
    }

    if (!(option->state & State_Children))
        return;

    const QRect target(
        option->rect.center().x(),
        option->rect.top() + (option->rect.height() - icon_width) / 2,
        icon_width,
        icon_width);

    loadIcon((option->state & State_Open) ? Icon::ExpandDown : Icon::ExpandRight).paint(painter, target);
}

void StandardTree::TreeItemDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

    QRect text_rect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);

    const QString text = opt.text;

    opt.text.clear();
    opt.icon = QIcon();

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    if (text.isEmpty())
        return;

    text_rect.translate(text_offset, 0);

    const QString elided = option.fontMetrics.elidedText(text, option.textElideMode, text_rect.width());

    const auto role = (option.state & QStyle::State_Selected)
        ? QPalette::HighlightedText
        : QPalette::Text;

    painter->save();
    painter->setFont(option.font);

    style->drawItemText(
        painter,
        text_rect,
        option.displayAlignment,
        option.palette,
        option.state & QStyle::State_Enabled,
        elided,
        role);

    painter->restore();
}

StandardTree::StandardTree(Config config, QWidget* parent)
    : QTreeWidget(parent)
    , config_(config)
{
    setHeaderHidden(true);
    setRootIsDecorated(true);
    setIndentation(config_.indentation);

    auto* proxy = new TreeStyle(style());
    proxy->icon_width = config_.icon_width;
    setStyle(proxy);

    auto* delegate = new TreeItemDelegate(this);
    delegate->text_offset = config_.indentation;
    setItemDelegate(delegate);
}

QTreeWidgetItem* StandardTree::newCategory(const QString& title, bool bold, bool italic)
{
    auto* category = new QTreeWidgetItem(this, { title });

    QFont category_font = font();
    category_font.setBold(bold);
    category_font.setItalic(italic);
    category->setFont(0, category_font);

    category->setForeground(0, palette().color(QPalette::Text));
    category->setFlags(category->flags() & ~Qt::ItemIsSelectable);
    category->setSizeHint(0, QSize(0, config_.category_height));

    return category;
}

QTreeWidgetItem* StandardTree::newSpacer(QTreeWidgetItem* parent) const
{
    if (!config_.spacer_role)
        throw std::runtime_error("Spacer role is not configured");

    auto* spacer = new QTreeWidgetItem(parent);
    spacer->setData(0, *config_.spacer_role, true);
    spacer->setSizeHint(0, QSize(0, config_.spacer_height));
    spacer->setFlags(Qt::NoItemFlags);
    spacer->setForeground(0, Qt::transparent);
    spacer->setBackground(0, Qt::transparent);
    return spacer;
}

void StandardTree::drawRow(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (config_.spacer_role && index.data(*config_.spacer_role).toBool())
        return;

    QTreeWidget::drawRow(painter, option, index);
}

} // crucible::gui
