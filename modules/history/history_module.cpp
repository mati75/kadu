/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLabel>

#include <sys/stat.h>
#include <sys/types.h>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "action.h"
#include "chat_edit_box.h"
#include "chat_manager.h"
#include "chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "history.h"
#include "history_dialog.h"
#include "hot_key.h"
#include "kadu.h"
#include "misc/misc.h"
#include "message_box.h"
#include "pending_msgs.h"
#include "toolbar.h"
#include "userbox.h"

#include "history_module.h"

extern "C" KADU_EXPORT int history_init(bool firstLoad)
{
	kdebugf();

	history_module = new HistoryModule(firstLoad);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/history.ui"), history_module);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void history_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/history.ui"), history_module);
	delete history_module;
	history_module = 0;

	kdebugf2();
}

void disableNonProtocolUles(KaduAction *action)
{
	kdebugf();

	const UserListElements &ules = action->userListElements();

	if (!ules.count())
	{
		action->setEnabled(false);
		return;
	}

	foreach(const UserListElement &user, ules)
		if (!user.protocolList().isEmpty())
		{
			action->setEnabled(true);
			return;
		}

	action->setEnabled(false);
	kdebugf2();
}

HistoryModule::HistoryModule(bool firstLoad)
{
	kdebugf();

	createDefaultConfiguration();

	QString path_ = ggPath(QString::null);
	path_.append("/history/");
#ifdef Q_OS_WIN
	QDir().mkdir(path_);
#else
	mkdir(qPrintable(path_), 0700);
#endif

	history = new HistoryManager();

	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget*)));

	foreach(ChatWidget *it, chat_manager->chats())
		chatCreated(it);

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		history, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)));
	connect(kadu, SIGNAL(removingUsers(UserListElements)), this, SLOT(removingUsers(UserListElements)));

	if (firstLoad)
	{
		Kadu::addAction("showHistoryAction");
		ChatEditBox::addAction("showHistoryAction");
	}

	historyActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "showHistoryAction",
		this, SLOT(historyActionActivated(QAction *, bool)),
		"History", tr("Show history"), false
	);
	historyActionDescription->setShortcut("kadu_viewhistory");
	UserBox::insertActionDescription(5, historyActionDescription);

	clearHistoryActionDescription = new ActionDescription(
		ActionDescription::TypeHistory, "clearHistoryAction",
		this, SLOT(clearHistoryActionActivated(QAction *, bool)),
		"ClearHistory", tr("Clear history"), false, "",
		disableNonProtocolUles
	);
	UserBox::insertManagementActionDescription(7, clearHistoryActionDescription);

	kdebugf2();
}

HistoryModule::~HistoryModule()
{
	kdebugf();

	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));

	foreach(ChatWidget *it, chat_manager->chats())
		chatDestroying(it);

	UserBox::removeActionDescription(historyActionDescription);
	delete historyActionDescription;
	historyActionDescription = 0;

	UserBox::removeManagementActionDescription(clearHistoryActionDescription);
	delete clearHistoryActionDescription;
	clearHistoryActionDescription = 0;

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	disconnect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		history, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, quint32, quint32, const QString &)));
	disconnect(kadu, SIGNAL(removingUsers(UserListElements)),
		this, SLOT(removingUsers(UserListElements)));

	delete history;
	history = 0;

	kdebugf2();
}

void HistoryModule::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	dontCiteOldMessagesLabel = dynamic_cast<QLabel *>(mainConfigurationWindow->widgetById("history/dontCiteOldMessagesLabel"));
	connect(mainConfigurationWindow->widgetById("history/dontCiteOldMessages"), SIGNAL(valueChanged(int)),
		this, SLOT(updateQuoteTimeLabel(int)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/statusChanges"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/citation"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/shortCuts"), SLOT(setEnabled(bool)));
}

void HistoryModule::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLabel->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
}

void HistoryModule::historyActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	
	UinsList uins;
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (window)
	{
		UserListElements users = window->userListElements();

		if (users.count() > 0)
			foreach(const UserListElement &user, users)
				uins.append(user.ID("Gadu").toUInt());
	}
	//TODO: throw out UinsList as soon as possible!
	(new HistoryDialog(uins))->show();

	kdebugf2();
}

void HistoryModule::clearHistoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UinsList uins;
	UserListElements users = window->userListElements();

	foreach(const UserListElement &user, users)
		if (user.usesProtocol("Gadu"))
			uins.append(user.ID("Gadu").toUInt());

	history->removeHistory(uins);

	kdebugf2();
}

void HistoryModule::chatKeyPressed(QKeyEvent *e, ChatWidget *chatWidget, bool &handled)
{
	if (HotKey::shortCut(e, "ShortCuts", "kadu_viewhistory"))
	{
		const UserGroup *users = chatWidget->users();
		UinsList uins;
		foreach(UserListElement user, *users)
			uins.append(user.ID("Gadu").toUInt());
		//TODO: throw out UinsList as soon as possible!
		(new HistoryDialog(uins))->show();

		handled = true;
	}
}

