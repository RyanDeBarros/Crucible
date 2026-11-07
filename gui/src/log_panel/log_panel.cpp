#include <crucible/gui/log_panel/log_panel.hpp>

#include <crucible/gui/log_panel/log_categories.hpp>
#include <crucible/gui/log_panel/log_controls.hpp>
#include <crucible/gui/log_panel/log_model.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <crucible/gui/util/scrollbar_preserver.hpp>

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QScrollBar>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

namespace crucible::gui
{

LogPanel::LogPanel(core::Logger& logger, QWidget* parent)
    : QWidget(parent)
{
    LOG.debug(kLog) << "Initializing log panel...";

    model_ = new LogModel(logger, this);
    createLayout();
    initPoll();
}

bool LogPanel::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == table_ || watched == table_->viewport()) && event->type() == QEvent::KeyPress)
    {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->matches(QKeySequence::Copy))
        {
            QString text;

            const auto indexes = table_->selectionModel()->selectedIndexes();

            for (const auto& index : indexes)
            {
                if (!text.isEmpty())
                    text += '\n';

                text += index.data(Qt::DisplayRole).toString();
            }

            QApplication::clipboard()->setText(text);
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void LogPanel::setWordWrap(bool on)
{
    LOG.debug(kLog) << "Log panel word wrap set to " << (on ? "on" : "off");
    
    model_->message_tooltip = !on;

    table_->setWordWrap(on);
    table_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table_->setTextElideMode(on ? Qt::ElideNone : Qt::ElideRight);
}

void LogPanel::createLayout()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    controls_ = new LogControls(this);
    model_->setLevelFilter(controls_->level_mask());
    createTable();

    layout->addWidget(controls_);
    layout->addWidget(table_);

    connect(controls_, &LogControls::levelFilterChanged, this, &LogPanel::setLevelFilter);
    connect(controls_, &LogControls::columnFilterChanged, this, &LogPanel::setColumnFilter);
    connect(controls_, &LogControls::clearRequested, this, &LogPanel::requestClearLog);
}

void LogPanel::createTable()
{
    table_ = new QTableView(this);
    table_->setModel(model_);

    table_->setSelectionBehavior(QAbstractItemView::SelectItems);
    table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table_->installEventFilter(this);

    table_->verticalHeader()->hide();
    table_->horizontalHeader()->setStretchLastSection(true);

    table_->setShowGrid(false);
    table_->setAlternatingRowColors(true);

    table_->horizontalHeader()->setSectionResizeMode(
        LogColumn::Timestamp,
        QHeaderView::ResizeToContents);

    table_->horizontalHeader()->setSectionResizeMode(
        LogColumn::Level,
        QHeaderView::ResizeToContents);

    table_->horizontalHeader()->setSectionResizeMode(
        LogColumn::Category,
        QHeaderView::ResizeToContents);

    table_->horizontalHeader()->setSectionResizeMode(
        LogColumn::Message,
        QHeaderView::Stretch);

    setWordWrap(PREFS.logging.word_wrap);
    connect(
        &PREFS.logging,
        &LoggingSettings::modified,
        this,
        [this](LoggingSettings::Property property) {
            if (property == LoggingSettings::WordWrap)
            {
                setWordWrap(PREFS.logging.word_wrap); // TODO setting word wrap does not work
                refresh();
            }
        });
}

void LogPanel::initPoll()
{
    poll_timer_ = new QTimer(this);
    connect(poll_timer_, &QTimer::timeout, this, &LogPanel::refresh);

    poll_timer_->start(PREFS.logging.refresh_interval);
    connect(
        &PREFS.logging,
        &LoggingSettings::modified,
        this,
        [this](LoggingSettings::Property property) {
            if (property == LoggingSettings::RefreshInterval)
                poll_timer_->setInterval(PREFS.logging.refresh_interval);
        });
}

void LogPanel::refresh()
{
    ScrollbarPreserver sp(*table_);
    model_->refresh();
}

void LogPanel::setLevelFilter(core::LogLevelMask mask)
{
    ScrollbarPreserver sp(*table_);
    LOG.debug(kLog) << "Log level filter set to " << mask;

    model_->setLevelFilter(mask);
}

void LogPanel::setColumnFilter(LogColumnMask mask)
{
    ScrollbarPreserver sp(*table_);
    LOG.debug(kLog) << "Log column filter set to " << mask;

    for (size_t i = 0; i < static_cast<size_t>(LogColumn::_Count); ++i)
        table_->setColumnHidden(i, !(mask & static_cast<LogColumn>(i)));
}

void LogPanel::requestClearLog()
{
    auto reply = QMessageBox::question(
        this,
        "Clear log",
        "Are you sure you want to clear the log? This action cannot be undone.",
        { QMessageBox::Yes, QMessageBox::No },
        QMessageBox::No);

    if (reply == QMessageBox::Yes)
        model_->clear();
}

} // crucible::gui
