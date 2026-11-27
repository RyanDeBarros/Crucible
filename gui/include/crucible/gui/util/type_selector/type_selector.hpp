#pragma once

#include <crucible/gui/util/type_selector/opt_type.hpp>

#include <crucible/core/typing/user_registries.hpp>

#include <QWidget>

namespace crucible::gui
{

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
    std::optional<std::vector<size_t>> hovered_path_;

public:
    explicit TypeSelector(QWidget* parent = nullptr);

signals:
    void editingFinished();
    void layoutSizeChanged();

public:
    QSize sizeHint() const override;

    OptType type() const;
    void setType(OptType type);

    static QSize typeSizeHint(const OptType& type, const QFont& font);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void rebuildLayout();
    void openPopup(std::vector<size_t> path, const QRect& rect);
    void selectSimplex(const std::vector<size_t>& path, Simplex simplex);
    OptType* findNode(std::span<const size_t> path);

    std::optional<std::vector<size_t>> hitTest(const NodeLayout& layout, const QPoint& pos) const;
    const NodeLayout& layoutAtPath(const NodeLayout& root, const std::vector<size_t>& path) const;
    void paintNode(
        QPainter* painter,
        const OptType& node,
        const std::vector<size_t>& path, // TODO using Path = std::vector<size_t>
        const NodeLayout& layout,
        const QFont& font,
        const QPalette& palette) const;

    static QSize calculateSize(const OptType& node, const QFont& font);
    static NodeLayout calculateLayout(const OptType& node, const QRect& rect, const QFont& font);
};

} // crucible::gui
