/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "gui/widgets/talkable-tree-view.h"
#include "icons/kadu-icon.h"

#include "talkable-delegate-configuration.h"

TalkableDelegateConfiguration::TalkableDelegateConfiguration(TalkableTreeView *listView) :
		ListView(listView), ShowIdentityName(true), ShowMessagePixmap(true), UseConfigurationColors(false)
{
	Q_ASSERT(ListView);

	DefaultAvatarSize = QSize(32, 32);
	MessagePixmap = KaduIcon("protocols/common/message").icon().pixmap(16, 16);

	configurationUpdated();
}

void TalkableDelegateConfiguration::configurationUpdated()
{
	Font = QFont(config_file.readFontEntry("Look", "UserboxFont"), ListView);
	BoldFont = Font;
	BoldFont.setBold(true);

	DescriptionFont = Font;
	DescriptionFont.setPointSize(Font.pointSize() - 2);

	ShowAvatars = config_file.readBoolEntry("Look", "ShowAvatars");
	AvatarBorder = config_file.readBoolEntry("Look", "AvatarBorder");
	AvatarGreyOut = config_file.readBoolEntry("Look", "AvatarGreyOut");
	AlignTop = config_file.readBoolEntry("Look", "AlignUserboxIconsTop");
	ShowBold = config_file.readBoolEntry("Look", "ShowBold");
	ShowDescription = config_file.readBoolEntry("Look", "ShowDesc");
	ShowMultiLineDescription = config_file.readBoolEntry("Look", "ShowMultilineDesc");
	DescriptionColor = config_file.readColorEntry("Look", "DescriptionColor");
	FontColor = config_file.readColorEntry("Look", "UserboxFgColor");

	ListView->scheduleDelayedItemsLayout();
}
