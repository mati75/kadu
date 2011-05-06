/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugins/sound/sound-manager.h"

#include "phonon-player.h"

#include "phonon-plugin.h"

PhononPlugin::~PhononPlugin()
{
}

int PhononPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	PhononPlayer::createInstance();
	SoundManager::instance()->setPlayer(PhononPlayer::instance());

	return 0;
}

void PhononPlugin::done()
{
	SoundManager::instance()->setPlayer(0);
	PhononPlayer::destroyInstance();
}

Q_EXPORT_PLUGIN2(phonon_sound, PhononPlugin)