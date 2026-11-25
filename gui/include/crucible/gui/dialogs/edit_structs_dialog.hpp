#pragma once

#include <crucible/core/typing/type.hpp>

#include <crucible/gui/util/element_table_model.hpp>
#include <crucible/gui/util/modal_dialog.hpp>

class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableView;
class QVBoxLayout;

namespace crucible::gui
{

class StructListView
    : public QWidget
{
    Q_OBJECT;

private:
    struct StructTemplate
    {
        QString title;
        core::typing::UserInstance user;
        QString id;
    };

    using TableModel = ElementTableModel<StructTemplate, 4>;

    TableModel* model_ = nullptr;

public:
    explicit StructListView(QWidget* parent = nullptr);

signals:
    void editRequested(core::typing::UserInstance user);
    void deleteRequested(core::typing::UserInstance user);

public:
    void loadStructs();

private:
    QTableView* initTable();
    QVBoxLayout* initOptions();

    void newStruct();
};

class StructEditView
    : public QWidget
{
    Q_OBJECT;

private:
    struct FieldTemplate
    {
        QString name;
        std::optional<core::typing::Type> type;
        std::optional<QString> repr;
    };

    using TableModel = ElementTableModel<FieldTemplate, 4>;

    TableModel* model_ = nullptr;
    QTableView* fields_table_ = nullptr;

    QLineEdit* title_edit_ = nullptr;
    QLineEdit* id_label_ = nullptr;
    core::typing::UserInstance user_ = {};

    QPushButton* apply_button_ = nullptr;
    QPushButton* cancel_button_ = nullptr;

public:
    explicit StructEditView(QWidget* parent = nullptr);

signals:
    void backRequested();
    void editFieldTypeRequested(size_t field_idx, core::typing::Type type);

public:
    void loadStruct(core::typing::UserInstance user);
    void setFieldType(size_t field_idx, core::typing::Type type);
    bool unsavedChanges() const;
    bool alertUnsavedChanges();

private:
    QVBoxLayout* initMainLayout();
    QTableView* initTable();
    QVBoxLayout* initOptions();

    void syncDirtyState();
    void syncTitleStatus();
    void syncFieldStatuses();
    QLineEdit& fieldNameEdit(size_t row);

    void applyChanges();
    void cancelChanges();
};

class EditFieldTypeView
    : public QWidget
{
    Q_OBJECT;

private:
    size_t field_idx_;
    std::optional<core::typing::Type> working_type_;

public:
    explicit EditFieldTypeView(QWidget* parent = nullptr);

signals:
    void acceptRequested(size_t field_idx, core::typing::Type type);
    void cancelRequested();

public:
    void loadType(size_t field_idx, core::typing::Type type);

private:
    QVBoxLayout* initMainLayout();
    QVBoxLayout* initOptions();
};

class EditStructsDialog
    : public ModalDialog
{
    Q_OBJECT;

private:
    QStackedWidget* stack_ = nullptr;

    StructListView* list_view_ = nullptr;
    StructEditView* edit_view_ = nullptr;
    EditFieldTypeView* field_type_view_ = nullptr;

public:
    explicit EditStructsDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event) override;

private:
    void initStack();

    void showListView();
    void showEditView(core::typing::UserInstance user);
    void deleteStruct(core::typing::UserInstance user);

    bool checkForUnsavedChanges();
};

} // crucible::gui
