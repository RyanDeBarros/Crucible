#pragma once

#include <crucible/core/typing/kind.hpp>

#include <crucible/gui/util/modal_dialog.hpp>

#include <QAbstractTableModel>

class QLineEdit;
class QListWidget;
class QStackedWidget;
class QTableView;
class QVBoxLayout;

namespace crucible::gui
{

class EnumTableModel
    : public QAbstractTableModel
{
    Q_OBJECT;

public:
    enum Column
    {
        Name,
        Edit,
        Id,
        Delete,
        _Count
    };

    struct EnumTemplate
    {
        QString name;
        core::typing::Specialization instance;
        QString id;
    };

private:
    QVector<EnumTemplate> enums_;

public:
    explicit EnumTableModel(QTableView& table, QObject* parent = nullptr);

signals:
    void editRequested(core::typing::Specialization enum_instance);
    void deleteRequested(core::typing::Specialization enum_instance);

public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addEnum(QString name, core::typing::Specialization instance);
};

class EnumListView
    : public QWidget
{
    Q_OBJECT;

private:
    EnumTableModel* model_;

public:
    explicit EnumListView(QWidget* parent = nullptr);

signals:
    void editRequested(core::typing::Specialization enum_instance);
    void deleteRequested(core::typing::Specialization enum_instance);

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
    QLineEdit* title_edit_ = nullptr;
    QLineEdit* id_label_ = nullptr;
    QListWidget* elements_list_ = nullptr;

public:
    explicit EnumEditView(QWidget* parent = nullptr);

signals:
    void backRequested();

public:
    void loadEnum(core::typing::Specialization enum_instance);

private:
    QVBoxLayout* initList();
    QVBoxLayout* initOptions();
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

private:
    void initStack();

    void showListView();
    void showEditView(core::typing::Specialization enum_instance);
    void deleteEnum(core::typing::Specialization enum_instance);
};

} // crucible::gui
