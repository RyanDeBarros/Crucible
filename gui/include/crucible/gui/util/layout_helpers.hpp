#pragma once

class QFrame;
class QHBoxLayout;
class QVBoxLayout;
class QWidget;

namespace crucible::gui
{

extern QFrame* addSeparator(QWidget& parent, QHBoxLayout& layout);
extern QFrame* addSeparator(QWidget& parent, QVBoxLayout& layout);

} // crucible::gui
