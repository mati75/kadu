/*
 * %kadu copyright begin%
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

#include "desktop-aware-object.h"

#include "misc/misc.h"

KADU_AWARE_CLASS(DesktopAwareObject)

DesktopAwareObjectHelper *DesktopAwareObject::Helper = 0;

DesktopAwareObject::DesktopAwareObject(QWidget *widget) :
		AwareObject<DesktopAwareObject>(), Widget(widget)
{
	if (!Helper)
		Helper = new DesktopAwareObjectHelper();
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Call this method to call resolutionChanged in each DesktopAwareObject object.
 *
 * Calling this method results in calling resolutionChanged in each DesktopAwareObject
 * in system.
 */
void DesktopAwareObject::notifyDesktopModified()
{
	foreach (DesktopAwareObject *object, Objects)
		object->desktopModified();
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Default implementation.
 *
 * Default implementation moves the window to the closest available desktop.
 */
void DesktopAwareObject::desktopModified()
{
	if (!Widget)
		return;

	if (!Widget->isWindow())
		return;

	QRect geometry = properGeometry(Widget->geometry());
	if (geometry != Widget->geometry())
	{
		bool visible = Widget->isVisible();
		Widget->hide(); // workaround for window frame positioning
		Widget->setGeometry(geometry);
		if (visible)
			Widget->show();
	}
}