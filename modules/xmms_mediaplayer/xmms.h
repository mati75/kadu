#ifndef XMMS_H
#define XMMS_H

#include <QtCore/QObject>

#include "../mediaplayer/player_info.h"
#include "../mediaplayer/player_commands.h"

class XmmsMediaPlayer : public PlayerInfo, public PlayerCommands
{
	Q_OBJECT

	static QString convert(QString str);

public:
	XmmsMediaPlayer();
	~XmmsMediaPlayer();

	QString getTitle(int position = -1);
	QString getAlbum(int position = -1);
	QString getArtist(int position = -1);
	QString getFile(int position = -1);
	int getLength(int position = -1);
	int getCurrentPos();
	bool isPlaying();
	bool isActive();
	QStringList getPlayListTitles();
	QStringList getPlayListFiles();
	uint getPlayListLength();
	QString getPlayerName();
	QString getPlayerVersion();

	void nextTrack();
	void prevTrack();
	void play();
	void stop();
	void pause();
	void setVolume(int vol);
	void incrVolume();
	void decrVolume();

};

#endif
