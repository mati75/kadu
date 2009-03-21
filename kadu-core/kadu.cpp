/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSettings>
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtWebKit/QWebFrame>

#ifndef _MSC_VER
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "chat/chat_manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-kadu-data.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"

#include "contacts/model/contacts-model.h"

#include "contacts/model/filter/group-contact-filter.h"
#include "contacts/model/filter/has-description-contact-filter.h"
#include "contacts/model/filter/online-contact-filter.h"
#include "contacts/model/filter/online-and-description-contact-filter.h"
#include "contacts/model/filter/offline-contact-filter.h"
#include "contacts/model/filter/anonymous-contact-filter.h"
#include "contacts/model/filter/anonymous-without-messages-contact-filter.h"

#include "core/core.h"

#include "file-transfer/file-transfer-manager.h"

#include "gui/widgets/chat_edit_box.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/widgets/group-tab-bar.h"

#include "gui/windows/contact-data-window.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"

#include "../modules/gadu_protocol/gadu-contact-account-data.h"
#include "../modules/gadu_protocol/gadu-protocol.h"

#include "about.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "expimp.h"
#include "hot_key.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu_parser.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "modules.h"
#include "misc/misc.h"
#include "personal_info.h"
#include "search.h"
#include "status_changer.h"
#include "syntax_editor.h"
#include "toolbar.h"
#include "updates.h"
#include "xml_config_file.h"

#include "kadu.h"

#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *); //there's no header for it
#endif

//look for comment in config_file.h
ConfigFile *config_file_ptr;

Kadu *kadu;

static QTimer *blinktimer;
QMenu *dockMenu;

const char *Kadu::SyntaxText = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size\n"
);

const char *Kadu::SyntaxTextNotify = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{protocol} - protocol that triggered event,\n"
	"#{event} - name of event,\n"
);

