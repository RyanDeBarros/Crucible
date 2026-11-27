#pragma once

#include <crucible/core/typing/kind.hpp>

#include <crucible/gui/util/element_table_model.hpp>
#include <crucible/gui/util/modal_dialog.hpp>

#include <crucible/core/typing/user_registries.hpp>

class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableView;
class QVBoxLayout;

namespace crucible::gui
{

class EnumListView
    : public QWidget
{
    Q_OBJECT;

private:
    struct EnumTemplate
    {
        QString title;
        core::typing::UserInstance user;
        QString id;
    };

    using TableModel = ElementTableModel<EnumTemplate, 4>;

    TableModel* model_ = nullptr;

public:
    explicit EnumListView(QWidget* parent = nullptr);

signals:
    void editRequested(core::typing::UserInstance user);
    void deleteRequested(core::typing::UserInstance user);

public:
    void loadEnums();

private:
    QTableView* initTable();
    QVBoxLayout* initOptions();

    void newEnum();
};

class EnumEditView
    : public QWidget
{
    Q_OBJECT;

private:
    using TableModel = ElementTableModel<QString, 4>;

    TableModel* model_ = nullptr;
    QTableView* values_table_ = nullptr;

    QLineEdit* title_edit_ = nullptr;
    QLineEdit* id_label_ = nullptr;
    core::typing::UserInstance user_ = {};

    QPushButton* apply_button_ = nullptr;
    QPushButton* cancel_button_ = nullptr;

public:
    explicit EnumEditView(QWidget* parent = nullptr);

signals:
    void backRequested();

public:
    void loadEnum(core::typing::UserInstance user);
    bool unsavedChanges() const;
    bool alertUnsavedChanges();

private:
    QVBoxLayout* initMainLayout();
    QTableView* initTable();
    QVBoxLayout* initOptions();

    void syncDirtyState();
    void syncTitleStatus();
    void syncValueStatuses();
    QLineEdit& valueEdit(size_t row);

    bool applyChanges();
    void cancelChanges();

    QString titleError(const core::typing::EnumRegistry::Lock& lock) const;
    QString valueError() const;
};

class EditEnumsDialog
    : public ModalDialog
{
    Q_OBJECT;

private:
    QStackedWidget* stack_ = nullptr;

    EnumListView* list_view_ = nullptr;
    EnumEditView* edit_view_ = nullptr;

public:
    explicit EditEnumsDialog(QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event) override;

private:
    void initStack();

    void showListView();
    void showEditView(core::typing::UserInstance user);
    void deleteEnum(core::typing::UserInstance user);

    bool checkForUnsavedChanges();
};

} // crucible::gui
