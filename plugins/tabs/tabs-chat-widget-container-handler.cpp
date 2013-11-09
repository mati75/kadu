/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "tabs-chat-widget-container-handler.h"

#include "gui/widgets/tab-widget.h"
#include "tabs.h"

TabsChatWidgetContainerHandler::TabsChatWidgetContainerHandler(QObject *parent) :
		ChatWidgetContainerHandler(parent)
{
}

TabsChatWidgetContainerHandler::~TabsChatWidgetContainerHandler()
{
}

void TabsChatWidgetContainerHandler::setTabsManager(TabsManager *tabsManager)
{
	m_tabsManager = tabsManager;
}

void TabsChatWidgetContainerHandler::setTabWidget(TabWidget *tabWidget)
{
	m_tabWidget = tabWidget;
}

bool TabsChatWidgetContainerHandler::containChatWidget(ChatWidget *chatWidget)
{
	if (!m_tabsManager)
		return false;

	return m_tabsManager.data()->containChatWidget(chatWidget);
}

bool TabsChatWidgetContainerHandler::isChatWidgetActive(ChatWidget *chatWidget)
{
	return m_tabWidget ? m_tabWidget.data()->isChatWidgetActive(chatWidget) : false;
}

void TabsChatWidgetContainerHandler::tryActivateChatWidget(ChatWidget *chatWidget)
{
	if (m_tabWidget)
		m_tabWidget.data()->tryActivateChatWidget(chatWidget);
}
