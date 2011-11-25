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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtSql/QSqlError>

#include "gui/windows/message-dialog.h"
#include "misc/path-conversion.h"

#include "storage/history-sql-storage.h"

#include "sql-initializer.h"

#define SCHEMA_VERSION "2"
#define OLD_HISTORY_FILE "history/history.db"
#define HISTORY_FILE "history1.db"

SqlInitializer::SqlInitializer(QObject *parent) :
		QObject(parent)
{
}

SqlInitializer::~SqlInitializer()
{
}

void SqlInitializer::initialize()
{
	initDatabase();

	bool ok = Database.isOpen();
	Database.close();

	emit databaseReady(ok);
	emit initialized();

	deleteLater();
}

void SqlInitializer::importVersion1Schema()
{
	QSqlQuery query(Database);
	Database.transaction();

	QStringList queries;
	queries
	        << "PRAGMA foreign_keys = ON;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_rowid;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_send_time;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date_send_time;"
	        << "DROP INDEX IF EXISTS kadu_statuses_contact;"
	        << "DROP INDEX IF EXISTS kadu_statuses_contact_time;"
	        << "DROP INDEX IF EXISTS kadu_statuses_contact_time_date;"
	        << "DROP INDEX IF EXISTS kadu_sms_receipient;"
	        << "DROP INDEX IF EXISTS kadu_sms_receipient_time;"
	        << "DROP INDEX IF EXISTS kadu_sms_receipient_time_date;"

	        << "CREATE TABLE kadu_chats (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid VARCHAR(16));"
	        << "CREATE TABLE kadu_contacts (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid VARCHAR(16));"
	        << "CREATE TABLE kadu_message_contents (id INTEGER PRIMARY KEY AUTOINCREMENT, content TEXT, attributes VARCHAR(25));"
	        << "CREATE TABLE kadu_dates (id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER);"

	        << "ALTER TABLE kadu_messages RENAME TO kadu_messages_old;"

	        << "INSERT INTO kadu_chats (uuid) SELECT DISTINCT chat FROM kadu_messages_old;"
	        << "INSERT INTO kadu_contacts (uuid) SELECT DISTINCT sender FROM kadu_messages_old;"
	        << "INSERT INTO kadu_dates (date) SELECT DISTINCT REPLACE(substr(receive_time,0,11), '-', '') FROM kadu_messages_old;"
	        << "INSERT INTO kadu_message_contents (content) SELECT DISTINCT content FROM kadu_messages_old;"

	        << "CREATE TABLE kadu_messages ("
	           "chat_id INTEGER,"
	           "contact_id INTEGER,"
	           "date_id INTEGER,"
	           "send_time TIMESTAMP,"
	           "receive_time TIMESTAMP,"
	           "content_id INTEGER,"
	           "is_outgoing BOOL, "
	           "FOREIGN KEY (chat_id) REFERENCES kadu_chats(id), FOREIGN KEY (contact_id) REFERENCES kadu_contacts(id), "
	           "FOREIGN KEY (date_id) REFERENCES kadu_dates(id), FOREIGN KEY (content_id) REFERENCES kadu_message_contents(id));"

	        << "INSERT INTO kadu_messages ("
	           "chat_id, contact_id, date_id, send_time, receive_time, content_id, is_outgoing) "
	           "SELECT (SELECT id FROM kadu_chats WHERE uuid=old.chat LIMIT 1), (SELECT id FROM kadu_contacts WHERE uuid=old.sender LIMIT 1), "
	           "(SELECT id FROM kadu_dates WHERE date = REPLACE(substr(old.receive_time,0,11), '-', '')), send_time, receive_time, "
	           "(SELECT id FROM kadu_message_contents WHERE content=old.content LIMIT 1), substr(attributes, 10, 1) FROM kadu_messages_old old;"

	        << "DROP TABLE kadu_messages_old;"

	        << "CREATE TABLE schema_version(version id INTEGER);"
	        << "DELETE FROM schema_version;"
	        << "INSERT INTO schema_version (version) VALUES (" SCHEMA_VERSION ");";

	foreach (const QString &queryString, queries)
	{
		query.prepare(queryString);
		query.setForwardOnly(true);
		query.exec();
	}

	Database.commit();

	query.prepare("VACUUM;");
	query.exec();

	initIndexes();

	Database.commit();
}

bool SqlInitializer::isCopyingNeeded()
{
	QFileInfo scheme1FileInfo(profilePath(HISTORY_FILE));
	if (scheme1FileInfo.exists())
		return false;

	QFileInfo scheme0FileInfo(profilePath(OLD_HISTORY_FILE));
	return scheme0FileInfo.exists();
}

void SqlInitializer::copyHistoryFile()
{
	QFileInfo scheme1FileInfo(profilePath(HISTORY_FILE));
	if (scheme1FileInfo.exists())
		return;

	QFileInfo scheme0FileInfo(profilePath(OLD_HISTORY_FILE));
	if (scheme0FileInfo.exists())
		QFile::copy(scheme0FileInfo.absoluteFilePath(), scheme1FileInfo.absoluteFilePath());
}

