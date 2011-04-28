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

#include "plugins/encryption_ng/encryption-manager.h"
#include "plugins/encryption_ng/encryption-provider-manager.h"
#include "exports.h"

#include "encryption-ng-simlite-key-generator.h"
#include "encryption-ng-simlite-key-importer.h"
#include "encryption-ng-simlite-provider.h"

#include "encryption-ng-simlite-plugin.h"

EngryptionNgSimlitePlugin::~EngryptionNgSimlitePlugin()
{
}

int EngryptionNgSimlitePlugin::init(bool firstLoad)
{
	if (firstLoad)
		EncryptioNgSimliteKeyImporter::createInstance();

	EncryptioNgSimliteKeyGenerator::createInstance();
	EncryptionManager::instance()->setGenerator(EncryptioNgSimliteKeyGenerator::instance());

	EncryptioNgSimliteProvider::createInstance();
	EncryptionProviderManager::instance()->registerProvider(EncryptioNgSimliteProvider::instance());

	return 0;
}

void EngryptionNgSimlitePlugin::done()
{
	EncryptionProviderManager::instance()->unregisterProvider(EncryptioNgSimliteProvider::instance());
	EncryptioNgSimliteProvider::destroyInstance();

	EncryptionManager::instance()->setGenerator(0);
	EncryptioNgSimliteKeyGenerator::destroyInstance();

	// it can work without createInstance too, so don't care about firstLoad here
	EncryptioNgSimliteKeyImporter::destroyInstance();
}

Q_EXPORT_PLUGIN2(encryption_ng_simlite, EngryptionNgSimlitePlugin)