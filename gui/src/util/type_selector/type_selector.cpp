#include <crucible/gui/util/type_selector/type_selector.hpp>

#include <crucible/gui/util/fuzzy.hpp>
#include <crucible/gui/util/repr.hpp>

#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace crucible::gui
{

constexpr int kPadding = 4;
constexpr int kSpacing = 3;
constexpr int kRadius = 4;

static const char* kUnselected = "--select--";
static const QColor kUnselectedColor = QColor(0xAA, 0xAA, 0xAA);

static QColor colorForKind(TypeSelector::Kind kind)
{
    // TODO color scheme + different colors for each built-in type

    switch (kind)
    {
    case TypeSelector::Kind::Bool:
    case TypeSelector::Kind::Int:
    case TypeSelector::Kind::Float:
    case TypeSelector::Kind::String:
        return QColor(0x47, 0x7C, 0xA8);

    case TypeSelector::Kind::Pointer:
    case TypeSelector::Kind::List:
    case TypeSelector::Kind::Set:
    case TypeSelector::Kind::Map:
        return QColor(0x98, 0x73, 0xB5);

    case TypeSelector::Kind::Enum:
        return QColor(0x4C, 0x96, 0x7C);

    case TypeSelector::Kind::Struct:
        return QColor(0xB5, 0x8A, 0x4B);

    case TypeSelector::Kind::Joker:
        return QColor(0x77, 0x77, 0x77);
    }

    return kUnselectedColor;
}

TypeSelector::Entry::Entry(const core::typing::UserRegistryLock& lock, Simplex simplex)
    : name(repr(lock, simplex))
    , simplex(simplex)
    , color(colorForKind(simplex.kind))
{
}

class TypeSelector::TypePopup
    : public QFrame
{
public:
    enum Role
    {
        KindRole = Qt::UserRole,
        UserInstanceRole = KindRole + 1,
    };

    TypeSelector& selector;
    std::vector<size_t> path;
    QLineEdit* search = nullptr;
    QTreeWidget* tree = nullptr;

    explicit TypePopup(TypeSelector& selector)
        : QFrame(&selector, Qt::Popup)
        , selector(selector)
    {
        setFrameShape(QFrame::StyledPanel);
        setMinimumWidth(240);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(5, 5, 5, 5);

        search = new QLineEdit(this);
        search->setPlaceholderText(tr("Search types..."));

        tree = new QTreeWidget(this);
        tree->setHeaderHidden(true);
        tree->setRootIsDecorated(false);

        layout->addWidget(search);
        layout->addWidget(tree);

        connect(search, &QLineEdit::textChanged, this, [this](const QString& text) {
            for (int i = 0; i < tree->topLevelItemCount(); ++i)
            {
                auto* category = tree->topLevelItem(i);
                bool anyVisible = false;

                for (int j = 0; j < category->childCount(); ++j)
                {
                    auto* item = category->child(j);
                    const bool visible = fuzzyContains(item->text(0), text);

                    item->setHidden(!visible);
                    anyVisible |= visible;
                }

                category->setHidden(!anyVisible);
            }
        });

        // TODO clearer tree styling
        connect(tree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
            if (!item || item->childCount() > 0)
                return;

            const Kind kind = static_cast<Kind>(item->data(0, TypePopup::KindRole).toInt());
            const UserInstance user = static_cast<UserInstance>(item->data(0, TypePopup::UserInstanceRole).toInt());

            this->selector.selectSimplex(this->path, { .kind = kind, .user = user });
            hide();
        });
    }

    void addCategory(const QString& title, std::vector<Entry> entries)
    {
        auto* category = new QTreeWidgetItem(tree, { title });

        for (const Entry& entry : entries)
        {
            auto* item = new QTreeWidgetItem(category, { entry.name });

            item->setData(0, KindRole, static_cast<int>(entry.simplex.kind));
            item->setData(0, UserInstanceRole, static_cast<int>(entry.simplex.user));
        }

        category->setExpanded(true);
    };
};

