/*
 * %kadu copyright begin%
 * 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef SQL_INITIALIZER_H
#define SQL_INITIALIZER_H

#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

class SqlInitializer : public QObject
{
	Q_OBJECT

	QSqlDatabase Database;

	bool isCopyingNeeded();
	void copyHistoryFile();

	void initDatabase();
	void initTables();
	void initIndexes();
	void initKaduSchemaTable();
	void initKaduMessagesTable();
	void initKaduStatusesTable();
	void initKaduSmsTable();

	quint16 loadSchemaVersion();

	void importVersion1Schema();

public:
	explicit SqlInitializer(QObject *parent = 0);
	virtual ~SqlInitializer();

public slots:
	void initialize();

signals:
	void initialized();
	void databaseReady(bool ok);

	void importStarted();
	void importFinished();

};

#endif // SQL_INITIALIZER_H