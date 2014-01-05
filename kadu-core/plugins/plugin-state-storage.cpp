/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-state-storage.h"

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include <QtCore/QVector>
#include <QtXml/QDomElement>

QMap<QString, PluginState> PluginStateStorage::load(StoragePoint &storagePoint) const
{
	auto result = QMap<QString, PluginState>();
	auto elements = storagePoint.storage()->getNodes(storagePoint.point(), QLatin1String("Plugin"));
	for (const auto &element : elements)
	{
		auto name = element.attribute("name");
		auto state = stringToState(storagePoint.storage()->getTextNode(element, QLatin1String("State")));
		result.insert(name, state);
	}

	return result;
}

PluginState PluginStateStorage::stringToState(const QString &string) const
{
	if (string == QLatin1String{"Loaded"})
		return PluginState::Enabled;
	else if (string == QLatin1String{"NotLoaded"})
		return PluginState::Disabled;
	else
		return PluginState::New;
}

void PluginStateStorage::store(StoragePoint &storagePoint, const QMap<QString, PluginState> &pluginStates) const
{
	storagePoint.storage()->removeChildren(storagePoint.point());

	for (const auto &name : pluginStates.keys())
	{
		auto stateString = stateToString(pluginStates.value(name));
		if (!stateString.isEmpty())
		{
			auto node = storagePoint.storage()->getNamedNode(storagePoint.point(), QLatin1String{"Plugin"}, name, XmlConfigFile::ModeAppend);
			storagePoint.storage()->appendTextNode(node, QLatin1String{"State"}, stateString);
		}
	}
}

QString PluginStateStorage::stateToString(PluginState state) const
{
	switch (state)
	{
		case PluginState::Enabled:
			return QLatin1String{"Loaded"};
		case PluginState::Disabled:
			return QLatin1String{"NotLoaded"};
		default:
			return {};
	}
}