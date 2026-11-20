#pragma once

#include <crucible/gui/util/item_delegates.hpp>

#include <QAbstractTableModel>
#include <QDataStream>
#include <QHeaderView>
#include <QIODevice>
#include <QMimeData>
#include <QString>
#include <QTableView>

#include <functional>

namespace crucible::gui
{

class ElementTableModelBase
    : public QAbstractTableModel
{
    Q_OBJECT;

public:
    using QAbstractTableModel::QAbstractTableModel;

signals:
    void editRequested(size_t row);
    bool deleteRequested(size_t row);
};

template<typename Element, size_t ColumnCount>
class ElementTableModel
    : public ElementTableModelBase
{
public:
    using ElementType = Element;

    struct Column
    {
        enum class Preset
        {
            None,
            DragHandle,
            DeleteButton,
            EditButton,
            LineEdit,
        };

        Preset preset = Preset::None;

        QHeaderView::ResizeMode resize_mode = QHeaderView::ResizeToContents;

        std::function<QVariant(size_t row, const Element& element, int role)> data_fn =
            [](size_t row, const Element& element, int role) -> QVariant { return {}; };

        std::function<bool(size_t row, Element& element, const QVariant& value, int role)> set_data_fn =
            [](size_t row, Element& element, const QVariant& value, int role) { return false; };
    };

    struct Config
    {
        bool reorderable = false;
        bool show_grid = false;

        std::function<bool(size_t row)> pre_delete_check = [](size_t row) { return true; };
    };

private:
    QTableView& table_;
    std::array<Column, ColumnCount> columns_;
    std::vector<Element> elements_;
    Config config_;
    QString mime_reorder_type_;

public:
    ElementTableModel(
        std::array<Column, ColumnCount> columns,
        QTableView& table,
        Config config = {},
        QObject* parent = nullptr)
        : ElementTableModelBase(parent)
        , table_(table)
        , columns_(std::move(columns))
        , config_(std::move(config))
    {
        table_.setModel(this);

        for (size_t i = 0; i < ColumnCount; ++i)
        {
            const auto& column = columns_[i];

            if (column.preset == Column::Preset::DragHandle)
                table_.setItemDelegateForColumn(i, new IconDelegate(Icon::DragHandle, {}, this));
            
            if (column.preset == Column::Preset::DeleteButton)
            {
                auto* delete_buttons = new ButtonDelegate(table_, Icon::Delete, { .tooltip = tr("Delete") }, this);
                table_.setItemDelegateForColumn(i, delete_buttons);
                connect(delete_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
                    if (!idx.isValid() || idx.row() >= elements_.size())
                        return;

                    if (!config_.pre_delete_check(idx.row()))
                        return;

                    Q_EMIT deleteRequested(idx.row());

                    beginRemoveRows({}, idx.row(), idx.row());
                    elements_.erase(elements_.begin() + idx.row());
                    endRemoveRows();
                });
            }

            if (column.preset == Column::Preset::EditButton)
            {
                auto* edit_buttons = new ButtonDelegate(table_, Icon::Edit, { .tooltip = tr("Edit") }, this);
                table_.setItemDelegateForColumn(i, edit_buttons);
                connect(edit_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
                    Q_EMIT editRequested(idx.row());
                });
            }

            if (column.preset == Column::Preset::LineEdit)
            {
                // TODO pass LineEditDelegate::Config - instead of preset enum, use variant of configs
                table_.setItemDelegateForColumn(i, new LineEditDelegate({}, this));
            }
        }

        connect(this, &ElementTableModel::rowsInserted, this, [this](const QModelIndex&, int first, int last) {
            openPersistentEditors(first, last);
        });

        connect(this, &ElementTableModel::modelReset, this, [this]() {
            openPersistentEditors(0, elements_.size() - 1);
        });

        table_.setEditTriggers(QAbstractItemView::NoEditTriggers);
        table_.setAlternatingRowColors(true);
        table_.verticalHeader()->hide();

        auto* header = table_.horizontalHeader();
        header->hide();
        for (size_t i = 0; i < ColumnCount; ++i)
            header->setSectionResizeMode(i, columns_[i].resize_mode);

