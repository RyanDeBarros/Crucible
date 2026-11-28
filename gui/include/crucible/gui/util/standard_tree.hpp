#pragma once

#include <QProxyStyle>
#include <QStyledItemDelegate>
#include <QTreeWidget>

namespace crucible::gui
{

class StandardTree
    : public QTreeWidget
{
public:
    struct Config
    {
        int icon_width = 16;
        int indentation = 12;
        int category_height = 24;
        std::optional<int> spacer_role;
        int spacer_height = 8;
    };

private:
    class TreeStyle
        : public QProxyStyle
    {
    public:
        int icon_width;

        using QProxyStyle::QProxyStyle;

        void drawPrimitive(
            PrimitiveElement element,
            const QStyleOption* option,
            QPainter* painter,
            const QWidget* widget = nullptr) const override;
    };

    class TreeItemDelegate
        : public QStyledItemDelegate
    {
    public:
        int text_offset;

        using QStyledItemDelegate::QStyledItemDelegate;

        void paint(
            QPainter* painter,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;
    };

private:
    Config config_;

public:
    explicit StandardTree(Config config = {}, QWidget* parent = nullptr);

    QTreeWidgetItem* newCategory(const QString& title, bool bold = true, bool italic = true);
    QTreeWidgetItem* newSpacer(QTreeWidgetItem* parent = nullptr) const;

protected:
    void drawRow(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

} // crucible::gui
