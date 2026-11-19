#include <crucible/gui/module_inspector/edit_enums_dialog.hpp>

#include <crucible/gui/util/item_delegates.hpp>
#include <crucible/gui/util/layout_helpers.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <crucible/core/typing/nominal_registries.hpp>

#include <QCloseEvent>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QSignalBlocker>
#include <QTableView>

#include <charconv>

namespace crucible::gui
{

static QString repr(core::typing::Specialization instance)
{
    return QString::fromStdString(std::format("0x{:08X}", instance));
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
    std::vector<EnumTemplate> enums;

    for (auto [handle, element] : core::typing::gEnumRegistry.range())
    {
        enums.push_back({
            .title = QString::fromStdString(element.title),
            .instance = handle,
            .id = repr(handle)
        });
    }

    model_->setElements(std::move(enums));
}

QTableView* EnumListView::initTable()
{
    std::array<TableModel::Column, 4> columns{
        TableModel::Column{
            .resize_mode = QHeaderView::Stretch,
            .data_fn = [](const EnumTemplate& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return element.title;
                else
                    return {};
            }
        },
        TableModel::Column{
            .preset = TableModel::Column::Preset::EditButton
        },
        TableModel::Column{
            .data_fn = [](const EnumTemplate& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return element.id;
                else
                    return {};
            }
        },
        TableModel::Column{
            .preset = TableModel::Column::Preset::DeleteButton
        },
    };

    auto* table = new QTableView(this);
    model_ = new TableModel(
        std::move(columns),
        *table,
        {
            .pre_delete_check = [this](size_t row) -> bool {
                auto reply = QMessageBox::question(
                    this,
                    tr("Delete enum"),
                    tr("Are you sure you want to delete '%1'? This action cannot be undone.")
                        .arg(model_->at(row).title),
                    { QMessageBox::Yes, QMessageBox::No },
                    QMessageBox::No);

                return reply == QMessageBox::Yes;
            }
        },
        this);

    connect(model_, &TableModel::editRequested, this, [this](size_t row) {
        Q_EMIT editRequested(model_->at(row).instance);
    });
    connect(model_, &TableModel::deleteRequested, this, [this](size_t row) {
        Q_EMIT deleteRequested(model_->at(row).instance);
    });

    table->setSelectionMode(QAbstractItemView::NoSelection);

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
    QString title = tr("New enum");
    core::typing::Specialization spec;

    {
        auto lock = core::typing::gEnumRegistry.lock();

        std::unordered_set<std::string> titles;
        for (const auto& entry : lock.registry.range(lock))
            titles.insert(entry.element.title);

        std::string std_title = title.toStdString();
        size_t suffix = 0;
        while (titles.contains(std_title))
            std_title = std::format("{} ({})", title.toStdString(), ++suffix);
        title = QString::fromStdString(std_title);

        spec = core::typing::gEnumRegistry.add(lock, {
            .title = title.toStdString(),
            .names = {}
        });
    }
    
    model_->addElement({
        .title = std::move(title),
        .instance = spec,
        .id = repr(spec)
    });
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

    {
        QSignalBlocker blockers[] = {
            QSignalBlocker(title_edit_),
            QSignalBlocker(model_),
        };

        auto enum_data = core::typing::gEnumRegistry.get(enum_instance_);

        id_label_->setText(repr(enum_instance_));
        title_edit_->setText(QString::fromStdString(enum_data->title));

        std::vector<QString> names;
        for (const auto& name : enum_data->names)
            names.push_back(QString::fromStdString(name));
        model_->setElements(std::move(names));
    }

    Q_EMIT title_edit_->textChanged(title_edit_->text());
    model_->externalReset();

    title_edit_->setFocus();

    syncDirtyState();
}

bool EnumEditView::unsavedChanges() const
{
    auto enum_data = core::typing::gEnumRegistry.get(enum_instance_);

    if (enum_data->title != title_edit_->text())
        return true;

    const auto& names = model_->getElements();
    if (names.size() != enum_data->names.size())
        return true;

    for (size_t i = 0; i < names.size(); ++i)
    {
        if (names[i] != enum_data->names[i])
            return true;
    }

    return false;
}

bool EnumEditView::alertUnsavedChanges()
{
    if (!unsavedChanges())
        return true;

    auto reply = QMessageBox::warning(
        this,
        tr("Unsaved Changes"),
        tr("What do you want to do with your unsaved changes?"),
        { QMessageBox::Save, QMessageBox::Discard, QMessageBox::Cancel },
        QMessageBox::Cancel);

    if (reply == QMessageBox::Save)
    {
        applyChanges();
        return true;
    }
    else if (reply == QMessageBox::Discard)
    {
        cancelChanges();
        return true;
    }
    else
        return false;
}

QVBoxLayout* EnumEditView::initList()
{
    auto* layout = new QVBoxLayout();

    auto* form = new QFormLayout();

    id_label_ = new QLineEdit(this);
    id_label_->setReadOnly(true);
    form->addRow(tr("ID"), id_label_);

    // TODO sync error for duplicate enum titles
    title_edit_ = new QLineEdit(this);
    connect(title_edit_, &QLineEdit::textChanged, this, &EnumEditView::syncDirtyState);
    form->addRow(tr("Name"), title_edit_);

    layout->addLayout(form);

    layout->addWidget(initTable());

    return layout;
}

QTableView* EnumEditView::initTable()
{
    auto* table = new QTableView(this);

    // TODO add column after drag handle for enum numerical value so it's clear for large enums
    std::array<TableModel::Column, 3> columns{
        TableModel::Column{
            .preset = TableModel::Column::Preset::DragHandle
        },
        TableModel::Column{
            .preset = TableModel::Column::Preset::LineEdit,
            .resize_mode = QHeaderView::Stretch,
            .data_fn = [](const QString& element, int role) -> QVariant {
                if (role == Qt::EditRole)
                    return element;
                else
                    return {};
            },
            .set_data_fn = [](QString& element, const QVariant& value, int role) -> bool {
                if (role == Qt::EditRole)
                {
                    element = value.toString();
                    return true;
                }
                else
                    return false;
            }
        },
        TableModel::Column{
            .preset = TableModel::Column::Preset::DeleteButton
        }
    };

    model_ = new TableModel(std::move(columns), *table, { .reorderable = true }, this);
    connect(model_, &TableModel::rowsInserted, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::rowsRemoved, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::rowsMoved, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::dataChanged, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::modelReset, this, &EnumEditView::syncDirtyState);

    return table;
}

QVBoxLayout* EnumEditView::initOptions()
{
    auto* layout = new QVBoxLayout();

    auto* back_button = new QPushButton(loadIcon(Icon::BackArrow), tr("Back"), this);
    back_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(back_button, &QPushButton::clicked, this, &EnumEditView::backRequested);
    layout->addWidget(back_button);

    addSeparator(*this, *layout);

    apply_button_ = new QPushButton(loadIcon(Icon::Apply), tr("Apply"), this);
    apply_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(apply_button_, &QPushButton::clicked, this, &EnumEditView::applyChanges);
    layout->addWidget(apply_button_);
    
    cancel_button_ = new QPushButton(loadIcon(Icon::Cancel), tr("Cancel"), this);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(cancel_button_, &QPushButton::clicked, this, &EnumEditView::cancelChanges);
    layout->addWidget(cancel_button_);

    addSeparator(*this, *layout);

    auto* new_element_button = new QPushButton(loadIcon(Icon::Plus), tr("New value"), this);
    new_element_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(new_element_button, &QPushButton::clicked, model_, [this]() {
        model_->addElement(tr("New value")); // TODO (#) suffix + sync error for duplicates
    });
    layout->addWidget(new_element_button);

    auto* delete_all_button = new QPushButton(loadIcon(Icon::DeleteAll), tr("Delete all values"), this);
    delete_all_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(delete_all_button, &QPushButton::clicked, model_, &TableModel::clearElements);
    layout->addWidget(delete_all_button);

    layout->addStretch();
    return layout;
}

void EnumEditView::syncDirtyState()
{
    const bool enabled = unsavedChanges();

    apply_button_->setEnabled(enabled);
    cancel_button_->setEnabled(enabled);
}

void EnumEditView::applyChanges()
{
    {
        // TODO validate no duplicates
        auto enum_data = core::typing::gEnumRegistry.get(enum_instance_);
        enum_data->title = title_edit_->text().toStdString();

        enum_data->names.clear();
        for (const auto& name : model_->getElements())
            enum_data->names.push_back(name.toStdString());
    }

    syncDirtyState();
    title_edit_->setFocus();
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

void EditEnumsDialog::closeEvent(QCloseEvent* event)
{
    if (checkForUnsavedChanges())
        ModalDialog::closeEvent(event);
    else
        event->ignore();
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
    if (checkForUnsavedChanges())
    {
        list_view_->loadEnums();
        stack_->setCurrentWidget(list_view_);
    }
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

bool EditEnumsDialog::checkForUnsavedChanges()
{
    if (stack_->currentWidget() == edit_view_)
        return edit_view_->alertUnsavedChanges();

    return true;
}

} // crucible::gui
