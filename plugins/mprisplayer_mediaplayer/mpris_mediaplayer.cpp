/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

/* Dorr: Amarok2 implements the MPRIS standard so this module should work
 * also with Audacious, BMPx, DragonPLayer, VLC, XMMS2 and maybe others.
 *
 * See http://mpris.org/ for more details about the standard
 */

#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpris_controller.h"
#include "mpris_mediaplayer.h"

MPRISMediaPlayer::MPRISMediaPlayer(QObject *parent) :
		PlayerCommands(parent)
{
	controller = new MPRISController(service, this);

	if (name == "Audacious")
		MediaPlayer::instance()->setInterval(5);
	else
		MediaPlayer::instance()->setInterval(0);
}

MPRISMediaPlayer::~MPRISMediaPlayer()
{
}

void MPRISMediaPlayer::setService(const QString &service)
{
	this->service = service;

	if (controller)
		delete controller;

	controller = new MPRISController(this->service, this);
}

void MPRISMediaPlayer::setName(const QString &name)
{
	this->name = name;

	if (name == "Audacious")
		MediaPlayer::instance()->setInterval(5);
	else
		MediaPlayer::instance()->setInterval(0);
}

void MPRISMediaPlayer::send(QString obj, QString func)
{
	if (!service.isEmpty())
	{
		QDBusInterface mprisApp(service, obj, "org.mpris.MediaPlayer2.Player");
		mprisApp.call(func);
	}
}

// PlayerInfo

QString MPRISMediaPlayer::getPlayerName()
{
	return name;
}

QString MPRISMediaPlayer::getPlayerVersion()
{
	if (controller)
		return controller->identity();
	else
		return QString();
}

// TrackList

QStringList MPRISMediaPlayer::getPlayListTitles()
{
	QStringList result;
	if (!isPlaying())
		return result;

	QList<TrackInfo> tracks = controller->getTrackList();

	foreach (const TrackInfo &track, tracks)
		result << track.title();

	return result;
}

QStringList MPRISMediaPlayer::getPlayListFiles()
{
	QStringList result;
	if (!isPlaying())
		return result;

	QList<TrackInfo> tracks = controller->getTrackList();

	foreach (const TrackInfo &track, tracks)
		result << track.file();

	return result;
}

QString MPRISMediaPlayer::getTitle()
{
	if (!isPlaying())
		return QString();

	return controller->track().title();
}

QString MPRISMediaPlayer::getAlbum()
{
	if (!isPlaying())
		return QString();

	return controller->track().album();
}

QString MPRISMediaPlayer::getArtist()
{
	if (!isPlaying())
		return QString();

	return controller->track().artist();
}

QString MPRISMediaPlayer::getFile()
{
	if (!isPlaying())
		return QString();

	return controller->track().file();
}

int MPRISMediaPlayer::getLength()
{
	if (!isPlaying())
		return 0;

	return controller->track().length();
}

// Player

int MPRISMediaPlayer::getCurrentPos()
{
	if (!isPlaying())
		return 0;

	return controller->getCurrentPosition();
}

void MPRISMediaPlayer::nextTrack()
{
	send("/org/mpris/MediaPlayer2", "Next");
}

void MPRISMediaPlayer::prevTrack()
{
	send("/org/mpris/MediaPlayer2", "Previous");
}

void MPRISMediaPlayer::play()
{
	send("/org/mpris/MediaPlayer2", "Play");
}

void MPRISMediaPlayer::stop()
{
	send("/org/mpris/MediaPlayer2", "Stop");
}

void MPRISMediaPlayer::pause()
{
	send("/org/mpris/MediaPlayer2", "Pause");
}

void MPRISMediaPlayer::setVolume(int vol)
{
	if (controller)
		controller->setVolume(vol);
}

void MPRISMediaPlayer::incrVolume()
{
	if (!controller)
		return;

	int vol = controller->getVolume();
	if (vol < 100)
		vol += 2;

	if (vol > 100)
		vol = 100;

	controller->setVolume(vol);
}

void MPRISMediaPlayer::decrVolume()
{
	if (!controller)
		return;

	int vol = controller->getVolume();
	if (vol > 0)
		vol -= 2;

	if (vol < 0)
		vol = 0;

	controller->setVolume(vol);
}

bool MPRISMediaPlayer::isPlaying()
{
	if (!controller)
		return false;

	return MPRISController::StatusPlaying == controller->status();
}

bool MPRISMediaPlayer::isActive()
{
	if (!controller)
		return false;

	return controller->active();
}
