/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "network/network-aware-object.h"
#include "kadu-network-config.h"

#include "network-manager.h"

#include NETWORK_IMPLEMENTATION_INCLUDE

NetworkManager *NetworkManager::Instance = 0;

NetworkManager * NetworkManager::instance()
{
	if (!Instance)
		Instance = new NETWORK_IMPLEMENTATION_CLASS_NAME();

	return Instance;
}

NetworkManager::NetworkManager()
{
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::onlineStateChanged(bool isOnline)
{
	NetworkAwareObject::notifyOnlineStateChanged(isOnline);
	if (isOnline)
		emit online();
	else
		emit offline();
}
