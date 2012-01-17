/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QApplication>

#include "buddies/buddy.h"

#include "history.h"

#include "buddy-history-delete-handler.h"

BuddyHistoryDeleteHandler * BuddyHistoryDeleteHandler::Instance = 0;

void BuddyHistoryDeleteHandler::createInstance()
{
	if (!Instance)
		Instance = new BuddyHistoryDeleteHandler();
}

void BuddyHistoryDeleteHandler::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

BuddyHistoryDeleteHandler * BuddyHistoryDeleteHandler::instance()
{
	return Instance;
}

BuddyHistoryDeleteHandler::BuddyHistoryDeleteHandler()
{
}

BuddyHistoryDeleteHandler::~BuddyHistoryDeleteHandler()
{
}

QString BuddyHistoryDeleteHandler::name()
{
	return QLatin1String("history-delete-handler");
}

QString BuddyHistoryDeleteHandler::displayName()
{
	return QApplication::tr("Chat history");
}

void BuddyHistoryDeleteHandler::deleteBuddyAdditionalData(Buddy buddy)
{
	History::instance()->deleteHistory(buddy);
}