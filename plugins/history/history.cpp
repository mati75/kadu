/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2002, 2003, 2004, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QScopedPointer>
#include <QtGui/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/actions/actions.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "misc/path-conversion.h"
#include "protocols/services/chat-service.h"

#include "debug.h"

#include "actions/show-history-action-description.h"
#include "gui/windows/history-window.h"
#include "model/dates-model-item.h"
#include "history-messages-prepender.h"
#include "history-save-thread.h"
#include "timed-status.h"

#include "history.h"

void disableNonHistoryContacts(Action *action)
{
	kdebugf();
	action->setEnabled(false);
	const ContactSet &contacts = action->context()->contacts();

	if (contacts.isEmpty())
		return;

	foreach (const Contact &contact, contacts)
	{
		if (Core::instance()->myself() == contact.ownerBuddy())
			return;

		Account account = contact.contactAccount();
		if (!account.protocolHandler() || !account.protocolHandler()->chatService())
			return;
	}

	action->setEnabled(true);
	kdebugf2();
}

History * History::Instance = 0;

void History::createInstance()
{
	if (!Instance)
		Instance = new History();
}

void History::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

History * History::instance()
{
	return Instance;
}

History::History() :
		ConfigurationUiHandler(0), SyncEnabled(true), SaveThread(0), CurrentStorage(0)
{
	kdebugf();
	createActionDescriptions();
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
		this, SLOT(accountRegistered(Account)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
		this, SLOT(accountUnregistered(Account)));
	connect(MessageManager::instance(), SIGNAL(messageReceived(Message)),
		this, SLOT(enqueueMessage(Message)));
	connect(MessageManager::instance(), SIGNAL(messageSent(Message)),
		this, SLOT(enqueueMessage(Message)));

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));

	createDefaultConfiguration();
	configurationUpdated();
	kdebugf2();
}

History::~History()
{
	kdebugf();

	disconnect(MessageManager::instance(), SIGNAL(messageReceived(Message)),
		this, SLOT(enqueueMessage(Message)));

	stopSaveThread();
	deleteActionDescriptions();

	kdebugf2();
}

void History::createActionDescriptions()
{
	Actions::instance()->blockSignals();

	ShowHistoryActionDescriptionInstance = new ShowHistoryActionDescription(this);

	TalkableMenuManager::instance()->addActionDescription(ShowHistoryActionDescriptionInstance, TalkableMenuItem::CategoryView, 100);
	Core::instance()->kaduWindow()->insertMenuActionDescription(ShowHistoryActionDescriptionInstance, KaduWindow::MenuKadu, 5);

	// The last ActionDescription will send actionLoaded() signal.
	Actions::instance()->unblockSignals();

	ClearHistoryActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "clearHistoryAction",
		this, SLOT(clearHistoryActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/clear-history"), tr("Clear History"), false,
		disableNonHistoryContacts
	);

}

void History::deleteActionDescriptions()
{
	TalkableMenuManager::instance()->removeActionDescription(ShowHistoryActionDescriptionInstance);
	Core::instance()->kaduWindow()->removeMenuActionDescription(ShowHistoryActionDescriptionInstance);

	delete ShowHistoryActionDescriptionInstance;
	ShowHistoryActionDescriptionInstance = 0;
}

void History::clearHistoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	if (!CurrentStorage)
		return;

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	if (action->context()->chat())
		CurrentStorage->clearChatHistory(action->context()->chat());
}

void History::chatCreated(ChatWidget *chatWidget)
{
	kdebugf();

	if (!chatWidget)
		return;

	if (!CurrentStorage)
		return;

	ChatMessagesView *chatMessagesView = chatWidget->chatMessagesView();
	if (!chatMessagesView)
		return;

	Chat chat = AggregateChatManager::instance()->aggregateChat(chatWidget->chat());

	QDateTime backTo = QDateTime::currentDateTime().addSecs(ChatHistoryQuotationTime * 3600);
	QFuture<QVector<Message> > futureMessages = CurrentStorage->asyncMessagesBackTo(chat ? chat : chatWidget->chat(), backTo,
			config_file.readNumEntry("Chat", "ChatPruneLen", 20));
	new HistoryMessagesPrepender(futureMessages, chatMessagesView);
}

