#include <crucible/gui/util/item_delegates.hpp>

#include <QLineEdit>
#include <QTimer>
#include <QToolButton>

namespace crucible::gui
{

ButtonDelegate::ButtonDelegate(
    QAbstractItemView& view,
    int column,
    Icon icon,
    QString tooltip,
    QObject* parent)
    : QStyledItemDelegate(parent)
    , view_(view)
    , icon_(icon)
    , tooltip_(std::move(tooltip))
{
    view.setItemDelegateForColumn(column, this);
}

QWidget* ButtonDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index) const
{
    auto* button = new QToolButton(parent);

    button->setIcon(loadIcon(icon_));
    button->setToolTip(tooltip_);

    connect(button, &QToolButton::clicked, this, [this, button]() {
        Q_EMIT onClick(view_.indexAt(button->pos()));
    });

    return button;
}

LineEditDelegate::LineEditDelegate(
    QAbstractItemView& view,
    int column,
    QString placeholder_text,
    QObject* parent)
    : QStyledItemDelegate(parent)
    , view_(view)
    , placeholder_text_(std::move(placeholder_text))
{
    view.setItemDelegateForColumn(column, this);
}

QWidget* LineEditDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index) const
{
    auto* edit = new QLineEdit(parent);
    edit->setPlaceholderText(placeholder_text_);
    return edit;
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
