#pragma once

#include <QWidget>

namespace crucible::gui
{

class RamControls
    : public QWidget
{
    Q_OBJECT;

public:
    RamControls(QWidget* parent = nullptr);
};

} // crucible::gui
