/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>

#include "misc/misc.h"

#include "languages-manager.h"

QMap<QString, QString> LanguagesManager::Languages;

void LanguagesManager::loadLanguages()
{
	QDir tranlationsDir(dataPath("kadu/translations"));

	QStringList languagesFilter;
	languagesFilter << "*.language";
	QFileInfoList languages = tranlationsDir.entryInfoList(languagesFilter, QDir::Files);

	foreach (const QFileInfo &languageFileInfo, languages)
	{
		QFile languageFile(languageFileInfo.filePath());
		if (languageFile.open(QIODevice::ReadOnly))
		{
			Languages.insert(languageFileInfo.fileName().left(2), QString::fromUtf8(languageFile.readAll()).trimmed());
			languageFile.close();
		}
	}
}

const QMap<QString, QString> & LanguagesManager::languages()
{
	if (Languages.isEmpty())
		loadLanguages();

	return Languages;
}