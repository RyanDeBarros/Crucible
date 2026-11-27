#include <crucible/gui/dialogs/edit_structs_dialog.hpp>

#include <crucible/gui/util/alerts.hpp>
#include <crucible/gui/util/delegate_columns.hpp>
#include <crucible/gui/util/item_delegates.hpp>
#include <crucible/gui/util/layout_helpers.hpp>
#include <crucible/gui/util/outlines.hpp>
#include <crucible/gui/util/repr.hpp>

#include <crucible/core/typing/user_registries.hpp>

#include <QCloseEvent>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace crucible::gui
{

static QString repr(core::typing::UserInstance user)
{
    return QString::fromStdString(std::format("0x{:08X}", user));
}

StructListView::StructListView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addWidget(initTable(), 1);
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

void StructListView::loadStructs()
{
    std::vector<StructTemplate> structs;

    auto lock = core::typing::gStructRegistry.lock();
    for (auto [handle, element] : core::typing::gStructRegistry.range(lock))
    {
        structs.push_back({
            .title = QString::fromStdString(element.title),
            .user = handle,
            .id = repr(handle)
        });
    }

    model_->setElements(std::move(structs));
}

QTableView* StructListView::initTable()
{
    TableModel::ColumnList columns{
        std::make_unique<DataColumn<StructTemplate>>(
            [](size_t row, const StructTemplate& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return element.title;
                else
                    return {};
            }),
        std::make_unique<EditButtonColumn<StructTemplate>>(),
        std::make_unique<DataColumn<StructTemplate>>(
            [](size_t row, const StructTemplate& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return element.id;
                else
                    return {};
            }),
        std::make_unique<DeleteButtonColumn<StructTemplate>>()
    };

    auto* table = new QTableView(this);
    model_ = new TableModel(
        std::move(columns),
        *table,
        {
            .pre_delete_check = [this](size_t row) -> bool {
                auto reply = QMessageBox::question(
                    this,
                    tr("Delete struct"),
                    tr("Are you sure you want to delete '%1'? This action cannot be undone.")
                        .arg(model_->at(row).title),
                    { QMessageBox::Yes, QMessageBox::No },
                    QMessageBox::No);

                return reply == QMessageBox::Yes;
            }
        },
        this);

    connect(model_, &TableModel::editRequested, this, [this](size_t row) {
        Q_EMIT editRequested(model_->at(row).user);
    });
    connect(model_, &TableModel::deleteRequested, this, [this](size_t row) {
        Q_EMIT deleteRequested(model_->at(row).user);
    });

    table->setSelectionMode(QAbstractItemView::NoSelection);

    return table;
}

QVBoxLayout* StructListView::initOptions()
{
    auto* layout = new QVBoxLayout();

    auto* add_button = new QPushButton(loadIcon(Icon::Plus), tr("New struct"), this);
    add_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(add_button, &QPushButton::clicked, this, &StructListView::newStruct);
    layout->addWidget(add_button);

    layout->addStretch();

    return layout;
}

void StructListView::newStruct()
{
    QString title = tr("New struct");
    core::typing::UserInstance user;

    {
        auto lock = core::typing::gStructRegistry.lock();

        std::unordered_set<std::string> titles;
        for (const auto& entry : core::typing::gStructRegistry.range(lock))
            titles.insert(entry.element.title);

        std::string std_title = title.toStdString();
        size_t suffix = 0;
        while (titles.contains(std_title))
            std_title = std::format("{} ({})", title.toStdString(), ++suffix);
        title = QString::fromStdString(std_title);

        user = core::typing::gStructRegistry.add(lock, {
            .title = title.toStdString(),
            .fields = {}
        });
    }

    model_->addElement({
        .title = std::move(title),
        .user = user,
        .id = repr(user)
    });
}

StructEditView::StructEditView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addLayout(initMainLayout(), 1);
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

void StructEditView::loadStruct(core::typing::UserInstance user)
{
    user_ = user;

    {
        QSignalBlocker blockers[] = {
            QSignalBlocker(title_edit_),
            QSignalBlocker(model_),
        };

        core::typing::UserRegistryLock lock;

        const auto& struct_data = core::typing::gStructRegistry.get(lock.struct_lock, user_);

        id_label_->setText(repr(user_));
        title_edit_->setText(QString::fromStdString(struct_data.title));

        std::vector<FieldTemplate> fields;
        for (const auto& field : struct_data.fields)
        {
            fields.push_back({
                .name = QString::fromStdString(field.first),
                .type = field.second,
                .repr = repr(lock, field.second)
            });
        }
        model_->setElements(std::move(fields));
    }

    Q_EMIT title_edit_->textChanged(title_edit_->text());
    model_->externalReset();

    title_edit_->setFocus();

    syncDirtyState();
}

void StructEditView::setFieldType(size_t field_idx, core::typing::Type type)
{
    model_->at(field_idx).type = type;
    model_->at(field_idx).repr = repr({}, type);
}

bool StructEditView::unsavedChanges() const
{
    auto lock = core::typing::gStructRegistry.lock();

    const auto& struct_data = core::typing::gStructRegistry.get(lock, user_);

    if (struct_data.title != title_edit_->text())
        return true;

    const auto& fields = model_->getElements();
    if (fields.size() != struct_data.fields.size())
        return true;

    for (size_t i = 0; i < fields.size(); ++i)
    {
        if (fields[i].name != struct_data.fields[i].first)
            return true;

        if (auto type = fields[i].type.toType())
        {
            if (*type != struct_data.fields[i].second)
                return true;
        }
        else
            return true;
    }

    return false;
}

bool StructEditView::alertUnsavedChanges()
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
        return applyChanges();
    else if (reply == QMessageBox::Discard)
    {
        cancelChanges();
        return true;
    }
    else
        return false;
}

