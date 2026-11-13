#include <crucible/gui/util/layout_helpers.hpp>

#include <crucible/gui/util/stylesheet.hpp>

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace crucible::gui
{

QFrame* addSeparator(QWidget& parent, QHBoxLayout& layout)
{
    auto* line = new QFrame(&parent);
    line->setFrameShape(QFrame::VLine);
    line->setStyleSheet(loadStyleSheet(StyleSheet::Separator));
    layout.addWidget(line);
    return line;
}

QFrame* addSeparator(QWidget& parent, QVBoxLayout& layout)
{
    auto* line = new QFrame(&parent);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet(loadStyleSheet(StyleSheet::Separator));
    layout.addWidget(line);
    return line;
}

} // crucible::gui
