/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "echo.h"

#include "events.h"
#include "message_box.h"
#include "gadu.h"

extern "C" void init_module()
{
	echo=new Echo();
	QObject::connect(&event_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		echo,SLOT(chatReceived(UinsList,const QString&,time_t)));
}

extern "C" void close_module()
{
	QObject::disconnect(&event_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		echo,SLOT(chatReceived(UinsList,const QString&,time_t)));
	delete echo;
}

Echo::Echo() : QObject()
{
	MessageBox::msg("Echo started");
}

Echo::~Echo()
{
	MessageBox::msg("Echo stopped");
}

void Echo::chatReceived(UinsList senders,const QString& msg,time_t time)
{
	QString resp=QString("KADU ECHO: ")+msg;
	gadu->sendMessage(senders,resp);
}

Echo* echo;