void History::accountRegistered(Account account)
{
	if (!account.protocolHandler())
		return;

	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
}

void History::accountUnregistered(Account account)
{
	if (!account.protocolHandler())
		return;

	disconnect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));

	ChatService *service = account.protocolHandler()->chatService();
	if (service)
		disconnect(service, SIGNAL(messageSent(const Message &)),
				this, SLOT(enqueueMessage(const Message &)));
}

void History::enqueueMessage(const Message &message)
{
	if (!CurrentStorage || !SaveChats)
		return;

	if (!SaveChatsWithAnonymous && message.messageChat().contacts().count() == 1
		&& (*message.messageChat().contacts().constBegin()).isAnonymous())
		return;

	UnsavedDataMutex.lock();
	UnsavedMessages.enqueue(message);
	UnsavedDataMutex.unlock();

	SaveThread->newDataAvailable();
}

void History::contactStatusChanged(Contact contact, Status oldStatus)
{
	Q_UNUSED(oldStatus)

	Status status = contact.currentStatus();
	if (!CurrentStorage || !SaveStatuses)
		return;

	if (SaveOnlyStatusesWithDescription && status.description().isEmpty())
		return;

	UnsavedDataMutex.lock();
	UnsavedStatusChanges.enqueue(qMakePair(contact, status));
	UnsavedDataMutex.unlock();

	SaveThread->newDataAvailable();
}

Message History::dequeueUnsavedMessage()
{
	QMutexLocker locker(&UnsavedDataMutex);

	if (UnsavedMessages.isEmpty())
		return Message::null;

	return UnsavedMessages.dequeue();
}

QPair<Contact, Status> History::dequeueUnsavedStatusChange()
{
	QMutexLocker locker(&UnsavedDataMutex);

	if (UnsavedStatusChanges.isEmpty())
		return qMakePair(Contact::null, Status());

	return UnsavedStatusChanges.dequeue();
}

void History::crash()
{
	// don't try to stop/wait on thread here, see bug #2228
	if (SaveThread)
		SaveThread->forceSync(true);
}

void History::startSaveThread()
{
	if (!SaveThread)
	{
		SaveThread = new HistorySaveThread(this, this);
		SaveThread->setEnabled(SyncEnabled);
	}

	if (!SaveThread->isRunning())
		SaveThread->start();
}

void History::stopSaveThread()
{
	if (SaveThread && SaveThread->isRunning())
	{
		SaveThread->stop();
		SaveThread->wait(30000);

		if (SaveThread->isRunning())
		{
			kdebugm(KDEBUG_WARNING, "terminating history save thread!\n");

			SaveThread->terminate();
			SaveThread->wait(2000);
		}
	}
}

void History::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	dontCiteOldMessagesLabel = static_cast<QLabel *>(mainConfigurationWindow->widget()->widgetById("history/dontCiteOldMessagesLabel"));
	QSlider *dontCiteOldMessagesSlider = static_cast<QSlider *>(mainConfigurationWindow->widget()->widgetById("history/dontCiteOldMessages"));
	updateQuoteTimeLabel(dontCiteOldMessagesSlider->value());
	connect(dontCiteOldMessagesSlider, SIGNAL(valueChanged(int)),
		this, SLOT(updateQuoteTimeLabel(int)));

	connect(mainConfigurationWindow->widget()->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/savechatswithanonymous"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("history/savestatuschanges"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("history/saveonlystatuswithdescription"), SLOT(setEnabled(bool)));
}

void History::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLabel->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
}

