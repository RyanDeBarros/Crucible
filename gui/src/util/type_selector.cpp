#include <crucible/gui/util/type_selector.hpp>

#include <crucible/gui/util/repr.hpp>

#include <QApplication>
#include <QFrame>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace crucible::gui
{

constexpr int kPadding = 9;
constexpr int kSpacing = 7;
constexpr int kRadius = 5;

static const char* kUnselected = "--select--";
static const QColor kUnselectedColor = QColor(0xAA, 0xAA, 0xAA);

static QColor colorForKind(TypeSelector::Kind kind)
{
    // TODO color scheme

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

OptType::OptType(core::typing::Type type)
    : type(std::move(type))
{
}

OptType::operator bool() const
{
    return type.has_value();
}

const core::typing::Type& OptType::operator*() const
{
    return *type;
}

core::typing::Type& OptType::operator*()
{
    return *type;
}

const core::typing::Type* OptType::operator->() const
{
    return &*type;
}

core::typing::Type* OptType::operator->()
{
    return &*type;
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

    explicit TypePopup(TypeSelector& selector, std::vector<size_t> path)
        : QFrame(&selector, Qt::Popup)
        , selector(selector)
        , path(std::move(path))
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
                    const bool visible = item->text(0).contains(text, Qt::CaseInsensitive);

                    item->setHidden(!visible);
                    anyVisible |= visible;
                }

                category->setHidden(!anyVisible);
            }
        });

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
    setMinimumHeight(36);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    setFocusPolicy(Qt::StrongFocus);
}

QSize TypeSelector::sizeHint() const
{
    if (!root_)
        return QSize(100, 36);

    return calculateSize(*root_, font());
}

OptType TypeSelector::type() const
{
    return root_ ? toType(*root_) : OptType{};
}

void TypeSelector::setType(const OptType& type)
{
    root_ = fromType(type);
    rebuildLayout();
    updateGeometry();
    Q_EMIT update();
}

QSize TypeSelector::typeSizeHint(const OptType& type, const QFont& font)
{
    return calculateSize(fromType(type), font);
}

void TypeSelector::paintType(
    QPainter* painter,
    const QRect& rect,
    const OptType& type,
    const QFont& font,
    const QPalette& palette,
    bool selected)
{
    auto node = fromType(type);
    NodeLayout layout = calculateLayout(node, rect, font);

    painter->save();

    if (selected)
        painter->fillRect(rect, palette.color(QPalette::Highlight));

    painter->setFont(font);
    paintNode(painter, node, layout, font, palette);

    painter->restore();
}

void TypeSelector::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!root_)
    {
        painter.setPen(palette().color(QPalette::PlaceholderText));
        painter.drawText(rect(), Qt::AlignCenter, tr("Select type"));
        return;
    }

    rebuildLayout();
    paintNode(&painter, *root_, layout_, font(), palette());
}

void TypeSelector::mousePressEvent(QMouseEvent* event)
{
    // TODO get the right child path
    openPopup({}, rect());
    QWidget::mousePressEvent(event);
}

void TypeSelector::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    rebuildLayout();
}

void TypeSelector::rebuildLayout()
{
    if (!root_)
        return;

    layout_ = calculateLayout(*root_, rect(), font());
}

void TypeSelector::openPopup(std::vector<size_t> path, const QRect& rect)
{
    if (!popup_)
        popup_ = new TypePopup(*this, std::move(path));

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
    // TODO reserve size
    for (auto it : core::typing::gEnumRegistry.range(lock.enum_lock))
        user_enums.push_back(Entry(lock, { .kind = Kind::Enum, .user = it.handle }));
    popup_->addCategory("Enum", user_enums);

    std::vector<Entry> user_structs;
    // TODO reserve size
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
    Node replacement{ .simplex = simplex };
    replacement.arguments.resize(simplex.argcount());

    if (path.empty())
        root_ = std::move(replacement);
    else
    {
        if (Node* parent = findNode(std::span<const size_t>(path.begin(), path.end() - 1)))
            parent->arguments[path.back()] = std::move(replacement);
        else
            return;
    }

    rebuildLayout();
    updateGeometry();
    update();

    if (auto t = type())
    {
        Q_EMIT typeChanged(*t);
        Q_EMIT editingFinished();
    }
}

TypeSelector::Node* TypeSelector::findNode(std::span<const size_t> path)
{
    if (!root_)
        return nullptr;

    Node* node = &*root_;

    for (size_t index : path)
    {
        if (index >= node->arguments.size() || !node->arguments[index])
            return nullptr;

        node = &*node->arguments[index];
    }

    return node;
}

OptType TypeSelector::toType(const Node& node)
{
    std::vector<Type> args;

    for (const auto& argument : node.arguments)
    {
        if (!argument)
            return {};

        if (auto type = toType(*argument))
            args.push_back(std::move(*type));
        else
            return {};
    }

    // TODO instead of make_*(), just do make(Kind) with overloads for different numbers of arguments - then throw error if arg count doesn't match.
    switch (node.simplex.kind)
    {
    case Kind::Bool:
        return Type::make_bool();

    case Kind::Int:
        return Type::make_int();

    case Kind::Float:
        return Type::make_float();

    case Kind::String:
        return Type::make_string();

    case Kind::Pointer:
        return Type::make_pointer(args[0]);

    case Kind::Enum:
        return Type::make_enum(node.simplex.user);

    case Kind::List:
        return Type::make_list(args[0]);

    case Kind::Set:
        return Type::make_set(args[0]);

    case Kind::Map:
        return Type::make_map(args[0], args[1]);

    case Kind::Struct:
        return Type::make_struct(node.simplex.user);

    case Kind::Joker:
        return Type::make_joker();

    default:
        return {};
    }
}

