/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_STYLES_MANAGER_H
#define CHAT_STYLES_MANAGER_H

#include <memory>
#include <QtCore/QMap>
#include <QtCore/QObject>

#include "chat/style-engine/chat-messages-renderer.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QCheckBox;
class QComboBox;
class QPushButton;

class WebkitMessagesView;
class ChatStyleEngine;
class ChatStylePreview;
class ConfiguredChatMessagesRendererProvider;
class FormattedStringFactory;

//TODO: review
struct StyleInfo
{
	bool global;
	ChatStyleEngine *engine;

	StyleInfo() : global(false), engine(0) {}
};

class KADUAPI ChatStylesManager : public QObject, ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

	static ChatStylesManager *Instance;

	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;
	QPointer<ConfiguredChatMessagesRendererProvider> CurrentConfiguredChatMessagesRendererProvider;

	ChatStylesManager();

	void init();

	std::map<QString, std::unique_ptr<ChatStyleEngine>> RegisteredEngines;
	QList<WebkitMessagesView *> ChatViews;
	QMap<QString, StyleInfo> AvailableStyles;

	ChatStyleEngine *CurrentEngine;

	bool CompositingEnabled;

	bool CfgNoHeaderRepeat; /*!< Remove repeated message headers. */
	int CfgHeaderSeparatorHeight; /*!< Header separator height. */
	int CfgNoHeaderInterval; /*!< Time Interval, in which headers will not be repeated*/
	int ParagraphSeparator; /*!< Message separator height. */

	int Prune; /*!< Maximal number of visible messages. */

	bool NoServerTime; /*!< Remove server time */
	int NoServerTimeDiff; /*!< Maximal time difference between server time and local time, for which server time will be removed */

	QString MainStyle;

	//configuration
	QComboBox *SyntaxListCombo;
	QComboBox *VariantListCombo;
	QCheckBox *TurnOnTransparency;

	ChatStylePreview *EnginePreview;

	void compositingEnabled();
	void compositingDisabled();

	QString fixedStyleName(QString styleName);
	QString fixedVariantName(const QString &styleName, QString variantName);

private slots:
	void styleChangedSlot(const QString &styleName);
	void variantChangedSlot(const QString &variantName);
	void configurationWindowDestroyed();
	void configurationApplied();

protected:
	virtual void configurationUpdated();

public:
	static ChatStylesManager * instance();

	~ChatStylesManager();

	void setConfiguredChatMessagesRendererProvider(ConfiguredChatMessagesRendererProvider *configuredChatMessagesRendererProvider);
	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	void chatViewCreated(WebkitMessagesView * view);
	void chatViewDestroyed(WebkitMessagesView * view);
	void registerChatStyleEngine(const QString &name, std::unique_ptr<ChatStyleEngine>);
	void unregisterChatStyleEngine(const QString &name);

	bool hasChatStyle(const QString &name) { return  AvailableStyles.contains(name); }
	StyleInfo chatStyleInfo(const QString &name);

	void loadStyles();

	bool cfgNoHeaderRepeat() { return CfgNoHeaderRepeat; }
	int cfgHeaderSeparatorHeight() { return CfgHeaderSeparatorHeight; }
	int cfgNoHeaderInterval() { return CfgNoHeaderInterval; }
	int paragraphSeparator() { return ParagraphSeparator; }

	bool noServerTime() { return NoServerTime; }
	int noServerTimeDiff() { return NoServerTimeDiff; }

	int prune() { return Prune; }

	const QString & mainStyle() { return MainStyle; }

	void mainConfigurationWindowCreated(MainConfigurationWindow *window);
	QComboBox * syntaxListCombo() { return SyntaxListCombo; }

	void addStyle(const QString &syntaxName, ChatStyleEngine *engine);

signals:
	void previewSyntaxChanged(const QString &syntaxName);

};

#endif
