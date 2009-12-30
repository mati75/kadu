/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>

#include "gui/widgets/buddy-contacts-table-delegate.h"
#include "gui/widgets/buddy-contacts-table-model.h"

#include "buddy-contacts-table.h"
#include <QHeaderView>

BuddyContactsTable::BuddyContactsTable(Buddy buddy, QWidget *parent) :
		QWidget(parent)
{
	Delegate = new BuddyContactsTableDelegate(this);
	Model = new BuddyContactsTableModel(buddy, this);

	createGui();
}

BuddyContactsTable::~BuddyContactsTable()
{
}

void BuddyContactsTable::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	View = new QTableView(this);
	View->setAlternatingRowColors(true);
	View->setDragEnabled(true);
	View->setEditTriggers(QAbstractItemView::AllEditTriggers);
	View->setItemDelegate(Delegate);
	View->setModel(Model);
	View->setVerticalHeader(0);

	View->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	View->horizontalHeader()->setStretchLastSection(true);

	layout->addWidget(View);

	QWidget *buttons = new QWidget(View);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);

	QPushButton *moveUpButton = new QPushButton(tr("Move up"), buttons);
	connect(moveUpButton, SIGNAL(clicked(bool)), this, SLOT(moveUpClicked()));
	buttonsLayout->addWidget(moveUpButton);

	QPushButton *moveDownButton = new QPushButton(tr("Move down"), buttons);
	connect(moveDownButton, SIGNAL(clicked(bool)), this, SLOT(moveDownClicked()));
	buttonsLayout->addWidget(moveDownButton);

	QPushButton *addContactButton = new QPushButton(tr("Add contact"), buttons);
	connect(addContactButton, SIGNAL(clicked(bool)), this, SLOT(addClicked()));
	buttonsLayout->addWidget(addContactButton);

	QPushButton *detachContactButton = new QPushButton(tr("Detach contact"), buttons);
	connect(detachContactButton, SIGNAL(clicked(bool)), this, SLOT(detachClicked()));
	buttonsLayout->addWidget(detachContactButton);

	QPushButton *removeContactButton = new QPushButton(tr("Remove contact"), buttons);
	connect(removeContactButton, SIGNAL(clicked(bool)), this, SLOT(removeClicked()));
	buttonsLayout->addWidget(removeContactButton);

	layout->addWidget(buttons);
}

void BuddyContactsTable::save()
{
	Model->save();
}

void BuddyContactsTable::moveUpClicked()
{
}

void BuddyContactsTable::moveDownClicked()
{
}

void BuddyContactsTable::addClicked()
{
	Model->insertRow(Model->rowCount());
}

void BuddyContactsTable::detachClicked()
{
}

void BuddyContactsTable::removeClicked()
{
}
