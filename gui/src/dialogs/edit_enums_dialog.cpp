#include <crucible/gui/dialogs/edit_enums_dialog.hpp>

#include <crucible/gui/util/alerts.hpp>
#include <crucible/gui/util/delegate_columns.hpp>
#include <crucible/gui/util/item_delegates.hpp>
#include <crucible/gui/util/layout_helpers.hpp>
#include <crucible/gui/util/outlines.hpp>
#include <crucible/gui/util/repr.hpp>

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

static QString repr(core::typing::UserInstance user)
{
    return QString::fromStdString(std::format("0x{:08X}", user));
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

    auto lock = core::typing::gEnumRegistry.lock();
    for (auto [handle, element] : core::typing::gEnumRegistry.range(lock))
    {
        enums.push_back({
            .title = QString::fromStdString(element.title),
            .user = handle,
            .id = repr(handle)
        });
    }

    model_->setElements(std::move(enums));
}

QTableView* EnumListView::initTable()
{
    TableModel::ColumnList columns{
        std::make_unique<DataColumn<EnumTemplate>>(
            [](size_t row, const EnumTemplate& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return element.title;
                else
                    return {};
            }),
        std::make_unique<EditButtonColumn<EnumTemplate>>(),
        std::make_unique<DataColumn<EnumTemplate>>(
            [](size_t row, const EnumTemplate& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return element.id;
                else
                    return {};
            }),
        std::make_unique<DeleteButtonColumn<EnumTemplate>>()
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
        Q_EMIT editRequested(model_->at(row).user);
    });
    connect(model_, &TableModel::deleteRequested, this, [this](size_t row) {
        Q_EMIT deleteRequested(model_->at(row).user);
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
    core::typing::UserInstance user;

    {
        auto lock = core::typing::gEnumRegistry.lock();

        std::unordered_set<std::string> titles;
        for (const auto& entry : core::typing::gEnumRegistry.range(lock))
            titles.insert(entry.element.title);

        std::string std_title = title.toStdString();
        size_t suffix = 0;
        while (titles.contains(std_title))
            std_title = std::format("{} ({})", title.toStdString(), ++suffix);
        title = QString::fromStdString(std_title);

        user = core::typing::gEnumRegistry.add(lock, {
            .title = title.toStdString(),
            .names = {}
        });
    }
    
    model_->addElement({
        .title = std::move(title),
        .user = user,
        .id = repr(user)
    });
}

EnumEditView::EnumEditView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    layout->addLayout(initMainLayout(), 1);
    addSeparator(*this, *layout);
    layout->addLayout(initOptions());
}

