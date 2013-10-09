/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QApplication>

#include <QtCrypto>

#include "accounts/account-manager.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "chat/chat-styles-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/main-configuration-holder.h"
#include "contacts/contact-manager.h"
#include "dom/dom-processor-service.h"
#include "file-transfer/file-transfer-manager.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/services/clipboard-html-transformer-service.h"
#include "gui/widgets/account-configuration-widget-factory-repository.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-top-bar-widget-factory-repository.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/buddy-data-window-repository.h"
#include "gui/windows/chat-data-window-repository.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/search-window.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message-html-renderer-service.h"
#include "message/message-manager.h"
#include "misc/date-time-parser-tags.h"
#include "misc/kadu-paths.h"
#include "notify/notification-manager.h"
#include "parser/parser.h"
#include "plugins/plugins-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "provider/default-provider.h"
#include "provider/simple-provider.h"
#include "services/chat-image-request-service-configurator.h"
#include "services/chat-image-request-service.h"
#include "services/image-storage-service.h"
#include "services/message-filter-service.h"
#include "services/message-transformer-service.h"
#include "services/notification-service.h"
#include "services/raw-message-transformer-service.h"
#include "status/status-container-manager.h"
#include "status/status-setter.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "themes/icon-theme-manager.h"
#include "url-handlers/url-handler-manager.h"
#include "activate.h"
#include "debug.h"
#include "kadu-application.h"
#include "kadu-config.h"
#include "updates.h"

#if WITH_LIBINDICATE_QT
#include <libindicate-qt/qindicateserver.h>
#endif

#include "core.h"

Core * Core::Instance = 0;

Core * Core::instance()
{
	if (!Instance)
	{
		Instance = new Core();
		Instance->init();
	}

	return Instance;
}

QString Core::name()
{
	return QLatin1String("Kadu");
}

QString Core::version()
{
	return QLatin1String(KADU_VERSION);
}

QString Core::nameWithVersion()
{
	return name() + QLatin1String(" ")  + version();
}

KaduApplication * Core::application()
{
	return static_cast<KaduApplication *>(qApp);
}

Core::Core() :
		KaduWindowProvider(new SimpleProvider<QWidget *>(0)),
		MainWindowProvider(new DefaultProvider<QWidget *>(KaduWindowProvider)),
		CurrentChatImageRequestService(0), CurrentImageStorageService(0),
		CurrentMessageFilterService(0), CurrentMessageTransformerService(0),
		CurrentNotificationService(0), CurrentFormattedStringFactory(0),
		Window(0),
		Myself(Buddy::create()), IsClosing(false),
		ShowMainWindowOnStart(true), QcaInit(new QCA::Initializer())
{
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

	import_0_6_5_configuration();
	createDefaultConfiguration();
	configurationUpdated();

	MainConfigurationHolder::createInstance();

	Parser::GlobalVariables.insert(QLatin1String("DATA_PATH"), KaduPaths::instance()->dataPath());
	Parser::GlobalVariables.insert(QLatin1String("HOME"), KaduPaths::homePath());
	Parser::GlobalVariables.insert(QLatin1String("KADU_CONFIG"), KaduPaths::instance()->profilePath());
	DateTimeParserTags::registerParserTags();

	importPre10Configuration();
}

Core::~Core()
{
	IsClosing = true;

	// unloading modules does that
	/*StatusContainerManager::instance()->disconnectAndStoreLastStatus(disconnectWithCurrentDescription, disconnectDescription);*/
	ChatWidgetManager::instance()->closeAllWindows();
	ConfigurationManager::instance()->flush();
// 	delete Configuration;
// 	Configuration = 0;

	xml_config_file->makeBackup();

	PluginsManager::instance()->deactivatePlugins();

#ifdef Q_OS_MAC
	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
#endif // Q_OS_MAC

	KaduWindowProvider->provideValue(0);
	QWidget *hiddenParent = Window->parentWidget();
	delete Window;
	Window = 0;
	delete hiddenParent;

	MainConfigurationHolder::destroyInstance();

	triggerAllAccountsUnregistered();

	// Sometimes it causes crash which I don't understand. For me 100% reproducible
	// if Kadu was compiled with Clang and we logged in to a jabber account. --beevvy
	// TODO: fix it
	// delete QcaInit;
	// QcaInit = 0;
}