        if (config_.reorderable)
        {
            mime_reorder_type_ = QStringLiteral("application/reorder/") + QString::number(reinterpret_cast<uintptr_t>(this), 16);

            table_.setSelectionMode(QAbstractItemView::SingleSelection);
            table_.setSelectionBehavior(QAbstractItemView::SelectItems);
            table_.setDragEnabled(true);
            table_.setAcceptDrops(true);
            table_.setDropIndicatorShown(true);
            table_.setDragDropMode(QAbstractItemView::InternalMove);
            table_.setDefaultDropAction(Qt::MoveAction);
        }
    }

public:
    int rowCount(const QModelIndex& parent = {}) const override
    {
        return parent.isValid() ? 0 : static_cast<int>(elements_.size());
    }

    int columnCount(const QModelIndex& parent = {}) const override
    {
        return parent.isValid() ? 0 : static_cast<int>(ColumnCount);
    }
    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= elements_.size() || index.column() >= columns_.size())
            return {};

        return columns_[index.column()].data_fn(index.row(), elements_[index.row()], role);
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (!index.isValid() || index.row() >= elements_.size() || index.column() >= columns_.size())
            return false;

        if (columns_[index.column()].set_data_fn(index.row(), elements_[index.row()], value, role))
        {
            Q_EMIT dataChanged(index, index, { role });
            return true;
        }

        return false;
    }

    QStringList mimeTypes() const override
    {
        if (config_.reorderable)
            return { mime_reorder_type_ };
        else
            return {};
    }

    QMimeData* mimeData(const QModelIndexList& indexes) const override
    {
        if (!config_.reorderable)
            return nullptr;

        auto* mime_data = new QMimeData();

        if (indexes.isEmpty())
            return mime_data;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << indexes.first().row();

        mime_data->setData(mime_reorder_type_, data);

        return mime_data;
    }

    bool dropMimeData(
        const QMimeData* data,
        Qt::DropAction action,
        int row,
        int column,
        const QModelIndex& parent) override
    {
        if (!config_.reorderable)
            return false;

        if (action == Qt::IgnoreAction)
            return true;

        if (action != Qt::MoveAction || !data->hasFormat(mime_reorder_type_))
            return false;

        QByteArray encoded = data->data(mime_reorder_type_);
        QDataStream stream(&encoded, QIODevice::ReadOnly);

        int source_row;
        stream >> source_row;

        if (stream.status() != QDataStream::Ok || source_row < 0 || source_row >= elements_.size())
            return false;

        if (row < 0)
            row = parent.isValid() ? parent.row() : elements_.size();

        if (row == source_row)
            return true;

        if (beginMoveRows({}, source_row, source_row, {}, row > source_row ? row + 1 : row))
        {
            auto first = elements_.begin() + std::min(source_row, row);
            auto last = elements_.begin() + std::max(source_row, row) + 1;
            auto mid = elements_.begin() + source_row + (source_row < row ? 1 : 0);
            std::rotate(first, mid, last);

            endMoveRows();
            return true;
        }
        else
            return false;
    }

    Qt::DropActions supportedDropActions() const override
    {
        if (config_.reorderable)
            return Qt::MoveAction;
        else
            return {};
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        auto flags = ElementTableModelBase::flags(index);

        if (index.isValid())
        {
            if (config_.reorderable)
                flags |= Qt::ItemIsDropEnabled;

            if (columns_[index.column()].preset == Column::Preset::DragHandle)
                flags |= Qt::ItemIsDragEnabled;
        }

        return flags;
    }

    void openPersistentEditors(int first, int last)
    {
        for (int row = first; row <= last; ++row)
        {
            for (size_t i = 0; i < ColumnCount; ++i)
            {
                if (columns_[i].preset != Column::Preset::None)
                    table_.openPersistentEditor(index(row, i));
            }
        }
    }

    const Element& at(size_t row) const
    {
        return elements_[row];
    }

    Element& at(size_t row)
    {
        return elements_[row];
    }

    void addElement(Element element)
    {
        const int row = static_cast<int>(elements_.size());
        beginInsertRows({}, row, row);
        elements_.push_back(std::move(element));
        endInsertRows();
    }

    void setElements(std::vector<Element> elements)
    {
        beginResetModel();
        elements_ = std::move(elements);
        endResetModel();
    }

    size_t elementCount() const
    {
        return elements_.size();
    }

    void clearElements()
    {
        beginResetModel();
        elements_.clear();
        endResetModel();
    }

    void externalReset()
    {
        beginResetModel();
        endResetModel();
    }

    const std::vector<Element>& getElements() const
    {
        return elements_;
    }
};

} // crucible::gui
