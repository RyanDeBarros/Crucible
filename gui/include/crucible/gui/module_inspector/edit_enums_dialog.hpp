#pragma once

#include <crucible/gui/util/modal_dialog.hpp>

#include <QAbstractTableModel>

class QFormLayout;
class QHBoxLayout;
class QLineEdit;
class QListWidget;
class QPushButton;
class QStackedWidget;

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

    struct Entry
    {
        QString name;
        QString id;
    };

private:
    QVector<Entry> entries_;

public:
    explicit EnumTableModel(QObject* parent = nullptr);

signals:
    void editRequested(int row);
    void deleteRequested(int row);

public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void setEntries(QVector<Entry> entries);
    void addEntry(Entry entry);
    void removeEntry(int row);

    const Entry* entryAt(int row) const;
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
    void editEnumRequested(size_t idx);

private:
    void newEnum();
};

class EnumEditView
    : public QWidget
{
    Q_OBJECT;

private:
    QLineEdit* title_edit_ = nullptr;
    QListWidget* elements_list_ = nullptr;

public:
    explicit EnumEditView(QWidget* parent = nullptr);

signals:
    void backRequested();

public:
    void loadEnum(size_t idx);

private:
    QHBoxLayout* initNavigation();
    QFormLayout* initHeader();
    QListWidget* initElementList();
    QHBoxLayout* initListActions();
    QHBoxLayout* initFooter();
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
    void showEditView(size_t idx);
};

} // crucible::gui
