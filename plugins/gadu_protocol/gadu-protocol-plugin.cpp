/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>
#include <QtCrypto>

#include <libgadu.h>

#include "accounts/account-manager.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocols-manager.h"
#include "url-handlers/url-handler-manager.h"
#include "debug.h"

#include "helpers/gadu-importer.h"
#include "server/gadu-servers-manager.h"
#include "gadu-id-validator.h"
#include "gadu-protocol-factory.h"
#include "gadu-resolver.h"
#include "gadu-url-handler.h"

#include "gadu-protocol-plugin.h"

GaduProtocolPlugin::~GaduProtocolPlugin()
{
}

int GaduProtocolPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

#ifdef DEBUG_ENABLED
	// 8 bits for gadu debug
	gg_debug_level = debug_mask & 255;
#else
	gg_debug_level = 0;
#endif

	if (!gg_libgadu_check_feature(GG_LIBGADU_FEATURE_USERLIST100))
	{
		MessageDialog::exec(KaduIcon("dialog-error"), tr("Gadu-Gadu Protocol"),
				tr("Cannot load Gadu-Gadu Protocol plugin. Please compile libgadu with zlib support."));

		return -1;
	}

	gg_proxy_host = 0;
	gg_proxy_username = 0;
	gg_proxy_password = 0;

	gg_global_set_custom_resolver(gadu_resolver_start, gadu_resolver_cleanup);

	GaduIdValidator::createInstance();

	GaduServersManager::createInstance();

	GaduProtocolFactory::createInstance();

	ProtocolsManager::instance()->registerProtocolFactory(GaduProtocolFactory::instance());
	UrlHandlerManager::instance()->registerUrlHandler("Gadu", new GaduUrlHandler());

	GaduImporter::createInstance();

	if (AccountManager::instance()->allItems().isEmpty())
		GaduImporter::instance()->importAccounts();
	GaduImporter::instance()->importContacts();

	return 0;
}

void GaduProtocolPlugin::done()
{
	GaduImporter::destroyInstance();

	UrlHandlerManager::instance()->unregisterUrlHandler("Gadu");
	ProtocolsManager::instance()->unregisterProtocolFactory(GaduProtocolFactory::instance());

	GaduProtocolFactory::destroyInstance();

	GaduIdValidator::destroyInstance();
	GaduServersManager::destroyInstance();
}

Q_EXPORT_PLUGIN2(gadu_protocol, GaduProtocolPlugin)