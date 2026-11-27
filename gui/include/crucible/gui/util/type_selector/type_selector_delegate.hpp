#pragma once

#include <crucible/gui/util/type_selector/opt_type.hpp>

#include <crucible/gui/util/element_table_model.hpp>

#include <QStyledItemDelegate>

namespace crucible::gui
{

class TypeSelectorDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TypeSelectorDelegate(QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(
        QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;

    QSize sizeHint(
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

template<typename Element>
struct TypeSelectorColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using GetterFn = std::function<OptType(size_t row, const Element& element)>;
    using SetterFn = std::function<bool(size_t row, Element& element, OptType type)>;

    GetterFn get_type;
    SetterFn set_type;

    TypeSelectorColumn(GetterFn get_type, SetterFn set_type, ColumnConfig config = ColumnConfig::kStretchXFitY)
        : Super(std::move(config))
        , get_type(std::move(get_type))
        , set_type(std::move(set_type))
    {
    }

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        return new TypeSelectorDelegate(&table);
    }

    QVariant data(size_t row, const Element& element, int role) const override
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QVariant::fromValue(get_type(row, element));
        else
            return {};
    }

    bool setData(size_t row, Element& element, const QVariant& value, int role) override
    {
        if (role != Qt::EditRole)
            return false;

        if (value.canConvert<OptType>())
            return set_type(row, element, value.value<OptType>());
        else
            return false;
    }
};

} // crucible::gui
