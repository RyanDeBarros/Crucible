#pragma once

#include <QAbstractTableModel>
#include <QDataStream>
#include <QHeaderView>
#include <QIODevice>
#include <QMimeData>
#include <QString>
#include <QTableView>

#include <functional>
#include <memory>

namespace crucible::gui
{

class ElementTableModelBase
    : public QAbstractTableModel
{
    Q_OBJECT;

protected:
    QTableView& table_;

public:
    explicit ElementTableModelBase(QTableView& table, QObject* parent = nullptr)
        : QAbstractTableModel(parent)
        , table_(table)
    {
    }

signals:
    void editRequested(size_t row);
    bool deleteRequested(size_t row);

public:
    QTableView& table()
    {
        return table_;
    }

    virtual void signalDelete(size_t row) = 0;
};

template<typename Element>
struct DelegateColumn
{
    QHeaderView::ResizeMode resize_mode;

    explicit DelegateColumn(QHeaderView::ResizeMode resize_mode = QHeaderView::ResizeToContents)
        : resize_mode(resize_mode)
    {
    }

    virtual ~DelegateColumn() = default;

    virtual QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const
    {
        return nullptr;
    }

    virtual bool draggable() const
    {
        return false;
    }

    virtual QVariant data(size_t row, const Element& element, int role = Qt::DisplayRole) const
    {
        return {};
    }

    virtual bool setData(size_t row, Element& element, const QVariant& value, int role = Qt::DisplayRole)
    {
        return false;
    }
};

template<typename Element, size_t ColumnCount>
class ElementTableModel
    : public ElementTableModelBase
{
public:
    using ElementType = Element;
    using ColumnList = std::array<std::unique_ptr<DelegateColumn<Element>>, ColumnCount>;

    struct Config
    {
        bool reorderable = false;
        bool show_grid = false;

        std::function<bool(size_t row)> pre_delete_check = [](size_t row) { return true; };
    };

private:
    ColumnList columns_;
    std::array<QAbstractItemDelegate*, ColumnCount> delegates_;
    std::vector<Element> elements_;
    Config config_;
    QString mime_reorder_type_;

public:
    ElementTableModel(
        ColumnList columns,
        QTableView& table,
        Config config = {},
        QObject* parent = nullptr)
        : ElementTableModelBase(table, parent)
        , columns_(std::move(columns))
        , config_(std::move(config))
    {
        table_.setModel(this);

        for (size_t i = 0; i < ColumnCount; ++i)
        {
            if (const auto& column = columns_[i])
            {
                if (delegates_[i] = column->newDelegate(*this); delegates_[i])
                    table_.setItemDelegateForColumn(i, delegates_[i]);
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
        table_.setShowGrid(config_.show_grid);
        table_.verticalHeader()->hide();

        auto* header = table_.horizontalHeader();
        header->hide();
        for (size_t i = 0; i < ColumnCount; ++i)
        {
            if (const auto& column = columns_[i])
                header->setSectionResizeMode(i, column->resize_mode);
        }

        if (config_.reorderable)
        {
            mime_reorder_type_ = QStringLiteral("application/reorder/")
                + QString::number(reinterpret_cast<uintptr_t>(this), 16);

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

        if (const auto& column = columns_[index.column()])
            return column->data(index.row(), elements_[index.row()], role);
        else
            return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (!index.isValid() || index.row() >= elements_.size() || index.column() >= columns_.size())
            return false;

        if (const auto& column = columns_[index.column()])
        {
            if (column->setData(index.row(), elements_[index.row()], value, role))
            {
                Q_EMIT dataChanged(index, index, { role });
                return true;
            }
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

            if (const auto& column = columns_[index.column()])
            {
                if (column->draggable())
                    flags |= Qt::ItemIsDragEnabled;
            }
        }

        return flags;
    }

    void signalDelete(size_t row) override
    {
        if (row >= elements_.size() || !config_.pre_delete_check(row))
            return;

        Q_EMIT deleteRequested(row);
        removeElement(row);
    }

    void openPersistentEditors(int first, int last)
    {
        for (int row = first; row <= last; ++row)
        {
            for (size_t i = 0; i < ColumnCount; ++i)
            {
                if (delegates_[i])
                    table_.openPersistentEditor(index(row, i));
            }
        }
    }

    size_t elementCount() const
    {
        return elements_.size();
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

    void removeElement(size_t row)
    {
        beginRemoveRows({}, row, row);
        elements_.erase(elements_.begin() + row);
        endRemoveRows();
    }

    void setElements(std::vector<Element> elements)
    {
        beginResetModel();
        elements_ = std::move(elements);
        endResetModel();
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
