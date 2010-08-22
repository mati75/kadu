/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QAbstractItemView>

#include "accounts/account.h"
#include "buddies/model/abstract-buddies-model.h"
#include "gui/widgets/buddies-list-view-item-painter.h"

#include "buddies-list-view-delegate.h"

BuddiesListViewDelegate::BuddiesListViewDelegate(QObject *parent) :
		QItemDelegate(parent), Model(0), Configuration(parent)
{
	triggerAllAccountsRegistered();
}

BuddiesListViewDelegate::~BuddiesListViewDelegate()
{
	triggerAllAccountsUnregistered();
}

void BuddiesListViewDelegate::setModel(AbstractBuddiesModel *model)
{
	Model = model;
	QAbstractItemModel *itemModel = dynamic_cast<QAbstractItemModel *>(Model);
	if (itemModel)
		connect(itemModel, SIGNAL(destroyed(QObject *)), this, SLOT(modelDestroyed()));
}

void BuddiesListViewDelegate::accountRegistered(Account account)
{
	connect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(buddyStatusChanged(Contact, Status)));
}

void BuddiesListViewDelegate::accountUnregistered(Account account)
{
	disconnect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(buddyStatusChanged(Contact, Status)));
}

void BuddiesListViewDelegate::buddyStatusChanged(Contact contact, Status oldStatus)
{
	Q_UNUSED(oldStatus)

	if (Model)
		emit sizeHintChanged(Model->buddyIndex(contact.ownerBuddy()));
}

void BuddiesListViewDelegate::modelDestroyed()
{
	Model = 0;
}

void BuddiesListViewDelegate::setShowAccountName(bool showAccountName)
{
	Configuration.setShowAccountName(showAccountName);
}

QStyleOptionViewItemV4 BuddiesListViewDelegate::getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const
{
	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;

	return opt;
}

QSize BuddiesListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize baseSizeHint = QItemDelegate::sizeHint(option, index);

	BuddiesListViewItemPainter buddyPainter(Configuration, getOptions(index, option), index);
	return QSize(0, buddyPainter.height());
}

void BuddiesListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 options = getOptions(index, option);

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(options.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &options, painter, widget);

	BuddiesListViewItemPainter buddyPainter(Configuration, options, index);
	buddyPainter.paint(painter);
/*
		// TODO: 0.6.6
/ *
		if (User.protocolData("Gadu", "Blocking").toBool())
			painter->setPen(QColor(255, 0, 0));
		else if (IgnoredManager::isIgnored(UserListElements(users)))
			painter->setPen(QColor(192, 192, 0));
		else if (config_file.readBoolEntry("General", "PrivateStatus") && User.protocolData("Gadu", "OfflineTo").toBool())
			painter->setPen(QColor(128, 128, 128));
* /
//		if (User.data("HideDescription").toString() != "true")


		}
	}

*/
}
