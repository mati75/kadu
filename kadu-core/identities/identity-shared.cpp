/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "identities/identity-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "identity-shared.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "status/status-group.h"

IdentityShared * IdentityShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	IdentityShared *identityShared = loadFromStorage(storagePoint);
	identityShared->loadStub();

	return identityShared;
}

IdentityShared * IdentityShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	IdentityShared *identityShared = new IdentityShared();
	identityShared->setStorage(storagePoint);

	return identityShared;
}

IdentityShared::IdentityShared(const QUuid &uuid) :
		BaseStatusContainer(this), Shared(uuid), Permanent(false)
{
	setState(StateNotLoaded);
}

IdentityShared::~IdentityShared()
{
	ref.ref();
}

StorableObject * IdentityShared::storageParent()
{
	return IdentityManager::instance();
}

QString IdentityShared::storageNodeName()
{
	return QLatin1String("Identity");
}

void IdentityShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Permanent = loadValue<bool>("Permanent");
	Name = loadValue<QString>("Name");
}

void IdentityShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Permanent", Permanent);
	storeValue("Name", Name);
}

void IdentityShared::aboutToBeRemoved()
{
	Accounts.clear();
}

void IdentityShared::addAccount(Account account)
{
	ensureLoaded();

	Accounts.append(account);
	connect(account, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	emit statusUpdated();
}

void IdentityShared::removeAccount(Account account)
{
	ensureLoaded();

	Accounts.removeAll(account);
	disconnect(account, SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	emit statusUpdated();
}

bool IdentityShared::hasAccount(Account account)
{
	ensureLoaded();

	return Accounts.contains(account);
}

bool IdentityShared::hasAnyAccountWithDetails()
{
	ensureLoaded();

	foreach (const Account &account, Accounts)
		if (account.details())
			return true;

	return false;
}

bool IdentityShared::isEmpty()
{
	ensureLoaded();

	return Accounts.isEmpty();
}

void IdentityShared::doSetStatus(Status status)
{
	ensureLoaded();

	foreach (const Account &account, Accounts)
		if (account)
			account.data()->setStatus(status);
}

Account IdentityShared::bestAccount()
{
	ensureLoaded();

	Account result;
	if (Accounts.isEmpty())
		return result;

	foreach (const Account &account, Accounts)
		if (account.details() && account.data())
		{
			// TODO: hack
			bool newConnected = account.data()->protocolHandler() && account.data()->protocolHandler()->isConnected();
			bool oldConnected = false;
			if (result)
				oldConnected = result.data()->protocolHandler() && result.data()->protocolHandler()->isConnected();

			if (!result || (newConnected && !oldConnected)  || (account.protocolName() == "gadu" && result.protocolName() != "gadu"))
			{
				result = account;
				if (newConnected && result.protocolName() == "gadu")
					break;
			}
		}

	return result;
}

Status IdentityShared::status()
{
	Account account = bestAccount();
	return account ? account.data()->status() : Status();
}

bool IdentityShared::isStatusSettingInProgress()
{
	Account account = bestAccount();
	return account ? account.data()->isStatusSettingInProgress() : false;
}

QString IdentityShared::statusDisplayName()
{
	return status().displayName();
}

KaduIcon IdentityShared::statusIcon()
{
	return statusIcon(status());
}

KaduIcon IdentityShared::statusIcon(const Status &status)
{
	Account account = bestAccount();
	return account ? account.data()->statusIcon(status) : KaduIcon();
}

KaduIcon IdentityShared::statusIcon(const QString &statusType)
{
	Account account = bestAccount();
	return account ? account.data()->statusIcon(statusType) : KaduIcon();
}

QList<StatusType *> IdentityShared::supportedStatusTypes()
{
	Account account = bestAccount();
	return account ? account.data()->supportedStatusTypes() : QList<StatusType *>();
}

int IdentityShared::maxDescriptionLength()
{
	Account account = bestAccount();
	return account ? account.data()->maxDescriptionLength() : -1;
}