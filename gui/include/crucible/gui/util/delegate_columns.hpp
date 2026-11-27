#pragma once

#include <crucible/gui/util/element_table_model.hpp>
#include <crucible/gui/util/item_delegates.hpp>

namespace crucible::gui
{

template<typename Element>
struct DataColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using GetterFn = std::function<QVariant(size_t row, const Element& element, int role)>;
    using SetterFn = std::function<bool(size_t row, Element& element, const QVariant& value, int role)>;

    GetterFn data_fn = [](size_t row, const Element& element, int role) -> QVariant {
        return {};
    };
    
    SetterFn set_data_fn = [](size_t row, Element& element, const QVariant& value, int role) -> bool {
        return false;
    };

    explicit DataColumn(ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
    {
    }

    explicit DataColumn(GetterFn data_fn, ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , data_fn(std::move(data_fn))
    {
    }

    explicit DataColumn(SetterFn set_data_fn, ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , set_data_fn(std::move(set_data_fn))
    {
    }

    DataColumn(GetterFn data_fn, SetterFn set_data_fn, ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , data_fn(std::move(data_fn))
        , set_data_fn(std::move(set_data_fn))
    {
    }

    QVariant data(size_t row, const Element& element, int role) const override
    {
        return data_fn(row, element, role);
    }

    bool setData(size_t row, Element& element, const QVariant& value, int role) override
    {
        return set_data_fn(row, element, value, role);
    }
};

template<typename Element>
struct LabelColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using GetterFn = std::function<QVariant(size_t row, const Element& element, int role)>;

    GetterFn data_fn = [](size_t row, const Element& element, int role) -> QVariant { return {}; };

    explicit LabelColumn(ColumnConfig config = ColumnConfig::kFitX)
        : Super(std::move(config))
    {
    }

    explicit LabelColumn(GetterFn data_fn, ColumnConfig config = ColumnConfig::kFitX)
        : Super(std::move(config))
        , data_fn(std::move(data_fn))
    {
    }

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        return new LabelDelegate({}, &table);
    }

    QVariant data(size_t row, const Element& element, int role) const override
    {
        return data_fn(row, element, role);
    }
};

template<typename Element>
struct DragHandleColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using Super::Super;

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        return new IconDelegate(Icon::DragHandle, {}, &table);
    }

    bool draggable() const override
    {
        return true;
    }
};

template<typename Element>
struct DeleteButtonColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using Super::Super;

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        auto* delete_buttons = new ButtonDelegate(
            table.table(),
            Icon::Delete,
            { .tooltip = table.tr("Delete") },
            &table);

        table.connect(delete_buttons, &ButtonDelegate::onClick, &table, [&table](const QModelIndex& idx) {
            if (idx.isValid())
                table.signalDelete(idx.row());
        });

        return delete_buttons;
    }
};

template<typename Element>
struct EditButtonColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using Super::Super;

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        auto* edit_buttons = new ButtonDelegate(
            table.table(),
            Icon::Edit,
            { .tooltip = table.tr("Edit") },
            &table);

        table.connect(edit_buttons, &ButtonDelegate::onClick, &table, [&table](const QModelIndex& idx) {
            if (idx.isValid())
            {
                Q_EMIT table.editRequested(idx.row());
            }
        });

        return edit_buttons;
    }
};

template<typename Element>
struct LineEditColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using GetterFn = std::function<QVariant(size_t row, const Element& element, int role)>;
    using SetterFn = std::function<bool(size_t row, Element& element, const QVariant& value, int role)>;

    GetterFn data_fn = [](size_t row, const Element& element, int role) -> QVariant { return {}; };
    SetterFn set_data_fn = [](size_t row, Element& element, const QVariant& value, int role) -> bool { return false; };

    LineEditDelegate::Config ini_config;

    explicit LineEditColumn(
        LineEditDelegate::Config ini_config = {},
        ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , ini_config(std::move(ini_config))
    {
    }

    explicit LineEditColumn(
        GetterFn data_fn,
        LineEditDelegate::Config ini_config = {},
        ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , data_fn(std::move(data_fn))
        , ini_config(std::move(ini_config))
    {
    }

    explicit LineEditColumn(
        SetterFn set_data_fn,
        LineEditDelegate::Config ini_config = {},
        ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , set_data_fn(std::move(set_data_fn))
        , ini_config(std::move(ini_config))
    {
    }

    LineEditColumn(
        GetterFn data_fn,
        SetterFn set_data_fn,
        LineEditDelegate::Config ini_config = {},
        ColumnConfig config = ColumnConfig::kStretchX)
        : Super(std::move(config))
        , data_fn(std::move(data_fn))
        , set_data_fn(std::move(set_data_fn))
        , ini_config(std::move(ini_config))
    {
    }

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        return new LineEditDelegate(std::move(ini_config), &table);
    }

    QVariant data(size_t row, const Element& element, int role) const override
    {
        return data_fn(row, element, role);
    }

    bool setData(size_t row, Element& element, const QVariant& value, int role) override
    {
        return set_data_fn(row, element, value, role);
    }
};

} // crucible::gui
