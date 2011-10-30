/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/chat-widget-manager.h"
#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

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
	Instance = new EncryptionManager();
}

void EncryptionManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionManager::EncryptionManager() :
		Generator(0)
{
	foreach (ChatWidget *chatWidget, ChatWidgetManager::instance()->chats())
		chatWidgetCreated(chatWidget);

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)),
			this, SLOT(chatWidgetCreated(ChatWidget*)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)),
			this, SLOT(chatWidgetDestroying(ChatWidget*)));

	triggerAllAccountsRegistered();
}

EncryptionManager::~EncryptionManager()
{
	triggerAllAccountsUnregistered();

	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)),
			this, SLOT(chatWidgetCreated(ChatWidget*)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)),
			this, SLOT(chatWidgetDestroying(ChatWidget*)));

	foreach (ChatWidget *chatWidget, ChatWidgetManager::instance()->chats())
		chatWidgetDestroying(chatWidget);
}

void EncryptionManager::accountRegistered(Account account)
{
	if (!account.protocolHandler())
		return;

	ChatService *chatService = account.protocolHandler()->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)),
				this, SLOT(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)));
		connect(chatService, SIGNAL(filterRawOutgoingMessage(Chat,QByteArray&,bool&)),
				this, SLOT(filterRawOutgoingMessage(Chat,QByteArray&,bool&)));
	}
}

void EncryptionManager::accountUnregistered(Account account)
{
	if (!account.protocolHandler())
		return;

	ChatService *chatService = account.protocolHandler()->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)),
				this, SLOT(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)));
		disconnect(chatService, SIGNAL(filterRawOutgoingMessage(Chat,QByteArray&,bool&)),
				this, SLOT(filterRawOutgoingMessage(Chat,QByteArray&,bool&)));
	}
}

bool EncryptionManager::setEncryptionEnabled(const Chat &chat, bool enable, bool overrideChatDataSetting)
{
	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", this, true);
	if (enable)
	{
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

		if (overrideChatDataSetting)
			encryptionChatData->setEncrypt(enableSucceeded
					? EncryptionChatData::EncryptStateEnabled
					: EncryptionChatData::EncryptStateDisabled);
		EncryptionActions::instance()->checkEnableEncryption(chat, enableSucceeded);

		return enableSucceeded;
	}
	else
	{
		Encryptor *encryptor = encryptionChatData->encryptor();
		if (encryptor)
			encryptor->provider()->releaseEncryptor(chat, encryptor);
		encryptionChatData->setEncryptor(0);
		if (overrideChatDataSetting)
			encryptionChatData->setEncrypt(EncryptionChatData::EncryptStateDisabled);
		EncryptionActions::instance()->checkEnableEncryption(chat, false);

		return true; // we can always disable
	}
}

void EncryptionManager::filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore)
{
	Q_UNUSED(sender)
	Q_UNUSED(ignore)

	if (!chat)
		return;

	if (!EncryptionProviderManager::instance()->canDecrypt(chat))
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", this, true);
	if (!encryptionChatData->decryptor())
		encryptionChatData->setDecryptor(EncryptionProviderManager::instance()->acquireDecryptor(chat));

	bool decrypted;
	message = encryptionChatData->decryptor()->decrypt(message, chat, &decrypted);

	if (decrypted && EncryptionNgConfiguration::instance()->encryptAfterReceiveEncryptedMessage())
		setEncryptionEnabled(chat, true, false);
}

void EncryptionManager::filterRawOutgoingMessage(Chat chat, QByteArray &message, bool &stop)
{
	Q_UNUSED(stop)

	if (!chat)
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", this, false);
	if (encryptionChatData && encryptionChatData->encryptor())
		message = encryptionChatData->encryptor()->encrypt(message);
}

void EncryptionManager::chatWidgetCreated(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	if (!EncryptionProviderManager::instance()->canEncrypt(chat))
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", this, false);
	bool encryptFromDefault = (!encryptionChatData || encryptionChatData->encrypt() == EncryptionChatData::EncryptStateDefault)
			&& EncryptionNgConfiguration::instance()->encryptByDefault();

	if (encryptFromDefault || (encryptionChatData && encryptionChatData->encrypt() == EncryptionChatData::EncryptStateEnabled))
		setEncryptionEnabled(chat, true, !encryptFromDefault);
}

void EncryptionManager::chatWidgetDestroying(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", this, false);
	if (!encryptionChatData)
		return;

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