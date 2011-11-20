/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtGui/QLineEdit>

#include "buddies/buddy-manager.h"
#include "buddies/filter/anonymous-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model-proxy.h"
#include "buddies/model/buddies-model.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/model-chain.h"
#include "model/roles.h"

#include "select-buddy-popup.h"

SelectBuddyPopup::SelectBuddyPopup(QWidget *parent) :
		FilteredTreeView(FilterAtBottom, parent)
{
	setWindowFlags(Qt::Popup);

	View = new TalkableTreeView(this);
	setTreeView(View);

	ModelChain *chain = new ModelChain(new BuddiesModel(this), this);
	ProxyModel = new BuddiesModelProxy(chain);
	ProxyModel->setSortByStatus(false);

	AnonymousBuddyFilter *anonymousFilter = new AnonymousBuddyFilter(ProxyModel);
	anonymousFilter->setEnabled(true);
	ProxyModel->addFilter(anonymousFilter);

	BuddyNameFilter *nameFilter = new BuddyNameFilter(ProxyModel);
	connect(this, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));

	ProxyModel->addFilter(nameFilter);
	chain->addProxyModel(ProxyModel);

	connect(View, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
	connect(View, SIGNAL(talkableActivated(Talkable)), this, SLOT(talkableActivated(Talkable)));

	View->setItemsExpandable(false);
	View->setChain(chain);
	View->setRootIsDecorated(false);
	View->setShowAccountName(false);
	View->setSelectionMode(QAbstractItemView::SingleSelection);

	nameFilter->setIgnoreNextFilters(false);
}

SelectBuddyPopup::~SelectBuddyPopup()
{
}

void SelectBuddyPopup::show(Buddy buddy)
{
#ifndef Q_WS_MAEMO_5
	filterWidget()->setFocus();
#endif

	const QModelIndexList &indexes = View->chain()->indexListForValue(buddy);
	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	View->setCurrentIndex(index);
	FilteredTreeView::show();
}

void SelectBuddyPopup::itemClicked(const QModelIndex &index)
{
	close();

	Buddy buddy = index.data(BuddyRole).value<Buddy>();
	if (!buddy)
		return;

	emit buddySelected(buddy);
}

void SelectBuddyPopup::talkableActivated(const Talkable &talkable)
{
	const Buddy &buddy = talkable.buddy();
	if (buddy)
	{
		emit buddySelected(buddy);
		close();
	}
}

void SelectBuddyPopup::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void SelectBuddyPopup::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
}
