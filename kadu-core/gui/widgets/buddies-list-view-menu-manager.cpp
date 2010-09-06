/*
 * %kadu copyright begin%
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"

#include "buddies-list-view-menu-manager.h"

BuddiesListViewMenuManager * BuddiesListViewMenuManager::Instance = 0;

BuddiesListViewMenuManager * BuddiesListViewMenuManager::instance()
{
	if (!Instance)
		Instance = new BuddiesListViewMenuManager();

	return Instance;
}

BuddiesListViewMenuManager::BuddiesListViewMenuManager()
{
}

void BuddiesListViewMenuManager::addActionDescription(ActionDescription *actionDescription)
{
	BuddiesContexMenu.append(actionDescription);
}

void BuddiesListViewMenuManager::insertActionDescription(int pos, ActionDescription *actionDescription)
{
	BuddiesContexMenu.insert(pos, actionDescription);
}

void BuddiesListViewMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	BuddiesContexMenu.removeAll(actionDescription);
}

void BuddiesListViewMenuManager::addSeparator()
{
	BuddiesContexMenu.append(0);
}

void BuddiesListViewMenuManager::insertSeparator(int pos)
{
	BuddiesContexMenu.insert(pos, 0);
}

void BuddiesListViewMenuManager::addListActionDescription(ActionDescription *actionDescription)
{
	BuddyListActions.append(actionDescription);
}

void BuddiesListViewMenuManager::insertListActionDescription(int pos, ActionDescription *actionDescription)
{
	BuddyListActions.insert(pos, actionDescription);
}

void BuddiesListViewMenuManager::removeListActionDescription(ActionDescription *actionDescription)
{
	BuddyListActions.removeAll(actionDescription);
}

void BuddiesListViewMenuManager::addListSeparator()
{
	BuddyListActions.append(0);
}

QMenu * BuddiesListViewMenuManager::menu(QWidget *parent, ActionDataSource *actionDataSource, QList<Contact> contacts)
{
	//TODO 0.8 :
	int separatorsCount = 0;
	QMenu *menu = new QMenu(parent);

	QMenu *actions = new QMenu(tr("More Actions..."));
	foreach (ActionDescription *actionDescription, BuddyListActions)
		if (actionDescription)
		{
			Action *action = actionDescription->createAction(actionDataSource, parent);
			actions->addAction(action);
			action->checkState();
		}
		else
			actions->addSeparator();

	foreach (ActionDescription *actionDescription, BuddiesContexMenu)
	{
		if (actionDescription)
		{

			Action *action = actionDescription->createAction(actionDataSource, parent);
			menu->addAction(action);
			action->checkState();
		}
		else
		{
			++separatorsCount;
			if (separatorsCount == 2)
				menu->addMenu(actions);

			menu->addSeparator();
		}
	}

	foreach (Contact contact, contacts)
	{
		if (!contact.contactAccount() || !contact.contactAccount().protocolHandler())
			continue;

		Account account = contact.contactAccount();
		ProtocolFactory *protocolFactory = account.protocolHandler()->protocolFactory();

		if (!account.protocolHandler()->protocolFactory() || !protocolFactory->protocolMenuManager())
			continue;

		QMenu *account_menu = menu->addMenu(account.accountIdentity().name());
		if (!protocolFactory->icon().isNull())
			account_menu->setIcon(protocolFactory->icon());

		if (protocolFactory->protocolMenuManager()->protocolActions(account, contact.ownerBuddy()).size() == 0)
			continue;

		foreach (ActionDescription *actionDescription, protocolFactory->protocolMenuManager()->protocolActions(account, contact.ownerBuddy()))
			if (actionDescription)
			{
				Action *action = actionDescription->createAction(actionDataSource, parent);
				account_menu->addAction(action);
				action->checkState();
			}
			else
				account_menu->addSeparator();
	}

	return menu;
}
