/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "mediaplayer.h"

#include "mediaplayer-plugin.h"

MediaplayerPlugin::~MediaplayerPlugin()
{
}

int MediaplayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MediaPlayer::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/mediaplayer.ui"));
	MainConfigurationWindow::registerUiHandler(MediaPlayer::instance());

	return 0;
}

void MediaplayerPlugin::done()
{
	MainConfigurationWindow::unregisterUiHandler(MediaPlayer::instance());
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/mediaplayer.ui"));

	MediaPlayer::destroyInstance();
}

Q_EXPORT_PLUGIN2(mediaplayer, MediaplayerPlugin)