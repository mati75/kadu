/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <qmessagebox.h>

#include <sys/stat.h>
//#include <locale.h>

#include "kadu.h"
#include "../config.h"

#ifdef VOICE_ENABLED
#include "voice.h"
#include "voice_dsp.h"
#endif

#include "config_file.h"
#include "config_dialog.h"
#include "register.h"
#ifdef HAVE_OPENSSL
#include "simlite.h"
#endif

Kadu *kadu;	
QApplication *a;

int main(int argc, char *argv[])
{
	gg_debug_level = 255;

//	setlocale(LC_ALL, "");
//	bindtextdomain("kadu", (QString(DATADIR)+"/locale").local8Bit().data());
//	textdomain("kadu");

	a = new QApplication(argc, argv);

#ifdef VOICE_ENABLED
	voice_manager = new VoiceManager();
	sound_dsp = new SoundDsp();
#endif

	// ladowanie tlumaczenia
	QTranslator qt_qm(0);
	qt_qm.load(QString(DATADIR) + QString("/locale/qt_") + QTextCodec::locale(), ".");
	a->installTranslator(&qt_qm);
	QTranslator kadu_qm(0);
	kadu_qm.load(QString(DATADIR) + QString("/locale/kadu_") + QTextCodec::locale(), ".");
	a->installTranslator(&kadu_qm);

	QString dir;
	dir = QString(DATADIR) + "/apps/kadu/icons";
	icons = new IconsManager(dir);
	kadu = new Kadu(0, "Kadu");
	QPixmap *pix = icons->loadIcon("offline");
	kadu->setIcon(*pix);
	a->setMainWidget(kadu);
	if (!config_file.readNumEntry("General","UIN")) {
		QString path_;
		path_ = ggPath("");
		mkdir(path_.local8Bit(), 0700);
		path_.append("/history/");
		mkdir(path_.local8Bit(), 0700);
		switch (QMessageBox::information(kadu, "Kadu",
			qApp->translate("@default", QT_TR_NOOP("You don't have a config file.\nWhat would you like to do?")),
			qApp->translate("@default", QT_TR_NOOP("New UIN")),
			qApp->translate("@default", QT_TR_NOOP("Configure")),
			qApp->translate("@default", QT_TR_NOOP("Cancel")), 0, 1) ) {
			case 1: // Configure
				ConfigDialog::showConfigDialog(a);
				break;
			case 0: // Register
				Register *reg;
				reg = new Register;
				reg->show();
				break;
			case 2: // Nothing
				break;
			}
		kadu->setCaption(qApp->translate("@default", QT_TR_NOOP("Kadu: new user")));
		}

	own_description = defaultdescriptions.first();
	if (config_file.readNumEntry("General","DefaultStatus",GG_STATUS_NOT_AVAIL) != GG_STATUS_NOT_AVAIL && config_file.readNumEntry("General","DefaultStatus",GG_STATUS_NOT_AVAIL) != GG_STATUS_NOT_AVAIL_DESCR) {
		kadu->autohammer = true;
		kadu->setStatus(config_file.readNumEntry("General","DefaultStatus",GG_STATUS_NOT_AVAIL));
		}
#ifdef HAVE_OPENSSL
	sim_key_path = strdup(ggPath("keys/").local8Bit());
#endif

	QObject::connect(a, SIGNAL(aboutToQuit()), kadu, SLOT(quitApplication()));

//	ArtsSoundDevice *snd = new ArtsSoundDevice(11000, 8, 1);
/*	DspSoundDevice *snd = new DspSoundDevice(11000, 8, 1);
	char buf[65536];
	int file = open("/usr/share/licq/sounds/icq/Online.wav", O_RDONLY);
	int size = read(file, buf, 65536);
	close(file);
	fprintf(stderr, "KK main(): size=%d\n", size);
	snd->play(buf, size);*/

	return a->exec();
}
