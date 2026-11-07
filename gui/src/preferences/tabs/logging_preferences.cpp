#include <crucible/gui/preferences/tabs/logging_preferences.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <QCheckBox>
#include <QFormLayout>
#include <QSpinBox>

namespace crucible::gui
{

LoggingPreferences::LoggingPreferences(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QFormLayout(this);

    word_wrap_ = new QCheckBox(this); // TODO make this a toggle box directly in logging controls instead of preferences
    connect(word_wrap_, &QCheckBox::checkStateChanged, this, [this]() {
        modified();
        PREFS.logging.modified(LoggingSettings::WordWrap);
    });
    layout->addRow(tr("Word wrap"), word_wrap_);

    refresh_interval_ = new QSpinBox(this);
    refresh_interval_->setRange(50, 5000);
    refresh_interval_->setSingleStep(50);
    refresh_interval_->setSuffix(tr(" ms"));
    connect(refresh_interval_, &QSpinBox::valueChanged, this, [this]() {
        modified();
        PREFS.logging.modified(LoggingSettings::RefreshInterval);
    });
    layout->addRow(tr("Refresh interval"), refresh_interval_);
}

void LoggingPreferences::load(QSettings& settings)
{
    PREFS.logging.load(settings);
    
    // TODO BlockSignals RAII
    word_wrap_->blockSignals(true);
    word_wrap_->setChecked(PREFS.logging.word_wrap);
    word_wrap_->blockSignals(false);

    refresh_interval_->blockSignals(true);
    refresh_interval_->setValue(PREFS.logging.refresh_interval);
    refresh_interval_->blockSignals(false);
}

void LoggingPreferences::save(QSettings& settings) const
{
    PREFS.logging.word_wrap = word_wrap_->isChecked();
    PREFS.logging.refresh_interval = refresh_interval_->value();
    PREFS.logging.save(settings);
}

void LoggingPreferences::resetToDefaults()
{
    PREFS.logging.resetToDefaults();
    word_wrap_->setChecked(PREFS.logging.word_wrap);
    refresh_interval_->setValue(PREFS.logging.refresh_interval);
}

} // crucible::gui
