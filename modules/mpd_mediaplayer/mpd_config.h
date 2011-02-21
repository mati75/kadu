/*
 * %kadu copyright begin%
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef MPD_CONFIG
#define MPD_CONFIG

#include "gui/windows/main-configuration-window.h"

class MPDConfig : public ConfigurationAwareObject
{
	void createDefaultConfiguration();

protected:
	 void configurationUpdated();

public:
	MPDConfig();
	~MPDConfig();

	QString host;
	QString port;
	QString timeout;
};

#endif // MPD_CONFIG