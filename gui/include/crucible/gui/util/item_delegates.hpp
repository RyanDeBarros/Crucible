#pragma once

#include <crucible/gui/util/icons.hpp>

#include <QAbstractItemView>
#include <QStyledItemDelegate>

namespace crucible::gui
{

class ButtonDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT;

private:
    QAbstractItemView& view_;
    QString tooltip_;
    Icon icon_;

public:
    explicit ButtonDelegate(
        QAbstractItemView& view,
        int column,
        Icon icon,
        QString tooltip,
        QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem&,
        const QModelIndex&) const override;

signals:
    void onClick(const QModelIndex& index) const;
};

class LineEditDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT;

private:
    QAbstractItemView& view_;
    QString placeholder_text_;

public:
    explicit LineEditDelegate(
        QAbstractItemView& view,
        int column,
        QString placeholder_text = {},
        QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem&,
        const QModelIndex&) const override;

    void setEditorData(QWidget*, const QModelIndex&) const override;
    void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const override;
};

} // crucible::gui