void SqlInitializer::initDatabase()
{
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
		return;

	if (QSqlDatabase::contains("kadu-history"))
	{
		if (Database.isOpen())
			Database.close();
		QSqlDatabase::removeDatabase("kadu-history");
	}

	QDir historyDir(profilePath("history"));
	if (!historyDir.exists())
		historyDir.mkpath(profilePath("history"));

	bool importStartedEmited = false;
	if (isCopyingNeeded())
	{
		emit importStarted();
		importStartedEmited = true;
		copyHistoryFile();
	}

	Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	Database.setDatabaseName(profilePath(HISTORY_FILE));

	if (!Database.open())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), Database.lastError().text());
		return;
	}

	quint16 storedSchemaVersion = loadSchemaVersion();
	switch (storedSchemaVersion)
	{
		case 0:
			initTables();
			initIndexes();
			break;
		case 1:
			if (!importStartedEmited)
				emit importStarted();
			importVersion1Schema();
			break;
		default:
			break; // no need to import
	}

	if (importStartedEmited)
		emit importFinished();

	Database.transaction();
}

quint16 SqlInitializer::loadSchemaVersion()
{
	// no schema_version table
	if (!Database.tables().contains("schema_version"))
	{
		if (!Database.tables().contains("kadu_messages"))
			return 0; // first run of module, so no version available
		else
			return 1; // first slow version of SQL module
	}

	QSqlQuery query(Database);
	query.prepare("SELECT version FROM schema_version");

	if (!query.exec()) // looks like broken database, we should make a fatal error or something now
		return 0;

	if (!query.next()) // looks like broken database, we should make a fatal error or something now
		return 0;

	return query.value(0).toUInt();
}

void SqlInitializer::initTables()
{
	initKaduSchemaTable();
	initKaduMessagesTable();
	initKaduStatusesTable();
	initKaduSmsTable();
}

void SqlInitializer::initKaduSchemaTable()
{
	QSqlQuery query(Database);

	query.prepare("CREATE TABLE schema_version(version id INTEGER);");
	query.exec();

	query.prepare("DELETE FROM schema_version;");
	query.exec();

	query.prepare("INSERT INTO schema_version (version) VALUES (" SCHEMA_VERSION ");");
	query.exec();
}

void SqlInitializer::initKaduMessagesTable()
{
	QSqlQuery query(Database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	query.exec();

	query.prepare("PRAGMA synchronous = OFF;");
	query.exec();

	query.prepare("PRAGMA foreign_keys = ON;");
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_chats ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"uuid VARCHAR(16));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_contacts ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"uuid VARCHAR(16));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_dates ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"date VARCHAR(8));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_message_contents ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"content TEXT,"
			"attributes VARCHAR(25));"
	);
	query.exec();

	query.prepare(
			"CREATE TABLE kadu_messages ("
			"chat_id INTEGER,"
			"contact_id INTEGER,"
			"date_id INTEGER,"
			"send_time TIMESTAMP,"
			"receive_time TIMESTAMP,"
			"content_id INTEGER,"
			"is_outgoing BOOL,"
			"FOREIGN KEY (chat_id) REFERENCES kadu_chats(id),"
			"FOREIGN KEY (contact_id) REFERENCES kadu_contacts(id),"
			"FOREIGN KEY (date_id) REFERENCES kadu_dates(id),"
			"FOREIGN KEY (content_id) REFERENCES kadu_message_contents(id));"
	);
	query.exec();
}

void SqlInitializer::initKaduStatusesTable()
{
	QSqlQuery query(Database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	query.exec();

	query.prepare("PRAGMA synchronous = OFF;");
	query.exec();

	query.prepare(
		"CREATE TABLE kadu_statuses ("
			"contact VARCHAR(255),"
			"status VARCHAR(255),"
			"set_time TIMESTAMP,"
			"description TEXT);"
	);
	query.exec();
}

void SqlInitializer::initKaduSmsTable()
{
	QSqlQuery query(Database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	query.exec();

	query.prepare("PRAGMA synchronous = OFF;");
	query.exec();

	query.prepare(
		"CREATE TABLE kadu_sms ("
			"receipient VARCHAR(255),"
			"send_time TIMESTAMP,"
			"content TEXT);"
	);
	query.exec();
}

void SqlInitializer::initIndexes()
{
	QSqlQuery query(Database);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_chat_pk ON kadu_chats (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_chat_uuid ON kadu_chats (uuid)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_contact_pk ON kadu_contacts (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_contact_uuid ON kadu_contacts (uuid)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_dates_pk ON kadu_dates (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_dates_val ON kadu_dates (date)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_content_pk ON kadu_message_contents (id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_chat ON kadu_messages (chat_id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_contact ON kadu_messages (contact_id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_date ON kadu_messages (date_id)");
	query.exec();

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_content ON kadu_messages (content_id)");
	query.exec();
}