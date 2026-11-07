#include <crucible/gui/preferences/tabs/logging_preferences.hpp>

#include <QCheckBox>
#include <QFormLayout>
#include <QSettings>
#include <QSpinBox>

namespace crucible::gui
{

static const char* const kLoggingGroup = "logging";
static const char* const kWordWrap = "word_wrap";
static const char* const kRefreshInterval = "refresh_interval";

static const struct
{
    bool word_wrap = false;
    int refresh_interval = 250;
} kDefaults = {};

LoggingPreferences::LoggingPreferences(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QFormLayout(this);

    word_wrap_ = new QCheckBox(this); // TODO make this a toggle box directly in logging controls instead of preferences
    connect(word_wrap_, &QCheckBox::checkStateChanged, this, &LoggingPreferences::modified);
    layout->addRow(tr("Word wrap"), word_wrap_);

    refresh_interval_ = new QSpinBox(this);
    refresh_interval_->setRange(50, 5000);
    refresh_interval_->setSingleStep(50);
    refresh_interval_->setSuffix(tr(" ms"));
    connect(refresh_interval_, &QSpinBox::valueChanged, this, &LoggingPreferences::modified);
    layout->addRow(tr("Refresh interval"), refresh_interval_);
}

void LoggingPreferences::load(QSettings& settings)
{
    settings.beginGroup(kLoggingGroup);

    word_wrap_->setChecked(settings.value(kWordWrap, kDefaults.word_wrap).toBool());
    refresh_interval_->setValue(settings.value(kRefreshInterval, kDefaults.refresh_interval).toInt());

    settings.endGroup();
}

void LoggingPreferences::save(QSettings& settings) const
{
    settings.beginGroup(kLoggingGroup);

    settings.setValue(kWordWrap, word_wrap_->isChecked());
    settings.setValue(kRefreshInterval, refresh_interval_->value());

    settings.endGroup();
}

void LoggingPreferences::resetToDefaults()
{
    word_wrap_->setChecked(kDefaults.word_wrap);
    refresh_interval_->setValue(kDefaults.refresh_interval);
}

} // crucible::gui
