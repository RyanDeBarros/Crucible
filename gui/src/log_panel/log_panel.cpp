#include <crucible/gui/log_panel/log_panel.hpp>

#include <crucible/gui/log_panel/log_model.hpp>

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

namespace crucible::gui
{

LogPanel::LogPanel(const core::Logger& logger, QWidget* parent)
    : QWidget(parent)
{
    model_ = new LogModel(logger, this);
    createLayout();
    initPoll();


    auto* testTimer = new QTimer(this);

    connect(testTimer, &QTimer::timeout, this, [this]() {
        static int counter = 0;

        switch (counter++ % 4)
        {
        case 0:
            LOG.debug() << "Debug message #" << counter;
            break;

        case 1:
            LOG.info() << "Info message #" << counter;
            break;

        case 2:
            LOG.warning() << "Warning message #" << counter;
            break;

        case 3:
            LOG.error() << "Error message #" << counter;
            break;
        }

    });

    testTimer->start(1000);
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

void LogPanel::createFilterButton()
{
    filter_button_ = new QToolButton(this);
    filter_button_->setIcon(QIcon(":/icons/filter"));
    filter_button_->setToolTip(tr("Filter log levels"));
    filter_button_->setPopupMode(QToolButton::InstantPopup);

    filter_menu_ = new QMenu(filter_button_);

    const auto add_filter = [this](core::LogLevel level, QString const& label) {
        auto* action = filter_menu_->addAction(label);
        action->setCheckable(true);
        action->setChecked(true);

        filter_actions_[static_cast<size_t>(level)] = action;

        connect(action, &QAction::toggled, this, [this] { updateFilter(); });
    };

    add_filter(core::LogLevel::Debug, tr("Debug"));
    add_filter(core::LogLevel::Info, tr("Info"));
    add_filter(core::LogLevel::Warning, tr("Warning"));
    add_filter(core::LogLevel::Error, tr("Error"));

    filter_button_->setMenu(filter_menu_);
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

void LogPanel::createLayout()
{
    createFilterButton();
    createTable();

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* controls_layout = new QHBoxLayout();
    controls_layout->setContentsMargins(4, 2, 4, 2);

    controls_layout->addStretch();
    controls_layout->addWidget(filter_button_);

    layout->addLayout(controls_layout);
    layout->addWidget(table_);
}

void LogPanel::initPoll()
{
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LogPanel::refresh); // TODO use beginInsertRows()/endInsertRows() instead of rebuilding with refresh()
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

void LogPanel::updateFilter()
{
    core::LogLevelMask mask = 0;

    for (size_t i = 0; i < filter_actions_.size(); ++i)
    {
        if (filter_actions_[i]->isChecked())
            mask |= core::logLevelMask(static_cast<core::LogLevel>(i));
    }

    model_->setFilter(mask);
}

} // crucible::gui
