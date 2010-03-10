/*
 * %kadu copyright begin%
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

#include "gui/windows/sms-image-dialog.h"

#include "sms-token-reader.h"
#include <stdio.h>
SmsTokenReader::SmsTokenReader(QObject *parent) :
		QObject(parent)
{
}

SmsTokenReader::~SmsTokenReader()
{
}

void SmsTokenReader::readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod)
{
	printf("Read token: %s\n", qPrintable(tokenImageUrl));
	SmsImageDialog *smsImageDialog = new SmsImageDialog(tokenImageUrl, callbackObject, callbackMethod, 0);
	smsImageDialog->exec();
}