TypeSelector::TypeSelector(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFocusPolicy(Qt::StrongFocus);
}

QSize TypeSelector::sizeHint() const
{
    return calculateSize(root_, font());
}

OptType TypeSelector::type() const
{
    return root_;
}

void TypeSelector::setType(OptType type)
{
    root_ = std::move(type);
    rebuildLayout();
    updateGeometry();
    update();
    Q_EMIT layoutSizeChanged();
}

QSize TypeSelector::typeSizeHint(const OptType& type, const QFont& font)
{
    return calculateSize(type, font);
}

void TypeSelector::paintType(
    QPainter* painter,
    const QRect& rect,
    const OptType& type,
    const QFont& font,
    const QPalette& palette,
    bool selected)
{
    const QSize size = calculateSize(type, font);
    const QRect expression_rect(
        rect.left(),
        rect.top() + (rect.height() - size.height()) / 2,
        size.width(),
        size.height());

    NodeLayout layout = calculateLayout(type, expression_rect, font);

    painter->save();

    if (selected)
        painter->fillRect(rect, palette.color(QPalette::Highlight));

    painter->setFont(font);
    paintNode(painter, type, layout, font, palette);

    painter->restore();
}

void TypeSelector::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    rebuildLayout();
    paintNode(&painter, root_, layout_, font(), palette());
}

void TypeSelector::mousePressEvent(QMouseEvent* event)
{
    if (const auto path = hitTest(layout_, event->pos()))
        openPopup(*path, layoutAtPath(layout_, *path).rect);

    QWidget::mousePressEvent(event);
}

void TypeSelector::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    rebuildLayout();
}

void TypeSelector::rebuildLayout()
{
    const QSize size = calculateSize(root_, font());
    const QRect expression_rect(0, (height() - size.height()) / 2, size.width(), size.height());
    layout_ = calculateLayout(root_, expression_rect, font());
}

void TypeSelector::openPopup(std::vector<size_t> path, const QRect& rect)
{
    if (!popup_)
        popup_ = new TypePopup(*this);

    popup_->path = std::move(path);
    popup_->tree->clear();

    core::typing::UserRegistryLock lock;

    popup_->addCategory("Primitive", {
        Entry(lock, { .kind = Kind::Bool }),
        Entry(lock, { .kind = Kind::Int }),
        Entry(lock, { .kind = Kind::Float }),
        Entry(lock, { .kind = Kind::String }),
        Entry(lock, { .kind = Kind::Joker })
    });

    popup_->addCategory("Template", {
        Entry(lock, { .kind = Kind::Pointer }),
        Entry(lock, { .kind = Kind::List }),
        Entry(lock, { .kind = Kind::Set }),
        Entry(lock, { .kind = Kind::Map })
    });

    std::vector<Entry> user_enums;
    user_enums.reserve(core::typing::gEnumRegistry.size(lock.enum_lock));
    for (auto it : core::typing::gEnumRegistry.range(lock.enum_lock))
        user_enums.push_back(Entry(lock, { .kind = Kind::Enum, .user = it.handle }));
    popup_->addCategory("Enum", user_enums);

    std::vector<Entry> user_structs;
    user_structs.reserve(core::typing::gStructRegistry.size(lock.struct_lock));
    for (auto it : core::typing::gStructRegistry.range(lock.struct_lock))
        user_structs.push_back(Entry(lock, { .kind = Kind::Struct, .user = it.handle }));
    popup_->addCategory("Struct", user_structs);

    popup_->resize(280, 380);

    popup_->move(mapToGlobal(rect.bottomLeft()));
    popup_->show();

    popup_->search->setFocus();
}