/* a monstrous constructor so Kadu would take longer to start up */
Kadu::Kadu(QWidget *parent)
	: KaduMainWindow(parent)
{/*
	kdebugf();

	Core *core = Core::instance();
	connect(core, SIGNAL(connecting()), this, SLOT(connecting()));
	connect(core, SIGNAL(connected()), this, SLOT(connected()));
	connect(core, SIGNAL(disconnected()), this, SLOT(disconnected()));

	kadu = this;
	blinktimer = 0;

	createDefaultConfiguration();

#ifdef Q_OS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	MainWidget = new QWidget;
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	QSplitter *split = new QSplitter(Qt::Vertical, this);
	MainLayout->addWidget(split);

	QWidget* hbox = new QWidget(split);
	QHBoxLayout *hbox_layout = new QHBoxLayout(hbox);
	hbox_layout->setMargin(0);
	hbox_layout->setSpacing(0);

	// groupbar
	GroupBar = new GroupTabBar(this);
	hbox_layout->setStretchFactor(GroupBar, 1);*/

#if 0
	splitStatusChanger = new SplitStatusChanger(GG_STATUS_DESCR_MAXSIZE);
	status_changer_manager->registerStatusChanger(splitStatusChanger);
#endif

	// gadu, chat, search
/*

	ContactsWidget = new ContactsListWidget(this);
	ContactsWidget->setModel(new ContactsModel(ContactManager::instance(), this));
	ContactsWidget->addFilter(GroupBar->filter());

	hbox_layout->setStretchFactor(ContactsWidget, 100);
	hbox_layout->addWidget(GroupBar);
	hbox_layout->addWidget(ContactsWidget);
	hbox_layout->setAlignment(GroupBar, Qt::AlignTop);

	connect(ContactsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(sendMessage(Contact)));
	connect(ContactsWidget, SIGNAL(currentContactChanged(Contact)), this, SLOT(currentChanged(Contact)));
*/

// 	groups_manager->setTabBar(GroupBar);

//	loadWindowGeometry(this, "General", "Geometry", 0, 50, 205, 465);

// 	IgnoredManager::loadFromConfiguration();

	/* a newbie? */
// TODO: 0.6.6 some way of setting title needed
//	setWindowTitle(tr("Kadu: %1").arg(Myself.ID("Gadu")));

/*
	inactiveUsersAction = new ActionDescription(0,
		ActionDescription::TypeUserList, "inactiveUsersAction",
		this, SLOT(inactiveUsersActionActivated(QAction *, bool)),
		"ShowHideInactiveUsers", tr("Hide offline users"),
		true, tr("Show offline users")
	);
	connect(inactiveUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(inactiveUsersActionCreated(KaduAction *)));
	inactiveUsersAction->setShortcut("kadu_showoffline");

	descriptionUsersAction = new ActionDescription(0,
		ActionDescription::TypeUserList, "descriptionUsersAction",
		this, SLOT(descriptionUsersActionActivated(QAction *, bool)),
		"ShowOnlyDescriptionUsers", tr("Hide users without description"),
		true, tr("Show users without description")
	);
	connect(descriptionUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(descriptionUsersActionCreated(KaduAction *)));
	descriptionUsersAction->setShortcut("kadu_showonlydesc");

	onlineAndDescriptionUsersAction = new ActionDescription(0,
		ActionDescription::TypeUserList, "onlineAndDescriptionUsersAction",
		this, SLOT(onlineAndDescUsersActionActivated(QAction *, bool)),
		"ShowOnlineAndDescriptionUsers", tr("Show only online and description users"),
		true, tr("Show all users")
	);
	connect(onlineAndDescriptionUsersAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(onlineAndDescUsersActionCreated(KaduAction *)));

	editUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "editUserAction",
		this, SLOT(editUserActionActivated(QAction *, bool)),
		"EditUserInfo", tr("Contact data"), false, QString::null,
		disableNotOneUles
	);
	connect(editUserActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(editUserActionCreated(KaduAction *)));
	editUserActionDescription->setShortcut("kadu_persinfo");
	ContactsListWidgetMenuManager::instance()->addActionDescription(editUserActionDescription);

	showStatusActionDescription = new ActionDescription(0,
		ActionDescription::TypeGlobal, "openStatusAction",
		this, SLOT(showStatusActionActivated(QAction *, bool)),
		"Offline", tr("Change status")
	);
	connect(showStatusActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(showStatusActionCreated(KaduAction *)));

	useProxyActionDescription = new ActionDescription(0,
		ActionDescription::TypeGlobal, "useProxyAction",
		this, SLOT(useProxyActionActivated(QAction *, bool)),
		"UseProxy", tr("Use proxy"), true, tr("Don't use proxy")
	);
	connect(useProxyActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(useProxyActionCreated(KaduAction *)));

	createStatusPopupMenu();

	connect(statusMenu, SIGNAL(aboutToHide()), this, SLOT(statusMenuAboutToHide()));
	connect(dockMenu, SIGNAL(aboutToHide()), this, SLOT(dockMenuAboutToHide()));
	connect(RecentChatsMenu, SIGNAL(aboutToShow()), this, SLOT(createRecentChatsMenu()));

	dockMenu->addSeparator();
	dockMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), this, SLOT(quit()));

	statusButton = new QPushButton(icons_manager->loadIcon("Offline"), tr("Offline"), this);
	MainLayout->addWidget(statusButton);
	statusButton->setMenu(statusMenu);

	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		statusButton->hide();

	QList<int> splitsizes;

	splitsizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitsizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	split->setSizes(splitsizes);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);

#ifdef Q_OS_MAC
	qt_mac_set_dock_menu(dockMenu);
#endif

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->loadOpenedWindows();

	configurationUpdated();

	kdebugf2();*/
}

void Kadu::inactiveUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<OfflineContactFilter *>())
	{
		OfflineContactFilter *ofcf = v.value<OfflineContactFilter *>();
		ofcf->setEnabled(toggled);
	}
}

void Kadu::descriptionUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<HasDescriptionContactFilter *>())
	{
		HasDescriptionContactFilter *hdcf = v.value<HasDescriptionContactFilter *>();
		hdcf->setEnabled(toggled);
	}
}

void Kadu::onlineAndDescUsersActionActivated(QAction *sender, bool toggled)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	QVariant v = sender->data();
	if (v.canConvert<OnlineAndDescriptionContactFilter *>())
	{
		OnlineAndDescriptionContactFilter *oadcf = v.value<OnlineAndDescriptionContactFilter *>();
		oadcf->setEnabled(toggled);
	}
}

