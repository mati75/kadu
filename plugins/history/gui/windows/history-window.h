/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QtGui/QDateEdit>
#include <QtGui/QTreeWidget>

#include "buddies/buddy-set.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/windows/main-window.h"
#include "talkable/talkable.h"

#include "history.h"
#include "history_exports.h"

class BaseActionContext;
class BuddyListModel;
class BuddyStatusDatesModel;
class ChatDatesModel;
class ChatsListModel;
class HistoryChatsModel;
class HistoryChatsModelProxy;
class HistoryTreeItem;
class ModelChain;
class NameTalkableFilter;
class SmsDatesModel;
class TalkableTreeView;
class TimedStatus;

/*!
\class HistoryWindow
\author Juzef, Vogel
*/
class HistoryWindow : public MainWindow
{
	Q_OBJECT

	friend class History;

	static HistoryWindow *Instance;

	BuddyListModel *BuddiesModel;
	ChatsListModel *ChatsModel2;

	TalkableTreeView *ChatsTalkableTree;
	ModelChain *ChatsModelChain;

	QTreeView *ChatsTree;
	QTreeView *DetailsListView;
	HistoryChatsModel *ChatsModel;
	HistoryChatsModelProxy *ChatsModelProxy;
	NameTalkableFilter *StatusBuddyNameFilter;

	ChatDatesModel *MyChatDatesModel;
	BuddyStatusDatesModel *MyBuddyStatusDatesModel;
	SmsDatesModel *MySmsDatesModel;

	QMenu *DetailsPopupMenu;

	ChatMessagesView *ContentBrowser;

	ActionDescription *historySearchActionDescription;
	ActionDescription *historyNextResultsActionDescription;
	ActionDescription *historyPrevResultsActionDescription;

	BaseActionContext *Context;

	explicit HistoryWindow(QWidget *parent = 0);

	void createGui();
	void createChatTree(QWidget *parent);
	void connectGui();

	void updateData();
	void selectChat(const Chat &chat);

	void chatActivated(const Chat &chat);
	void statusBuddyActivated(const Buddy &buddy);
	void smsRecipientActivated(const QString &recipient);
	void treeItemActivated(const HistoryTreeItem &item);

	ContactSet selectedContacts() const;
	Chat selectedChat() const;

	QVector<Message> statusesToMessages(const QList<TimedStatus> &statuses);

private slots:
	void currentChatChanged(const Talkable &talkable);

	void treeCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	void dateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	void showChatsPopupMenu(const QPoint &pos);
	void showMainPopupMenu(const QPoint &pos);
	void showDetailsPopupMenu(const QPoint &pos);

	void openChat();
	void clearChatHistory2();
	void clearChatHistory();
	void clearStatusHistory();
	void clearSmsHistory();
	void removeHistoryEntriesPerDate();

	void updateContext();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static void show(const Chat &chat);

	virtual ~HistoryWindow();

	virtual QTreeView * detailsListView() { return DetailsListView; }

	virtual ChatMessagesView * contentBrowser() { return ContentBrowser; }

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual ChatWidget * chatWidget() { return 0; }
	virtual TalkableProxyModel * talkableProxyModel() { return 0; }

};

class HistoryChatsModel;

#endif // HISTORY_WINDOW_H