QVBoxLayout* StructEditView::initMainLayout()
{
    auto* layout = new QVBoxLayout();

    auto* form = new QFormLayout();

    id_label_ = new QLineEdit(this);
    id_label_->setReadOnly(true);
    form->addRow(tr("ID"), id_label_);

    title_edit_ = new QLineEdit(this);
    connect(title_edit_, &QLineEdit::editingFinished, this, &StructEditView::syncDirtyState);
    connect(title_edit_, &QLineEdit::editingFinished, this, &StructEditView::syncTitleStatus);
    form->addRow(tr("Name"), title_edit_);

    layout->addLayout(form);

    layout->addWidget(initTable());

    return layout;
}

static constexpr size_t kFieldNameColumn = 1;

QTableView* StructEditView::initTable()
{
    fields_table_ = new QTableView(this);

    TableModel::ColumnList columns{
        std::make_unique<DragHandleColumn<FieldTemplate>>(),
        std::make_unique<LineEditColumn<FieldTemplate>>(
            [](size_t, const FieldTemplate& element, int role) -> QVariant {
                if (role == Qt::EditRole)
                    return element.name;
                else
                    return {};
            },
            [](size_t, FieldTemplate& element, const QVariant& value, int role) -> bool {

                if (role == Qt::EditRole)
                {
                    element.name = value.toString();
                    return true;
                }
                else
                    return false;
            }),
        std::make_unique<TypeSelectorColumn<FieldTemplate>>(
            [](size_t, const FieldTemplate& element) -> OptType {
                return element.type;
            },
            [](size_t, FieldTemplate& element, OptType type) -> bool {
                element.type = std::move(type);
                return true;
            }),
        std::make_unique<DeleteButtonColumn<FieldTemplate>>()
    };

    model_ = new TableModel(std::move(columns), *fields_table_, { .reorderable = true }, this);
    connect(model_, &TableModel::rowsInserted, this, &StructEditView::syncDirtyState);
    connect(model_, &TableModel::rowsRemoved, this, &StructEditView::syncDirtyState);
    connect(model_, &TableModel::rowsMoved, this, &StructEditView::syncDirtyState);
    connect(model_, &TableModel::dataChanged, this, &StructEditView::syncDirtyState);
    connect(model_, &TableModel::dataChanged, this, &StructEditView::syncFieldStatuses);
    connect(model_, &TableModel::modelReset, this, &StructEditView::syncDirtyState);

    return fields_table_;
}

QVBoxLayout* StructEditView::initOptions()
{
    auto* layout = new QVBoxLayout();

    auto* back_button = new QPushButton(loadIcon(Icon::BackArrow), tr("Back"), this);
    back_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(back_button, &QPushButton::clicked, this, &StructEditView::backRequested);
    layout->addWidget(back_button);

    addSeparator(*this, *layout);

    apply_button_ = new QPushButton(loadIcon(Icon::Apply), tr("Apply"), this);
    apply_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(apply_button_, &QPushButton::clicked, this, &StructEditView::applyChanges);
    layout->addWidget(apply_button_);

    cancel_button_ = new QPushButton(loadIcon(Icon::Cancel), tr("Cancel"), this);
    cancel_button_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(cancel_button_, &QPushButton::clicked, this, &StructEditView::cancelChanges);
    layout->addWidget(cancel_button_);

    addSeparator(*this, *layout);

    auto* new_element_button = new QPushButton(loadIcon(Icon::Plus), tr("New field"), this);
    new_element_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(new_element_button, &QPushButton::clicked, model_, [this]() {
        const char* base_name = "New value";
        std::unordered_set<QString> existing_names;

        for (const auto& field : model_->getElements())
            existing_names.insert(field.name);

        QString new_name = tr(base_name);
        size_t index = 0;
        while (existing_names.contains(new_name))
            new_name = tr(std::format("{} (%1)", base_name).c_str()).arg(++index);

        model_->addElement({ .name = new_name });
    });
    layout->addWidget(new_element_button);

    auto* delete_all_button = new QPushButton(loadIcon(Icon::DeleteAll), tr("Delete all fields"), this);
    delete_all_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(delete_all_button, &QPushButton::clicked, model_, &TableModel::clearElements);
    layout->addWidget(delete_all_button);

    layout->addStretch();
    return layout;
}

void StructEditView::syncDirtyState()
{
    const bool enabled = unsavedChanges();

    apply_button_->setEnabled(enabled);
    cancel_button_->setEnabled(enabled);
}

