/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QResizeEvent>

#include "icons/kadu-icon.h"

#include "wait-overlay.h"

WaitOverlay::WaitOverlay(QWidget *parent) :
		QLabel(parent)
{
	setAlignment(Qt::AlignCenter);
	setMovie(new QMovie(KaduIcon("kadu_icons/64x64/please-wait.gif").fullPath(), QByteArray(), this));
	setStyleSheet("background-color: rgba(127, 127, 127, 127)");
	movie()->start();

	if (parent)
	{
		move(0, 0);
		resize(parent->size());
		parent->installEventFilter(this);
	}

	show();
	raise();
}

WaitOverlay::~WaitOverlay()
{
}

bool WaitOverlay::eventFilter(QObject *object, QEvent *event)
{
	if (QEvent::Resize != event->type())
		return QWidget::eventFilter(object, event);

	QResizeEvent *resizeEvent = static_cast<QResizeEvent *>(event);
	resize(resizeEvent->size());

	return QWidget::eventFilter(object, event);
}
