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

#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/wait-overlay.h"
#include "model/roles.h"

#include "model/history-query-results-model.h"
#include "model/history-query-results-proxy-model.h"
#include "history-query-result.h"

#include "timeline-chat-messages-view.h"

TimelineChatMessagesView::TimelineChatMessagesView(QWidget *parent) :
		QWidget(parent),
		TimelineWaitOverlay(0), MessagesViewWaitOverlay(0),
		ResultsFutureWatcher (0), MessagesFutureWatcher(0)
{
	ResultsModel = new HistoryQueryResultsModel(this);
	ResultsProxyModel = new HistoryQueryResultsProxyModel(ResultsModel);
	ResultsProxyModel->setSourceModel(ResultsModel);

	setLayout(new QVBoxLayout(this));
	layout()->setMargin(0);
	layout()->setSpacing(0);

	createGui();
}

TimelineChatMessagesView::~TimelineChatMessagesView()
{
}

void TimelineChatMessagesView::createGui()
{
	Splitter = new QSplitter(Qt::Vertical, this);

	Timeline = new QTreeView(Splitter);

	Timeline->setAlternatingRowColors(true);
	Timeline->setModel(ResultsProxyModel);
	Timeline->setRootIsDecorated(false);
	Timeline->setUniformRowHeights(true);

	connect(Timeline->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SIGNAL(currentDateChanged()));

	QFrame *frame = new QFrame(Splitter);
	frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	QVBoxLayout *frameLayout = new QVBoxLayout(frame);
	frameLayout->setMargin(0);

	MessagesView = new ChatMessagesView(Chat::null, false, frame);
	MessagesView->setFocusPolicy(Qt::StrongFocus);
	MessagesView->setForcePruneDisabled(true);

	frameLayout->addWidget(MessagesView);

	layout()->addWidget(Splitter);
}

QDate TimelineChatMessagesView::currentDate() const
{
	return Timeline->currentIndex().data(DateRole).value<QDate>();
}

void TimelineChatMessagesView::setResults(const QVector<HistoryQueryResult> &results)
{
	ResultsModel->setResults(results);

	if (results.isEmpty())
	{
		emit currentDateChanged();
		return;
	}

	Timeline->setCurrentIndex(ResultsModel->index(results.size() - 1, 0));
	QScrollBar *scrollBar = Timeline->verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
}

void TimelineChatMessagesView::futureResultsAvailable()
{
	hideTimelineWaitOverlay();

	if (!ResultsFutureWatcher)
		return;

	setResults(ResultsFutureWatcher->result());

	ResultsFutureWatcher->deleteLater();
	ResultsFutureWatcher = 0;
}

void TimelineChatMessagesView::futureResultsCanceled()
{
	hideTimelineWaitOverlay();

	if (!ResultsFutureWatcher)
		return;

	ResultsFutureWatcher->deleteLater();
	ResultsFutureWatcher = 0;
}

void TimelineChatMessagesView::setFutureResults(const QFuture<QVector<HistoryQueryResult> > &futureResults)
{
	if (ResultsFutureWatcher)
	{
		ResultsFutureWatcher->cancel();
		ResultsFutureWatcher->deleteLater();
	}

	ResultsFutureWatcher = new QFutureWatcher<QVector<HistoryQueryResult> >(this);
	connect(ResultsFutureWatcher, SIGNAL(finished()), this, SLOT(futureResultsAvailable()));
	connect(ResultsFutureWatcher, SIGNAL(canceled()), this, SLOT(futureResultsCanceled()));

	ResultsFutureWatcher->setFuture(futureResults);

	showTimelineWaitOverlay();
}

void TimelineChatMessagesView::setMessages(const QVector<Message> &messages)
{
	MessagesView->setUpdatesEnabled(false);

	MessagesView->clearMessages();
	MessagesView->appendMessages(messages);
	MessagesView->refresh();

	MessagesView->setUpdatesEnabled(true);
}

void TimelineChatMessagesView::futureMessagesAvailable()
{
	if (!MessagesFutureWatcher)
	{
		hideMessagesViewWaitOverlay();
		return;
	}

	setMessages(MessagesFutureWatcher->result());
	hideMessagesViewWaitOverlay(); // wait for messages to display before hiding

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void TimelineChatMessagesView::futureMessagesCanceled()
{
	hideMessagesViewWaitOverlay();

	if (!MessagesFutureWatcher)
		return;

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void TimelineChatMessagesView::setFutureMessages(const QFuture<QVector<Message> > &futureMessages)
{
	if (MessagesFutureWatcher)
	{
		MessagesFutureWatcher->cancel();
		MessagesFutureWatcher->deleteLater();
	}

	MessagesFutureWatcher = new QFutureWatcher<QVector<Message> >(this);
	connect(MessagesFutureWatcher, SIGNAL(finished()), this, SLOT(futureMessagesAvailable()));
	connect(MessagesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureMessagesCanceled()));

	MessagesFutureWatcher->setFuture(futureMessages);

	showMessagesViewWaitOverlay();
}

void TimelineChatMessagesView::showTimelineWaitOverlay()
{
	if (!TimelineWaitOverlay)
		TimelineWaitOverlay = new WaitOverlay(this);
	else
		TimelineWaitOverlay->show();
}

void TimelineChatMessagesView::hideTimelineWaitOverlay()
{
	TimelineWaitOverlay->deleteLater();
	TimelineWaitOverlay = 0;
}

void TimelineChatMessagesView::showMessagesViewWaitOverlay()
{
	if (!MessagesViewWaitOverlay)
		MessagesViewWaitOverlay = new WaitOverlay(MessagesView);
	else
		MessagesViewWaitOverlay->show();
}

void TimelineChatMessagesView::hideMessagesViewWaitOverlay()
{
	MessagesViewWaitOverlay->deleteLater();
	MessagesViewWaitOverlay = 0;
}

void TimelineChatMessagesView::setTalkableVisible(const bool talkableVisible)
{
	ResultsProxyModel->setTalkableVisible(talkableVisible);
}

void TimelineChatMessagesView::setTitleVisible(const bool titleVisible)
{
	ResultsProxyModel->setTitleVisible(titleVisible);
}

void TimelineChatMessagesView::setTalkableHeader(const QString &talkableHeader)
{
	ResultsModel->setTalkableHeader(talkableHeader);
}

void TimelineChatMessagesView::setLengthHeader(const QString &lengthHeader)
{
	ResultsModel->setLengthHeader(lengthHeader);
}

QList<int> TimelineChatMessagesView::sizes() const
{
	return Splitter->sizes();
}

void TimelineChatMessagesView::setSizes(const QList<int> &newSizes)
{
	Q_ASSERT(newSizes.size() == 2);

	Splitter->setSizes(newSizes);
}
