#include "encryption.h"
extern "C"
{
#include "simlite.h"
};
#include "config_dialog.h"
#include "kadu.h"
#include "modules.h"
#include "events.h"
#include "gadu.h"
#include "debug.h"
#include <qfile.h>
#include <qmessagebox.h>
// uzywamy mkdir z sys/stat.h - nie ma w QT mozliwosci ustawienia praw do kat.
#include <sys/stat.h>

EncryptionManager* encryption_manager;

extern "C" int encryption_init()
{
	encryption_manager=new EncryptionManager();
	return 0;
}

extern "C" void encryption_close()
{
	delete encryption_manager;
}

EncryptionManager::EncryptionManager()
{
	QT_TRANSLATE_NOOP("@default", "Use encryption");
	QT_TRANSLATE_NOOP("@default", "Encryption properties");
	QT_TRANSLATE_NOOP("@default", "Keys length");
	QT_TRANSLATE_NOOP("@default", "Generate keys");

	ConfigDialog::addCheckBox("Chat", "Chat", "Use encryption", "Encryption", false);	
	ConfigDialog::addHGroupBox("Chat", "Chat", "Encryption properties");
	ConfigDialog::addComboBox("Chat", "Encryption properties", "Keys length");
	ConfigDialog::addPushButton("Chat", "Encryption properties", "Generate keys");

	ConfigDialog::registerSlotOnCreate(this,SLOT(createConfigDialogSlot()));
	ConfigDialog::connectSlot("Chat", "Generate keys", SIGNAL(clicked()), this, SLOT(generateMyKeys()));
	ConfigDialog::connectSlot("Chat", "Use encryption", SIGNAL(toggled(bool)), this, SLOT(onUseEncryption(bool)));

	connect(chat_manager,SIGNAL(chatCreated(const UinsList&)),this,SLOT(chatCreated(const UinsList&)));
	connect(&event_manager,SIGNAL(messageFiltering(const UinsList&,char*)),this,SLOT(receivedMessageFilter(const UinsList&,char*)));
	connect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));
	
	Chat::registerButton("encryption_button",this,SLOT(encryptionButtonClicked()));
	UserBox::userboxmenu->addItemAtPos(2,"SendPublicKey", tr("Send my public key"), this, SLOT(sendPublicKey()));

	sim_key_path = strdup(ggPath("keys/").local8Bit());
}

EncryptionManager::~EncryptionManager()
{
	ConfigDialog::unregisterSlotOnCreate(this,SLOT(createConfigDialogSlot()));
	ConfigDialog::disconnectSlot("Chat", "Generate keys", SIGNAL(clicked()), this, SLOT(generateMyKeys()));
	ConfigDialog::disconnectSlot("Chat", "Use encryption", SIGNAL(toggled(bool)), this, SLOT(onUseEncryption(bool)));

	disconnect(chat_manager,SIGNAL(chatCreated(const UinsList&)),this,SLOT(chatCreated(const UinsList&)));
	disconnect(&event_manager,SIGNAL(messageFiltering(const UinsList&,char*)),this,SLOT(receivedMessageFilter(const UinsList&,char*)));
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));

	Chat::unregisterButton("encryption_button");
	int sendkeyitem = UserBox::userboxmenu->getItem(tr("Send my public key"));
	UserBox::userboxmenu->removeItem(sendkeyitem);
}

void EncryptionManager::createConfigDialogSlot()
{
	QComboBox* cb_keylength= ConfigDialog::getComboBox("Chat", "Keys length");
	cb_keylength->insertItem("1024");

	QCheckBox *c_useencryption= ConfigDialog::getCheckBox("Chat", "Use encryption");
	QHGroupBox *h_encryption= ConfigDialog::getHGroupBox("Chat", "Encryption properties");
	h_encryption->setEnabled(c_useencryption->isChecked());
}

void EncryptionManager::generateMyKeys()
{
	int myUin=config_file.readNumEntry("General","UIN");
	QString keyfile_path;	
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(myUin));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	
	if (keyfile.permission(QFileInfo::WriteUser))
		if(QMessageBox::warning(0, "Kadu",
			tr("Keys exist. Do you want to overwrite them?"),
			tr("Yes"), tr("No"),QString::null, 0, 1)==1)
				return;

	QCString tmp=ggPath("keys").local8Bit();
	// uzywamy mkdir z sys/stat.h - nie ma w QT mozliwosci ustawienia praw do kat.
	mkdir(tmp.data(), 0700);

	if (sim_key_generate(myUin) < 0) {
		QMessageBox::critical(0, "Kadu", tr("Error generating keys"), tr("OK"), QString::null, 0);
		return;
	}

	QMessageBox::information(0, "Kadu", tr("Keys have been generated and written"), tr("OK"), QString::null, 0);
}

void EncryptionManager::onUseEncryption(bool toggled)
{
	QHGroupBox *h_encryption = ConfigDialog::getHGroupBox("Chat", "Encryption properties");
	h_encryption->setEnabled(toggled);
}

void EncryptionManager::chatCreated(const UinsList& uins)
{

	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uins[0]));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	bool encryption_possible =
		(keyfile.permission(QFileInfo::ReadUser) && uins.count() == 1);

	Chat* chat=chat_manager->findChatByUins(uins);
	connect(chat,SIGNAL(messageFiltering(const UinsList&,char*&)),this,SLOT(sendMessageFilter(const UinsList&,char*&)));

	QPushButton* encryption_btn=chat->button("encryption_button");	
	setupEncryptButton(chat,config_file.readBoolEntry("Chat", "Encryption") && encryption_possible);	
	encryption_btn->setEnabled(encryption_possible);

	EncryptionButtonChat[encryption_btn]=chat;
}