void Core::import_0_6_5_configuration()
{
	config_file.addVariable("Look", "UserboxAlternateBgColor", config_file.readEntry("Look", "UserboxBgColor"));
}

void Core::importPre10Configuration()
{
	if (config_file.readBoolEntry("General", "ImportedPre10"))
	{
		return;
	}

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		bool notify = buddy.property("notify:Notify", false).toBool() || config_file.readBoolEntry("Notify", "NotifyAboutAll");

		if (notify)
			buddy.removeProperty("notify:Notify");
		else
			buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}

	config_file.addVariable("General", "ImportedPre10", true);
}

void Core::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Chat", "AutoSend", true);
	config_file.addVariable("Chat", "BlinkChatTitle", true);
	config_file.addVariable("Chat", "ChatCloseTimer", true);
	config_file.addVariable("Chat", "ChatCloseTimerPeriod", 2);
	config_file.addVariable("Chat", "ChatPrune", false);
	config_file.addVariable("Chat", "ChatPruneLen", 20);
	config_file.addVariable("Chat", "ConfirmChatClear", true);
	config_file.addVariable("Chat", "IgnoreAnonymousRichtext", true);
	config_file.addVariable("Chat", "IgnoreAnonymousUsers", false);
	config_file.addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	config_file.addVariable("Chat", "LastImagePath", QDir::homePath() + '/');
	config_file.addVariable("Chat", "NewMessagesInChatTitle", false);
	config_file.addVariable("Chat", "OpenChatOnMessage", false);
	config_file.addVariable("Chat", "OpenChatOnMessageWhenOnline", false);
	config_file.addVariable("Chat", "SaveOpenedWindows", true);
	config_file.addVariable("Chat", "ReceiveMessages", true);
	config_file.addVariable("Chat", "RecentChatsTimeout", 240);
	config_file.addVariable("Chat", "RecentChatsClear", false);
	config_file.addVariable("Chat", "RememberPosition", true);
	config_file.addVariable("Chat", "ShowEditWindowLabel", true);

	config_file.addVariable("General", "AllowExecutingFromParser", false);
	config_file.addVariable("General", "AutoRaise", false);
	config_file.addVariable("General", "CheckUpdates", true);
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	config_file.addVariable("General", "DescriptionHeight", 60);
	config_file.addVariable("General", "DisconnectWithCurrentDescription", true);
#ifdef Q_WS_WIN
	config_file.addVariable("General", "HideMainWindowFromTaskbar", false);
#endif
	config_file.addVariable("General", "Language",  QLocale::system().name().left(2));
	config_file.addVariable("General", "Nick", tr("Me"));
	config_file.addVariable("General", "NumberOfDescriptions", 20);
	config_file.addVariable("General", "ParseStatus", false);
	config_file.addVariable("General", "ShowBlocked", true);
	config_file.addVariable("General", "ShowBlocking", true);
	config_file.addVariable("General", "ShowMyself", false);
	config_file.addVariable("General", "ShowOffline", true);
	config_file.addVariable("General", "ShowOnlineAndDescription", false);
	config_file.addVariable("General", "ShowWithoutDescription", true);

	if (config_file.readBoolEntry("General", "AdvancedMode", false))
	{
		config_file.addVariable("General", "StatusContainerType", "Account");
		config_file.addVariable("Look", "ShowExpandingControl", true);
	}
	else
	{
		config_file.addVariable("General", "StatusContainerType", "Identity");
		config_file.addVariable("Look", "ShowExpandingControl", false);
	}

	config_file.addVariable("General", "StartupLastDescription", true);
	config_file.addVariable("General", "StartupStatus", "LastStatus");
	config_file.addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", false);
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "WindowActivationMethod", 0);
	config_file.addVariable("General", "MainConfiguration_Geometry", "50, 50, 790, 580");
	config_file.addVariable("General", "LookChatAdvanced_Geometry", "50, 50, 620, 540");

	config_file.addVariable("Look", "AlignUserboxIconsTop", true);
	config_file.addVariable("Look", "AvatarBorder", false);
	config_file.addVariable("Look", "AvatarGreyOut", true);
	config_file.addVariable("Look", "ChatContents", QString());
	config_file.addVariable("Look", "ForceCustomChatFont", false);
	QFont chatFont = qApp->font();
