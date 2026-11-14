#pragma once

#include <QDialog>
#include <QSize>

class QWidget;

namespace crucible::gui
{

class ModalDialog
    : public QDialog
{
    Q_OBJECT;

public:
    explicit ModalDialog(const QString& title, QSize size, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

} // crucible::gui
