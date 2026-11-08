#include <crucible/gui/preferences/preferences_dialog.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/tabs/general_preferences.hpp>
#include <crucible/gui/preferences/tabs/logging_preferences.hpp>

#include <QDialogButtonBox>
#include <QEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QWidget>

namespace crucible::gui
{

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Preferences[*]"));
    setWindowModified(false);
    setModal(true);
    resize(1440, 810);
    installEventFilter(this);

    createLayout();

    if (parent)
    {
        auto* bkg_overlay = new QWidget(parent);
        bkg_overlay->setStyleSheet("background-color: rgba(0, 0, 0, 200);");
        bkg_overlay->setGeometry(parent->rect());
        bkg_overlay->raise();
        bkg_overlay->show();

        connect(this, &QDialog::finished, bkg_overlay, &QWidget::deleteLater);
    }
}

bool PreferencesDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this && event->type() == QEvent::KeyPress)
    {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            return true; // enter key should not close dialog
    }

    return QDialog::eventFilter(watched, event);
}

void PreferencesDialog::createLayout()
{
    auto* layout = new QVBoxLayout(this);

    tabs_ = new QTabWidget(this);
    layout->addWidget(tabs_);

    buttons_ = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset,
        this);
    layout->addWidget(buttons_);

    connect(buttons_, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });

    connect(
        buttons_->button(QDialogButtonBox::Reset),
        &QPushButton::clicked,
        this,
        &PreferencesDialog::resetToDefaults);

    connect(buttons_, &QDialogButtonBox::rejected, this, &QDialog::reject);

    general_tab_ = new GeneralPreferences(tabs_);
    connect(general_tab_, &GeneralPreferences::modified, this, &PreferencesDialog::modified);
    tabs_->addTab(general_tab_, tr("General"));

    logging_tab_ = new LoggingPreferences(tabs_);
    connect(logging_tab_, &LoggingPreferences::modified, this, &PreferencesDialog::modified);
    tabs_->addTab(logging_tab_, tr("Logging"));
}

void PreferencesDialog::modified()
{
    setWindowModified(true);
}

void PreferencesDialog::saveSettings()
{
    auto settings = PREFS.getSettings();

    general_tab_->save(settings);
    logging_tab_->save(settings);

    settings.sync();
    setWindowModified(false);
}

void PreferencesDialog::resetToDefaults()
{
    general_tab_->resetToDefaults();
    logging_tab_->resetToDefaults();
}

} // crucible::gui
