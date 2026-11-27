#pragma once

#include <crucible/gui/util/element_table_model.hpp>

#include <crucible/core/typing/type.hpp>
#include <crucible/core/typing/user_registries.hpp>

#include <QStyledItemDelegate>
#include <QWidget>

namespace crucible::gui
{

struct OptType
{
    std::optional<core::typing::Simplex> simplex = std::nullopt;
    std::vector<OptType> arguments;

    OptType() = default;
    OptType(core::typing::Simplex simplex);
    OptType(const core::typing::Type& type);

    std::optional<core::typing::Type> toType() const;
};

class TypeSelector
    : public QWidget
{
    Q_OBJECT;

public:
    using Type = core::typing::Type;
    using Simplex = core::typing::Simplex;
    using Kind = core::typing::Kind;
    using UserInstance = core::typing::UserInstance;

    struct Entry
    {
        QString name;
        Simplex simplex;
        QColor color;

        Entry(const core::typing::UserRegistryLock& lock, Simplex simplex);
    };

private:
    struct NodeLayout
    {
        QRect rect;
        std::vector<NodeLayout> children;
    };

    class TypePopup;

private:
    OptType root_;

    TypePopup* popup_ = nullptr;
    NodeLayout layout_;

public:
    explicit TypeSelector(QWidget* parent = nullptr);

signals:
    // TODO connect to this in table
    void editingFinished();
    void layoutSizeChanged();

public:
    QSize sizeHint() const override;

    OptType type() const;
    void setType(OptType type);

    static QSize typeSizeHint(const OptType& type, const QFont& font);
    static void paintType(
        QPainter* painter,
        const QRect& rect,
        const OptType& type,
        const QFont& font,
        const QPalette& palette,
        bool selected = false);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void rebuildLayout();
    void openPopup(std::vector<size_t> path, const QRect& rect);
    void selectSimplex(const std::vector<size_t>& path, Simplex simplex);
    OptType* findNode(std::span<const size_t> path);

    std::optional<std::vector<size_t>> hitTest(const NodeLayout& layout, const QPoint& pos) const;
    const NodeLayout& layoutAtPath(const NodeLayout& root, const std::vector<size_t>& path) const;

    static QSize calculateSize(const OptType& node, const QFont& font);
    static NodeLayout calculateLayout(const OptType& node, const QRect& rect, const QFont& font);
    static void paintNode(
        QPainter* painter,
        const OptType& node,
        const NodeLayout& layout,
        const QFont& font,
        const QPalette& palette);
};

class TypeSelectorDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TypeSelectorDelegate(QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(
        QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index) const override;

    QSize sizeHint(
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

template<typename Element>
struct TypeSelectorColumn
    : public DelegateColumn<Element>
{
    using Super = DelegateColumn<Element>;

    using GetterFn = std::function<OptType(size_t row, const Element& element)>;
    using SetterFn = std::function<bool(size_t row, Element& element, OptType type)>;

    GetterFn get_type;
    SetterFn set_type;

    TypeSelectorColumn(GetterFn get_type, SetterFn set_type, ColumnConfig config = ColumnConfig::kStretchXFitY)
        : Super(std::move(config))
        , get_type(std::move(get_type))
        , set_type(std::move(set_type))
    {
    }

    QAbstractItemDelegate* newDelegate(ElementTableModelBase& table) const override
    {
        return new TypeSelectorDelegate(&table);
    }

    QVariant data(size_t row, const Element& element, int role) const override
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QVariant::fromValue(get_type(row, element));
        else
            return {};
    }

    bool setData(size_t row, Element& element, const QVariant& value, int role) override
    {
        if (role != Qt::EditRole)
            return false;

        if (value.canConvert<OptType>())
            return set_type(row, element, value.value<OptType>());
        else
            return false;
    }
};

} // crucible::gui
