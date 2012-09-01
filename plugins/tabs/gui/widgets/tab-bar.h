/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TAB_BAR_H
#define TAB_BAR_H

#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtCore/QPoint>
#include <QtGui/QTabBar>


class TabBar: public QTabBar
{
	Q_OBJECT

protected:
	/**
	* Metoda wywoływana w momencie kliknięcia na tabbarze
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mousePressEvent(QMouseEvent *e);

	/**
	* Metoda wywoływana w momencie puszczenia przycisku myszy
	* po kliknięciu
	* Tu odpowiada za zamknięcie karty środokowym przyciskiem myszy
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mouseReleaseEvent(QMouseEvent *e);

	/**
	* Metoda wywoływana w momencie dwukliku myszy
	* Tu powoduje otwarcie okna openChatWith;
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

public:
	explicit TabBar(QWidget *parent = 0);

signals:
	void contextMenu(int id, const QPoint &pos);
	void mouseDoubleClickEventSignal(QMouseEvent *e);

};

#endif // TAB_BAR_H