void EncryptionManager::setupEncryptButton(Chat* chat,bool enabled)
{
	EncryptionEnabled[chat] = enabled;
	QPushButton* encryption_btn=chat->button("encryption_button");		
	QToolTip::remove(encryption_btn);
	if (enabled)
	{
		QToolTip::add(encryption_btn, tr("Disable encryption for this conversation"));
		encryption_btn->setPixmap(icons_manager.loadIcon("EncryptedChat"));
	}
	else
	{
		QToolTip::add(encryption_btn, tr("Enable encryption for this conversation"));
		encryption_btn->setPixmap(icons_manager.loadIcon("DecryptedChat"));
	}
}

void EncryptionManager::encryptionButtonClicked()
{
	Chat* chat=EncryptionButtonChat[dynamic_cast<const QPushButton*>(sender())];
	setupEncryptButton(chat,!EncryptionEnabled[chat]);
}

void EncryptionManager::receivedMessageFilter(const UinsList& senders,char* msg)
{
	if (config_file.readBoolEntry("Chat","Encryption"))
	{
		if (!strncmp(msg, "-----BEGIN RSA PUBLIC KEY-----", 20))
		{
			QFile keyfile;
			QString keyfile_path;
			QWidget *parent;

			Chat* chat=chat_manager->findChatByUins(senders);
			if (chat == NULL)
				parent = kadu;
			else
				parent = chat;

			SavePublicKey *spk = new SavePublicKey(senders[0], msg, NULL);
			spk->show();
			msg[0]=0;
			return;
		}
	}

	if (msg != NULL)
	{
		kdebug("Decrypting encrypted message...\n");
		char* decoded = sim_message_decrypt((unsigned char*)msg, senders[0]);
		kdebug("Decrypted message is: %s\n",decoded);
		if (decoded != NULL)
		{
			strcpy(msg,"[SSL]\n");
			strcat(msg, decoded);
		}
	}
}

void EncryptionManager::enableEncryptionBtnForUins(UinsList uins)
{
	Chat* chat=chat_manager->findChatByUins(uins);
	if(chat==NULL)
		return;
	QPushButton* encryption_btn=chat->button("encryption_button");	
	encryption_btn->setEnabled(true);
}

void EncryptionManager::sendMessageFilter(const UinsList& uins,char*& msg)
{
	Chat* chat=chat_manager->findChatByUins(uins);
	if (uins.count()==1 && EncryptionEnabled[chat])
	{
		char* encrypted = sim_message_encrypt((unsigned char *)msg, uins[0]);
		free(msg);
		msg = encrypted;
	}
}

void EncryptionManager::userBoxMenuPopup()
{
	int sendkeyitem = UserBox::userboxmenu->getItem(tr("Send my public key"));

	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;
	
	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("General", "UIN")));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	
	const UinsList& uins = activeUserBox->getSelectedUins();
	uin_t uin = uins.first();
	
	if ((keyfile.permission(QFileInfo::ReadUser) && uin) && (uins.count() == 1))
		UserBox::userboxmenu->setItemEnabled(sendkeyitem, true);
	else
		UserBox::userboxmenu->setItemEnabled(sendkeyitem, false);
}

void EncryptionManager::sendPublicKey()
{
	QString keyfile_path;
	QString mykey;
	QFile keyfile;

	UserBox *activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)
		return;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(config_file.readEntry("General", "UIN"));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (keyfile.open(IO_ReadOnly))
	{
		QTextStream t(&keyfile);
		mykey = t.read();
		keyfile.close();
		QCString tmp(mykey.local8Bit());
		UinsList uins;
		uins.append(activeUserBox->getSelectedUins().first());
		gadu->sendMessage(uins, tmp.data());
		QMessageBox::information(kadu, "Kadu",
			tr("Your public key has been sent"), tr("OK"), QString::null, 0);
	}
}

SavePublicKey::SavePublicKey(uin_t uin, QString keyData, QWidget *parent, const char *name) :
	QDialog(parent, name, Qt::WDestructiveClose), uin(uin), keyData(keyData) {
	
	kdebug("SavePublicKey::SavePublicKey()\n");

	setCaption(tr("Save public key"));
	resize(200, 80);
	
	QLabel *l_info = new QLabel(
		tr("User %1 is sending you his public key. Do you want to save it?").arg(userlist.byUin(uin).altnick),
		this);

	QPushButton *yesbtn = new QPushButton(tr("Yes"), this);
	QPushButton *nobtn = new QPushButton(tr("No"), this);

	QObject::connect(yesbtn, SIGNAL(clicked()), this, SLOT(yesClicked()));
	QObject::connect(nobtn, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this, 2, 2, 3, 3);
	grid->addMultiCellWidget(l_info, 0, 0, 0, 1);
	grid->addWidget(yesbtn, 1, 0);
	grid->addWidget(nobtn, 1, 1);

	kdebug("SavePublicKey::SavePublicKey(): finished\n");
}

void SavePublicKey::yesClicked() {
	QFile keyfile;
	QString keyfile_path;

	kdebug("SavePublicKey::yesClicked()\n");

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uin));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (!(keyfile.open(IO_WriteOnly))) {
		QMessageBox::critical(this, tr("Error"), tr("Error writting the key"), tr("OK"), QString::null, 0);
		kdebug("eventRecvMsg(): Error opening key file %s\n", (const char *)keyfile_path.local8Bit());
		return;
		}
	else {
		keyfile.writeBlock(keyData.local8Bit(), keyData.length());
		keyfile.close();
		UinsList uins;
		uins.append(uin);
		encryption_manager->enableEncryptionBtnForUins(uins);
		}
	accept();

	kdebug("SavePublicKey::yesClicked(): finished\n");
}