void Kadu::inactiveUsersActionCreated(KaduAction *action)
{
	KaduMainWindow *window = qobject_cast<KaduMainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->contactsListWidget())
		return;
	bool enabled = !config_file.readBoolEntry("General", "ShowOffline");
	OfflineContactFilter *ofcf = new OfflineContactFilter(action);
	ofcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(ofcf));
	action->setChecked(enabled);

	window->contactsListWidget()->addFilter(ofcf);
}

void Kadu::descriptionUsersActionCreated(KaduAction *action)
{
	KaduMainWindow *window = qobject_cast<KaduMainWindow *>(action->parent());
	if (!window)
		return;
	if (!window->contactsListWidget())
		return;

	bool enabled = !config_file.readBoolEntry("General", "ShowWithoutDescription");
	HasDescriptionContactFilter *hdcf = new HasDescriptionContactFilter(action);
	hdcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(hdcf));
	action->setChecked(enabled);

	window->contactsListWidget()->addFilter(hdcf);
}

void Kadu::onlineAndDescUsersActionCreated(KaduAction *action)
{
	KaduMainWindow *window = qobject_cast<KaduMainWindow *>(action->parent());
	if (!window)
		return;
	bool enabled = config_file.readBoolEntry("General", "ShowOnlineAndDescription");
	OnlineAndDescriptionContactFilter *oadcf = new OnlineAndDescriptionContactFilter(action);
	oadcf->setEnabled(enabled);

	action->setData(QVariant::fromValue(oadcf));
	action->setChecked(enabled);

	window->contactsListWidget()->addFilter(oadcf);
}

/*
void Kadu::editUserActionSetParams(QString / *protocolName* /, UserListElement user)
{
	kdebugf();
 	foreach (KaduAction *action, editUserActionDescription->actions())
	{
		KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(action->parent());
		if (!window)
			continue;

		Contact contact = window->contact();

		if (!contact.isNull())
		{
			if (contact.isAnonymous())
			{
				action->setIcon(icons_manager->loadIcon("AddUser"));
				action->setText(tr("Add user"));
			}
			else
			{
				action->setIcon(icons_manager->loadIcon("EditUserInfo"));
				action->setText(tr("Contact data"));
			}
		}
	}
	kdebugf2();
}*/

void Kadu::editUserActionCreated(KaduAction *action)
{
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(action->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isAnonymous())
	{
		action->setIcon(icons_manager->loadIcon("AddUser"));
		action->setText(tr("Add user"));
	}
}

void Kadu::editUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	(new ContactDataWindow(contact, kadu))->show();

	kdebugf2();
}

void Kadu::openChatWith()
{
	kdebugf();

	(new OpenChatWith(this))->show();

	kdebugf2();
}

void Kadu::showStatusActionActivated(QAction *sender, bool toggled)
{
// 	statusMenu->exec(QCursor::pos());
}

void Kadu::showStatusActionCreated(KaduAction *action)
{
	Account *gadu = AccountManager::instance()->defaultAccount();

	if (gadu != NULL)
		action->setIcon(gadu->protocol()->statusPixmap());
}

void Kadu::setStatusActionsIcon()
{
	Account *gadu = AccountManager::instance()->defaultAccount();

	if (gadu != NULL)
	{
		QPixmap pixmap = gadu->protocol()->statusPixmap();

		foreach (KaduAction *action, showStatusActionDescription->actions())
			action->setIcon(pixmap);
	}
}

void Kadu::useProxyActionActivated(QAction *sender, bool toggled)
{
	config_file.writeEntry("Network", "UseProxy", toggled);

	setProxyActionsStatus(toggled);
}

void Kadu::useProxyActionCreated(KaduAction *action)
{
	action->setChecked(config_file.readBoolEntry("Network", "UseProxy", false));
}

void Kadu::setProxyActionsStatus(bool checked)
{
	foreach (KaduAction *action, useProxyActionDescription->actions())
		action->setChecked(checked);
}

void Kadu::setProxyActionsStatus()
{
	setProxyActionsStatus(config_file.readBoolEntry("Network", "UseProxy", false));
}

