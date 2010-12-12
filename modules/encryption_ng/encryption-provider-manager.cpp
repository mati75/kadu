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

#include "decryptor.h"
#include "encryption-provider.h"
#include "encryptor.h"

#include "encryption-provider-manager.h"

EncryptionProviderManager * EncryptionProviderManager::Instance = 0;

void EncryptionProviderManager::createInstance()
{
	Instance = new EncryptionProviderManager();
}

void EncryptionProviderManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionProviderManager::EncryptionProviderManager()
{
}

EncryptionProviderManager::~EncryptionProviderManager()
{
}

void EncryptionProviderManager::registerProvider(EncryptionProvider *provider)
{
	Providers.append(provider);
}

void EncryptionProviderManager::unregisterProvider(EncryptionProvider *provider)
{
	Providers.removeAll(provider);
}

Decryptor * EncryptionProviderManager::decryptor(const Chat &chat)
{
	foreach (EncryptionProvider *provider, Providers)
	{
		Decryptor *result = provider->decryptor(chat);
		if (result)
			return result;
	}

	return 0;
}

Encryptor * EncryptionProviderManager::encryptor(const Chat &chat)
{
	foreach (EncryptionProvider *provider, Providers)
	{
		Encryptor *result = provider->encryptor(chat);
		if (result)
			return result;
	}

	return 0;
}
