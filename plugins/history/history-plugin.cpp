/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "misc/path-conversion.h"

#include "buddy-history-delete-handler.h"
#include "history.h"

#include "history-plugin.h"

HistoryPlugin::~HistoryPlugin()
{
}

int HistoryPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	History::createInstance();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/history.ui"));
	MainConfigurationWindow::registerUiHandler(History::instance());

	BuddyHistoryDeleteHandler::createInstance();
	BuddyAdditionalDataDeleteHandlerManager::instance()->registerAdditionalDataDeleteHandler(BuddyHistoryDeleteHandler::instance());

	return 0;
}

void HistoryPlugin::done()
{
	BuddyAdditionalDataDeleteHandlerManager::instance()->unregisterAdditionalDataDeleteHandler(BuddyHistoryDeleteHandler::instance());
	BuddyHistoryDeleteHandler::destroyInstance();

	MainConfigurationWindow::unregisterUiHandler(History::instance());
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/history.ui"));
	History::destroyInstance();
}

Q_EXPORT_PLUGIN2(history, HistoryPlugin)