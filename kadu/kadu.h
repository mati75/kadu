#ifndef KADU_H
#define KADU_H

#include <qapplication.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <qsocketnotifier.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qnetworkprotocol.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qvaluelist.h>
#include <qfile.h>
#include <qhbox.h>

#include "misc.h"
#include "userbox.h"
#include "userlist.h"
#include "tabbar.h"
#include "status.h"

/**
	Toolbar Kadu
**/
class ToolBar : public QToolBar
{
	private:
		struct ToolButton
		{
			QIconSet iconfile;
			QString caption, name;
			QObject* receiver;
			QString slot;
			QToolButton* button;
			int position;
		};
		static QValueList<ToolButton> RegisteredToolButtons;
		void createControls();
	public:
		static ToolBar* instance;
		ToolBar(QMainWindow* parent);
		~ToolBar();
		static void registerButton(const QIconSet& iconfile, const QString& caption,
			QObject* receiver, const char* slot, const int position=-1, const char* name="");
		static void unregisterButton(const char* name);
		static void registerSeparator(int position=-1);
		static QToolButton* getButton(const char* name);
};

/**
	G��wne okno Kadu
**/
class Kadu : public QMainWindow
{
	Q_OBJECT

	private:
		KaduTextBrowser* InfoPanel;
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;

		Status status;

		bool ShowMainWindowOnStart;
		bool Autohammer;
		bool DoBlink;
		bool BlinkOn;
		bool UpdateChecked;
		bool Docked;

		void createMenu();
		void createToolBar();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);

		void showStatusOnMenu(int);

	public slots:
		void show();
		void mouseButtonClicked(int, QListBoxItem *);
		void infopanelUpdate(UinType);
		void currentChanged(QListBoxItem *item);
		void sendMessage(QListBoxItem *);
		void configure();

	private slots:
		void userListModified();
		void userListStatusModified(UserListElement *, bool);
		void openChat();
		void userListUserAdded(const UserListElement& user);
		void chatMsgReceived(UinsList senders, const QString &msg, time_t time);
		void userListChanged();

		void wentOnline(const QString &);
		void wentBusy(const QString &);
		void wentInvisible(const QString &);
		void wentOffline(const QString &);
		void groupTabSelected(int id);
		void connected();
		void connecting();
		void disconnected();
		void error(GaduError);
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void systemMessageReceived(QString &);
		void userStatusChanged(UserListElement &, const Status &oldstatus, bool onConnection);

	protected:
		void keyPressEvent(QKeyEvent *e);
		virtual void resizeEvent(QResizeEvent *);
//		virtual void moveEvent(QMoveEvent *);

	public:
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		bool userInActiveGroup(UinType uin);
		void removeUser(QStringList &, bool);
		/**
			Zwraca wskaznik do belki menu glownego okna.
		**/
		QMenuBar* menuBar();
		/**
			Zwraca wskaznik do glownego menu programu.
		**/
		QPopupMenu* mainMenu();
		/**
			Zwraca wskaznik do zakladek z nazwami grup.
		**/
		KaduTabBar* groupBar();
		/**
			Zwraca wskaznik do userbox-a w glownym oknie.
		**/
		UserBox* userbox();
		/**
			Zwraca true je�li kadu jest zadokowane.
		**/
		bool docked();
		void startupProcedure();
		int personalInfoMenuId;//potrzebne dla modu�u account_management

	public slots:
		void slotHandleState(int command);
		void changeAppearance();
		void blink();
		void refreshGroupTabBar();
		//void setCurrentStatus(int status);
		//void setStatus(int);
		void showdesc(bool show = true);
		void statusMenuAboutToHide(void);
		virtual bool close(bool quit = false);
		void quitApplication();
		/**
			Potrzebne dla modu�u dokuj�cego �eby
			g��wne okno nie miga�o przy starcie...
		**/
		void setShowMainWindowOnStart(bool show);
		/**
			Modu� dokowania powinien to ustawic, aby kadu
			wiedzialo, ze jest zadokowane.
		**/
		void setDocked(bool docked);
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);

		void about();
		void addUserAction();
		void blockUser();
		void deleteHistory();
		void deleteUsers();
		void help();
		void hideKadu();
		void ignoreUser();
		void importExportUserlist();
		void lookupInDirectory();
		void manageIgnored();
		void notifyUser();
		void offlineToUser();
		void personalInfo();
		void quit();
		void searchInDirectory();
		void showUserInfo();
		void viewHistory();
		void popupMenu();

		// odczytuje z obrazka tekst i zapisuje go w drugim parametrze
		void readTokenValue(QPixmap, QString &);

	signals:
		void disconnectingNetwork();
		void disconnectedNetwork();
		void keyPressed(QKeyEvent* e);
		void statusPixmapChanged(QPixmap &);
		/**
			Podczas ��czenia (mruganie ikonki) czas na zmian�
			ikony na t� reprezentuj�c� docelowy status.
		**/
		//void connectingBlinkShowStatus(eStatus, bool);
		/**
			Podczas ��czenia (mruganie ikonki) czas na zmian�
			ikony na t� reprezentuj�c� status offline.
		**/
		//void connectingBlinkShowOffline();
		/**
			Zmieni� si� nasz aktualny status GG.
		**/
		//	void currentStatusChanged(int status);		TODO
		/**
			U�ytkownik chce zmieni� status. Je�li jest to status
			bezopisowy, to zmienna sigDesc b�dzie r�wna QString::null.
			Je�li zostanie ustawiona zmienna stop, to status nie
			b�dzie zmieniony.
		**/
		void changingStatus(Status &, bool &stop);
		/**
			wywo�ana zosta�a funkcja show() na g��wnym oknie
		**/
		void shown();
		/**
			u�ytkownik chce zarejestrowa� nowy numer gg
		**/
		void wantRegister();
		/**
			wyst�pi� b��d po��czenia
		**/
		void connectionError(const QString &reason);
		/**
			u�ywany przez modu� hints do zdobycia pozycji traya od modu�u docking
			TODO: trzeba wymy�li� jaki� elegancki spos�b na komunikacj� pomi�dzy modu�ami, kt�re nie zale�� od siebie
		**/
		void searchingForTrayPosition(QPoint &point);
};

class KaduSlots : public QObject
{
	Q_OBJECT

	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseColor(const char *name, const QColor& color);
		void chooseFont(const char *name, const QFont& font);
		void updatePreview();
};

extern Kadu* kadu;

extern QPopupMenu* statusppm;
extern QPopupMenu* dockppm;

extern int lockFileHandle;
extern QFile *lockFile;

#endif
