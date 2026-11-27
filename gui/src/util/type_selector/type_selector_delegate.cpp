#include <crucible/gui/util/type_selector/type_selector_delegate.hpp>

#include <crucible/gui/util/type_selector/type_selector.hpp>

namespace crucible::gui
{

TypeSelectorDelegate::TypeSelectorDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* TypeSelectorDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex& index) const
{
    auto* editor = new TypeSelector(parent);
    auto* view = qobject_cast<QTableView*>(parent->parentWidget());

    connect(editor, &TypeSelector::editingFinished, this, [this, editor, view, row = index.row()]() {
        auto* delegate = const_cast<TypeSelectorDelegate*>(this);
        Q_EMIT delegate->commitData(editor);
        Q_EMIT delegate->closeEditor(editor);

        if (view)
            view->resizeRowToContents(row);
    });

    connect(editor, &TypeSelector::layoutSizeChanged, editor, [editor, view, row = index.row()] {
        if (view)
            view->setRowHeight(row, editor->sizeHint().height() + 4);
    });

    return editor;
}

void TypeSelectorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (auto* selector = qobject_cast<TypeSelector*>(editor))
    {
        auto variant = index.data(Qt::EditRole);
        if (auto* type = get_if<OptType>(&variant))
            selector->setType(*type);
    }
}

void TypeSelectorDelegate::setModelData(
    QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const
{
    if (auto* selector = qobject_cast<TypeSelector*>(editor))
        model->setData(index, QVariant::fromValue(selector->type()), Qt::EditRole);
}

QSize TypeSelectorDelegate::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    auto variant = index.data(Qt::DisplayRole);
    if (variant.canConvert<OptType>())
        return TypeSelector::typeSizeHint(variant.value<OptType>(), option.font) + QSize(6, 4);

    return QStyledItemDelegate::sizeHint(option, index);
}

} // crucible::gui
