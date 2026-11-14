#pragma once

#include <QWidget>

namespace crucible::gui
{

class RamControls
    : public QWidget
{
    Q_OBJECT;

public:
    explicit RamControls(QWidget* parent = nullptr);
};

} // crucible::gui