void History::configurationUpdated()
{
	kdebugf();

	ChatHistoryCitation = config_file.readNumEntry("History", "ChatHistoryCitation");
	ChatHistoryQuotationTime = config_file.readNumEntry("History", "ChatHistoryQuotationTime", -24);

	SaveChats = config_file.readBoolEntry("History", "SaveChats", true);
	SaveChatsWithAnonymous = config_file.readBoolEntry("History", "SaveChatsWithAnonymous", true);
	SaveStatuses = config_file.readBoolEntry("History", "SaveStatusChanges", false);
	SaveOnlyStatusesWithDescription = config_file.readBoolEntry("History", "SaveOnlyStatusWithDescription", false);

	kdebugf2();
}

void History::registerStorage(HistoryStorage *storage)
{
	CurrentStorage = storage;

	stopSaveThread();

	if (!CurrentStorage)
		return;

	startSaveThread();

	foreach (ChatWidget *chat, ChatWidgetManager::instance()->chats())
		chatCreated(chat);

	foreach (const Account &account, AccountManager::instance()->items())
		accountRegistered(account);
}

void History::unregisterStorage(HistoryStorage *storage)
{
	if (CurrentStorage != storage)
		return;

	foreach (const Account &account, AccountManager::instance()->items())
		accountUnregistered(account);

	stopSaveThread();

	delete CurrentStorage;
	CurrentStorage = 0;
}

QFuture<QVector<Chat> > History::chatsList()
{
	kdebugf();

	return CurrentStorage->chats();
}

QFuture<QVector<DatesModelItem> > History::datesForChat(const Chat &chat)
{
	kdebugf();

	return CurrentStorage->chatDates(chat);
}

QVector<Message> History::messages(const Chat &chat, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->messages(chat, date, limit);
}

QFuture<QVector<Buddy> > History::statusBuddiesList()
{
	kdebugf();

	return CurrentStorage->statusBuddiesList();
}

QFuture<QVector<DatesModelItem> > History::datesForStatusBuddy(const Buddy &buddy)
{
	kdebugf();

	return CurrentStorage->datesForStatusBuddy(buddy);
}

QList<TimedStatus> History::statuses(const Buddy &buddy, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->statuses(buddy, date, limit);
}

QFuture<QVector<DatesModelItem> > History::datesForStatusContact(const Contact &contact)
{
	kdebugf();

	return CurrentStorage->datesForStatusContact(contact);
}

QList<TimedStatus> History::statuses(const Contact &contact, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->statuses(contact, date, limit);
}

QFuture<QList<QString> > History::smsRecipientsList()
{
	kdebugf();

	return CurrentStorage->smsRecipientsList();
}

QFuture<QVector<DatesModelItem> > History::datesForSmsRecipient(const QString &recipient)
{
	kdebugf();

	return CurrentStorage->datesForSmsRecipient(recipient);
}

QVector<Message> History::sms(const QString &recipient, const QDate &date, int limit)
{
	kdebugf();

	return CurrentStorage->sms(recipient, date, limit);
}

void History::deleteHistory(const Buddy &buddy)
{
	if (CurrentStorage)
		CurrentStorage->deleteHistory(buddy);
}

void History::createDefaultConfiguration()
{
	config_file.addVariable("History", "SaveChats", config_file.readBoolEntry("History", "Logging", true));
	config_file.removeVariable("History", "Logging");

	config_file.addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");

	config_file.addVariable("History", "SaveStatusChanges", true);

	config_file.addVariable("History", "SaveChatsWithAnonymous", true);

	config_file.addVariable("History", "SaveOnlyStatusWithDescription", true);

	config_file.addVariable("History", "ChatHistoryCitation", 10);
	config_file.addVariable("History", "ChatHistoryQuotationTime", -24);
}

void History::forceSync()
{
	if (SaveThread)
		SaveThread->forceSync();
}

void History::setSyncEnabled(bool syncEnabled)
{
	if (SaveThread)
		SaveThread->setEnabled(syncEnabled);
}
