#include <crucible/gui/util/button_delegate.hpp>

#include <QToolButton>

namespace crucible::gui
{

ButtonDelegate::ButtonDelegate(Icon icon, QString tooltip, QObject* parent)
    : QStyledItemDelegate(parent)
    , icon_(icon)
    , tooltip_(std::move(tooltip))
{
}

QWidget* ButtonDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index) const
{
    auto* button = new QToolButton(parent);

    button->setIcon(loadIcon(icon_));
    button->setToolTip(tooltip_);

    connect(button, &QToolButton::clicked, this, [this, index]() {
        Q_EMIT onClick(index);
    });

    return button;
}

} // crucible::gui