void EnumEditView::loadEnum(core::typing::UserInstance user)
{
    user_ = user;

    {
        QSignalBlocker blockers[] = {
            QSignalBlocker(title_edit_),
            QSignalBlocker(model_),
        };

        auto lock = core::typing::gEnumRegistry.lock();

        const auto& enum_data = core::typing::gEnumRegistry.get(lock, user_);

        id_label_->setText(repr(user_));
        title_edit_->setText(QString::fromStdString(enum_data.title));

        std::vector<QString> names;
        for (const auto& name : enum_data.names)
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
    auto lock = core::typing::gEnumRegistry.lock();

    const auto& enum_data = core::typing::gEnumRegistry.get(lock, user_);

    if (enum_data.title != title_edit_->text())
        return true;

    const auto& names = model_->getElements();
    if (names.size() != enum_data.names.size())
        return true;

    for (size_t i = 0; i < names.size(); ++i)
    {
        if (names[i] != enum_data.names[i])
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

QVBoxLayout* EnumEditView::initMainLayout()
{
    auto* layout = new QVBoxLayout();

    auto* form = new QFormLayout();

    id_label_ = new QLineEdit(this);
    id_label_->setReadOnly(true);
    form->addRow(tr("ID"), id_label_);

    title_edit_ = new QLineEdit(this);
    connect(title_edit_, &QLineEdit::editingFinished, this, &EnumEditView::syncDirtyState);
    connect(title_edit_, &QLineEdit::editingFinished, this, &EnumEditView::syncTitleStatus);
    form->addRow(tr("Name"), title_edit_);

    layout->addLayout(form);

    layout->addWidget(initTable());

    return layout;
}

static constexpr size_t kValueColumn = 2;

QTableView* EnumEditView::initTable()
{
    values_table_ = new QTableView(this);

    TableModel::ColumnList columns{
        std::make_unique<DragHandleColumn<QString>>(),
        std::make_unique<LabelColumn<QString>>(
            [](size_t row, const QString& element, int role) -> QVariant {
                if (role == Qt::DisplayRole)
                    return QString::number(row);
                else
                    return {};
            }),
        std::make_unique<LineEditColumn<QString>>(
            [](size_t row, const QString& element, int role) -> QVariant {
                if (role == Qt::EditRole)
                    return element;
                else
                    return {};
            },
            [](size_t row, QString& element, const QVariant& value, int role) -> bool {

                if (role == Qt::EditRole)
                {
                    element = value.toString();
                    return true;
                }
                else
                    return false;
            }),
        std::make_unique<DeleteButtonColumn<QString>>()
    };

    model_ = new TableModel(std::move(columns), *values_table_, { .reorderable = true }, this);
    connect(model_, &TableModel::rowsInserted, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::rowsRemoved, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::rowsMoved, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::dataChanged, this, &EnumEditView::syncDirtyState);
    connect(model_, &TableModel::dataChanged, this, &EnumEditView::syncValueStatuses);
    connect(model_, &TableModel::modelReset, this, &EnumEditView::syncDirtyState);

    return values_table_;
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
        const char* base_name = "New value";
        std::unordered_set<QString> existing_names;

        for (const auto& name : model_->getElements())
            existing_names.insert(name);

        QString new_name = tr(base_name);
        size_t index = 0;
        while (existing_names.contains(new_name))
            new_name = tr(std::format("{} (%1)", base_name).c_str()).arg(++index);

        model_->addElement(new_name);
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

void EnumEditView::syncTitleStatus()
{
    bool duplicate = false;

    auto lock = core::typing::gEnumRegistry.lock();

    for (const auto& entry : core::typing::gEnumRegistry.range(lock))
    {
        if (entry.handle != user_ && entry.element.title == title_edit_->text().toStdString())
        {
            duplicate = true;
            break;
        }
    }

    syncErrorOutline(*title_edit_, duplicate, tr("An enum already exists with this name"));

    if (reservedTypeName(title_edit_->text()))
        syncErrorOutline(*title_edit_, true, tr("Cannot use reserved name"));

    if (!validCharactersInTypeName(title_edit_->text()))
        syncErrorOutline(*title_edit_, true, tr("Cannot use invalid characters ('[', ']', '*') in name"));
}

void EnumEditView::syncValueStatuses()
{
    std::unordered_multimap<QString, size_t> lut;

    for (size_t i = 0; i < model_->elementCount(); ++i)
        lut.insert(std::make_pair(model_->at(i), i));

    for (auto it = lut.begin(); it != lut.end(); )
    {
        auto [first, last] = lut.equal_range(it->first);
        size_t count = 0;

        for (auto jt = first; jt != last; ++jt)
        {
            ++count;

            if (count <= 1)
                syncErrorOutline(valueEdit(jt->second), false, {});

            if (count > 1)
                syncErrorOutline(valueEdit(jt->second), true, tr("Duplicate enum value"));

            if (count == 2)
                syncErrorOutline(valueEdit(first->second), true, tr("Duplicate enum value"));
        }

        it = last;
    }
}

QLineEdit& EnumEditView::valueEdit(size_t row)
{
    return *static_cast<QLineEdit*>(values_table_->indexWidget(model_->index(row, kValueColumn)));
}

void EnumEditView::applyChanges()
{
    {
        auto lock = core::typing::gEnumRegistry.lock();

        std::unordered_set<std::string> existing_titles;
        for (const auto& entry : core::typing::gEnumRegistry.range(lock))
        {
            if (entry.handle != user_)
                existing_titles.insert(entry.element.title);
        }

        std::string title = title_edit_->text().toStdString();
        if (existing_titles.contains(title))
        {
            alerts::cannotApplyChanges(
                *this,
                tr("An enum with the name '%1' already exists")
                    .arg(title));
            return;
        }

        if (reservedTypeName(title_edit_->text()))
        {
            alerts::cannotApplyChanges(
                *this,
                tr("Cannot use reserved enum name '%1'")
                    .arg(title_edit_->text()));
            return;
        }

        if (!validCharactersInTypeName(title_edit_->text()))
        {
            alerts::cannotApplyChanges(
                *this,
                tr("Cannot use invalid characters ('[', ']', '*') in enum name '%1'")
                    .arg(title_edit_->text()));
            return;
        }

        std::unordered_set<QString> values;
        for (size_t i = 0; i < model_->elementCount(); ++i)
        {
            if (!values.insert(model_->at(i)).second)
            {
                alerts::cannotApplyChanges(
                    *this,
                    tr("There are duplicate enum values"));
                return;
            }
        }

        auto& enum_data = core::typing::gEnumRegistry.get(lock, user_);
        enum_data.title = std::move(title);

        enum_data.names.clear();
        for (const auto& name : model_->getElements())
            enum_data.names.push_back(name.toStdString());
    }

    syncDirtyState();
    title_edit_->setFocus();
}

void EnumEditView::cancelChanges()
{
    loadEnum(user_);
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

void EditEnumsDialog::showEditView(core::typing::UserInstance user)
{
    edit_view_->loadEnum(user);
    stack_->setCurrentWidget(edit_view_);
}

void EditEnumsDialog::deleteEnum(core::typing::UserInstance user)
{
    core::typing::gEnumRegistry.remove(core::typing::gEnumRegistry.lock(), user);
}

bool EditEnumsDialog::checkForUnsavedChanges()
{
    if (stack_->currentWidget() == edit_view_)
        return edit_view_->alertUnsavedChanges();

    return true;
}

} // crucible::gui