void Kadu::changeAppearance()
{
	kdebugf();

	QApplication::setStyle(config_file.readEntry("Look", "QtStyle"));

// 	GroupBar->setFont(QFont(config_file.readFontEntry("Look", "UserboxFont").family(), config_file.readFontEntry("Look", "UserboxFont").pointSize(),75));

// 	kadu->statusButton->setShown(config_file.readBoolEntry("Look", "ShowStatusButton"));

	const Status &stat = AccountManager::instance()->status();

	Account *account = AccountManager::instance()->defaultAccount();
	QPixmap pix;
	if (account)
		pix = account->statusPixmap(stat);

	QIcon icon(pix);
// 	statusButton->setIcon(icon);

	setStatusActionsIcon();

	Core::instance()->setIcon(pix);

	emit statusPixmapChanged(icon, Status::name(stat));
	kdebugf2();
}

// TODO: 0.6.6 remove blink from here
void Kadu::blink()
{
	QIcon icon;

	kdebugf();
/*
	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	if (!DoBlink && gadu->isConnected())
		return;
	else if (!DoBlink && !gadu->isConnected())
	{
		icon = QIcon(gadu->statusPixmap(Status::Offline));
// 		statusButton->setIcon(icon);
		emit statusPixmapChanged(icon, "Offline");
		return;
	}*/
/*
	QString iconName;
	if (BlinkOn)
	{
		icon = QIcon(gadu->statusPixmap(Status::Offline));
		iconName = "Offline";
	}
	else
	{
		const Status &stat = gadu->nextStatus();
		icon = QIcon(gadu->statusPixmap(stat));
		iconName = Status::name(stat);
	}*/

// 	statusButton->setIcon(icon);
// 	emit statusPixmapChanged(icon, iconName);
/*
	BlinkOn = !BlinkOn;*/
/*
	blinktimer->setSingleShot(true);
	blinktimer->start(1000);*/
}

void Kadu::mouseButtonClicked(int button, Q3ListBoxItem *item)
{
	kdebugmf(KDEBUG_FUNCTION_START, "button=%d\n", button);
// 	if (!item)
// 		InfoPanel->setHtml("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	kdebugf2();
}

/* if something's pending, open it, if not, open new message */
void Kadu::sendMessage(Contact contact)
{
	ContactsListWidget *widget = dynamic_cast<ContactsListWidget *>(sender());
	if (!widget)
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactList contacts = widget->selectedContacts();

	if (!contacts.isEmpty())
	{
		Contact contact = contacts[0];

		if (contacts[0] != Core::instance()->myself()) //TODO: elem.hasFeature("SendingMessages")
			chat_manager->sendMessage(contact, contacts);
		else if (contact.mobile().isEmpty() && !contact.email().isEmpty())
			openMailClient(contact.email());

	}
}

void Kadu::connecting()
{
	kdebugf();
/*
	DoBlink = true;

	if (!blinktimer)
	{
		blinktimer = new QTimer(this);
		QObject::connect(blinktimer, SIGNAL(timeout()), kadu, SLOT(blink()));
	}

	blinktimer->setSingleShot(true);
	blinktimer->start(1000);*/
	kdebugf2();
}

void Kadu::connected()
{
	kdebugf();
// 	DoBlink = false;
	kdebugf2();
}

void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
{
// 	for (int i = 0, count = pending.count(); i < count; i++)
// 	{
// 		PendingMsgs::Element& e = pending[i];
//	TODO: 0.6.6 or sth?
// 		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
// 	}
}

void Kadu::disconnected()
{
	kdebugmf(KDEBUG_FUNCTION_START, "Disconnection has occured\n");

	chat_manager->refreshTitles();
/*
	DoBlink = false;

	if (blinktimer)
	{
		blinktimer->stop();
		delete blinktimer;
		blinktimer = NULL;
	}*/

	kdebugf2();
}

