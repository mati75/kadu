/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol.h"

#include "all-accounts-status-container.h"

AllAccountsStatusContainer::AllAccountsStatusContainer(QObject *parent) :
		StatusContainer(parent)
{
	triggerAllAccountsRegistered();
}

AllAccountsStatusContainer::~AllAccountsStatusContainer()
{
	triggerAllAccountsUnregistered();
}

void AllAccountsStatusContainer::accountRegistered(Account account)
{
	Accounts.append(account);
	connect(account.statusContainer(), SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	emit statusUpdated();
}

void AllAccountsStatusContainer::accountUnregistered(Account account)
{
	if (Accounts.removeAll(account) > 0)
	{
		disconnect(account.statusContainer(), SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));
		emit statusUpdated();
	}
}

Status AllAccountsStatusContainer::status()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->status() : Status();
}

bool AllAccountsStatusContainer::isStatusSettingInProgress()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->isStatusSettingInProgress() : false;
}

KaduIcon AllAccountsStatusContainer::statusIcon()
{
	return statusIcon(status());
}

KaduIcon AllAccountsStatusContainer::statusIcon(const Status &status)
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->statusIcon(status) : KaduIcon();
}

QList<StatusType> AllAccountsStatusContainer::supportedStatusTypes()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->supportedStatusTypes() : QList<StatusType>();
}

int AllAccountsStatusContainer::maxDescriptionLength()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->maxDescriptionLength() : -1;
}

void AllAccountsStatusContainer::setStatus(Status status)
{
	foreach (const Account &account, Accounts)
		if (account)
			account.statusContainer()->setStatus(status);
}

Status AllAccountsStatusContainer::loadStatus()
{
	Account account = AccountManager::bestAccount(Accounts);
	if (account)
		return account.statusContainer()->loadStatus();
	else
		return Status();
}

void AllAccountsStatusContainer::storeStatus(Status status)
{
	foreach (const Account &account, Accounts)
		account.statusContainer()->storeStatus(status);
}