/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"

#include "simple-chat.h"
#include "chat-manager.h"

SimpleChat::SimpleChat(Account *currentAccount, Contact contact, QUuid uuid)
	: Chat(currentAccount, uuid), CurrentContact(contact)
{
}

SimpleChat::~SimpleChat()
{
}

void SimpleChat::load()
{
	if (!isValidStorage())
		return;

	Chat::load();
	CurrentContact = ContactManager::instance()->byUuid(loadValue<QString>("Contact"));
}

void SimpleChat::store()
{
	if (!isValidStorage())
		return;

	Chat::store();
	storeValue("Type", "Simple");
	storeValue("Contact", CurrentContact.uuid().toString());
}

ContactSet SimpleChat::contacts()
{
	return ContactSet(CurrentContact);
}