bool Kadu::close(bool quit)
{
	if (!quit/* && Docked*/)
	{
		kdebugmf(KDEBUG_INFO, "hiding\n");
		hide();
		return false;
	}
	else
	{

#if 0
		status_changer_manager->unregisterStatusChanger(splitStatusChanger);
		delete splitStatusChanger;
		splitStatusChanger = 0;
#endif

		ModulesManager::closeModule();

		Updates::closeModule();
		delete defaultFontInfo;
		delete defaultFont;

// TODO: 0.6.6
// 		disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
// 				this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));

		ChatManager::closeModule();
		SearchDialog::closeModule();

		EmoticonsManager::closeModule();
		IconsManager::closeModule();

#ifdef Q_OS_MACX
		//na koniec przywracamy domy�ln� ikon�, je�eli tego nie zrobimy, to pozostanie bie��cy status
		setMainWindowIcon(QPixmap(dataPath("kadu.png")));
#endif

		QWidget::close();

		kdebugmf(KDEBUG_INFO, "Graceful shutdown...\n");

		qDeleteAll(children());

		kdebugmf(KDEBUG_INFO, "Sync configuration...\n");
		xml_config_file->sync();

		delete xml_config_file;
		delete config_file_ptr;

		return true;
	}
}

Kadu::~Kadu(void)
{
	kdebugf();

	qDeleteAll(KaduActions.values());

	kdebugf2();
}

//
ContactList Kadu::contacts()
{
	return ContactList();
// 	return ContactsWidget->selectedContacts();
}

void Kadu::show()
{
	QWidget::show();

	// no columns now ...
	// TODO: remove after 0.6
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox", false))
	{
// 		int columns = Userbox->visibleWidth() / config_file.readUnsignedNumEntry("Look", "MultiColumnUserboxWidth", Userbox->visibleWidth());
// 		if (columns < 1)
// 			columns = 1;
// 		config_file.writeEntry("Look", "UserBoxColumnCount", columns);
// 		KaduListBoxPixmap::setColumnCount(columns);
// 		Userbox->refresh();
	}
	config_file.removeVariable("Look", "MultiColumnUserbox");
	config_file.removeVariable("Look", "MultiColumnUserboxWidth");

	emit shown();
}

void Kadu::hide()
{
	emit hiding();
	QWidget::hide();
}

void Kadu::refreshPrivateStatusFromConfigFile()
{
	bool privateStatus = config_file.readBoolEntry("General", "PrivateStatus");

	// je�li stan nie uleg� zmianie to nic nie robimy
// 	if (changePrivateStatus->isChecked() == privateStatus)
// 		return;

// 	Status status = userStatusChanger->status();
// TODO: 0.6.6
// 	status.setFriendsOnly(privateStatus);
// 	userStatusChanger->userStatusSet(status);

// 	changePrivateStatus->setChecked(privateStatus);
}

