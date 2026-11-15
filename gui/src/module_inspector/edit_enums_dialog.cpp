#include <crucible/gui/module_inspector/edit_enums_dialog.hpp>

#include <crucible/gui/util/button_delegate.hpp>
#include <crucible/gui/util/layout_helpers.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <crucible/core/typing/nominal_registries.hpp>

#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableView>

#include <charconv>

namespace crucible::gui
{

EnumTableModel::EnumTableModel(QTableView& table, QObject* parent)
    : QAbstractTableModel(parent)
{
    table.setModel(this);

    auto* edit_buttons = new ButtonDelegate(Icon::Edit, tr("Edit enum"), this);
    connect(edit_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
        editRequested(enums_[idx.row()].instance);
    });
    table.setItemDelegateForColumn(Edit, edit_buttons);

    auto* delete_buttons = new ButtonDelegate(Icon::Delete, tr("Delete enum"), this);
    connect(delete_buttons, &ButtonDelegate::onClick, this, [this](const QModelIndex& idx) {
        const int row = idx.row();
        beginRemoveRows(QModelIndex(), row, row);
        enums_.removeAt(row);
        endRemoveRows();

        deleteRequested(enums_[idx.row()].instance);
    });
    table.setItemDelegateForColumn(Delete, delete_buttons);
}

int EnumTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return enums_.size();
}

int EnumTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _Count;
}

QVariant EnumTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= enums_.size())
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

QVariant EnumTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section)
    {
    case Name:
        return tr("Name");

    case Edit:
        return {};

    case Id:
        return tr("ID");

    case Delete:
        return {};

    default:
        return {};
    }
}

Qt::ItemFlags EnumTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == Edit || index.column() == Delete)
        flags |= Qt::ItemIsEditable;

    return flags;
}

void EnumTableModel::addEnum(QString name, core::typing::Specialization instance)
{
    const int row = enums_.size();
    beginInsertRows(QModelIndex(), row, row);
    enums_.push_back(
        {
            .name = std::move(name),
            .instance = instance,
            .id = QString::fromStdString(std::format("0x{:08X}", instance))
        });
    endInsertRows();
}

EnumListView::EnumListView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addWidget(initTable());
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

QTableView* EnumListView::initTable()
{
    auto* table = new QTableView(this);
    model_ = new EnumTableModel(*table, this);

    connect(model_, &EnumTableModel::editRequested, this, &EnumListView::editRequested);
    connect(model_, &EnumTableModel::deleteRequested, this, &EnumListView::deleteRequested);
    connect(model_, &EnumTableModel::rowsInserted, this, [this, table](const QModelIndex&, int first, int last) {
        for (int row = first; row <= last; ++row)
        {
            table->openPersistentEditor(model_->index(row, EnumTableModel::Edit));
            table->openPersistentEditor(model_->index(row, EnumTableModel::Delete));
        }
    });

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

EnumEditView::EnumEditView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addLayout(initList());
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

void EnumEditView::loadEnum(core::typing::Specialization enum_instance)
{
    // TODO
}

QVBoxLayout* EnumEditView::initList()
{
    auto* layout = new QVBoxLayout();

    auto* form = new QFormLayout();

    id_label_ = new QLineEdit(this);
    id_label_->setReadOnly(true);
    form->addRow(tr("ID"), id_label_);

    title_edit_ = new QLineEdit(this);
    title_edit_->setPlaceholderText(tr("...")); // TODO sync error
    form->addRow(tr("Name"), title_edit_);

    layout->addLayout(form);

    elements_list_ = new QListWidget(this);
    elements_list_->setSelectionMode(QAbstractItemView::SingleSelection);
    elements_list_->setDragDropMode(QAbstractItemView::InternalMove);
    elements_list_->setDefaultDropAction(Qt::MoveAction);
    elements_list_->setDragEnabled(true);
    elements_list_->setAcceptDrops(true);
    elements_list_->setDropIndicatorShown(true);
    layout->addWidget(elements_list_);

    return layout;
}

QVBoxLayout* EnumEditView::initOptions()
{
    auto* layout = new QVBoxLayout();

    auto* back_button = new QPushButton(loadIcon(Icon::BackArrow), tr("Back"), this);
    connect(back_button, &QPushButton::clicked, this, &EnumEditView::backRequested);
    layout->addWidget(back_button);

    addSeparator(*this, *layout);

    auto* apply_button = new QPushButton(tr("Apply"), this);
    connect(apply_button, &QPushButton::clicked, this, []() { /* TODO */ });
    layout->addWidget(apply_button); // TODO grey out apply/cancel if no changes are made

    auto* cancel_button = new QPushButton(tr("Cancel"), this);
    connect(cancel_button, &QPushButton::clicked, this, []() { /* TODO */ });
    layout->addWidget(cancel_button);

    addSeparator(*this, *layout);

    auto* new_element_button = new QPushButton(loadIcon(Icon::Plus), tr("New value"), this);
    connect(new_element_button, &QPushButton::clicked, this, []() { /* TODO */ });
    layout->addWidget(new_element_button);

    auto* delete_all_button = new QPushButton(loadIcon(Icon::DeleteAll), tr("Delete all values"), this);
    connect(delete_all_button, &QPushButton::clicked, this, []() { /* TODO */ });
    layout->addWidget(delete_all_button);

    layout->addStretch();
    return layout;
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

    stack_->setCurrentWidget(list_view_);
}

void EditEnumsDialog::showListView()
{
    // TODO check for unsaved changes
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
