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

#ifndef NETWORK_MANAGER_QT_H
#define NETWORK_MANAGER_QT_H

#include <QtCore/QObject>

#include "exports.h"
#include "network-manager.h"

class QNetworkConfigurationManager;

/**
 * @addtogroup Network
 * @{
 */

/**
 * @class NetworkManagerQt
 * @author Rafał 'Vogel' Malinowski
 * @author Piotr 'ultr' Dąbrowski
 * @short Class responsible for network online-offline notifications using QNetworkConfigurationManager.
 * @see NetworkAwareObject
 *
 * This class provides information about network availability. To check if network is available use isOnline()
 * method. Any object can connect to online() and offline() signals to get real-time notifications.
 *
 * This class uses QNetworkConfigurationManager to get network availability information. This is only possible
 * when QNetworkConfigurationManager::capabilities() has flag QNetworkConfigurationManager::CanStartAndStopInterfaces.
 * Otherwise constant online state will be assumed.
 */
class KADUAPI NetworkManagerQt : public NetworkManager
{
	Q_OBJECT

	QNetworkConfigurationManager *ConfigurationManager;
	bool HasValidCapabilities;

public:
	NetworkManagerQt();
	virtual ~NetworkManagerQt();

	virtual bool isOnline();

};

/**
 * @}
 */

#endif // NETWORK_MANAGER_QT_H