void Kadu::configurationUpdated()
{
	refreshPrivateStatusFromConfigFile();

	changeAppearance();

// 	groups_manager->refreshTabBar();

	setProxyActionsStatus();

#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		       QSettings::NativeFormat);
	if(config_file.readBoolEntry("General", "RunOnStartup"))
		settings.setValue("Kadu",
				QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
	else
		settings.remove("Kadu");
#endif

#ifdef DEBUG_ENABLED
	debug_mask = config_file.readNumEntry("General", "DEBUG_MASK");
	gg_debug_level = debug_mask | ~255;
#endif
}

void Kadu::setDefaultStatus()
{
	kdebugf();

	QString description;
	QString startupStatus = config_file.readEntry("General", "StartupStatus");
	Status status;

	if (config_file.readBoolEntry("General", "StartupLastDescription"))
		description = config_file.readEntry("General", "LastStatusDescription");
	else
		description = config_file.readEntry("General", "StartupDescription");

	bool offlineToInvisible = false;
	Status::StatusType type;

	if (startupStatus == "LastStatus")
	{
		int statusIndex = config_file.readNumEntry("General", "LastStatusIndex", 6);
		switch (statusIndex)
		{
			case 0:
			case 1:
				type = Status::Online;
				break;
			case 2:
			case 3:
				type = Status::Busy;
				break;
			case 4:
			case 5:
				type = Status::Invisible;
				break;
			case 6:
			case 7:
				type = Status::Offline;
				break;
		}

		offlineToInvisible = config_file.readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline");
	}
	else if (startupStatus == "Online")
		type = Status::Online;
	else if (startupStatus == "Busy")
		type = Status::Busy;
	else if (startupStatus == "Invisible")
		type = Status::Invisible;
	else if (startupStatus == "Offline")
		type = Status::Offline;

	if ((Status::Offline == type) && offlineToInvisible)
		type = Status::Invisible;

	status.setType(type);
	status.setDescription(description);

// TODO: 0.6.6
// 	status.setFriendsOnly(config_file.readBoolEntry("General", "PrivateStatus"));
	Core::instance()->setStatus(status);

	kdebugf2();
}

void Kadu::startupProcedure()
{
	kdebugf();

	status_changer_manager->enable();
	setDefaultStatus();

	kdebugf2();
}

void Kadu::statusChanged(Account *account, Status status)
{
// 	DoBlink = false;

	bool hasDescription = !status.description().isEmpty();
	int index;

	switch (status.type())
	{
		case Status::Online:
			index = hasDescription ? 1 : 0;
			break;
		case Status::Busy:
			index = hasDescription ? 3 : 2;
			break;
		case Status::Invisible:
			index = hasDescription ? 5 : 4;
			break;
		default:
			index = hasDescription ? 7 : 6;
	}

	showStatusOnMenu(index);
}

void Kadu::showStatusOnMenu(int statusNr)
{
	kdebugf();

// 	emit statusPixmapChanged(icon, iconName);
}

void Kadu::readTokenValue(QPixmap tokenImage, QString &tokenValue)
{
	TokenDialog *td = new TokenDialog(tokenImage, 0);

	if (td->exec() == QDialog::Accepted)
		td->getValue(tokenValue);
	else
		tokenValue.truncate(0);

	delete td;
}

QString Kadu::readToken(const QPixmap &tokenPixmap)
{
	TokenDialog *td = new TokenDialog(tokenPixmap, 0);
	QString result;

	if (td->exec() == QDialog::Accepted)
		result = td->getValue();
	delete td;

	return result;
}

char *SystemUserName;
void Kadu::deleteOldConfigFiles()
{
	kdebugf();

	QDir oldConfigs2(ggPath(), "kadu.conf.xml.backup.*", QDir::Name, QDir::Files);
//	kdebugm(KDEBUG_INFO, "%d\n", oldConfigs2.count());
	if (oldConfigs2.count() > 20)
		for (unsigned int i = 0, max = oldConfigs2.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldConfigs2[i]));
			QFile::remove(ggPath(oldConfigs2[i]));
		}
//	kdebugm(KDEBUG_INFO, "configs2 deleted\n");

	QDir oldBacktraces(ggPath(), "kadu.backtrace.*", QDir::Name, QDir::Files);
	if (oldBacktraces.count() > 20)
		for (unsigned int i = 0, max = oldBacktraces.count() - 20; i < max; ++i)
		{
//			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldBacktraces[i]));
			QFile::remove(ggPath(oldBacktraces[i]));
		}
//	kdebugm(KDEBUG_INFO, "bts deleted\n");

#ifdef Q_OS_WIN
	QString tmp(getenv("TEMP") ? getenv("TEMP") : ".");
	QString mask("kadu-dbg-*.txt");
#else
	QString tmp("/tmp");
	QString mask=QString("kadu-%1-*.dbg").arg(SystemUserName);
#endif
	QDir oldDebugs(tmp, mask, QDir::Name, QDir::Files);
	if (oldDebugs.count() > 5)
		for (unsigned int i = 0, max = oldDebugs.count() - 5; i < max; ++i)
		{
			kdebugm(KDEBUG_DUMP, "deleting %s\n", qPrintable(oldDebugs[i]));
			QFile::remove(tmp + "/" + oldDebugs[i]);
		}
//	kdebugm(KDEBUG_INFO, "debugs deleted\n");
	kdebugf2();
}

const QDateTime &Kadu::startTime() const
{
	return QDateTime();
// 	return StartTime;
}