#ifdef Q_WS_WIN
	// On Windows default app font is often "MS Shell Dlg 2", and the default sans
	// family (Arial, at least in Qt 4.8) is better. Though, on X11 the default
	// sans family is the same while most users will have some nice default app
	// font, like DejaVu, Ubuntu (the font, not the distro) or alike.
	chatFont.setStyleHint(QFont::SansSerif);
	chatFont.setFamily(chatFont.defaultFamily());
#endif
	config_file.addVariable("Look", "ChatFont", chatFont);
	config_file.addVariable("Look", "ChatBgFilled", // depends on configuration imported from older version
		config_file.readColorEntry("Look", "ChatBgColor").isValid() &&
		config_file.readColorEntry("Look", "ChatBgColor") != QColor("#ffffff"));
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatTextCustomColors", // depends on configuration imported from older version
		config_file.readColorEntry("Look", "ChatTextBgColor").isValid() &&
		config_file.readColorEntry("Look", "ChatTextBgColor") != QColor("#ffffff"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatTextFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ConferenceContents", QString());
	config_file.addVariable("Look", "ConferencePrefix", QString());
	config_file.addVariable("Look", "DescriptionColor", w.palette().text().color());
	config_file.addVariable("Look", "DisplayGroupTabs", true);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "InfoPanelFgColor", w.palette().text().color());
	config_file.addVariable("Look", "InfoPanelBgFilled", false);
	config_file.addVariable("Look", "InfoPanelBgColor", w.palette().base().color());
	config_file.addVariable("Look", "InfoPanelSyntaxFile", "ultr");
	config_file.addVariable("Look", "NiceDateFormat", true);
	config_file.addVariable("Look", "NoHeaderInterval", 30);
	config_file.addVariable("Look", "NoHeaderRepeat", true);
	config_file.addVariable("Look", "NoServerTime", true);
	config_file.addVariable("Look", "NoServerTimeDiff", 60);
	config_file.addVariable("Look", "PanelFont", qApp->font());
	config_file.addVariable("Look", "PanelVerticalScrollbar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
	config_file.addVariable("Look", "ShowAvatars", true);
	config_file.addVariable("Look", "IconTheme", IconThemeManager::defaultTheme());
	config_file.addVariable("Look", "ShowGroupAll", true);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowInfoPanel", false);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "Satin");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxTransparency", false);
	config_file.addVariable("Look", "UserboxAlpha", 0);
	config_file.addVariable("Look", "UserboxBlur", true);
	config_file.addVariable("Look", "UserboxBgColor", w.palette().base().color());
	config_file.addVariable("Look", "UserboxAlternateBgColor", w.palette().alternateBase().color());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.palette().text().color());
	QFont userboxfont(qApp->font());
	userboxfont.setPointSize(qApp->font().pointSize() + 1);
	config_file.addVariable("Look", "UserboxFont", userboxfont);
	config_file.addVariable("Look", "UseUserboxBackground", false);
#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the icon notification to animated which
	 * will prevent from blinking the dock icon
	 */
	config_file.addVariable("Look", "NewMessageIcon", 2);
