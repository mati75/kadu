/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "configuration/configuration-file.h"
#include "misc/error.h"
#include "protocols/services/chat-image.h"
#include "debug.h"

#include "helpers/gadu-formatter.h"
#include "helpers/gadu-protocol-helper.h"
#include "server/gadu-connection.h"
#include "gadu-account-details.h"

#include "gadu-chat-image-service.h"

GaduChatImageService::GaduChatImageService(Account account, QObject *parent) :
		ChatImageService(account, parent), CurrentMinuteSendImageRequests(0)
{
}

GaduChatImageService::~GaduChatImageService()
{
}

void GaduChatImageService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

ChatImage GaduChatImageService::saveImage(const ChatImageKey &key, const char *data)
{
	ChatImage result;

	QString path = ChatImageService::imagesPath();
	if (!QFileInfo(path).isDir() && !QDir().mkdir(path))
	{
		kdebugm(KDEBUG_INFO, "Failed creating directory: %s\n", qPrintable(path));
		return result;
	}

	QFile file(path + key.toString());
	if (!file.open(QIODevice::WriteOnly))
		return result;

	file.write(data, key.size());
	file.close();

	QFileInfo fileInfo(file.fileName());

	result.setLocalFileName(fileInfo.fileName());
	result.setSize(key.size());
	result.setCrc32(key.crc32());

	return result;
}

QByteArray GaduChatImageService::loadFileContent(const QString &localFileName)
{
	QFile file(localFileName);

	if (!file.open(QIODevice::ReadOnly))
		return QByteArray();

	QByteArray result = file.readAll();
	file.close();

	if (result.size() != file.size())
		return QByteArray();

	return result;
}

void GaduChatImageService::handleEventImageRequest(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(e->event.image_request.sender).arg(e->event.image_request.size).arg(e->event.image_request.crc32)));

	if (!Connection || !Connection.data()->hasSession())
		return;

	ChatImageKey key(e->event.image_request.size, e->event.image_request.crc32);
	if (!ChatImages.contains(key))
		return;

	ChatImage chatImage = ChatImages.value(key);
	QByteArray content = loadFileContent(chatImage.localFileName());
	if (content.isEmpty())
		return;

	Connection.data()->beginWrite();
	gg_image_reply(Connection.data()->session(), e->event.image_request.sender, chatImage.localFileName().toUtf8().constData(),
			content.constData(), content.length());
	Connection.data()->endWrite();
}

void GaduChatImageService::handleEventImageReply(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
			.arg(e->event.image_reply.sender).arg(e->event.image_reply.size)
			.arg(e->event.image_reply.crc32).arg(e->event.image_reply.filename)));

	ChatImageKey key(e->event.image_reply.size, e->event.image_reply.crc32);
	ChatImage image = saveImage(key, e->event.image_reply.image);

	if (image.isNull())
		return;

	emit chatImageAvailable(key, image);
}

void GaduChatImageService::requestChatImage(const QString &id, const ChatImageKey &imageKey)
{
	if (!Connection || !Connection.data()->hasSession())
		return;

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());

	if (id.isEmpty() || (CurrentMinuteSendImageRequests > gaduAccountDetails->maximumImageRequests()))
		return;

	CurrentMinuteSendImageRequests++;

	Connection.data()->beginWrite();
	gg_image_request(Connection.data()->session(), id.toUInt(), imageKey.size(), imageKey.crc32());
	Connection.data()->endWrite();
}

ChatImage GaduChatImageService::createChatImage(const QString &localFileName)
{
	QByteArray content = loadFileContent(localFileName);

	ChatImage result;

	result.setLocalFileName(localFileName);
	result.setSize(content.size());

	if (!content.isEmpty())
		result.setCrc32(gg_crc32(0, (const unsigned char*)content.constData(), content.length()));

	ChatImages.insert(ChatImageKey(result.size(), result.crc32()), result);

	return result;
}

Error GaduChatImageService::checkImageSize(qint64 size) const
{
	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!details || !details->chatImageSizeWarning() || size <= RECOMMENDED_MAXIMUM_SIZE)
		return Error(NoError, QString());

	QString message = tr("This image has %1 KiB and exceeds recommended maximum size of %2 KiB.") + '\n' + tr("Do you really want to send this image?");
	message = message.arg((size + 1023) / 1024).arg(RECOMMENDED_MAXIMUM_SIZE / 1024);

	return Error(ErrorLow, message);
}

