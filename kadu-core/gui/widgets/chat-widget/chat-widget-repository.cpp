/*
 * %kadu copyright begin%
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

#include "chat-widget-repository.h"

#include "gui/widgets/chat-widget/chat-widget.h"

ChatWidget * ChatWidgetRepository::converter(ChatWidgetRepository::WrappedIterator iterator)
{
	return iterator->second;
}

ChatWidgetRepository::ChatWidgetRepository(QObject *parent) :
		QObject{parent}
{
}

ChatWidgetRepository::~ChatWidgetRepository()
{
}

ChatWidgetRepository::Iterator ChatWidgetRepository::begin()
{
	return Iterator{m_widgets.begin(), converter};
}

ChatWidgetRepository::Iterator ChatWidgetRepository::end()
{
	return Iterator{m_widgets.end(), converter};
}

void ChatWidgetRepository::addChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget || hasWidgetForChat(chatWidget->chat()))
		return;

	emit chatWidgetAdded(chatWidget);
	m_widgets.insert(std::make_pair(chatWidget->chat(), chatWidget));

	connect(chatWidget, SIGNAL(widgetDestroyed(ChatWidget*)), this, SLOT(removeChatWidget(ChatWidget*)));
}

void ChatWidgetRepository::removeChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget || (widgetForChat(chatWidget->chat()) != chatWidget))
		return;

	m_widgets.erase(chatWidget->chat());
	emit chatWidgetRemoved(chatWidget);
}

void ChatWidgetRepository::removeChatWidget(Chat chat)
{
	removeChatWidget(widgetForChat(chat));
}

bool ChatWidgetRepository::hasWidgetForChat(const Chat &chat) const
{
	return m_widgets.end() != m_widgets.find(chat);
}

ChatWidget * ChatWidgetRepository::widgetForChat(const Chat &chat)
{
	if (!chat)
		return nullptr;

	auto it = m_widgets.find(chat);
	return it != m_widgets.end()
			? it->second
			: nullptr;
}

#include "moc_chat-widget-repository.cpp"