#endif

	config_file.addVariable("Network", "AllowDCC", true);
	config_file.addVariable("Network", "DefaultPort", 0);
	config_file.addVariable("Network", "isDefServers", true);
	config_file.addVariable("Network", "Server", QString());
	config_file.addVariable("Network", "UseProxy", false);

#ifdef Q_OS_MAC
	/* Dorr: for MacOS X define the function keys with 'apple' button
	 * as it is the default system configuration */
	config_file.addVariable("ShortCuts", "chat_clear", "Ctrl+F9");
	config_file.addVariable("ShortCuts", "kadu_configure", "Ctrl+F2");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "Ctrl+F4");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "Ctrl+F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "Ctrl+F10");
#else
	config_file.addVariable("ShortCuts", "chat_clear", "F9");
	config_file.addVariable("ShortCuts", "kadu_configure", "F2");
	config_file.addVariable("ShortCuts", "kadu_modulesmanager", "F4");
	config_file.addVariable("ShortCuts", "kadu_showoffline", "F9");
	config_file.addVariable("ShortCuts", "kadu_showonlydesc", "F10");
#endif
	config_file.addVariable("ShortCuts", "chat_bold", "Ctrl+B");
	config_file.addVariable("ShortCuts", "chat_close", "Esc");
	config_file.addVariable("ShortCuts", "chat_italic", "Ctrl+I");
	config_file.addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	config_file.addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	config_file.addVariable("ShortCuts", "kadu_deleteuser", "Del");
	config_file.addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	config_file.addVariable("ShortCuts", "kadu_persinfo", "Ins");
	config_file.addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");
	config_file.addVariable("ShortCuts", "kadu_exit", "Ctrl+Q");

	config_file.addVariable("Chat", "UseDefaultWebBrowser", config_file.readEntry("Chat", "WebBrowser").isEmpty());
	config_file.addVariable("Chat", "UseDefaultEMailClient", config_file.readEntry("Chat", "MailClient").isEmpty());
	config_file.addVariable("Chat", "ContactStateChats", true);
	config_file.addVariable("Chat", "ContactStateWindowTitle", true);
	config_file.addVariable("Chat", "ContactStateWindowTitleSyntax", QString());
	config_file.addVariable("Chat", "ContactStateWindowTitlePosition", 1);

	createAllDefaultToolbars();
}

void Core::createAllDefaultToolbars()
{
	// don't use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);
	SearchWindow::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Core::init()
{
	runServices();

	// protocol modules should be loaded before gui
	// it fixes crash on loading pending messages from config, contacts import from 0.6.5, and maybe other issues
	PluginsManager::instance()->activateProtocolPlugins();

	Myself.setAnonymous(false);
	Myself.setDisplay(config_file.readEntry("General", "Nick", tr("Me")));

	new Updates(this);

	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(updateIcon()));
	QTimer::singleShot(15000, this, SLOT(deleteOldConfigurationFiles()));

	// TODO: add some life-cycle management
	NotificationManager::instance();

	AccountManager::instance()->ensureLoaded();
	BuddyManager::instance()->ensureLoaded();
	ContactManager::instance()->ensureLoaded();
	// Without that MessageManager is loaded while filtering buddies list for the first time.
	// It has to happen earlier because MessageManager::loaded() might add buddies to the BuddyManager
	// which (the buddies) otherwise will not be taken into account by buddies list before its next update.
	MessageManager::instance()->ensureLoaded();
	AvatarManager::instance(); // initialize that

#if WITH_LIBINDICATE_QT
	// Use a symbol from libindicate-qt so that it will not get dropped for example by --as-needed.
	(void)QIndicate::Server::defaultInstance();
#endif
}

void Core::initialized()
{
	StatusSetter::instance()->coreInitialized();
}

