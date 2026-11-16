#include <crucible/gui/util/item_delegates.hpp>

#include <crucible/core/utility/optional_copy.hpp>

#include <QLineEdit>
#include <QTimer>
#include <QToolButton>

namespace crucible::gui
{

IconDelegate::IconDelegate(Icon icon, Config config, QObject* parent)
    : QStyledItemDelegate(parent)
    , icon_(icon)
    , config_(std::move(config))
{
}

void IconDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    OptionalCopy opt(option);

    if (!config_.show_select_highlight)
        opt.modify().state &= ~QStyle::State_Selected;

    QStyledItemDelegate::paint(painter, *opt, index);

    const int icon_size = 16;
    const QRect icon_rect(
        opt->rect.center().x() - icon_size / 2,
        opt->rect.center().y() - icon_size / 2,
        icon_size,
        icon_size);

    loadIcon(icon_).paint(painter, icon_rect);
}

ButtonDelegate::ButtonDelegate(
    QAbstractItemView& view,
    Icon icon,
    Config config,
    QObject* parent)
    : QStyledItemDelegate(parent)
    , view_(view)
    , icon_(icon)
    , config_(std::move(config))
{
}

QWidget* ButtonDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index) const
{
    auto* button = new QToolButton(parent);

    button->setIcon(loadIcon(icon_));
    button->setToolTip(config_.tooltip);

    connect(button, &QToolButton::clicked, this, [this, button]() {
        Q_EMIT onClick(view_.indexAt(button->pos()));
    });

    return button;
}

void ButtonDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    OptionalCopy opt(option);

    if (!config_.show_select_highlight)
        opt.modify().state &= ~QStyle::State_Selected;

    QStyledItemDelegate::paint(painter, *opt, index);
}

LineEditDelegate::LineEditDelegate(
    Config config,
    QObject* parent)
    : QStyledItemDelegate(parent)
    , config_(std::move(config))
{
}

QWidget* LineEditDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index) const
{
    auto* edit = new QLineEdit(parent);
    edit->setPlaceholderText(config_.placeholder_text);
    return edit;
}

void LineEditDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    OptionalCopy opt(option);

    if (!config_.show_select_highlight)
        opt.modify().state &= ~QStyle::State_Selected;

    QStyledItemDelegate::paint(painter, *opt, index);
}

void LineEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto* edit = qobject_cast<QLineEdit*>(editor);
    edit->setText(index.data(Qt::EditRole).toString());

    QTimer::singleShot(0, edit, [edit]() {
        edit->deselect();
        edit->setCursorPosition(edit->text().length());
    });
}

void LineEditDelegate::setModelData(
    QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const
{
    auto* edit = qobject_cast<QLineEdit*>(editor);
    model->setData(index, edit->text(), Qt::EditRole);
}

} // crucible::gui
