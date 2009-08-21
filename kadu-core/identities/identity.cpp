/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPixmap>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "identity-manager.h"

#include "identity.h"

Identity::Identity(StoragePoint *storagePoint) :
		UuidStorableObject(storagePoint)
{
	connect(AccountManager::instance(), SIGNAL(accountRemoved(Account*)), this, SLOT(removeAccount(Account*)));
}

Identity::Identity(const QUuid &uuid) :
		UuidStorableObject("Identity", IdentityManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
    	connect(AccountManager::instance(), SIGNAL(accountRemoved(Account*)), this, SLOT(removeAccount(Account*)));
}

Identity::~Identity()
{
}

Identity * Identity::loadFromStorage(StoragePoint *identityStoragePoint)
{
	Identity *identity = new Identity(identityStoragePoint);
	identity->load();

	return identity;
}

void Identity::load()
{
    	StorableObject::load();

	if (!isValidStorage())
		return;

	Uuid = QUuid(storage()->point().attribute("uuid"));
	Name = loadValue<QString>("Name");

	XmlConfigFile *configurationStorage = storage()->storage();

	QDomElement accountsNode = configurationStorage->getNode(storage()->point(), "Accounts", XmlConfigFile::ModeFind);
	if (!accountsNode.isNull())
	{
		QDomNodeList accountsList = accountsNode.elementsByTagName("Account");

		int count = accountsList.count();
		for (int i = 0; i < count; i++)
		{
			QDomElement accountElement = accountsList.at(i).toElement();
			if (accountElement.isNull())
				continue;

			AccountsUuids << accountElement.text();
		}
	}

	triggerAllAccountsRegistered();
}

void Identity::store()
{
	if (!isValidStorage())
		return;

	storage()->point().setAttribute("uuid", Uuid.toString());

	storeValue("Name", Name);

	XmlConfigFile *configurationStorage = storage()->storage();

	if (AccountsUuids.count())
	{
		QDomElement accountsNode = configurationStorage->getNode(storage()->point(), "Accounts", XmlConfigFile::ModeCreate);
		foreach (const QString &account, AccountsUuids)
			configurationStorage->appendTextNode(accountsNode, "Account", account);
	}
	else
		configurationStorage->removeNode(storage()->point(), "Accounts");
}

void Identity::accountRegistered(Account *account)
{
	if (AccountsUuids.contains(account->uuid()))
		Accounts << account;
}

void Identity::accountUnregistered(Account *account)
{
	Accounts.removeAll(account);
}

void Identity::addAccount(Account *account)
{
	if (Accounts.contains(account))
		return;

	Accounts << account;
	AccountsUuids << account->uuid();
}

void Identity::removeAccount(Account *account)
{
    	Accounts.removeAll(account);
	AccountsUuids.removeAll(account->uuid());
}

//TODO 0.6.6:
void Identity::setStatus(Status status)
{
	foreach (Account *account, Accounts)
		account->setStatus(status);
}

Status Identity::status()
{
	return Accounts.count()
		? Accounts[0]->status()
		: Status();
}

QString Identity::statusName()
{
	return Status::name(status(), false);
}

QPixmap Identity::statusPixmap()
{
	return statusPixmap(status().type());
}

QPixmap Identity::statusPixmap(const QString &statusType)
{
	return Accounts.count()
		? Accounts[0]->statusPixmap(statusType)
		: QPixmap();
}

QList<StatusType *> Identity::supportedStatusTypes()
{
    	return Accounts.count()
		? Accounts[0]->supportedStatusTypes()
		: QList<StatusType *>();
}

int Identity::maxDescriptionLength()
{
    	return Accounts.count()
		? Accounts[0]->maxDescriptionLength()
		: -1;
}

void Identity::setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription)
{
	QString description;
    	if (StartupLastDescription)
		description = loadValue<QString>("LastStatusDescription");
	else
		description = startupDescription;

	QString name;
	if (startupStatus == "LastStatus")
	{
		name = loadValue<QString>("LastStatusName");
		if ("Offline" == name && offlineToInvisible)
			name = "Invisible";
	}
	else
		name = startupStatus;

	if ("Offline" == name && offlineToInvisible)
		name = "Invisible";

	Status status;
	status.setType(name);
	status.setDescription(description);

	setStatus(status);
}
void Identity::disconnectAndStoreLastStatus(bool disconnectWithCurrentDescription,
						  const QString &disconnectDescription)
{
	storeValue("LastStatusDescription", status().description());

	storeValue("LastStatusName", statusName());

	if (status().type() == "Offline")
		return;

	Status disconnectStatus;
	disconnectStatus.setType("Offline");
	QString description;
	if (disconnectWithCurrentDescription)
		description = status().description();
	else
		description = disconnectDescription;

	disconnectStatus.setDescription(description);
	setStatus(disconnectStatus);
}