void HistoryModule::appendHistory(ChatWidget *chat)
{
	UserListElements senders = chat->users()->toUserListElements();

	QList<HistoryEntry> entries;
	QList<HistoryEntry> entriestmp;
	QDateTime date = QDateTime::currentDateTime();
	unsigned int from, end, count;

	UinsList uins;//TODO: throw out UinsList as soon as possible!
	foreach(const UserListElement &user, senders)
		uins.append(user.ID("Gadu").toUInt());

	count = history->getHistoryEntriesCount(uins);
	end = count - 1;
	from = count;
	unsigned int chatHistoryQuotation = config_file.readUnsignedNumEntry("History", "ChatHistoryCitation") + pending.pendingMsgsCount(senders);
	while (from >= 1 && entries.count() < chatHistoryQuotation)
	{
		if (end < chatHistoryQuotation)
			from = 0;
		else
			from = end - chatHistoryQuotation + 1;

		entriestmp = history->getHistoryEntries(uins, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebugmf(KDEBUG_INFO, "temp entries = %u\n", entriestmp.count());

		QList<HistoryEntry>::iterator it = entriestmp.begin();
		while (it != entriestmp.end())
		{
			if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
				|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV)
			{
				kdebugmf(KDEBUG_INFO, "%s %s\n",
					qPrintable(date.toString("dd.MM.yyyy hh:mm:ss")),
					qPrintable((*it).date.toString("dd.MM.yyyy hh:mm:ss")));
				if (date <= (*it).date)
					it = entriestmp.remove(it);
				else
					++it;
			}
			else
				++it;
		}

		if (!entriestmp.isEmpty())
			entries = entriestmp + entries;
		kdebugmf(KDEBUG_INFO, "entries = %u\n", entries.count());
		end = from - 1;
	}

	unsigned int entryCount = entries.count();
	if (entryCount < chatHistoryQuotation)
		from = 0;
	else
		from = entryCount - chatHistoryQuotation;

	QList<ChatMessage *> messages;

	int quotTime = config_file.readNumEntry("History", "ChatHistoryQuotationTime");

	QList<HistoryEntry>::const_iterator entry = entries.begin() + from;
	QList<HistoryEntry>::const_iterator entriesEnd = entries.end();
	for (; entry != entriesEnd; ++entry)
		if ((*entry).date.secsTo(QDateTime::currentDateTime()) <= -quotTime * 3600)
		{
			ChatMessage *message;
			if ((*entry).type == HISTORYMANAGER_ENTRY_MSGSEND || (*entry).type == HISTORYMANAGER_ENTRY_CHATSEND)
				message = new ChatMessage(kadu->myself(), senders, (*entry).message, TypeSent, (*entry).date);
			else
			message = new ChatMessage(userlist->byID("Gadu", QString::number((*entry).uin)), senders, (*entry).message, TypeReceived, (*entry).date, (*entry).sdate);
			messages.append(message);
		}

	if (!messages.empty())
		chat->appendMessages(messages);
}

void HistoryModule::chatCreated(ChatWidget *chat)
{
	kdebugf();

	connect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));

	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));

	// don't do it for already opened chats with discussions
	if (!chat->countMessages())
		appendHistory(chat);

	kdebugf2();
}

void HistoryModule::chatDestroying(ChatWidget *chat)
{
	disconnect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void HistoryModule::messageSentAndConfirmed(UserListElements receivers, const QString& message)
{
	UinsList uins;
	foreach(const UserListElement &user, receivers)
		uins.append(user.ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	history->addMyMessage(uins, message);
}

void HistoryModule::removingUsers(UserListElements users)
{
	kdebugf();
	if (!users.count())
		return;

	QString altNicks = users.altNicks().join(", ");
	QString tmp;

	for (unsigned int i = 0; i < users.count(); i += 10)
		tmp += (altNicks.section(", ", i, i + 9) + ",\n");

	if (!MessageBox::ask(tr("The following users were deleted:\n%0Do you want to remove history as well?").arg(tmp), "Warning", kadu))
		return;
	QString fname;
	foreach(const UserListElement &user, users)
	{
		if (user.usesProtocol("Gadu"))
		{
			fname = ggPath("history/") + user.ID("Gadu");
			kdebugmf(KDEBUG_INFO, "deleting %s\n", qPrintable(fname));
			QFile::remove(fname);
			QFile::remove(fname + ".idx");
		}
	}
	kdebugf2();
}

void HistoryModule::createDefaultConfiguration()
{
	config_file.addVariable("History", "ChatHistoryCitation", 10);
	config_file.addVariable("History", "ChatHistoryQuotationTime", -24);
	config_file.addVariable("History", "DontSaveStatusChanges", true);
	config_file.addVariable("History", "DontShowStatusChanges", true);
	config_file.addVariable("History", "Logging", true);

	config_file.addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");
}

HistoryModule* history_module = 0;
