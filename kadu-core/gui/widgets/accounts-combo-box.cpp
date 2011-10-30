/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>

#include "accounts/filter/abstract-account-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "model/roles.h"
#include "model/model-chain.h"

#include "accounts-combo-box.h"

AccountsComboBox::AccountsComboBox(bool includeSelectAccount, ActionsProxyModel::ActionVisibility visibility, QWidget *parent) :
		ActionsComboBox(parent)
{
	Model = new AccountsModel(this);
	ProxyModel = new AccountsProxyModel(this);

	ModelChain *chain = new ModelChain(Model, this);
	chain->addProxyModel(ProxyModel);
	setUpModel(AccountRole, chain);

	if (includeSelectAccount)
		addBeforeAction(new QAction(tr(" - Select account - "), this), visibility);
}

AccountsComboBox::~AccountsComboBox()
{
}

void AccountsComboBox::setCurrentAccount(Account account)
{
	setCurrentValue(account);
}

Account AccountsComboBox::currentAccount()
{
	return currentValue().value<Account>();
}

void AccountsComboBox::setIncludeIdInDisplay(bool includeIdInDisplay)
{
	Model->setIncludeIdInDisplay(includeIdInDisplay);
}

void AccountsComboBox::valueChanged(const QVariant &value, const QVariant &previousValue)
{
	emit accountChanged(value.value<Account>(), previousValue.value<Account>());
}

bool AccountsComboBox::compare(const QVariant &left, const QVariant &right) const
{
	return left.value<Account>() == right.value<Account>();
}

void AccountsComboBox::addFilter(AbstractAccountFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void AccountsComboBox::removeFilter(AbstractAccountFilter *filter)
{
	ProxyModel->removeFilter(filter);
}