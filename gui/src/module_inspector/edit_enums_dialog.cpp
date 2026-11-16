#include <crucible/gui/module_inspector/edit_enums_dialog.hpp>

#include <crucible/gui/util/item_delegates.hpp>
#include <crucible/gui/util/layout_helpers.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <crucible/core/typing/nominal_registries.hpp>

#include <QDataStream>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMimeData>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableView>

#include <charconv>

namespace crucible::gui
{

static QString repr(core::typing::Specialization instance)
{
    return QString::fromStdString(std::format("0x{:08X}", instance));
}

EnumTableModel::EnumTableModel(QTableView& table, QObject* parent)
    : QAbstractTableModel(parent)
    , table_(table)
{
    table_.setModel(this);

    auto* edit_buttons = new ButtonDelegate(table, Icon::Edit, { .tooltip = tr("Edit enum") }, this);
    table_.setItemDelegateForColumn(Edit, edit_buttons);
    connect(edit_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
        Q_EMIT editRequested(enums_[idx.row()].instance);
    });

    auto* delete_buttons = new ButtonDelegate(table, Icon::Delete, { .tooltip = tr("Delete enum") }, this);
    table_.setItemDelegateForColumn(Delete, delete_buttons);
    connect(delete_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
        beginRemoveRows({}, idx.row(), idx.row());
        const auto enum_instance = enums_[idx.row()].instance;
        enums_.removeAt(idx.row());
        endRemoveRows();

        Q_EMIT deleteRequested(enum_instance);
    });

    connect(this, &EnumTableModel::rowsInserted, this, [this](const QModelIndex&, int first, int last) {
        openPersistentEditors(first, last);
    });

    connect(this, &EnumTableModel::modelReset, this, [this]() {
        openPersistentEditors(0, enums_.size() - 1);
    });
}

int EnumTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(enums_.size());
}

int EnumTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _Count;
}

QVariant EnumTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= enums_.size())
        return {};

    const auto& enum_instance = enums_[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case Name:
            return enum_instance.name;

        case Id:
            return enum_instance.id;

        default:
            return {};
        }
    }

    return {};
}

void EnumTableModel::openPersistentEditors(int first, int last)
{
    for (int row = first; row <= last; ++row)
    {
        table_.openPersistentEditor(index(row, EnumTableModel::Edit));
        table_.openPersistentEditor(index(row, EnumTableModel::Delete));
    }
}

void EnumTableModel::addEnum(QString name, core::typing::Specialization instance)
{
    const int row = static_cast<int>(enums_.size());
    beginInsertRows({}, row, row);
    enums_.push_back(
        {
            .name = std::move(name),
            .instance = instance,
            .id = repr(instance)
        });
    endInsertRows();
}

void EnumTableModel::setEnums(std::vector<std::pair<QString, core::typing::Specialization>> enums)
{
    beginResetModel();
    
    enums_.clear();
    enums_.reserve(enums.size());
    for (auto&& [name, instance] : enums)
    {
        enums_.push_back(
            {
                .name = std::move(name),
                .instance = instance,
                .id = repr(instance)
            });
    }

    endResetModel();
}

EnumListView::EnumListView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addWidget(initTable(), 1);
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

void EnumListView::loadEnums()
{
    std::vector<std::pair<QString, core::typing::Specialization>> enums;
    for (auto [handle, element] : core::typing::gEnumRegistry.range())
        enums.push_back(std::make_pair(QString::fromStdString(element.title), handle));
    model_->setEnums(std::move(enums));
}

QTableView* EnumListView::initTable()
{
    auto* table = new QTableView(this);
    model_ = new EnumTableModel(*table, this);

    connect(model_, &EnumTableModel::editRequested, this, &EnumListView::editRequested);
    connect(model_, &EnumTableModel::deleteRequested, this, &EnumListView::deleteRequested);

    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->hide();

    auto* header = table->horizontalHeader();
    header->hide();
    header->setSectionResizeMode(EnumTableModel::Name, QHeaderView::Stretch);
    header->setSectionResizeMode(EnumTableModel::Edit, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(EnumTableModel::Id, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(EnumTableModel::Delete, QHeaderView::ResizeToContents);

    return table;
}

QVBoxLayout* EnumListView::initOptions()
{
    auto* layout = new QVBoxLayout();

    auto* add_button = new QPushButton(loadIcon(Icon::Plus), tr("New enum"), this);
    add_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(add_button, &QPushButton::clicked, this, &EnumListView::newEnum);
    layout->addWidget(add_button);

    layout->addStretch();

    return layout;
}

void EnumListView::newEnum()
{
    QString title = tr("New enum"); // TODO (#) suffix
    
    auto handle = core::typing::gEnumRegistry.add(
        {
            .title = title.toStdString(),
            .names = {}
        });
    
    model_->addEnum(std::move(title), handle);
}

EnumValuesTableModel::EnumValuesTableModel(QTableView& table, QWidget* parent)
    : QAbstractTableModel(parent)
    , table_(table)
{
    table_.setModel(this);

    auto* value_edits = new LineEditDelegate({}, this);
    table_.setItemDelegateForColumn(Value, value_edits);

    auto* delete_buttons = new ButtonDelegate(table_, Icon::Delete, { .tooltip = tr("Delete value") }, this);
    table_.setItemDelegateForColumn(Delete, delete_buttons);
    connect(delete_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
        beginRemoveRows({}, idx.row(), idx.row());
        values_.removeAt(idx.row());
        endRemoveRows();
    });

    connect(this, &EnumValuesTableModel::rowsInserted, this, [this](const QModelIndex&, int first, int last) {
        openPersistentEditors(first, last);
    });

    connect(this, &EnumValuesTableModel::modelReset, this, [this]() {
        openPersistentEditors(0, values_.size() - 1);
    });
}

int EnumValuesTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(values_.size());
}

int EnumValuesTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : _Count;
}

QVariant EnumValuesTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= values_.size())
        return {};

    if (index.column() == Value && role == Qt::EditRole)
        return values_[index.row()];

    return {};
}

bool EnumValuesTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() >= values_.size())
        return false;

    if (index.column() == Value && role == Qt::EditRole)
    {
        values_[index.row()] = value.toString();
        return true;
    }

    return false;
}

static const char* kEnumReorderMimeType = "application/x-enum-value-row";

QStringList EnumValuesTableModel::mimeTypes() const
{
    return { kEnumReorderMimeType };
}

QMimeData* EnumValuesTableModel::mimeData(const QModelIndexList& indexes) const
{
    auto* mime_data = new QMimeData();

    if (indexes.isEmpty())
        return mime_data;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << indexes.first().row();

    mime_data->setData(kEnumReorderMimeType, data);

    return mime_data;
}

bool EnumValuesTableModel::dropMimeData(
    const QMimeData* data,
    Qt::DropAction action,
    int row,
    int column,
    const QModelIndex& parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (action != Qt::MoveAction || !data->hasFormat(kEnumReorderMimeType))
        return false;

    QByteArray encoded = data->data(kEnumReorderMimeType);
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    int source_row;
    stream >> source_row;

    if (stream.status() != QDataStream::Ok)
        return false;

    if (source_row < 0 || source_row >= values_.size())
        return false;

    if (row < 0)
        row = parent.isValid() ? parent.row() : values_.size();

    if (row == source_row)
        return true;

    if (beginMoveRows({}, source_row, source_row, {}, row > source_row ? row + 1 : row))
    {
        values_.move(source_row, row);
        endMoveRows();
        return true;
    }
    else
        return false;
}

Qt::DropActions EnumValuesTableModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags EnumValuesTableModel::flags(const QModelIndex& index) const
{
    auto flags = QAbstractTableModel::flags(index);

    if (index.isValid())
    {
        flags |= Qt::ItemIsDropEnabled;

        if (index.column() == DragHandle)
            flags |= Qt::ItemIsDragEnabled;
    }

    return flags;
}

void EnumValuesTableModel::openPersistentEditors(int first, int last)
{
    for (int row = first; row <= last; ++row)
    {
        table_.openPersistentEditor(index(row, EnumValuesTableModel::Value));
        table_.openPersistentEditor(index(row, EnumValuesTableModel::Delete));
    }
}

void EnumValuesTableModel::setValues(const std::vector<std::string>& values)
{
    beginResetModel();
    
    values_.resize(values.size());
    for (size_t i = 0; i < values.size(); ++i)
        values_[i] = QString::fromStdString(values[i]);
    
    endResetModel();
}

std::vector<std::string> EnumValuesTableModel::getValues() const
{
    std::vector<std::string> values;
    values.resize(values_.size());
    for (size_t i = 0; i < values_.size(); ++i)
        values[i] = values_[i].toStdString();
    return values;
}

void EnumValuesTableModel::newValue()
{
    const int row = static_cast<int>(values_.size());
    beginInsertRows({}, row, row);
    values_.push_back(tr("New value")); // TODO (#) suffix + sync error for duplicates
    endInsertRows();
}

void EnumValuesTableModel::clearValues()
{
    beginResetModel();
    values_.clear();
    endResetModel();
}

EnumEditView::EnumEditView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addLayout(initList(), 1);
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

void EnumEditView::loadEnum(core::typing::Specialization enum_instance)
{
    enum_instance_ = enum_instance;

    auto enum_data = core::typing::gEnumRegistry.get(enum_instance_);

    id_label_->setText(repr(enum_instance_));
    title_edit_->setText(QString::fromStdString(enum_data->title));
    title_edit_->setFocus();
    model_->setValues(enum_data->names);
}

