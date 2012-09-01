/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-image-key.h"

ChatImageKey::ChatImageKey(quint32 size, quint32 crc32) :
		QPair<quint32, quint32>(size, crc32)
{
}

bool ChatImageKey::isNull() const
{
	return 0 == first && 0 == second;
}

QString ChatImageKey::toString() const
{
	return QString("chat-image-%1-%2").arg(first).arg(second);
}

quint32 ChatImageKey::size() const
{
	return first;
}

quint32 ChatImageKey::crc32() const
{
	return second;
}