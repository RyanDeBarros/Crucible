#include <crucible/gui/log_panel/log_panel.hpp>

#include <crucible/gui/log_panel/log_controls.hpp>
#include <crucible/gui/log_panel/log_model.hpp>

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

namespace crucible::gui
{

LogPanel::LogPanel(core::Logger& logger, QWidget* parent)
    : QWidget(parent)
{
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

    controls_ = new LogControls(~core::logLevelMask(core::LogLevel::Debug), this);
    createTable();

    layout->addWidget(controls_);
    layout->addWidget(table_);

    connect(controls_, &LogControls::filterChanged, model_, &LogModel::setFilter);
    connect(controls_, &LogControls::clearRequested, model_, &LogModel::clear);
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
        LogModel::Timestamp,
        QHeaderView::ResizeToContents);

    table_->horizontalHeader()->setSectionResizeMode(
        LogModel::Level,
        QHeaderView::ResizeToContents);

    table_->horizontalHeader()->setSectionResizeMode(
        LogModel::Message,
        QHeaderView::Stretch);

    setWordWrap(false); // TODO app setting
}

void LogPanel::initPoll()
{
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LogPanel::refresh);
    timer->start(250); // TODO app setting
}

void LogPanel::refresh()
{
    auto* scrollbar = table_->verticalScrollBar();
    const bool at_bottom = scrollbar->value() >= scrollbar->maximum();

    model_->refresh();

    if (at_bottom)
        table_->scrollToBottom();
}

} // crucible::gui
