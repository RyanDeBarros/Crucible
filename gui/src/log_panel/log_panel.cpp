#include <crucible/gui/log_panel/log_panel.hpp>

#include <crucible/gui/log_panel/log_model.hpp>

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QTimer>
#include <QVBoxLayout>

namespace crucible::gui
{

LogPanel::LogPanel(const core::Logger& logger, QWidget* parent)
    : QWidget(parent)
{
    model_ = new LogModel(logger, this);

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

    table_->setWordWrap(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(table_);

    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LogPanel::refresh);
    timer->start(250); // TODO app setting

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

void LogPanel::refresh()
{
    model_->refresh();

    // TODO only if already at bottom
    table_->scrollToBottom();
}

} // crucible::gui
