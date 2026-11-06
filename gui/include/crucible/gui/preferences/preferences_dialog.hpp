#pragma once

#include <QDialog>

class QDialogButtonBox;
class QTabWidget;

namespace crucible::gui
{

class PreferencesDialog
    : public QDialog
{
    Q_OBJECT;

private:
    QTabWidget* tabs_ = nullptr;
    QDialogButtonBox* buttons_ = nullptr;

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);

private:
    void createLayout();
};

} // crucible::gui
