/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef NETWORK_ACCESS_MANAGER_WRAPPER_H
#define NETWORK_ACCESS_MANAGER_WRAPPER_H

#include <QtCore/QMap>
#include <QtNetwork/QNetworkAccessManager>
#include <QtScript/QScriptValue>

#include "configuration/configuration-aware-object.h"

class NetworkAccessManagerWrapper : public QNetworkAccessManager, ConfigurationAwareObject
{
	Q_OBJECT

	QScriptEngine *Engine;
	bool Utf8;
	QMap<QByteArray, QByteArray> Headers;

protected:
	void configurationUpdated();

public:
	explicit NetworkAccessManagerWrapper(QScriptEngine *engine, QObject *parent = 0);
	virtual ~NetworkAccessManagerWrapper();

public slots:
	void setUtf8(bool utf8) { Utf8 = utf8; }
	void setHeader(const QString &headerName, const QString &headerValue);
	void clearHeaders();

	QScriptValue get(const QString &url);
	QScriptValue post(const QString &url, const QString &data);

};

#endif // NETWORK_ACCESS_MANAGER_WRAPPER_H