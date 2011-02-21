/*
 * %kadu copyright begin%
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
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

#include "debug.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "image-link.h"

extern "C" KADU_EXPORT int imagelink_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ImageLink::createInstance();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/image-link.ui"));

	return 0;
}

extern "C" KADU_EXPORT void imagelink_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/image-link.ui"));
	ImageLink::destroyInstance();
}