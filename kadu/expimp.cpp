/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <stdlib.h>

#include "kadu.h"
#include "config_dialog.h"
#include "debug.h"
#include "events.h"
#include "message_box.h"
#include "expimp.h"

UserlistImport::UserlistImport(QWidget *parent, const char *name)
 : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout * grid = new QGridLayout(this, 2, 2, 3, 3);

	results = new QListView(this);

	fetchbtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Fetch userlist"),this);
	QObject::connect(fetchbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));

	QPushButton * savebtn = new QPushButton(QIconSet(loadIcon("filesave.png")),i18n("&Save results"),this);
	QObject::connect(savebtn, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	QPushButton * filebtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Import from file"),this);
	QObject::connect(filebtn, SIGNAL(clicked()), this, SLOT(fromfile()));

	results->addColumn(i18n("UIN"));
	results->addColumn(i18n("Nickname"));
	results->addColumn(i18n("Disp. nick"));
	results->addColumn(i18n("Name"));
	results->addColumn(i18n("Surname"));
	results->addColumn(i18n("Mobile no."));
	results->addColumn(i18n("Group"));
	results->addColumn(i18n("Email"));
	results->setAllColumnsShowFocus(true);

	grid->addMultiCellWidget(results, 0, 0, 0, 2);
	grid->addWidget(filebtn, 1, 0);	
	grid->addWidget(fetchbtn, 1, 1);
	grid->addWidget(savebtn, 1, 2);

	resize(450, 330);
	setCaption(i18n("Import userlist"));	
}

