/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat-widget-manager.h"

#include "chat/type/chat-type-contact.h"
#include "gui/widgets/chat-widget/chat-widget-actions.h"
#include "gui/widgets/chat-widget/chat-widget-container.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

#include <QtGui/QApplication>

ChatWidgetManager * ChatWidgetManager::m_instance = nullptr;

ChatWidgetManager * ChatWidgetManager::instance()
{
	if (!m_instance)
		m_instance = new ChatWidgetManager();

	return m_instance;
}

ChatWidgetManager::ChatWidgetManager()
{
	m_actions = new ChatWidgetActions(this);
}

ChatWidgetManager::~ChatWidgetManager()
{
}

void ChatWidgetManager::setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	m_chatWidgetContainerHandlerRepository = chatWidgetContainerHandlerRepository;
}

void ChatWidgetManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (m_chatWidgetRepository)
		connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetCreated(ChatWidget*)),
				this, SLOT(chatWidgetCreated(ChatWidget*)));
}

ChatWidget * ChatWidgetManager::byChat(const Chat &chat)
{
	if (!chat || !m_chatWidgetRepository)
		return nullptr;

	if (m_chatWidgetRepository.data()->hasWidgetForChat(chat))
		return m_chatWidgetRepository.data()->widgetForChat(chat);

	return nullptr;
}

void ChatWidgetManager::chatWidgetCreated(ChatWidget *chatWidget)
{
	if (!m_chatWidgetContainerHandlerRepository)
		return;

	auto chatWidgetContainerHandlers = m_chatWidgetContainerHandlerRepository.data()->chatWidgetContainerHandlers();
	for (auto chatWidgetContainerHandler : chatWidgetContainerHandlers)
		if (chatWidgetContainerHandler->containChatWidget(chatWidget))
			return;
}

void ChatWidgetManager::openChat(const Chat &chat, OpenChatActivation activation)
{
	if (!m_chatWidgetRepository)
		return;

	auto chatWidget = m_chatWidgetRepository.data()->widgetForChat(chat);
	if (activation == OpenChatActivation::Activate)
		chatWidget->tryActivate();
}

void ChatWidgetManager::closeChat(const Chat &chat)
{
	auto chatWidget = byChat(chat);
	if (chatWidget && chatWidget->container())
		chatWidget->container()->closeChatWidget(chatWidget);
}

#include "moc_chat-widget-manager.cpp"
