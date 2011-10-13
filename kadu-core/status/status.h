/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef STATUS_H
#define STATUS_H

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "status/status-type.h"
#include "status/status-type-group.h"

#include "exports.h"

class KADUAPI Status
{
	StatusType Type;
	StatusTypeGroup Group;
	QString Description;
	QString DisplayName;

public:
	Status(StatusType statusType = StatusTypeOffline, const QString &description = QString());
	Status(const Status &copyme);
	~Status();

	StatusType type() const { return Type; }
	void setType(StatusType type);

	StatusTypeGroup group() const { return Group; }

	const QString & displayName() const { return DisplayName; }

	const QString & description() const { return Description; }
	void setDescription(const QString &description) { Description = description; }
	bool hasDescription() const;

	bool isDisconnected() const;

	bool operator < (const Status &compare) const;
	bool operator == (const Status &compare) const;
	bool operator != (const Status &compare) const;

};

Q_DECLARE_METATYPE(Status)

#endif // STATUS_H
