/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact-list.h"

#include "contact-set.h"

ContactSet::ContactSet()
{
}

ContactSet::ContactSet(Contact contact)
{
	insert(contact);
}

ContactList ContactSet::toContactList() const
{
	return toList();
}

Account * ContactSet::prefferedAccount()
{
	QList<Account *> accounts,contactAccounts;
	// TODO 0.6.6 - Rework it if more than 1 account on the same proto.

	ContactList contacts = toList();

	foreach(Contact contact, contacts)
	{
		contactAccounts = contact.accounts();
		// one contact have no account = no common account
		if (contactAccounts.count() == 0)
			return 0;

		accounts.append(contactAccounts);
	}

	foreach(Account *account, AccountManager::instance()->accounts())
	{
		if (accounts.count(account) == count())
		{
			return account;
		}
	}

	return 0;
}
