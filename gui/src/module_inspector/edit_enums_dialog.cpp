#include <crucible/gui/module_inspector/edit_enums_dialog.hpp>

#include <crucible/gui/util/repr.hpp>

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

namespace crucible::gui
{

EnumTableModel::EnumTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int EnumTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return entries_.size();
}

int EnumTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _Count;
}

QVariant EnumTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size())
        return {};

    const auto& entry = entries_[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case Name:
            return entry.name;

        case Id:
            return entry.id;

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

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void EnumTableModel::setEntries(QVector<Entry> entries)
{
    beginResetModel();
    entries_ = std::move(entries);
    endResetModel();
}

void EnumTableModel::addEntry(Entry entry)
{
    const int row = entries_.size();

    beginInsertRows(QModelIndex(), row, row);
    entries_.push_back(std::move(entry));
    endInsertRows();
}

void EnumTableModel::removeEntry(int row)
{
    if (row < 0 || row >= entries_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    entries_.removeAt(row);
    endRemoveRows();
}

const EnumTableModel::Entry* EnumTableModel::entryAt(int row) const
{
    if (row < 0 || row >= entries_.size())
        return nullptr;

    return &entries_[row];
}

EnumListView::EnumListView(QWidget* parent)
    : QWidget(parent)
    , model_(new EnumTableModel(this))
{
    auto* layout = new QVBoxLayout(this);

    auto* table = new QTableView(this);
    table->setModel(model_);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->hide();

    auto* header = table->horizontalHeader();
    header->hide();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    layout->addWidget(table);

    auto* footer = new QHBoxLayout();
    footer->addStretch();

    auto* add_button = new QPushButton(QIcon::fromTheme("list-add"), tr("New enum"), this);
    connect(add_button, &QPushButton::clicked, this, &EnumListView::newEnum);
    footer->addWidget(add_button);

    layout->addLayout(footer);
}

void EnumListView::newEnum()
{
    QString title = tr("New enum"); // TODO (#) suffix
    auto handle = core::typing::gEnumRegistry.add({ .title = title.toStdString(), .names = {} });
    model_->addEntry({ .name = std::move(title), .id = repr(handle) });
}

EnumEditView::EnumEditView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    layout->addLayout(initNavigation());
    layout->addLayout(initHeader());
    layout->addWidget(initElementList(), 1);
    layout->addLayout(initListActions());
    layout->addLayout(initFooter());
}

void EnumEditView::loadEnum(size_t idx)
{
    // TODO
}

QHBoxLayout* EnumEditView::initNavigation()
{
    auto* navigation = new QHBoxLayout();
    auto* back_button = new QPushButton(QIcon::fromTheme("go-previous"), tr("Back"), this);
    connect(back_button, &QPushButton::clicked, this, &EnumEditView::backRequested);
    navigation->addWidget(back_button);
    navigation->addStretch();
    return navigation;
}

QFormLayout* EnumEditView::initHeader()
{
    auto* header = new QFormLayout();

    auto* id_edit = new QLineEdit(this);
    id_edit->setReadOnly(true);
    header->addRow(tr("ID"), id_edit);

    title_edit_ = new QLineEdit(this);
    title_edit_->setPlaceholderText(tr("...")); // TODO sync error
    header->addRow(tr("Name"), title_edit_);

    return header;
}

QListWidget* EnumEditView::initElementList()
{
    elements_list_ = new QListWidget(this);
    elements_list_->setSelectionMode(QAbstractItemView::SingleSelection);
    elements_list_->setDragDropMode(QAbstractItemView::InternalMove);
    elements_list_->setDefaultDropAction(Qt::MoveAction);
    elements_list_->setDragEnabled(true);
    elements_list_->setAcceptDrops(true);
    elements_list_->setDropIndicatorShown(true);
    return elements_list_;
}

QHBoxLayout* EnumEditView::initListActions()
{
    auto* element_actions = new QHBoxLayout();

    auto* new_element_button = new QPushButton(QIcon::fromTheme("list-add"), tr("New value"), this);
    connect(new_element_button, &QPushButton::clicked, this, []() { /* TODO */ });
    element_actions->addWidget(new_element_button);

    auto* delete_all_button = new QPushButton(QIcon::fromTheme("edit-delete"), tr("Delete all values"), this);
    connect(delete_all_button, &QPushButton::clicked, this, []() { /* TODO */ });
    element_actions->addWidget(delete_all_button);

    element_actions->addStretch();

    return element_actions;
}

QHBoxLayout* EnumEditView::initFooter()
{
    auto* footer = new QHBoxLayout();

    footer->addStretch();

    auto* cancel_button = new QPushButton(tr("Cancel"), this);
    connect(cancel_button, &QPushButton::clicked, this, []() { /* TODO */ });
    footer->addWidget(cancel_button);

    auto* apply_button = new QPushButton(tr("Apply"), this);
    connect(apply_button, &QPushButton::clicked, this, []() { /* TODO */ });
    apply_button->setDefault(true); // TODO remove?
    footer->addWidget(apply_button);

    return footer;
}

EditEnumsDialog::EditEnumsDialog(QWidget* parent)
    : ModalDialog(tr("Edit Enums"), QSize(720, 405), parent)
{
    initStack();

    connect(list_view_, &EnumListView::editEnumRequested, this, &EditEnumsDialog::showEditView);
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

void EditEnumsDialog::showEditView(size_t idx)
{
    edit_view_->loadEnum(idx);
    stack_->setCurrentWidget(edit_view_);
}

//void EditEnumsDialog::createEnum()
//{
//    // TODO validate Enum title doesn't exist already - add lut to gEnumRegistry for titles
//    LOG.info(kModules) << "Create new enum: " << title_edit_->text().toStdString(); // TODO
//    accept();
//}

} // crucible::gui