void Core::deleteOldConfigurationFiles()
{
	kdebugf();

	QDir oldConfigs(KaduPaths::instance()->profilePath(), "kadu-0.12.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs.count() > 20)
		for (unsigned int i = 0, max = oldConfigs.count() - 20; i < max; ++i)
			QFile::remove(KaduPaths::instance()->profilePath() + oldConfigs[i]);

	QDir oldConfigs2(KaduPaths::instance()->profilePath(), "kadu-0.6.6.conf.xml.backup.*", QDir::Name, QDir::Files);
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
			QFile::remove(KaduPaths::instance()->profilePath() + oldConfigs2[i]);

	QDir oldBacktraces(KaduPaths::instance()->profilePath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
			QFile::remove(KaduPaths::instance()->profilePath() + oldBacktraces[i]);

	QDir oldDebugs(KaduPaths::instance()->profilePath(), "kadu.log.*", QDir::Name, QDir::Files);
	if (oldDebugs.count() > 20)
		for (unsigned int i = 0, max = oldDebugs.count() - 20; i < max; ++i)
			QFile::remove(KaduPaths::instance()->profilePath() + oldDebugs[i]);

	kdebugf2();
}

void Core::updateIcon()
{
	if (isClosing())
		return;

	QApplication::setWindowIcon(KaduIcon("kadu_icons/kadu").icon());
}

void Core::kaduWindowDestroyed()
{
	KaduWindowProvider->provideValue(0);
	Window = 0;
}

void Core::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	connect(protocol, SIGNAL(connecting(Account)), this, SIGNAL(connecting()));
	connect(protocol, SIGNAL(connected(Account)), this, SIGNAL(connected()));
	connect(protocol, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
}

void Core::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (protocol)
		disconnect(protocol, 0, this, 0);
}

void Core::configurationUpdated()
{
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(config_file.readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

	bool ok;
	int newMask = qgetenv("DEBUG_MASK").toInt(&ok);
	debug_mask = ok ? newMask : config_file.readNumEntry("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);

	Myself.setDisplay(config_file.readEntry("General", "Nick", tr("Me")));
}

void Core::createGui()
{
	Window = new KaduWindow();
	connect(Window, SIGNAL(destroyed()), this, SLOT(kaduWindowDestroyed()));
	KaduWindowProvider->provideValue(Window);

	// initialize file transfers
	FileTransferManager::instance();
}

void Core::runServices()
{
	CurrentBuddyDataWindowRepository = new BuddyDataWindowRepository(this);
	CurrentChatDataWindowRepository = new ChatDataWindowRepository(this);
	CurrentChatImageRequestService = new ChatImageRequestService(this);
	CurrentDomProcessorService = new DomProcessorService(this);
	CurrentImageStorageService = new ImageStorageService(this);
	CurrentMessageFilterService = new MessageFilterService(this);
	CurrentMessageHtmlRendererService = new MessageHtmlRendererService(this);
	CurrentMessageTransformerService = new MessageTransformerService(this);
	CurrentFormattedStringFactory = new FormattedStringFactory();
	CurrentRawMessageTransformerService = new RawMessageTransformerService(this);
	CurrentClipboardHtmlTransformerService = new ClipboardHtmlTransformerService(this);
	CurrentAccountConfigurationWidgetFactoryRepository = new AccountConfigurationWidgetFactoryRepository(this);
	CurrentBuddyConfigurationWidgetFactoryRepository = new BuddyConfigurationWidgetFactoryRepository(this);
	CurrentChatConfigurationWidgetFactoryRepository = new ChatConfigurationWidgetFactoryRepository(this);
	CurrentChatTopBarWidgetFactoryRepository = new ChatTopBarWidgetFactoryRepository(this);

	// this instance lives forever
	// TODO: maybe make it QObject and make CurrentChatImageRequestService its parent
	ChatImageRequestServiceConfigurator *configurator = new ChatImageRequestServiceConfigurator();
	configurator->setChatImageRequestService(CurrentChatImageRequestService);

	CurrentChatImageRequestService->setImageStorageService(CurrentImageStorageService);
	CurrentChatImageRequestService->setAccountManager(AccountManager::instance());
	CurrentChatImageRequestService->setContactManager(ContactManager::instance());

	MessageManager::instance()->setMessageFilterService(CurrentMessageFilterService);
	MessageManager::instance()->setMessageTransformerService(CurrentMessageTransformerService);
	MessageManager::instance()->setFormattedStringFactory(CurrentFormattedStringFactory);

	CurrentFormattedStringFactory->setImageStorageService(CurrentImageStorageService);

	ChatStylesManager::instance()->setFormattedStringFactory(CurrentFormattedStringFactory);

	CurrentMessageHtmlRendererService->setDomProcessorService(CurrentDomProcessorService);
}

void Core::runGuiServices()
{
	CurrentNotificationService = new NotificationService(this);
}

BuddyDataWindowRepository * Core::buddyDataWindowRepository() const
{
	return CurrentBuddyDataWindowRepository;
}

ChatDataWindowRepository * Core::chatDataWindowRepository() const
{
	return CurrentChatDataWindowRepository;
}

ChatImageRequestService * Core::chatImageRequestService() const
{
	return CurrentChatImageRequestService;
}

DomProcessorService * Core::domProcessorService() const
{
	return CurrentDomProcessorService;
}

ImageStorageService * Core::imageStorageService() const
{
	return CurrentImageStorageService;
}

MessageFilterService * Core::messageFilterService() const
{
	return CurrentMessageFilterService;
}

MessageHtmlRendererService * Core::messageHtmlRendererService() const
{
	return CurrentMessageHtmlRendererService;
}

MessageTransformerService * Core::messageTransformerService() const
{
	return CurrentMessageTransformerService;
}

NotificationService * Core::notificationService() const
{
	return CurrentNotificationService;
}

FormattedStringFactory * Core::formattedStringFactory() const
{
	return CurrentFormattedStringFactory;
}

RawMessageTransformerService * Core::rawMessageTransformerService() const
{
	return CurrentRawMessageTransformerService;
}

ClipboardHtmlTransformerService * Core::clipboardHtmlTransformerService() const
{
	return CurrentClipboardHtmlTransformerService;
}

AccountConfigurationWidgetFactoryRepository * Core::accountConfigurationWidgetFactoryRepository() const
{
	return CurrentAccountConfigurationWidgetFactoryRepository;
}

BuddyConfigurationWidgetFactoryRepository * Core::buddyConfigurationWidgetFactoryRepository() const
{
	return CurrentBuddyConfigurationWidgetFactoryRepository;
}

ChatConfigurationWidgetFactoryRepository * Core::chatConfigurationWidgetFactoryRepository() const
{
	return CurrentChatConfigurationWidgetFactoryRepository;
}

ChatTopBarWidgetFactoryRepository * Core::chatTopBarWidgetFactoryRepository() const
{
	return CurrentChatTopBarWidgetFactoryRepository;
}

void Core::showMainWindow()
{
	if (ShowMainWindowOnStart)
		MainWindowProvider->provide()->show();

	// after first call which has to be placed in main(), this method should always show main window
	ShowMainWindowOnStart = true;
}

void Core::setShowMainWindowOnStart(bool show)
{
	ShowMainWindowOnStart = show;
}

KaduWindow * Core::kaduWindow()
{
	return Window;
}

const QSharedPointer<DefaultProvider<QWidget *> > & Core::mainWindowProvider() const
{
	return MainWindowProvider;
}

void Core::receivedSignal(const QString &signal)
{
	if ("activate" == signal)
		_activateWindow(MainWindowProvider->provide());
	else
		UrlHandlerManager::instance()->openUrl(signal.toUtf8(), true);
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}

#include "moc_core.cpp"
