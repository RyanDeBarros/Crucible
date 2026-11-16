#pragma once

#include <crucible/core/typing/kind.hpp>

#include <crucible/gui/util/modal_dialog.hpp>

#include <QAbstractListModel>
#include <QAbstractTableModel>

class QLineEdit;
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
    QTableView& table_;
    QVector<EnumTemplate> enums_;

public:
    explicit EnumTableModel(QTableView& table, QObject* parent = nullptr);

signals:
    void editRequested(core::typing::Specialization enum_instance);
    void deleteRequested(core::typing::Specialization enum_instance);

public:
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addEnum(QString name, core::typing::Specialization instance);
    void setEnums(std::vector<std::pair<QString, core::typing::Specialization>> enums);
};

class EnumListView
    : public QWidget
{
    Q_OBJECT;

private:
    EnumTableModel* model_ = nullptr;

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

class EnumValuesTableModel
    : public QAbstractTableModel
{
    Q_OBJECT;

public:
    enum Column
    {
        DragHandle,
        Value,
        Delete,
        _Count
    };

private:
    QVector<QString> values_;

public:
    explicit EnumValuesTableModel(QTableView& table, QWidget* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    bool dropMimeData(
        const QMimeData* data,
        Qt::DropAction action,
        int row,
        int column,
        const QModelIndex& parent) override;

    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void setValues(const std::vector<std::string>& values);
    std::vector<std::string> getValues() const;

    void newValue();
    void clearValues();
};

class EnumEditView
    : public QWidget
{
    Q_OBJECT;

private:
    EnumValuesTableModel* model_ = nullptr;

    QLineEdit* title_edit_ = nullptr;
    QLineEdit* id_label_ = nullptr;
    QTableView* values_table_ = nullptr;
    core::typing::Specialization enum_instance_ = {};

public:
    explicit EnumEditView(QWidget* parent = nullptr);

signals:
    void backRequested();

public:
    void loadEnum(core::typing::Specialization enum_instance);

private:
    QVBoxLayout* initList();
    QVBoxLayout* initOptions();

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

private:
    void initStack();

    void showListView();
    void showEditView(core::typing::Specialization enum_instance);
    void deleteEnum(core::typing::Specialization enum_instance);
};

} // crucible::gui
