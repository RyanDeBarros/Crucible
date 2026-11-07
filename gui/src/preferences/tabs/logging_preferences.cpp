#include <crucible/gui/preferences/tabs/logging_preferences.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <crucible/gui/util/block_signals.hpp>

#include <QCheckBox>
#include <QFormLayout>
#include <QSpinBox>

namespace crucible::gui
{

LoggingPreferences::LoggingPreferences(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QFormLayout(this);

    word_wrap_ = new QCheckBox(this); // TODO make this a toggle icon button directly in logging controls instead of preferences
    word_wrap_->setChecked(PREFS.logging.wordWrap());
    connect(word_wrap_, &QCheckBox::checkStateChanged, this, &LoggingPreferences::modified);
    layout->addRow(tr("Word wrap"), word_wrap_);

    refresh_interval_ = new QSpinBox(this);
    refresh_interval_->setRange(50, 5000);
    refresh_interval_->setSingleStep(50);
    refresh_interval_->setSuffix(tr(" ms"));
    refresh_interval_->setValue(PREFS.logging.refreshInterval());
    connect(refresh_interval_, &QSpinBox::valueChanged, this, &LoggingPreferences::modified);
    layout->addRow(tr("Refresh interval"), refresh_interval_);
}

void LoggingPreferences::save(QSettings& settings) const
{
    PREFS.logging.setWordWrap(word_wrap_->isChecked());
    PREFS.logging.setRefreshInterval(refresh_interval_->value());

    PREFS.logging.save(settings);
}

void LoggingPreferences::resetToDefaults()
{
    PREFS.logging.resetToDefaults();
    word_wrap_->setChecked(PREFS.logging.wordWrap());
    refresh_interval_->setValue(PREFS.logging.refreshInterval());
}

} // crucible::gui