std::optional<TypeSelector::Node> TypeSelector::fromType(const OptType& type)
{
    if (type)
    {
        Node result{ .simplex = type->simplex() };

        for (size_t i = 0; i < result.simplex.argcount(); ++i)
            result.arguments.emplace_back(fromType(type->underlying(i)));

        return result;
    }
    else
        return std::nullopt;
}

QSize TypeSelector::calculateSize(const std::optional<Node>& node, const QFont& font)
{
    QFontMetrics metrics(font);

    const QString name = node ? repr({}, node->simplex) : tr(kUnselected);

    const QSize label(metrics.horizontalAdvance(name), metrics.height());

    if (!node || node->arguments.empty())
        return label + QSize(kPadding * 2, kPadding * 2);

    int width = label.width() + kSpacing;
    int height = label.height();

    for (const auto& argument : node->arguments)
    {
        const QSize child = argument
            ? calculateSize(*argument, font)
            : QSize(metrics.horizontalAdvance(tr(kUnselected)) + kPadding * 2, metrics.height() + kPadding * 2);

        width += child.width() + kSpacing;
        height = std::max(height, child.height());
    }

    return QSize(width + kPadding * 2, height + kPadding * 2);
}

TypeSelector::NodeLayout TypeSelector::calculateLayout(
    const std::optional<Node>& node,
    const QRect& rect,
    const QFont& font)
{
    NodeLayout result;
    result.rect = rect;

    if (!node || node->arguments.empty())
        return result;

    const QFontMetrics metrics(font);
    const QString name = repr({}, node->simplex);

    int x = rect.left() + kPadding + metrics.horizontalAdvance(name) + kSpacing;

    const int availableHeight = rect.height() - kPadding * 2;

    for (const auto& argument : node->arguments)
    {
        QSize size;

        if (argument)
            size = calculateSize(*argument, font);
        else
            size = QSize(metrics.horizontalAdvance(tr(kUnselected)) + kPadding * 2, metrics.height() + kPadding * 2);

        QRect childRect(
            x,
            rect.center().y() - size.height() / 2,
            size.width(),
            std::max(size.height(), availableHeight));

        if (argument)
            result.children.push_back(calculateLayout(*argument, childRect, font));
        else
            result.children.push_back(NodeLayout{ childRect, {} });

        x += size.width() + kSpacing;
    }

    return result;
}

void TypeSelector::paintNode(
    QPainter* painter,
    const std::optional<Node>& node,
    const NodeLayout& layout,
    const QFont& font,
    const QPalette& palette)
{
    const QString name = node ? repr({}, node->simplex) : tr(kUnselected);
    const QColor color = node ? colorForKind(node->simplex.kind) : kUnselectedColor;

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

    if (!node)
        return;

    for (size_t i = 0; i < node->arguments.size(); ++i)
    {
        const auto& argument = node->arguments[i];
        const auto& childLayout = layout.children[i];

        if (argument)
            paintNode(painter, *argument, childLayout, font, palette);
        else
        {
            painter->setPen(QPen(palette.color(QPalette::PlaceholderText), 1, Qt::DashLine));
            painter->setBrush(palette.color(QPalette::AlternateBase));
            painter->drawRoundedRect(childLayout.rect, kRadius, kRadius);
            painter->drawText(childLayout.rect, Qt::AlignCenter, tr(kUnselected));
        }
    }
}

TypeSelectorDelegate::TypeSelectorDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* TypeSelectorDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem&,
    const QModelIndex&) const
{
    auto* editor = new TypeSelector(parent);

    connect(editor, &TypeSelector::editingFinished, this, [this, editor]() {
        // TODO remove?
        auto* delegate = const_cast<TypeSelectorDelegate*>(this);
        Q_EMIT delegate->commitData(editor);
        Q_EMIT delegate->closeEditor(editor);
    });

    return editor;
}

void TypeSelectorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (auto* selector = qobject_cast<TypeSelector*>(editor))
    {
        auto variant = index.data(Qt::EditRole);
        if (auto* type = get_if<OptType>(&variant))
            selector->setType(*type);
    }
}

void TypeSelectorDelegate::setModelData(
    QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const
{
    if (auto* selector = qobject_cast<TypeSelector*>(editor))
    {
        if (const auto type = selector->type())
            model->setData(index, QVariant::fromValue(*type), Qt::EditRole);
    }
}

void TypeSelectorDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    const QVariant variant = index.data(Qt::DisplayRole);
    auto* type = get_if<OptType>(&variant);

    if (!type)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionViewItem opt(option);

    initStyleOption(&opt, index);

    const bool selected = opt.state & QStyle::State_Selected;
    const QRect contentRect = opt.rect.adjusted(3, 2, -3, -2);

    if (selected)
        painter->fillRect(opt.rect, opt.palette.color(QPalette::Highlight));

    opt.state &= ~QStyle::State_Selected;
    TypeSelector::paintType(painter, contentRect, *type, opt.font, opt.palette, false);
}

QSize TypeSelectorDelegate::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    auto variant = index.data(Qt::DisplayRole);
    if (auto* type = get_if<OptType>(&variant))
        return TypeSelector::typeSizeHint(*type, option.font) + QSize(6, 4);

    return QStyledItemDelegate::sizeHint(option, index);
}

} // namespace crucible::gui
