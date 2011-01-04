/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
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

#ifndef ECHO_H
#define ECHO_H

#include <time.h>

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"

class QString;

class Chat;
class Contact;

class Echo : public QObject, AccountsAwareObject
{
	Q_OBJECT

	static Echo *Instance;

	explicit Echo(QObject *parent = 0);
	virtual ~Echo();

private slots:
	void filterIncomingMessage(Chat chat, Contact sender, QString &message, time_t time, bool &ignore);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static Echo * instance();
	static void createInstance();
	static void destroyInstance();

};

// for MOC
#include <QtCore/QString>
#include "chat/chat.h"
#include "contacts/contact.h"

#endif // ECHO_H