void StructEditView::syncTitleStatus()
{
    QString error = titleError(core::typing::gStructRegistry.lock());
    syncErrorOutline(*title_edit_, !error.isEmpty(), error);
}

void StructEditView::syncFieldStatuses()
{
    std::unordered_multimap<QString, size_t> lut;

    for (size_t i = 0; i < model_->elementCount(); ++i)
        lut.insert(std::make_pair(model_->at(i).name, i));

    for (auto it = lut.begin(); it != lut.end(); )
    {
        auto [first, last] = lut.equal_range(it->first);
        size_t count = 0;

        for (auto jt = first; jt != last; ++jt)
        {
            ++count;

            if (count <= 1)
                syncErrorOutline(fieldNameEdit(jt->second), false, {});

            if (count > 1)
                syncErrorOutline(fieldNameEdit(jt->second), true, tr("Duplicate field name"));

            if (count == 2)
                syncErrorOutline(fieldNameEdit(first->second), true, tr("Duplicate field name"));
        }

        it = last;
    }
}

QLineEdit& StructEditView::fieldNameEdit(size_t row)
{
    return *static_cast<QLineEdit*>(fields_table_->indexWidget(model_->index(row, kFieldNameColumn)));
}

bool StructEditView::applyChanges()
{
    QString error;

    {
        auto lock = core::typing::gStructRegistry.lock();

        if (error.isEmpty())
            error = titleError(lock);

        if (error.isEmpty())
            error = fieldError();

        if (error.isEmpty())
        {
            auto& struct_data = core::typing::gStructRegistry.get(lock, user_);
            struct_data.title = title_edit_->text().toStdString();

            struct_data.fields.clear();
            for (const auto& field : model_->getElements())
                struct_data.fields.push_back(std::make_pair(field.name.toStdString(), field.type.toType().value()));
        }
    }

    if (!error.isEmpty())
    {
        alerts::cannotApplyChanges(*this, error);
        return false;
    }

    syncDirtyState();
    title_edit_->setFocus();
    return true;
}

void StructEditView::cancelChanges()
{
    loadStruct(user_);
}

QString StructEditView::titleError(const core::typing::StructRegistry::Lock& lock) const
{
    QString title = title_edit_->text();
    std::string std_title = title.toStdString();

    for (const auto& entry : core::typing::gStructRegistry.range(lock))
    {
        if (entry.handle != user_ && entry.element.title == std_title)
            return tr("A struct with the name '%1' already exists").arg(title_edit_->text());
    }

    if (reservedTypeName(title_edit_->text()))
        return tr("Cannot use reserved struct name '%1'").arg(title_edit_->text());

    if (!validCharactersInTypeName(title_edit_->text()))
        return tr("Cannot use invalid characters ('[', ']', '*') in struct name '%1'").arg(title_edit_->text());

    return {};
}

QString StructEditView::fieldError() const
{
    std::unordered_set<QString> field_names;
    for (size_t i = 0; i < model_->elementCount(); ++i)
    {
        if (!field_names.insert(model_->at(i).name).second)
            return tr("There are duplicate field names");
    }

    for (const auto& field : model_->getElements())
    {
        if (!field.type.toType())
            return tr("Field '%1' has incomplete type").arg(field.name);
    }

    return {};
}

EditStructsDialog::EditStructsDialog(QWidget* parent)
    : ModalDialog(tr("Edit Structs"), QSize(720, 405), parent)
{
    initStack();

    connect(list_view_, &StructListView::editRequested, this, &EditStructsDialog::showEditView);
    connect(list_view_, &StructListView::deleteRequested, this, &EditStructsDialog::deleteStruct);
    connect(edit_view_, &StructEditView::backRequested, this, &EditStructsDialog::showListView);
}

void EditStructsDialog::closeEvent(QCloseEvent* event)
{
    if (checkForUnsavedChanges())
        ModalDialog::closeEvent(event);
    else
        event->ignore();
}

void EditStructsDialog::initStack()
{
    auto* layout = new QVBoxLayout(this);

    stack_ = new QStackedWidget(this);

    list_view_ = new StructListView(stack_);
    stack_->addWidget(list_view_);

    edit_view_ = new StructEditView(stack_);
    stack_->addWidget(edit_view_);

    layout->addWidget(stack_);

    showListView();
}

void EditStructsDialog::showListView()
{
    if (checkForUnsavedChanges())
    {
        list_view_->loadStructs();
        stack_->setCurrentWidget(list_view_);
    }
}

void EditStructsDialog::showEditView(core::typing::UserInstance user)
{
    edit_view_->loadStruct(user);
    stack_->setCurrentWidget(edit_view_);
}

void EditStructsDialog::deleteStruct(core::typing::UserInstance user)
{
    core::typing::gStructRegistry.remove(core::typing::gStructRegistry.lock(), user);
}

bool EditStructsDialog::checkForUnsavedChanges()
{
    if (stack_->currentWidget() == edit_view_)
        return edit_view_->alertUnsavedChanges();

    return true;
}

} // crucible::gui
