/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_SENDER_H
#define SMS_SENDER_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>
#include <QtScript/QScriptValue>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "http_client.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/token-acceptor.h"

#include "sms_exports.h"
#include "sms-gateway.h"

class QNetworkReply;

class TokenReader;

class SmsSender : public QObject, public TokenAcceptor
{
	Q_OBJECT

	QString GatewayId;
	QString Number;
	QString Message;
	QString Contact;
	QString Signature;

	TokenReader *MyTokenReader;
	QNetworkReply *TokenReply;

	QScriptValue TokenCallbackObject;
	QScriptValue TokenCallbackMethod;

	void fixNumber();
	bool validateNumber();
	bool validateSignature();

	void queryForGateway();
	void gatewaySelected();

	void sendSms();

private slots:
    void tokenImageDownloaded();

public:
	explicit SmsSender(const QString &number, const QString &gatewayId = QString::null, QObject *parent = 0);
	virtual ~SmsSender();

	void setContact(const QString& contact);
	void setSignature(const QString& signature);
	void sendMessage(const QString& message);

	void setTokenReader(TokenReader *tokenReader);

	void findGatewayForNumber(const QString &number);

	virtual void tokenRead(const QString &tokenValue);

public slots:
	void gatewayQueryDone(const QString &gatewayId);
	void readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod);

	void result();
	void failure(const QString &errorMessage);

signals:
	void finished(const QString &errorMessage);

};

#endif // SMS_SENDER_H
