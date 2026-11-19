#pragma once

#include <crucible/gui/util/icons.hpp>

#include <QAbstractItemView>
#include <QStyledItemDelegate>

namespace crucible::gui
{

class IconDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT;

public:
    struct Config
    {
        bool show_select_highlight = false;
    };

private:
    Icon icon_;
    Config config_;

public:
    IconDelegate(Icon icon, Config config = {}, QObject* parent = nullptr);

    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const override;

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

class ButtonDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT;

public:
    struct Config
    {
        QString tooltip = {};
        bool show_select_highlight = false;
    };

private:
    QAbstractItemView& view_;
    Icon icon_;
    Config config_;

public:
    explicit ButtonDelegate(
        QAbstractItemView& view,
        Icon icon,
        Config config = {},
        QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem&,
        const QModelIndex&) const override;

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

signals:
    void onClick(const QModelIndex& index) const;
};

class LineEditDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT;

public:
    struct Config
    {
        QString placeholder_text = {};
        bool show_select_highlight = false;
    };

private:
    Config config_;

public:
    explicit LineEditDelegate(
        Config config = {},
        QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem&,
        const QModelIndex&) const override;

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget*, const QModelIndex&) const override;
    void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const override;
};

} // crucible::gui
