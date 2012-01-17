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
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/chat-messages-view.h"

#include "gui/widgets/timeline-chat-messages-view.h"

#include "history-tab.h"

HistoryTab::HistoryTab(QWidget *parent) :
		QWidget(parent)
{
}

HistoryTab::~HistoryTab()
{
}

void HistoryTab::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

	createTreeView(splitter);

	TimelineView = new TimelineChatMessagesView(splitter);
	TimelineView->timeline()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TimelineView->timeline(), SIGNAL(customContextMenuRequested(QPoint)),
	        this, SLOT(showTimelinePopupMenu(QPoint)));

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	splitter->setSizes(sizes);

	layout->addWidget(splitter);
}

TimelineChatMessagesView * HistoryTab::timelineView() const
{
	return TimelineView;
}

void HistoryTab::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == QKeySequence::Copy && !TimelineView->messagesView()->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		TimelineView->messagesView()->pageAction(QWebPage::Copy)->trigger();
	else
		QWidget::keyPressEvent(event);
}