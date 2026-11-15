#pragma once

#include <crucible/gui/util/icons.hpp>

#include <QStyledItemDelegate>
#include <QTableView>

namespace crucible::gui
{

class ButtonDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT;

private:
    QString tooltip_;
    Icon icon_;

public:
    explicit ButtonDelegate(Icon icon, QString tooltip, QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem&,
        const QModelIndex&) const override;

    void setEditorData(QWidget*, const QModelIndex&) const override {}
    void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const override {}

signals:
    void onClick(const QModelIndex& pos) const;
};

} // crucible::gui
