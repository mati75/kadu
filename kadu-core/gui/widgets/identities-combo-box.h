/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IDENTITIES_COMBO_BOX_H
#define IDENTITIES_COMBO_BOX_H

#include "identities/identity.h"
#include "gui/widgets/actions-combo-box.h"
#include "exports.h"

class KADUAPI IdentitiesComboBox : public ActionsComboBox
{
	Q_OBJECT

	bool InActivatedSlot;

	QAction *CreateNewIdentityAction;

private slots:
	void createNewIdentity();

protected:
	virtual void valueChanged(const QVariant &value, const QVariant &previousValue);
	virtual bool compare(const QVariant &left, const QVariant &right) const;

public:
	explicit IdentitiesComboBox(bool includeSelectIdentity, QWidget *parent = 0);
	virtual ~IdentitiesComboBox();

	void setCurrentIdentity(Identity identity);
	Identity currentIdentity();

signals:
	void identityChanged(Identity identity);

};

#endif // IDENTITIES_COMBO_BOX_H