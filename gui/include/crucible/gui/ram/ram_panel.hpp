#pragma once

#include <QWidget>

namespace crucible::gui
{

class RamControls;

class RamPanel
    : public QWidget
{
    Q_OBJECT;

private:
    RamControls* controls_ = nullptr;

public:
    RamPanel(QWidget* parent = nullptr);

private:
    void createLayout();
};

} // crucible::gui