void Kadu::customEvent(QEvent *e)
{
// TODO: 0.6.6
// 	Account *defaultAccount = AccountManager::instance()->defaultAccount();
// 
// 	if (int(e->type()) == 4321)
// 		show();
//		QTimer::singleShot(0, this, SLOT(show()));
// 	else if (int(e->type()) == 5432)
// 	{
// 		OpenGGChatEvent *ev = static_cast<OpenGGChatEvent *>(e);
// 		if (ev->number() > 0)
// 		{
// 			Contact contact = userlist->byID("Gadu", QString::number(ev->number())).toContact(defaultAccount);
// 			ContactList contacts;
// 			contacts << contact;
// 			chat_manager->openChatWidget(defaultAccount, contacts, true);
// 		}
// 	}
// 	else
// 		QWidget::customEvent(e);
}

void Kadu::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Chat", "ActivateWithNewMessages", false);
	config_file.addVariable("Chat", "AutoSend", true);
	config_file.addVariable("Chat", "BlinkChatTitle", true);
	config_file.addVariable("Chat", "ChatCloseTimer", true);
	config_file.addVariable("Chat", "ChatCloseTimerPeriod", 2);
	config_file.addVariable("Chat", "ChatPrune", false);
	config_file.addVariable("Chat", "ChatPruneLen", 20);
	config_file.addVariable("Chat", "ConfirmChatClear", true);
	config_file.addVariable("Chat", "EmoticonsPaths", "");
	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	config_file.addVariable("Chat", "EmoticonsTheme", "penguins");
	config_file.addVariable("Chat", "FoldLink", true);
	config_file.addVariable("Chat", "LinkFoldTreshold", 50);
	config_file.addVariable("Chat", "IgnoreAnonymousRichtext", true);
	config_file.addVariable("Chat", "IgnoreAnonymousUsers", false);
	config_file.addVariable("Chat", "IgnoreAnonymousUsersInConferences", false);
	config_file.addVariable("Chat", "LastImagePath", QString(getenv("HOME")) + '/');
	config_file.addVariable("Chat", "MaxImageRequests", 5);
	config_file.addVariable("Chat", "MaxImageSize", 255);
	config_file.addVariable("Chat", "MessageAcks", false);
	config_file.addVariable("Chat", "NewMessagesInChatTitle", false);
	config_file.addVariable("Chat", "OpenChatOnMessage", true);
	config_file.addVariable("Chat", "OpenChatOnMessageWhenOnline", false);
	config_file.addVariable("Chat", "SaveOpenedWindows", true);
	config_file.addVariable("Chat", "ReceiveMessages", true);
	config_file.addVariable("Chat", "ReceiveImagesDuringInvisibility", true);
	config_file.addVariable("Chat", "RememberPosition", true);
	config_file.addVariable("Chat", "ShowEditWindowLabel", true);

	config_file.addVariable("General", "AllowExecutingFromParser", false);
	config_file.addVariable("General", "AutoRaise", false);
	config_file.addVariable("General", "CheckUpdates", true);
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	config_file.addVariable("General", "DescriptionHeight", 60);
	config_file.addVariable("General", "DisconnectWithCurrentDescription", true);
	config_file.addVariable("General", "HideBaseModules", true);
	config_file.addVariable("General", "Language",  QString(qApp->keyboardInputLocale().name()).mid(0,2));
	config_file.addVariable("General", "Nick", tr("Me"));
	config_file.addVariable("General", "NumberOfDescriptions", 20);
	config_file.addVariable("General", "ParseStatus", false);
	config_file.addVariable("General", "PrivateStatus", false);
	config_file.addVariable("General", "SaveStdErr", false);
	config_file.addVariable("General", "ShowBlocked", true);
	config_file.addVariable("General", "ShowBlocking", true);
	config_file.addVariable("General", "ShowEmotPanel", true);
	config_file.addVariable("General", "ShowOffline", true);
	config_file.addVariable("General", "ShowOnlineAndDescription", false);
	config_file.addVariable("General", "ShowWithoutDescription", true);
	config_file.addVariable("General", "StartDelay", 0);
	config_file.addVariable("General", "StartupLastDescription", true);
	config_file.addVariable("General", "StartupStatus", "LastStatus");
	config_file.addVariable("General", "StartupStatusInvisibleWhenLastWasOffline", true);
	config_file.addVariable("General", "UserBoxHeight", 300);
	config_file.addVariable("General", "WindowActivationMethod", 0);

	config_file.addVariable("Look", "AlignUserboxIconsTop", true);
	config_file.addVariable("Look", "ChatContents", "");
	config_file.addVariable("Look", "ChatFont", *defaultFont);
	config_file.addVariable("Look", "ChatBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatMyNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatTextBgColor", QColor("#ffffff"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrNickColor", QColor("#000000"));
	config_file.addVariable("Look", "ConferenceContents", "");
	config_file.addVariable("Look", "ConferencePrefix", "");
	config_file.addVariable("Look", "DescriptionColor", w.palette().foreground().color());
	config_file.addVariable("Look", "DisplayGroupTabs", true);
	config_file.addVariable("Look", "HeaderSeparatorHeight", 1);
	config_file.addVariable("Look", "IconsPaths", "");
	config_file.addVariable("Look", "IconTheme", "default");
	config_file.addVariable("Look", "InfoPanelBgColor", w.palette().background().color());
	config_file.addVariable("Look", "InfoPanelFgColor", w.palette().foreground().color());
	config_file.addVariable("Look", "InfoPanelSyntaxFile", "default");
	config_file.addVariable("Look", "NiceDateFormat", true);
	config_file.addVariable("Look", "NoHeaderInterval", 30);
	config_file.addVariable("Look", "NoHeaderRepeat", false);
	config_file.addVariable("Look", "NoServerTime", true);
	config_file.addVariable("Look", "NoServerTimeDiff", 60);
	config_file.addVariable("Look", "PanelFont", *defaultFont);
	config_file.addVariable("Look", "PanelVerticalScrollBar", false);
	config_file.addVariable("Look", "ParagraphSeparator", 4);
	config_file.addVariable("Look", "ShowGroupAll", true);
	config_file.addVariable("Look", "ShowBold", true);
	config_file.addVariable("Look", "ShowDesc", true);
	config_file.addVariable("Look", "ShowInfoPanel", true);
	config_file.addVariable("Look", "ShowMultilineDesc", true);
	config_file.addVariable("Look", "ShowStatusButton", true);
	config_file.addVariable("Look", "Style", "kadu");
	config_file.addVariable("Look", "UserboxBackgroundDisplayStyle", "Stretched");
	config_file.addVariable("Look", "UserboxBgColor", w.palette().background().color());
	config_file.addVariable("Look", "UserBoxColumnCount", 1);
	config_file.addVariable("Look", "UserboxFgColor", w.palette().foreground().color());
	QFont userboxfont(*defaultFont);
	userboxfont.setPointSize(defaultFont->pointSize()+1);
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
	config_file.addVariable("Network", "Server", "");
	config_file.addVariable("Network", "TimeoutInMs", 5000);
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
	config_file.addVariable("ShortCuts", "chat_newline", "Return");
	config_file.addVariable("ShortCuts", "chat_underline", "Ctrl+U");
	config_file.addVariable("ShortCuts", "kadu_adduser", "Ctrl+N");
	config_file.addVariable("ShortCuts", "kadu_deleteuser", "Del");
	config_file.addVariable("ShortCuts", "kadu_openchatwith", "Ctrl+L");
	config_file.addVariable("ShortCuts", "kadu_persinfo", "Ins");
	config_file.addVariable("ShortCuts", "kadu_searchuser", "Ctrl+F");

	createAllDefaultToolbars();
}

#include "gui/windows/kadu-window.h"

void Kadu::createAllDefaultToolbars()
{
	// dont use getToolbarsConfigElement here, we have to be sure that this element don'e exists
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (!toolbarsConfig.isNull())
		return; // no need for defaults...

	toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	KaduWindow::createDefaultToolbars(toolbarsConfig);
	ChatEditBox::createDefaultToolbars(toolbarsConfig);
	SearchDialog::createDefaultToolbars(toolbarsConfig);

	xml_config_file->sync();
}

void Kadu::addAction(const QString &actionName, bool showLabel)
{
	addToolButton(findExistingToolbar(""), actionName, showLabel);
	kadu->refreshToolBars("");
}
