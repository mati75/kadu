/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/chat-messages-view.h"
#include "icons/kadu-icon.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "gui/widgets/wait-overlay.h"
#include "model/dates-model-item.h"
#include "model/history-dates-model.h"

#include "history-tab.h"

HistoryTab::HistoryTab(bool showTitleInTimeline, QWidget *parent) :
		QWidget(parent), TabWaitOverlay(0), TimelineWaitOverlay(0),
		MessagesViewWaitOverlay(0), DatesFutureWatcher(0), MessagesFutureWatcher(0)
{
	DatesModel = new HistoryDatesModel(showTitleInTimeline, this);
}

HistoryTab::~HistoryTab()
{
}

void HistoryTab::createGui()
{
	TimelinePopupMenu = new QMenu(this);
	TimelinePopupMenu->addAction(KaduIcon("kadu_icons/clear-history").icon(), tr("&Remove entries"),
	                             this, SLOT(removeEntries()));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(2);

	Splitter = new QSplitter(Qt::Horizontal, this);

	createTreeView(Splitter);

	TimelineView = new TimelineChatMessagesView(Splitter);
	TimelineView->timeline()->setModel(DatesModel);
	TimelineView->timeline()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TimelineView->timeline(), SIGNAL(customContextMenuRequested(QPoint)),
	        this, SLOT(showTimelinePopupMenu()));
	connect(TimelineView->timeline()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(currentDateChanged()));

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	Splitter->setSizes(sizes);

	layout->addWidget(Splitter);
}

void HistoryTab::showTabWaitOverlay()
{
	if (!TabWaitOverlay)
		TabWaitOverlay = new WaitOverlay(this);
	else
		TabWaitOverlay->show();
}

void HistoryTab::hideTabWaitOverlay()
{
	TabWaitOverlay->deleteLater();
	TabWaitOverlay = 0;
}

void HistoryTab::showTimelineWaitOverlay()
{
	if (!TimelineWaitOverlay)
		TimelineWaitOverlay = new WaitOverlay(TimelineView);
	else
		TimelineWaitOverlay->show();
}

void HistoryTab::hideTimelineWaitOverlay()
{
	TimelineWaitOverlay->deleteLater();
	TimelineWaitOverlay = 0;
}

void HistoryTab::showMessagesViewWaitOverlay()
{
	if (!MessagesViewWaitOverlay)
		MessagesViewWaitOverlay = new WaitOverlay(TimelineView->messagesView());
	else
		MessagesViewWaitOverlay->show();
}

void HistoryTab::hideMessagesViewWaitOverlay()
{
	MessagesViewWaitOverlay->deleteLater();
	MessagesViewWaitOverlay = 0;
}

TimelineChatMessagesView * HistoryTab::timelineView() const
{
	return TimelineView;
}

void HistoryTab::setDates(const QVector<DatesModelItem> &dates)
{
	DatesModel->setDates(dates);

	if (!dates.isEmpty())
	{
		TimelineView->timeline()->setCurrentIndex(TimelineView->timeline()->model()->index(dates.size() - 1, 0));
		QScrollBar *scrollBar = TimelineView->timeline()->verticalScrollBar();
		scrollBar->setValue(scrollBar->maximum());
	}
	else
		currentDateChanged();
}

void HistoryTab::futureDatesAvailable()
{
	hideTimelineWaitOverlay();

	if (!DatesFutureWatcher)
		return;

	setDates(DatesFutureWatcher->result());

	DatesFutureWatcher->deleteLater();
	DatesFutureWatcher = 0;
}

void HistoryTab::futureDatesCanceled()
{
	hideTimelineWaitOverlay();

	if (!DatesFutureWatcher)
		return;

	DatesFutureWatcher->deleteLater();
	DatesFutureWatcher = 0;
}

void HistoryTab::setFutureDates(const QFuture<QVector<DatesModelItem> > & futureDates)
{
	setDates(QVector<DatesModelItem>());

	if (DatesFutureWatcher)
		DatesFutureWatcher->deleteLater();

	DatesFutureWatcher = new QFutureWatcher<QVector<DatesModelItem> >(this);
	connect(DatesFutureWatcher, SIGNAL(finished()), this, SLOT(futureDatesAvailable()));
	connect(DatesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureDatesCanceled()));

	DatesFutureWatcher->setFuture(futureDates);

	showTimelineWaitOverlay();
}

void HistoryTab::setMessages(const QVector<Message> &messages)
{
	timelineView()->messagesView()->setUpdatesEnabled(false);

	timelineView()->messagesView()->clearMessages();
	timelineView()->messagesView()->appendMessages(messages);
	timelineView()->messagesView()->refresh();

	timelineView()->messagesView()->setUpdatesEnabled(true);
}

void HistoryTab::futureMessagesAvailable()
{
	hideMessagesViewWaitOverlay();

	if (!MessagesFutureWatcher)
		return;

	setMessages(MessagesFutureWatcher->result());

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void HistoryTab::futureMessagesCanceled()
{
	hideMessagesViewWaitOverlay();

	if (!MessagesFutureWatcher)
		return;

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void HistoryTab::setFutureMessages(const QFuture<QVector<Message> > &futureMessages)
{
	setMessages(QVector<Message>());

	if (MessagesFutureWatcher)
		MessagesFutureWatcher->deleteLater();

	MessagesFutureWatcher = new QFutureWatcher<QVector<Message> >(this);
	connect(MessagesFutureWatcher, SIGNAL(finished()), this, SLOT(futureMessagesAvailable()));
	connect(MessagesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureMessagesCanceled()));

	MessagesFutureWatcher->setFuture(futureMessages);

	showMessagesViewWaitOverlay();
}

void HistoryTab::currentDateChanged()
{
	QDate date = timelineView()->currentDate();
	displayForDate(date);
}

void HistoryTab::showTimelinePopupMenu()
{
	if (TimelineView->currentDate().isValid())
		TimelinePopupMenu->exec(QCursor::pos());
}

void HistoryTab::removeEntries()
{
	QDate date = TimelineView->currentDate();
	if (date.isValid())
		removeEntriesPerDate(date);
}

void HistoryTab::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == QKeySequence::Copy && !TimelineView->messagesView()->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		TimelineView->messagesView()->pageAction(QWebPage::Copy)->trigger();
	else
		QWidget::keyPressEvent(event);
}

QList<int> HistoryTab::sizes() const
{
	QList<int> result = Splitter->sizes();
	result.append(TimelineView->sizes());

	return result;
}

void HistoryTab::setSizes(const QList<int> &newSizes)
{
	Q_ASSERT(newSizes.size() == 4);

	Splitter->setSizes(newSizes.mid(0, 2));
	TimelineView->setSizes(newSizes.mid(2, 2));
}
