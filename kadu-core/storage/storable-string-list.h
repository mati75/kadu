/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORABLE_STRING_LIST_H
#define STORABLE_STRING_LIST_H

#include <QtCore/QStringList>

#include "storage/storable-object.h"

class StorableStringList : protected QStringList, public StorableObject
{

protected:
	virtual void load();

public:
	StorableStringList();

	virtual QString storageItemNodeName() = 0;
	virtual void store();

	const QStringList content() const;

};

#endif // STORABLE_STRING_LIST_H
