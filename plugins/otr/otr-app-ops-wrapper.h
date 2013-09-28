/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_APP_OPS_WRAPPER_H
#define OTR_APP_OPS_WRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

#include "chat/chat.h"

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class Contact;

class OtrOpData;

class OtrAppOpsWrapper : public QObject
{
	Q_OBJECT

	friend const char * kadu_otr_resent_msg_prefix(void *, ConnContext *);
	friend void kadu_otr_handle_msg_event(void *, OtrlMessageEvent, ConnContext *, const char *, gcry_error_t);

	OtrlMessageAppOps Ops;

	QString resentMessagePrefix() const;
	void handleMsgEvent(OtrOpData *opData, OtrlMessageEvent event, const QString &message, gcry_error_t errorCode) const;
	QString messageString(OtrlMessageEvent event, const QString &message, gcry_error_t errorCode, const QString &peerDisplay) const;
	QString gpgErrorString(gcry_error_t errorCode) const;

public:
	explicit OtrAppOpsWrapper();
	virtual ~OtrAppOpsWrapper();

	const OtrlMessageAppOps * ops() const;

};

#endif // OTR_APP_OPS_WRAPPER_H
