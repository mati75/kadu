/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenu>

#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "chat/chat-list-mime-data-helper.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/group-edit-window.h"
#include "gui/windows/kadu-dialog.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "talkable/filter/group-filter.h"

#include "debug.h"

#include "group-filter-tab-data.h"
#include "group-tab-bar.h"

GroupTabBar::GroupTabBar(QWidget *parent) :
		QTabBar(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

 	setAcceptDrops(true);
	setDrawBase(false);
	setMovable(true);

	setShape(QTabBar::RoundedWest);
	setIconSize(QSize(16, 16));

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChangedSlot(int)));

	connect(GroupManager::instance(), SIGNAL(groupAdded(Group)), this, SLOT(addGroup(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)), this, SLOT(removeGroup(Group)));
	connect(GroupManager::instance(), SIGNAL(groupUpdated(Group)), this, SLOT(updateGroup(Group)));
}

GroupTabBar::~GroupTabBar()
{
}

void GroupTabBar::setInitialConfiguration(GroupTabBarConfiguration configuration)
{
	foreach (const auto &groupFilter, configuration.groupFilters())
		insertGroupFilter(count(), groupFilter);

	foreach (const Group &group, GroupManager::instance()->items())
		addGroup(group);

	setConfiguration(configuration);
}

void GroupTabBar::setConfiguration(GroupTabBarConfiguration configuration)
{
	Configuration = configuration;

	setVisible(Configuration.displayGroupTabs());

	if (Configuration.showGroupTabEverybody())
		insertGroupFilter(0, GroupFilter(GroupFilterEverybody));
	else
		removeGroupFilter(GroupFilter(GroupFilterEverybody));

	updateUngrouppedTab();

	if (Configuration.displayGroupTabs())
	{
		if (currentIndex() == configuration.currentGroupTab())
			currentChangedSlot(configuration.currentGroupTab());
		else
			setCurrentIndex(configuration.currentGroupTab());
	}
	else
	{
		CurrentGroupFilter = GroupFilter{GroupFilterEverybody};
		emit currentGroupFilterChanged(CurrentGroupFilter);
	}
}

void GroupTabBar::updateUngrouppedTab()
{
	if (shouldShowUngrouppedTab())
		insertGroupFilter(count(), GroupFilter(GroupFilterUngroupped));
	else
		removeGroupFilter(GroupFilter(GroupFilterUngroupped));
}

bool GroupTabBar::shouldShowUngrouppedTab() const
{
	return Configuration.alwaysShowGroupTabUngroupped()
			? true
			: !Configuration.showGroupTabEverybody();
}

GroupTabBarConfiguration GroupTabBar::configuration()
{
	Configuration.setGroupFilters(groupFilters()); // update only if needed
	return Configuration;
}

Group GroupTabBar::groupAt(int index) const
{
	return groupFilterAt(index).group();
}

GroupFilter GroupTabBar::groupFilter() const
{
	if (Configuration.displayGroupTabs())
		return groupFilterAt(currentIndex());
	else
		return GroupFilter{GroupFilterEverybody};
}

GroupFilter GroupTabBar::groupFilterAt(int index) const
{
	return tabData(index).value<GroupFilter>();
}

QVector<GroupFilter> GroupTabBar::groupFilters() const
{
	auto tabsCount = count();
	auto result = QVector<GroupFilter>(tabsCount);

	for (auto i = 0; i < tabsCount; i++)
		result.append(tabData(i).value<GroupFilter>());

	return result;
}

void GroupTabBar::currentChangedSlot(int index)
{
	Configuration.setCurrentGroupTab(index);
	if (Configuration.displayGroupTabs())
		CurrentGroupFilter = groupFilterAt(index);
	else
		CurrentGroupFilter = GroupFilter{GroupFilterEverybody};

	emit currentGroupFilterChanged(CurrentGroupFilter);

}

void GroupTabBar::contextMenuEvent(QContextMenuEvent *event)
{
	auto group = groupAt(tabAt(event->pos()));

	QMenu menu;

	QAction *addBuddyAction = menu.addAction(tr("Add Buddy"), this, SLOT(addBuddy()));
	addBuddyAction->setEnabled(group);
	addBuddyAction->setData(group);

	menu.addAction(tr("Add Group"), this, SLOT(createNewGroup()));

	QAction *deleteGroupAction = menu.addAction(tr("Delete Group"), this, SLOT(deleteGroup()));
	deleteGroupAction->setEnabled(group);
	deleteGroupAction->setData(group);

	menu.addSeparator();

	QAction *propertiesAction = menu.addAction(tr("Properties"), this, SLOT(groupProperties()));
	propertiesAction->setEnabled(group);
	propertiesAction->setData(group);

	menu.exec(event->globalPos());
}

