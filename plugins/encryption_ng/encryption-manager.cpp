/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"
#include "services/raw-message-transformer-service.h"

#include "configuration/encryption-ng-configuration.h"
#include "decryptor.h"
#include "encryption-actions.h"
#include "encryption-chat-data.h"
#include "encryption-provider-manager.h"
#include "encryptor.h"

#include "encryption-manager.h"

EncryptionManager * EncryptionManager::Instance = 0;

void EncryptionManager::createInstance()
{
	if (!Instance)
		new EncryptionManager();
}

void EncryptionManager::destroyInstance()
{
	delete Instance;
}

EncryptionManager::EncryptionManager() :
		Generator(0)
{
	Instance = this;

	foreach (ChatWidget *chatWidget, ChatWidgetManager::instance()->chats())
		chatWidgetCreated(chatWidget);

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)),
			this, SLOT(chatWidgetCreated(ChatWidget*)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)),
			this, SLOT(chatWidgetDestroying(ChatWidget*)));

	Core::instance()->rawMessageTransformerService()->registerTransformer(this);
}

EncryptionManager::~EncryptionManager()
{
	Core::instance()->rawMessageTransformerService()->unregisterTransformer(this);

	disconnect(ChatWidgetManager::instance(), 0, this, 0);

	foreach (ChatWidget *chatWidget, ChatWidgetManager::instance()->chats())
		chatWidgetDestroying(chatWidget);

	Instance = 0;
}

EncryptionChatData * EncryptionManager::chatEncryption(const Chat &chat)
{
	if (!ChatEnryptions.contains(chat))
		ChatEnryptions.insert(chat, new EncryptionChatData(chat, this));

	return ChatEnryptions.value(chat);
}

bool EncryptionManager::setEncryptionEnabled(const Chat& chat)
{
	if (!chat)
		return false;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);
	Encryptor *encryptor = encryptionChatData->encryptor();
	bool enableSucceeded;

	if (encryptor && encryptor->provider() == EncryptionProviderManager::instance()->defaultEncryptorProvider(chat))
		enableSucceeded = true;
	else
	{
		if (encryptor)
			encryptor->provider()->releaseEncryptor(chat, encryptor);

		encryptor = EncryptionProviderManager::instance()->acquireEncryptor(chat);
		encryptionChatData->setEncryptor(encryptor);
		enableSucceeded = (0 != encryptor);
	}

	EncryptionActions::instance()->checkEnableEncryption(chat, enableSucceeded);

	return enableSucceeded;
}

void EncryptionManager::setEncryptionProvider(const Chat &chat, EncryptionProvider *encryptionProvider)
{
	if (!chat)
		return;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);
	Encryptor *currentEncryptor = encryptionChatData->encryptor();

	if (currentEncryptor && currentEncryptor->provider() == encryptionProvider)
		return;

	if (currentEncryptor)
		currentEncryptor->provider()->releaseEncryptor(chat, currentEncryptor);

	encryptionChatData->setEncryptor(encryptionProvider ? encryptionProvider->acquireEncryptor(chat) : 0);
	EncryptionActions::instance()->checkEnableEncryption(chat, encryptionChatData->encryptor());
}

EncryptionProvider * EncryptionManager::encryptionProvider(const Chat &chat)
{
	if (!chat)
		return 0;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);
	Encryptor *currentEncryptor = encryptionChatData->encryptor();
	if (!currentEncryptor)
		return 0;

	return currentEncryptor->provider();
}

void EncryptionManager::chatWidgetCreated(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	if (!EncryptionProviderManager::instance()->canEncrypt(chat))
		return;

	if (chatEncryption(chat)->encrypt())
		setEncryptionEnabled(chat);
}

void EncryptionManager::chatWidgetDestroying(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);

	// free some memory, these objects will be recreated when needed
	if (encryptionChatData->decryptor())
	{
		encryptionChatData->decryptor()->provider()->releaseDecryptor(chat, encryptionChatData->decryptor());
		encryptionChatData->setDecryptor(0);
	}
	if (encryptionChatData->encryptor())
	{
		encryptionChatData->encryptor()->provider()->releaseEncryptor(chat, encryptionChatData->encryptor());
		encryptionChatData->setEncryptor(0);
	}
}

void EncryptionManager::setGenerator(KeyGenerator *generator)
{
	Generator = generator;
}

KeyGenerator * EncryptionManager::generator()
{
	return Generator;
}

QByteArray EncryptionManager::transformIncomingMessage(const QByteArray &rawMessage, const Message &message)
{
	if (!message.messageChat())
		return rawMessage;

	if (!EncryptionProviderManager::instance()->canDecrypt(message.messageChat()))
		return rawMessage;

	EncryptionChatData *encryptionChatData = chatEncryption(message.messageChat());
	if (!encryptionChatData->decryptor())
		encryptionChatData->setDecryptor(EncryptionProviderManager::instance()->acquireDecryptor(message.messageChat()));

	bool decrypted;
	QByteArray result = encryptionChatData->decryptor()->decrypt(rawMessage, message.messageChat(), &decrypted);

	if (decrypted && EncryptionNgConfiguration::instance()->encryptAfterReceiveEncryptedMessage())
		setEncryptionEnabled(message.messageChat());

	return result;
}

QByteArray EncryptionManager::transformOutgoingMessage(const QByteArray &rawMessage, const Message &message)
{
	if (!message.messageChat())
		return rawMessage;

	EncryptionChatData *encryptionChatData = chatEncryption(message.messageChat());
	if (encryptionChatData && encryptionChatData->encryptor())
		return encryptionChatData->encryptor()->encrypt(rawMessage);

	return rawMessage;
}

QByteArray EncryptionManager::transform(const QByteArray &rawMessage, const Message& message)
{
	switch (message.type())
	{
		case MessageTypeSent:
			return transformOutgoingMessage(rawMessage, message);
		case MessageTypeReceived:
			return transformIncomingMessage(rawMessage, message);
		default:
			return rawMessage;
	}
}
