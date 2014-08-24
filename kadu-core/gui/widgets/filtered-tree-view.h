/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef FILTERED_TREE_VIEW_H
#define FILTERED_TREE_VIEW_H

#include <QtGui/QWidget>

#include "exports.h"

class QAbstractItemView;
class QVBoxLayout;

class FilterWidget;

class KADUAPI FilteredTreeView : public QWidget
{
	Q_OBJECT

public:
	enum FilterPosition
	{
		FilterAtTop,
		FilterAtBottom
	};

private:
	FilterPosition CurrentFilterPosition;

	QVBoxLayout *Layout;
	FilterWidget *NameFilterWidget;
	QAbstractItemView *View;

	void removeView();
	void insertView();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	static bool shouldEventGoToFilter(QKeyEvent *event);

	explicit FilteredTreeView(FilterPosition filterPosition, QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual ~FilteredTreeView();

	FilterWidget * filterWidget() { return NameFilterWidget; }

	void setPosition(FilterPosition filterPosition);
	void setView(QAbstractItemView *view);

signals:
	void filterChanged(const QString &filterText);

};

#endif // FILTERED_TREE_VIEW_H