void UserlistImport::fromfile(){

	QStringList lines,userlist;
	QListViewItem * qlv;
	QString line;    
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (fname.length()) {
		QFile file(fname);
 		if (file.open(IO_ReadOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			importedUserlist.clear();
			results->clear();
			while (!stream.eof()) {
				line = stream.readLine();
				lines = QStringList::split(";", line, true);
				if (lines[6] == "0")
					lines[6].truncate(0);
				importedUserlist.addUser(lines[0], lines[1],
					lines[2], lines[3], lines[4],
					lines[6], GG_STATUS_NOT_AVAIL, 0,
					false, false, true, lines[5],
					QString::null, lines[7]);
				qlv = new QListViewItem(results, lines[6],
					lines[2], lines[3], lines[0],
					lines[1], lines[4], lines[5],
					lines[7]);
	  			}
			file.close();
			}
		else
			QMessageBox::critical(this, i18n("Import error"),
				i18n("The application encountered an internal error\nThe import userlist from file was unsuccessful"));
		}
}

void UserlistImport::startTransfer() {
	if (getActualStatus() == GG_STATUS_NOT_AVAIL)
		return;

	if (gg_userlist_request(sess, GG_USERLIST_GET, NULL) == -1) {
		kdebug("UserlistImport: gg_userlist_get() failed\n");
		QMessageBox::critical(this, i18n("Import error"),
			i18n("The application encountered an internal error\nThe import was unsuccessful"));
		return;
		}

	fetchbtn->setEnabled(false);

	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

void UserlistImport::closeEvent(QCloseEvent * e) {
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
	QWidget::closeEvent(e);
}

void UserlistImport::updateUserlist() {
	int i;
	
	kdebug("UserlistImport::updateUserlist()\n");
	
	i = 0;
	while (i < userlist.count()) {
		if (userlist[i].uin)
			gg_remove_notify(sess, userlist[i].uin);
		i++;
		}

	userlist = importedUserlist;
//	importedUserlist.clear();
	
	kadu->userbox->clear();
	kadu->userbox->clearUsers();
	for (i = 0; i < userlist.count(); i++)
		kadu->userbox->addUser(userlist[i].altnick);
		
	UserBox::all_refresh();

	for (i = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_add_notify(sess, userlist[i].uin);

	uin_t *uins;
	uins = (uin_t *) malloc(userlist.count() * sizeof(uin_t));

	for (i = 0; i < userlist.count(); i++)
		uins[i] = userlist[i].uin;

	gg_notify(sess, uins, userlist.count());
	kdebug("UserlistImport::updateUserlist(): Userlist sent\n");

	userlist.writeToFile();
	kdebug("UserlistImport::updateUserlist(): Wrote userlist\n");

	free(uins);			
}

void UserlistImport::userlistReplyReceivedSlot(char type, char *reply) {
/*	if (gg_http->state == GG_STATE_DONE && gg_http->data == NULL) {
		kdebug("ImportUserlist::socketEvent(): No results. Exit.\n");
		deleteSocketNotifiers();
		QMessageBox::information(this, "No results", i18n("Your action yielded no results") );
		fetchbtn->setEnabled(true);
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_ERROR) {
		fetchbtn->setEnabled(true);
		deleteSocketNotifiers();
		kdebug("ImportUserlist::socketEvent(): gg_userlist_get_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Import error", i18n("The application encountered a network error\nThe import was unsuccessful") );
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}*/

	kdebug("ImportUserlist::userlistReplyReceivedSlot()\n");
	if (type != GG_USERLIST_GET_REPLY)
		return;

	fetchbtn->setEnabled(true);
	kdebug("ImportUserlist::userlistReplyReceivedSlot(): Done\n");
	QStringList strlist;
	strlist = QStringList::split("\r\n", cp2unicode((unsigned char *)reply), true);
	kdebug("ImportUserlist::userlistReplyReceivedSlot()\n%s\n", reply);
	QStringList fieldlist;
	// this is temporary array dedicated to Adrian
	QString tmparray[16];
	QListViewItem *qlv;
	QStringList::Iterator it;

	results->clear();
	importedUserlist.clear();
	for ((it = strlist.begin()); it != strlist.end(); it++) {
		if ((*it).contains(';') != 11)
			continue;
		fieldlist = QStringList::split(";", *it, true);
		if (fieldlist[6] == "0")
			fieldlist[6].truncate(0);
		importedUserlist.addUser(fieldlist[0], fieldlist[1],
			fieldlist[2], fieldlist[3], fieldlist[4],
			fieldlist[6], GG_STATUS_NOT_AVAIL, 0,
			false, false, true, fieldlist[5], QString::null,
			fieldlist[7]);
		qlv = new QListViewItem(results, fieldlist[6],
			fieldlist[2], fieldlist[3], fieldlist[0],
			fieldlist[1], fieldlist[4], fieldlist[5],
			fieldlist[7]);
		}
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

UserlistExport::UserlistExport(QWidget *parent, const char *name)
 : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout *grid = new QGridLayout(this,3,1,3,3);

	QString message(i18n("%1 entries will be exported").arg(userlist.count()));

	QLabel *clabel = new QLabel(message,this);

	sendbtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Send userlist"),this);
	
	deletebtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Delete userlist"),this);
	
	tofilebtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Export to file"),this);

	QPushButton * closebtn = new QPushButton(QIconSet(loadIcon("stop.png")),i18n("&Close window"),this);

	QObject::connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	grid->addWidget(clabel,0,0);
	grid->addWidget(sendbtn,1,0);
	grid->addWidget(deletebtn,2,0);	
	grid->addWidget(tofilebtn,3,0);		
	grid->addWidget(closebtn,4,0);

	QObject::connect(sendbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));	
	QObject::connect(tofilebtn, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	QObject::connect(deletebtn, SIGNAL(clicked()), this, SLOT(clean()));		

	setCaption(i18n("Export userlist"));	
}

QString UserlistExport::saveContacts(){
	QString contacts;
	int i = 0;
	contacts="";
	while (i < userlist.count()) {
		contacts += userlist[i].first_name;
		contacts += ";";
		contacts += userlist[i].last_name;
		contacts += ";";
		contacts += userlist[i].nickname;
		contacts += ";";
		contacts += userlist[i].altnick;
		contacts += ";";
		contacts += userlist[i].mobile;
		contacts += ";";
		contacts += userlist[i].group();
		contacts += ";";
		if (userlist[i].uin)
			contacts += QString::number(userlist[i].uin);
		contacts += ";";
		contacts += userlist[i].email;
		contacts += ";0;;0;";
		contacts += "\r\n";
		i++;
		}
	contacts.replace(QRegExp("(null)"), "");
	
	return contacts;
}

void UserlistExport::startTransfer() {
	if (getActualStatus() == GG_STATUS_NOT_AVAIL)
		return;

	QString contacts;
	contacts = saveContacts();
	
	char *con2;	
	con2 = (char *)strdup(unicode2cp(contacts).data());
	
	if (gg_userlist_request(sess, GG_USERLIST_PUT, con2) == -1) {
		kdebug("UserlistExport: gg_userlist_put() failed\n");
		QMessageBox::critical(this, i18n("Export error"),
			i18n("The application encountered an internal error\nThe export was unsuccessful"));
		free(con2);
		return;
		}
	free(con2);

	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	
	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

void UserlistExport::ExportToFile(void) {
	QString contacts;
	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	

	QString fname = QFileDialog::getSaveFileName("/", QString::null,this);
	if (fname.length()) {
		contacts = saveContacts();

		QFile file(fname);
		if (file.open(IO_WriteOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			stream << contacts;
			file.close();
			QMessageBox::information(this, i18n("Export completed"),
				i18n("Your userlist has been successfully exported to file"));
			}
		else
			QMessageBox::critical(this, i18n("Export error"),
				i18n("The application encountered an internal error\nThe export userlist to file was unsuccessful"));
		}

	sendbtn->setEnabled(true);
	deletebtn->setEnabled(true);
	tofilebtn->setEnabled(true);
}

void UserlistExport::clean() {
	if (getActualStatus() == GG_STATUS_NOT_AVAIL)
		return;

	const char *con2 = "";
	
	if (gg_userlist_request(sess, GG_USERLIST_PUT, con2) == -1) {
		kdebug("UserlistExport::clean(): Delete failed\n");
		QMessageBox::critical(this, i18n("Export error"),
			i18n("The application encountered an internal error\nThe delete userlist on server was unsuccessful"));
		return;
		}

	deletebtn->setEnabled(false);
	sendbtn->setEnabled(false);
	tofilebtn->setEnabled(false);
	connect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
}

void UserlistExport::userlistReplyReceivedSlot(char type, char *reply) {
	kdebug("ExportUserlist::userlistReplyReceivedSlot()\n");
	if (type != GG_USERLIST_PUT_REPLY)
		return;
	kdebug("ExportUserlist::userlistReplyReceivedSlot(): Done\n");
	
/*	if (gg_http->state == GG_STATE_ERROR) {
		sendbtn->setEnabled(true);
		deletebtn->setEnabled(true);
		tofilebtn->setEnabled(true);
		deleteSocketNotifiers();
		kdebug("ExportUserlist::socketEvent(): gg_userlist_put_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}*/

	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
	sendbtn->setEnabled(true);
	tofilebtn->setEnabled(true);
	deletebtn->setEnabled(true);
//	QMessageBox::information(this, i18n("Export complete"),
//		i18n("Your userlist has been successfully exported to server"));
	MessageBox::msg(i18n("Your userlist has been successfully exported to server"));
}

void UserlistExport::closeEvent(QCloseEvent * e) {
	disconnect(&event_manager, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userlistReplyReceivedSlot(char, char *)));
	QWidget::closeEvent(e);
}