void TypeSelector::selectSimplex(const std::vector<size_t>& path, Simplex simplex)
{
    OptType replacement(simplex);
    replacement.arguments.resize(simplex.argcount());

    if (path.empty())
        root_ = std::move(replacement);
    else
    {
        if (OptType* parent = findNode(std::span<const size_t>(path.begin(), path.end() - 1)))
            parent->arguments[path.back()] = std::move(replacement);
        else
            return;
    }

    rebuildLayout();
    updateGeometry();
    update();

    Q_EMIT editingFinished();
    Q_EMIT layoutSizeChanged();
}

OptType* TypeSelector::findNode(std::span<const size_t> path)
{
    OptType* node = &root_;

    for (size_t index : path)
    {
        if (index >= node->arguments.size())
            return nullptr;

        node = &node->arguments[index];
    }

    return node;
}

// TODO use for mouse hover indicator
std::optional<std::vector<size_t>> TypeSelector::hitTest(const NodeLayout& layout, const QPoint& pos) const
{
    if (!layout.rect.contains(pos))
        return std::nullopt;

    for (size_t i = 0; i < layout.children.size(); ++i)
    {
        if (auto path = hitTest(layout.children[i], pos))
        {
            path->insert(path->begin(), i);
            return path;
        }
    }

    return std::vector<size_t>{};
}

const TypeSelector::NodeLayout& TypeSelector::layoutAtPath(
    const NodeLayout& root,
    const std::vector<size_t>& path) const
{
    const NodeLayout* current = &root;

    for (size_t index : path)
        current = &current->children[index];

    return *current;
}

QSize TypeSelector::calculateSize(const OptType& type, const QFont& font)
{
    QFontMetrics metrics(font);

    const QString name = type.simplex ? repr({}, *type.simplex) : tr(kUnselected);

    const QSize label(metrics.horizontalAdvance(name), metrics.height());

    if (type.arguments.empty())
        return label + QSize(kPadding * 2, kPadding * 2);

    int width = label.width() + kSpacing;
    int height = label.height();

    for (const auto& argument : type.arguments)
    {
        const QSize child = calculateSize(argument, font);
        width += child.width() + kSpacing;
        height = std::max(height, child.height());
    }

    return QSize(width + kPadding * 2, height + kPadding * 2);
}

TypeSelector::NodeLayout TypeSelector::calculateLayout(
    const OptType& type,
    const QRect& rect,
    const QFont& font)
{
    NodeLayout result;
    result.rect = rect;

    if (!type.simplex || type.arguments.empty())
        return result;

    const QFontMetrics metrics(font);
    const QString name = repr({}, *type.simplex);

    int x = rect.left() + kPadding + metrics.horizontalAdvance(name) + kSpacing;

    for (const auto& argument : type.arguments)
    {
        QSize size = calculateSize(argument, font);
        QRect childRect(x, rect.center().y() - size.height() / 2, size.width(), size.height());
        result.children.push_back(calculateLayout(argument, childRect, font));
        x += size.width() + kSpacing;
    }

    return result;
}

void TypeSelector::paintNode(
    QPainter* painter,
    const OptType& type,
    const NodeLayout& layout,
    const QFont& font,
    const QPalette& palette)
{
    const QString name = type.simplex ? repr({}, *type.simplex) : tr(kUnselected);
    const QColor color = type.simplex ? colorForKind(type.simplex->kind) : kUnselectedColor;

    painter->setPen(QPen(color.darker(130), 1));
    painter->setBrush(color.lighter(175));

    painter->drawRoundedRect(layout.rect, kRadius, kRadius);

    const QRect textRect(
        layout.rect.left() + kPadding,
        layout.rect.top(),
        QFontMetrics(font).horizontalAdvance(name) + 2,
        layout.rect.height());

    painter->setPen(color.darker(180));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, name);

    for (size_t i = 0; i < type.arguments.size(); ++i)
        paintNode(painter, type.arguments[i], layout.children[i], font, palette);
}

} // namespace crucible::gui
