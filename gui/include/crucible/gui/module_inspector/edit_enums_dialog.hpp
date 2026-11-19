#pragma once

#include <crucible/core/typing/kind.hpp>

#include <crucible/gui/util/element_table_model.hpp>
#include <crucible/gui/util/modal_dialog.hpp>

class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableView;
class QVBoxLayout;

namespace crucible::gui
{

struct EnumTemplate
{
    QString name;
    core::typing::Specialization instance;
    QString id;
};

class EnumListView
    : public QWidget
{
    Q_OBJECT;

private:
    using TableModel = ElementTableModel<EnumTemplate, 4>;

    TableModel* model_ = nullptr;

public:
    explicit EnumListView(QWidget* parent = nullptr);

signals:
    void editRequested(core::typing::Specialization enum_instance);
    void deleteRequested(core::typing::Specialization enum_instance);

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
    using TableModel = ElementTableModel<QString, 3>;

    TableModel* model_ = nullptr;

    QLineEdit* title_edit_ = nullptr;
    QLineEdit* id_label_ = nullptr;
    core::typing::Specialization enum_instance_ = {};

    QPushButton* apply_button_ = nullptr;
    QPushButton* cancel_button_ = nullptr;

public:
    explicit EnumEditView(QWidget* parent = nullptr);

signals:
    void backRequested();

public:
    void loadEnum(core::typing::Specialization enum_instance);
    bool unsavedChanges() const;
    bool alertUnsavedChanges();

private:
    QVBoxLayout* initList();
    QTableView* initTable();
    QVBoxLayout* initOptions();

    void syncDirtyState();

    void applyChanges();
    void cancelChanges();
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
    void showEditView(core::typing::Specialization enum_instance);
    void deleteEnum(core::typing::Specialization enum_instance);

    bool checkForUnsavedChanges();
};

} // crucible::gui