void GroupTabBar::dragEnterEvent(QDragEnterEvent *event)
{
	QTabBar::dragEnterEvent(event);

	if (event->mimeData()->hasFormat("application/x-kadu-buddy-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}

	if (event->mimeData()->hasFormat("application/x-kadu-chat-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}
}

void GroupTabBar::dragMoveEvent(QDragMoveEvent *event)
{
	QTabBar::dragMoveEvent(event);

	if (event->mimeData()->hasFormat("application/x-kadu-buddy-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}

	if (event->mimeData()->hasFormat("application/x-kadu-chat-list"))
	{
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}
}

void GroupTabBar::dropEvent(QDropEvent *event)
{
	QTabBar::dropEvent(event);

	if (!event->mimeData()->hasFormat("application/x-kadu-buddy-list") &&
	        !event->mimeData()->hasFormat("application/x-kadu-chat-list"))
	{
		event->ignore();
		return;
	}

	event->acceptProposedAction();

	BuddyList buddies = BuddyListMimeDataHelper::fromMimeData(event->mimeData());
	QList<Chat> chats = ChatListMimeDataHelper::fromMimeData(event->mimeData());

	QApplication::setOverrideCursor(Qt::ArrowCursor);
	int tabIndex = tabAt(event->pos());

	if (tabIndex == -1)
	{
		bool ok;
		QString newGroupName;
		do
		{
			newGroupName = QInputDialog::getText(this, tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString(), &ok);

			if (!ok)
			{
				QApplication::restoreOverrideCursor();
				return;
			}

			ok = GroupManager::instance()->acceptableGroupName(newGroupName);
		}
		while (!ok);

		Group group = GroupManager::instance()->byName(newGroupName);

		foreach (const Buddy &buddy, buddies)
			buddy.addToGroup(group);
		foreach (const Chat &chat, chats)
			chat.addToGroup(group);

		QApplication::restoreOverrideCursor();

		return;
	}

	auto clickedGroup = groupAt(tabIndex);

	DNDBuddies = buddies;
	DNDChats = chats;

	if (clickedGroup)
	{
		QMenu menu;
		if (CurrentGroupFilter.filterType() == GroupFilterRegular)
			menu.addAction(tr("Move to group %1").arg(clickedGroup.name()), this, SLOT(moveToGroup()))->setData(clickedGroup);
		menu.addAction(tr("Add to group %1").arg(clickedGroup.name()), this, SLOT(addToGroup()))->setData(clickedGroup);
		menu.exec(QCursor::pos());
	}

	QApplication::restoreOverrideCursor();

	kdebugf2();
}

void GroupTabBar::addBuddy()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	AddBuddyWindow *addBuddyWindow = new AddBuddyWindow(Core::instance()->kaduWindow());
	addBuddyWindow->setGroup(action->data().value<Group>());
	addBuddyWindow->show();
}

void GroupTabBar::deleteGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();
	if (!group)
		return;

	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-warning"),
						      tr("Delete group"),
						      tr("Group <i>%0</i> will be deleted, but without buddies. Are you sure?").arg(group.name()),
						      Core::instance()->kaduWindow());
	dialog->addButton(QMessageBox::Yes, tr("Delete group"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));

	if (group && dialog->ask())
		GroupManager::instance()->removeItem(group);
}

void GroupTabBar::createNewGroup()
{
	auto editWindow = new GroupEditWindow{GroupManager::instance(), Application::instance()->configuration()->deprecatedApi(), Group::null, Core::instance()->kaduWindow()};
	editWindow->show();
}

void GroupTabBar::groupProperties()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	auto group = action->data().value<Group>();
	if (!group)
		return;

	auto editWindow = new GroupEditWindow{GroupManager::instance(), Application::instance()->configuration()->deprecatedApi(), group, Core::instance()->kaduWindow()};
	editWindow->show();
}

void GroupTabBar::addToGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &group = action->data().value<Group>();

	foreach (const Buddy &buddy, DNDBuddies)
		buddy.addToGroup(group);
	foreach (const Chat &chat, DNDChats)
		chat.addToGroup(group);
}

void GroupTabBar::moveToGroup()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	const Group &removeFromGroup = GroupManager::instance()->byUuid(tabData(currentIndex()).toString());
	const Group &group = action->data().value<Group>();

	foreach (const Buddy &buddy, DNDBuddies)
	{
		buddy.removeFromGroup(removeFromGroup);
		buddy.addToGroup(group);
	}

	foreach (const Chat &chat, DNDChats)
	{
		chat.removeFromGroup(removeFromGroup);
		chat.addToGroup(group);
	}
}

int GroupTabBar::indexOf(GroupFilter groupFilter)
{
	auto tabsCount = count();
	for (auto i = 0; i < tabsCount; i++)
		if (groupFilter == tabData(i).value<GroupFilter>())
			return i;
	return -1;
}

void GroupTabBar::addGroup(Group group)
{
	insertGroupFilter(count(), GroupFilter(group));
}

void GroupTabBar::insertGroupFilter(int index, GroupFilter groupFilter)
{
	if (indexOf(groupFilter) >= 0)
		return;

	auto newTabIndex = insertTab(index, QString());
	setTabData(newTabIndex, QVariant::fromValue(groupFilter));
	updateTabData(newTabIndex, groupFilter);
}

void GroupTabBar::removeGroup(Group group)
{
	removeGroupFilter(GroupFilter(group));
}

void GroupTabBar::removeGroupFilter(GroupFilter groupFilter)
{
	auto index = indexOf(groupFilter);
	if (index >= 0)
		removeTab(index);
}

void GroupTabBar::updateGroup(Group group)
{
	auto groupFilter = GroupFilter(group);
	auto index = indexOf(groupFilter);
	if (index >= 0)
		updateTabData(index, groupFilter);
}

void GroupTabBar::updateTabData(int tabIndex, GroupFilter groupFilter)
{
	auto tabData = GroupFilterTabData(groupFilter);
	setTabText(tabIndex, tabData.tabName());
	setTabIcon(tabIndex, tabData.tabIcon());
}

#include "moc_group-tab-bar.cpp"