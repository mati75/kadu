/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SUBSCRIPTION_WINDOW_H
#define SUBSCRIPTION_WINDOW_H

#include <QtGui/QComboBox>
#include <QtGui/QDialog>

#include "contacts/contact.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

class KADUAPI SubscriptionWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	Contact CurrentContact;

private slots:
	void accepted();
	void allowed();
	void rejected();

public:
	static void getSubscription(Contact contact, QObject* receiver, const char* slot);

	explicit SubscriptionWindow(Contact contact, QWidget* parent = 0);
	virtual ~SubscriptionWindow();

signals:
	void requestConsidered(Contact contact, bool accepted);

};

#endif // SUBSCRIPTION_WINDOW_H
