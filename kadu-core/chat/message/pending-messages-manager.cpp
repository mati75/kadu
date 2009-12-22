/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextCodec>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-list-configuration-helper.h"
#include "buddies/buddy-shared.h"
#include "chat/chat-manager.h"
#include "configuration/xml-configuration-file.h"
#include "gui/widgets/chat-widget-manager.h"

#include "debug.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "pending-messages-manager.h"

PendingMessagesManager * PendingMessagesManager::Instance = 0;

PendingMessagesManager *  PendingMessagesManager::instance()
{
	if (0 == Instance)
		Instance = new PendingMessagesManager();

	return Instance;
}

PendingMessagesManager::PendingMessagesManager() : msgs()
{
}

void PendingMessagesManager::deleteMsg(int index)
{
	kdebugm(KDEBUG_INFO, "PendingMessagesManager::(pre)deleteMsg(%d), count=%d\n", index, count());
	Buddy e = msgs[index].messageSender().ownerBuddy();
	msgs.removeAt(index);
	storeConfiguration(xml_config_file);
	kdebugm(KDEBUG_INFO, "PendingMessagesManager::deleteMsg(%d), count=%d\n", index, count());
	emit messageFromUserDeleted(e);
}

bool PendingMessagesManager::pendingMsgs(Buddy buddy) const
{
	foreach (const Message &msg, msgs)
		if (msg.messageSender().ownerBuddy() == buddy)
			return true;

	return false;
}

unsigned int PendingMessagesManager::pendingMsgsCount(Chat chat) const
{
	unsigned int count = 0;

	foreach (const Message &msg, msgs)
	{
		if (chat == msg.messageChat())
			count++;
	}

	return count;
}

bool PendingMessagesManager::pendingMsgs() const
{
	return !msgs.isEmpty();
}

int PendingMessagesManager::count() const
{
	return msgs.count();
}

Message &PendingMessagesManager::operator[](int index)
{
	return msgs[index];
}

void PendingMessagesManager::addMsg(const Message &msg)
{
	Message message = msg;
	msgs.append(message);
	storeConfiguration(xml_config_file);
	emit messageFromUserAdded(msg.messageSender().ownerBuddy());
}

void PendingMessagesManager::loadConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement pendingMsgsNode = configurationStorage->getNode("PendingMessages", XmlConfigFile::ModeFind);
	if (pendingMsgsNode.isNull())
		return;

	QList<QDomElement> pendingMsgsElements = configurationStorage->getNodes(pendingMsgsNode, "PendingMessage");
	foreach (QDomElement messageElement, pendingMsgsElements)
	{
		Message msg = Message::create();
		QDomElement chatNode = configurationStorage->getNode(messageElement, "Chat", XmlConfigFile::ModeFind);
		Chat chat = ChatManager::instance()->byUuid(chatNode.text());
		msg.setMessageChat(chat);

		QDomElement timeNode = configurationStorage->getNode(messageElement, "ReceiveTime", XmlConfigFile::ModeFind);
		msg.setReceiveDate(QDateTime::fromString(timeNode.text()));

		timeNode = configurationStorage->getNode(messageElement, "SentTime", XmlConfigFile::ModeFind);
		msg.setSendDate(QDateTime::fromString(timeNode.text()));

		QDomElement messageNode = configurationStorage->getNode(messageElement, "Message", XmlConfigFile::ModeFind);
		msg.setContent(codec_latin2->toUnicode(messageNode.text().toLocal8Bit().data()));

		QDomElement senderNode = configurationStorage->getNode(messageElement, "Sender", XmlConfigFile::ModeFind);
		Buddy sender = BuddyManager::instance()->byUuid(senderNode.text());
		msg.setMessageSender(sender.contacts()[0]);

		msgs.append(msg);
		emit messageFromUserAdded(sender);
	}
}

void PendingMessagesManager::storeConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement pendingMsgsNode = configurationStorage->getNode("PendingMessages");
	configurationStorage->removeChildren(pendingMsgsNode);
	foreach (const Message &i, msgs)
	{
		QDomElement pendingMessageNode = configurationStorage->getNode(pendingMsgsNode,
			"PendingMessage", XmlConfigFile::ModeCreate);

		configurationStorage->createTextNode(pendingMessageNode, "Chat", i.messageChat().uuid().toString());
		configurationStorage->createTextNode(pendingMessageNode, "ReceiveTime", QString::number(i.receiveDate().toTime_t()));
		configurationStorage->createTextNode(pendingMessageNode, "SentTime", QString::number(i.sendDate().toTime_t()));

		configurationStorage->createTextNode(pendingMessageNode, "Message", codec_latin2->fromUnicode(i.content()));

		configurationStorage->createTextNode(pendingMessageNode, "Sender", i.messageSender().uuid().toString());
	}
}

void PendingMessagesManager::openMessages()
{
	ChatWidgetManager::instance()->openPendingMsgs();
}

bool PendingMessagesManager::removeContactFromStorage(Buddy buddy)
{
	return !pendingMsgs(buddy);
}


// void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
// {
// 	for (int i = 0, count = pending.count(); i < count; i++)
// 	{
// 		PendingMsgs::Element& e = pending[i];
//	TODO: 0.6.6 or sth?
// 		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
// 	}
// }