QVBoxLayout* EnumEditView::initList()
{
    auto* layout = new QVBoxLayout();

    auto* form = new QFormLayout();

    id_label_ = new QLineEdit(this);
    id_label_->setReadOnly(true);
    form->addRow(tr("ID"), id_label_);

    // TODO sync error
    title_edit_ = new QLineEdit(this);
    form->addRow(tr("Name"), title_edit_);

    layout->addLayout(form);

    values_table_ = new QTableView(this);
    model_ = new EnumValuesTableModel(*values_table_, this);

    values_table_->setDragEnabled(true);
    values_table_->setAcceptDrops(true);
    values_table_->setDropIndicatorShown(true);
    values_table_->setDragDropMode(QAbstractItemView::InternalMove);
    values_table_->setDefaultDropAction(Qt::MoveAction);

    values_table_->setSelectionMode(QAbstractItemView::SingleSelection);
    values_table_->setSelectionBehavior(QAbstractItemView::SelectItems);
    values_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    values_table_->setAlternatingRowColors(true);
    values_table_->setShowGrid(false);
    values_table_->verticalHeader()->hide();

    auto* header = values_table_->horizontalHeader();
    header->hide();
    header->setSectionResizeMode(EnumValuesTableModel::DragHandle, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(EnumValuesTableModel::Value, QHeaderView::Stretch);
    header->setSectionResizeMode(EnumValuesTableModel::Delete, QHeaderView::ResizeToContents);
    
    values_table_->setItemDelegateForColumn(
        EnumValuesTableModel::DragHandle,
        new IconDelegate(Icon::DragHandle, {}, this));

    layout->addWidget(values_table_);

    return layout;
}

QVBoxLayout* EnumEditView::initOptions()
{
    auto* layout = new QVBoxLayout();

    auto* back_button = new QPushButton(loadIcon(Icon::BackArrow), tr("Back"), this);
    back_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(back_button, &QPushButton::clicked, this, &EnumEditView::backRequested);
    layout->addWidget(back_button);

    addSeparator(*this, *layout);

    auto* apply_button = new QPushButton(loadIcon(Icon::Apply), tr("Apply"), this);
    apply_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(apply_button, &QPushButton::clicked, this, &EnumEditView::applyChanges);
    layout->addWidget(apply_button); // TODO grey out apply/cancel if no changes are made

    auto* cancel_button = new QPushButton(loadIcon(Icon::Cancel), tr("Cancel"), this);
    cancel_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(cancel_button, &QPushButton::clicked, this, &EnumEditView::cancelChanges);
    layout->addWidget(cancel_button);

    addSeparator(*this, *layout);

    auto* new_element_button = new QPushButton(loadIcon(Icon::Plus), tr("New value"), this);
    new_element_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(new_element_button, &QPushButton::clicked, model_, &EnumValuesTableModel::newValue);
    layout->addWidget(new_element_button);

    auto* delete_all_button = new QPushButton(loadIcon(Icon::DeleteAll), tr("Delete all values"), this);
    delete_all_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(delete_all_button, &QPushButton::clicked, model_, &EnumValuesTableModel::clearValues);
    layout->addWidget(delete_all_button);

    layout->addStretch();
    return layout;
}

void EnumEditView::applyChanges()
{
    // TODO validate no duplicates
    auto enm = core::typing::gEnumRegistry.get(enum_instance_);
    enm->title = title_edit_->text().toStdString();
    enm->names = model_->getValues();
}

void EnumEditView::cancelChanges()
{
    loadEnum(enum_instance_);
}

EditEnumsDialog::EditEnumsDialog(QWidget* parent)
    : ModalDialog(tr("Edit Enums"), QSize(720, 405), parent)
{
    initStack();

    connect(list_view_, &EnumListView::editRequested, this, &EditEnumsDialog::showEditView);
    connect(list_view_, &EnumListView::deleteRequested, this, &EditEnumsDialog::deleteEnum);
    connect(edit_view_, &EnumEditView::backRequested, this, &EditEnumsDialog::showListView);
}

void EditEnumsDialog::initStack()
{
    auto* layout = new QVBoxLayout(this);

    stack_ = new QStackedWidget(this);

    list_view_ = new EnumListView(stack_);
    stack_->addWidget(list_view_);

    edit_view_ = new EnumEditView(stack_);
    stack_->addWidget(edit_view_);

    layout->addWidget(stack_);

    showListView();
}

void EditEnumsDialog::showListView()
{
    // TODO check for unsaved changes
    list_view_->loadEnums();
    stack_->setCurrentWidget(list_view_);
}

void EditEnumsDialog::showEditView(core::typing::Specialization enum_instance)
{
    edit_view_->loadEnum(enum_instance);
    stack_->setCurrentWidget(edit_view_);
}

void EditEnumsDialog::deleteEnum(core::typing::Specialization enum_instance)
{
    core::typing::gEnumRegistry.remove(enum_instance);
}

} // crucible::gui
