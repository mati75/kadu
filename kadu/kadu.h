/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_H
#define KADU_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qapplication.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <qsocketnotifier.h>
#include <qarray.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qnetworkprotocol.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qevent.h>
#include <qframe.h>
#include <qlayout.h>

#include "userlist.h"
#include "userbox.h"
#include "pending_msgs.h"
#include "misc.h"
#include "status.h"
//#include "tabbar.h"
#include "../config.h"

struct colors {
	QColor userboxBg;
	QColor userboxFg;
	QColor userboxDescBg;
	QColor userboxDescText;
	QColor mychatBg;
	QColor mychatText;
	QColor usrchatBg;
	QColor usrchatText;
	QColor trayhintBg;
	QColor trayhintText;
};

struct fonts {
	QFont userbox;
	QFont userboxDesc;
	QFont chat;
	QFont trayhint;
};

struct config {
	uin_t uin;
	QString password;
	QString nick;
	QString soundmsg;
	QString soundchat;
	QString soundprog;
	double soundvol;
	bool soundvolctrl;
	bool playartsdsp;
	bool playsound;
	bool logmessages;
	bool savegeometry;
	bool playsoundchat;
	bool playsoundchatinvisible;
	int defaultstatus;
	QString defaultdescription;
	int sysmsgidx;
	bool allowdcc;
	QString dccip;
	char *extip;
	int extport;
	QStringList servers;

	bool dock;
	bool rundocked;
	bool dock_wmaker;
	
	bool privatestatus;
	bool grouptabs;
	bool checkupdates;
	bool addtodescription;
	bool trayhint;
	bool hinterror;
	bool hintalert;
	bool showdesc;

	QRect geometry;
	QSize splitsize;
	
	bool defaultwebbrowser;
	QString webbrowser;

	bool smsbuildin;
	char * smsapp;
	bool smscustomconf;
	char * smsconf;

	bool emoticons;
	QString emoticons_theme;
	bool autosend;
	bool scrolldown;
	int chatprunelen;
	bool chatprune;
	bool msgacks;
	bool blinkchattitle;
	bool ignoreanonusers;

	bool autoaway;
	int autoawaytime;
	int hinttime;

	QStringList notifies;
	char *soundnotify;
	bool notifyglobal;
	bool notifyall;
	bool notifydialog;
	bool notifysound;
	bool notifyhint;

	bool useproxy;
	QString proxyaddr;
	unsigned short proxyport;
	QString proxyuser;
	QString proxypassword;

	bool raise;

	struct colors colors;
	struct fonts fonts;
#ifdef HAVE_OPENSSL
	bool encryption;
	//int keyslen;
#endif
};

struct groups {
  int number;
  char * name;
};

class dccSocketClass;

class KaduTabBar;

class Kadu : public QMainWindow
{
	Q_OBJECT
	 
	public:
		Kadu(QWidget* parent=0, const char *name=0);
		void changeAppearance();
		~Kadu();
		bool autohammer;
		bool userInActiveGroup(uin_t uin);
		// code for addUser has been moved from adduser.cpp
		// for sharing with search.cpp
		void addUser(const QString& FirstName,const QString& LastName,
			const QString& NickName,const QString& AltNick,
			const QString& Mobile,const QString& Uin,const int Status,
			const QString& Group,const QString& Description, const bool Anonymous = false);
		void removeUser(QStringList &, bool);
		void refreshGroupTabBar();
		void setClosePermitted(bool permitted);
    
		UserBox *userbox;

		AutoStatusTimer* autostatus_timer;    

	protected:
		void closeEvent(QCloseEvent *);	
		bool event(QEvent *e);
		void watchDcc(void);
		void keyPressEvent(QKeyEvent *e);
		void resizeEvent(QResizeEvent *e);

		struct gg_event *dcc_e;
		int dcc_ret;

		int activegrpno;

		bool blinkOn;
		bool doBlink;

	public slots:
		void blink();
		void dccFinished(dccSocketClass *dcc);
		void slotHandleState(int command);
		void slotShowStatusMenu();
		void setCurrentStatus(int status);
		void sendMessage(QListBoxItem *);
		void listPopupMenu(QListBoxItem *);
		void commandParser(int);
		void dataReceived();
		void dataSent();
		void dccReceived();
		void dccSent();
		void eventHandler(int state);
		void prepareDcc(void);
		void cleanUp(void);
		void pingNetwork(void);
		void checkConnection(void);
		void setStatus(int);
		void disconnectNetwork(void);
		void changeGroup(int);
		int openChat(UinsList);
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void currentChanged(QListBoxItem *item);
		void showdesc(void);
		void hidedesc(void);
		void statusMenuAboutToHide(void);

	private:
		QFrame *centralFrame;
		QGridLayout *grid;
		QMenuBar *mmb;
		KaduTabBar *group_bar;
		QTextBrowser *descrtb;
		int commencing_startup;
		void createMenu();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);
		bool close_permitted;
	
	private slots:
		void groupTabSelected(int id);
		void userListModified();
		void userListStatusModified(UserListElement *);
};

struct acks {
    int ack;
    int seq;
    int type;
    QWidget *ptr;
};

class Operation : public QProgressDialog {
	Q_OBJECT
	public:
	Operation(const QString & labelText, const QString & cancelButtonText, int totalSteps, QWidget *parent = 0 );
	int laststate;
	QTimer * t;
	int steps;

	private slots:
	void perform();
	void cancel();
};

class MyLabel : public QLabel {
	Q_OBJECT
	public:
		MyLabel(QWidget *parent, const char *name): QLabel(parent, name) {};

	protected:
		void mousePressEvent (QMouseEvent * e);
};

class Chat;

struct chats {
    UinsList uins;
    Chat *ptr;
    bool operator==(const chats& r) const
    {
    	return (uins==r.uins)&&(ptr==r.ptr);
    };
};

extern const char **gg_xpm[];
//extern KApplication *a;
extern QApplication *a;
extern Kadu *kadu;
extern struct gg_session *sess;
extern struct config config;
// Ominiecie bledu w gcc 3.2
static QValueList<chats> chats_gcc32_bug;
////////////////////////////
extern QValueList<chats> chats;
extern UserList userlist;
extern QValueList<uin_t> ignored;
extern PendingMsgs pending;
// Ominiecie bledu w gcc 3.2
static QArray<groups> grouplist_gcc32_bug;
////////////////////////////
extern QArray<groups> grouplist;
extern bool mute;
extern bool userlist_sent;
extern int server_nr;
// Ominiecie bledu w gcc 3.2
static QArray<acks> acks_gcc32_bug;
////////////////////////////
extern QArray<acks> acks;
extern QString own_description;

extern QPopupMenu *statusppm;
extern QSocketNotifier *kadusnr;
extern QSocketNotifier *kadusnw;
extern QSocketNotifier *dccsnr;
extern QSocketNotifier *dccsnw;

void deletePendingMessage(int nr);
void sendUserlist(void);

void createConfig();
void addIgnored(uin_t);
void delIgnored(uin_t);
bool isIgnored(uin_t);
int writeIgnored(QString filename = "");
void *watch_socket(void *);
void kadu_debug(int, char*);
void playSound(const QString &sound, const QString player = QString::null);
void readConfig(void);
QString pwHash(const QString);
void confirmHistoryDeletion(const char *);
extern QPopupMenu *dockppm;

#endif
