/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include "plugins-manager.h"

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "gui/windows/plugin-error-dialog.h"
#include "misc/kadu-paths.h"
#include "plugins/dependency-graph/plugin-dependency-cycle-exception.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugins/plugin-activation-action.h"
#include "plugins/plugin-activation-error-exception.h"
#include "plugins/plugin-activation-error-handler.h"
#include "plugins/plugin-activation-service.h"
#include "plugins/plugin-info-finder.h"
#include "plugins/plugin-info-reader-exception.h"
#include "plugins/plugin-info-reader.h"
#include "plugins/plugin-info-repository.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin-root-component.h"
#include "plugins/plugin-state-service.h"
#include "plugins/plugin-state-storage.h"
#include "plugins/plugin-state-storage-09.h"
#include "plugins/plugins-common.h"
#include "plugin-state-storage.h"
#include "storage/storage-point-factory.h"
#include "debug.h"

#include <QtCore/QDir>
#include <QtCore/QTimer>

PluginsManager::PluginsManager(QObject *parent) :
		QObject{parent}
{
}

PluginsManager::~PluginsManager()
{
}

void PluginsManager::setPluginInfoFinder(PluginInfoFinder *pluginInfoFinder)
{
	m_pluginInfoFinder = pluginInfoFinder;
}

void PluginsManager::setPluginActivationErrorHandler(PluginActivationErrorHandler *pluginActivationErrorHandler)
{
	m_pluginActivationErrorHandler = pluginActivationErrorHandler;
}

void PluginsManager::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

void PluginsManager::setPluginInfoRepository(PluginInfoRepository *pluginInfoRepository)
{
	m_pluginInfoRepository = pluginInfoRepository;
}

void PluginsManager::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

void PluginsManager::setStoragePointFactory(StoragePointFactory *storagePointFactory)
{
	m_storagePointFactory = storagePointFactory;
}

void PluginsManager::initialize()
{
	loadPluginInfos();
	loadPluginStates();
	prepareDependencyGraph();
}

void PluginsManager::loadPluginInfos()
{
	if (!m_pluginInfoFinder || !m_pluginInfoRepository)
		return;

	auto pluginInfos = std::move(m_pluginInfoFinder.data()->readPluginInfos(KaduPaths::instance()->dataPath() + QLatin1String{"plugins"}));
	m_pluginInfoRepository.data()->setPluginInfos(std::move(pluginInfos));
}

void PluginsManager::loadPluginStates()
{
	if (!m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory.data()->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	bool importedFrom09 = storagePoint->loadAttribute("imported_from_09", false);
	storagePoint->storeAttribute("imported_from_09", true);

	auto pluginStates = loadPluginStates(storagePoint.get(), importedFrom09);
	m_pluginStateService.data()->setPluginStates(pluginStates);
}

QMap<QString, PluginState> PluginsManager::loadPluginStates(StoragePoint *storagePoint, bool importedFrom09) const
{
	return importedFrom09
			? PluginStateStorage{}.load(*storagePoint)
			: m_pluginInfoRepository
					? PluginStateStorage09{}.load(*m_pluginInfoRepository.data())
					: QMap<QString, PluginState>{};
}

void PluginsManager::storePluginStates()
{
	if (!m_pluginStateService || !m_storagePointFactory)
		return;

	auto storagePoint = m_storagePointFactory.data()->createStoragePoint(QLatin1String{"Plugins"});
	if (!storagePoint)
		return;

	auto pluginStateStorage = PluginStateStorage{};
	auto pluginStates = m_pluginStateService.data()->pluginStates();
	pluginStateStorage.store(*storagePoint.get(), pluginStates);
}

void PluginsManager::prepareDependencyGraph()
{

	auto dependencyGraph = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*m_pluginInfoRepository.data());
	auto pluginsInDependencyCycle = dependencyGraph.get()->findPluginsInDependencyCycle();
	for (auto &pluginInDependency : pluginsInDependencyCycle)
		m_pluginInfoRepository.data()->removePluginInfo(pluginInDependency);

	m_pluginDependencyDAG = Core::instance()->pluginDependencyGraphBuilder()->buildGraph(*m_pluginInfoRepository.data());
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all protocols plugins that are enabled.
 *
 * This method activates all plugins with type "protocol" that are either enabled (PluginState::Enabled)
 * or new (PluginState::New) with attribute "load by default" set. This method is generally called before
 * any other activation to ensure that all protocols and accounts are available for other plugins.
 */
void PluginsManager::activateProtocolPlugins()
{
	auto saveList = false;

	for (const auto &pluginName : pluginsToActivate([](const PluginInfo &pluginInfo){ return pluginInfo.type() == "protocol"; }))
	{
		auto activationReason = (m_pluginStateService.data()->pluginState(pluginName) == PluginState::Enabled)
				? PluginActivationReason::KnownDefault
				: PluginActivationReason::Other;

		if (!activatePluginWithDependencies(pluginName, activationReason))
			saveList = true;
	}

	// if not all plugins were loaded properly
	// save the list of plugins
	if (saveList)
	{
		storePluginStates();
		ConfigurationManager::instance()->flush();
	}
}

QVector<QString> PluginsManager::pluginsToActivate(std::function<bool(const PluginInfo &)> filter) const
{
	auto result = QVector<QString>{};

	if (!m_pluginInfoRepository)
		return result;

	for (auto const &pluginInfo : m_pluginInfoRepository.data())
		if (filter(pluginInfo) && shouldActivate(pluginInfo.name()))
			result.append(pluginInfo.name());

	return result;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all plugins that are enabled.
 *
 * This method activates all plugins that are either enabled (PluginState::Enabled) or new (PluginState::New)
 * with attribute "load by default" set. If given enabled plugin is no longer available replacement plugin is searched
 * (by checking Plugin::replaces()). Any found replacement plugin is activated.
 */
void PluginsManager::activatePlugins()
{
	if (!m_pluginActivationService || !m_pluginInfoRepository || !m_pluginStateService)
		return;

	auto saveList = false;

	for (const auto &pluginName : pluginsToActivate())
	{
		auto activationReason = (m_pluginStateService.data()->pluginState(pluginName) == PluginState::Enabled)
				? PluginActivationReason::KnownDefault
				: PluginActivationReason::Other;

		if (!activatePluginWithDependencies(pluginName, activationReason))
			saveList = true;
	}

	for (auto const &pluginToReplaceInfo : m_pluginInfoRepository.data())
	{
		if (m_pluginActivationService.data()->isActive(pluginToReplaceInfo.name()) || m_pluginStateService.data()->pluginState(pluginToReplaceInfo.name()) != PluginState::Enabled)
			continue;

		auto replacementPlugin = findReplacementPlugin(pluginToReplaceInfo.name());
		if (m_pluginStateService.data()->pluginState(replacementPlugin) == PluginState::New)
			if (activatePluginWithDependencies(replacementPlugin, PluginActivationReason::Other))
				saveList = true; // list has changed
	}

	// if not all plugins were loaded properly or new plugin was added
	// save the list of plugins
	if (saveList)
		ConfigurationManager::instance()->flush();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns true if this plugin should be activated.
 * @return true if this plugin should be activated
 *
 * Module should be activated only if:
 * <ul>
 *   <li>it is valid (has .desc file associated with it)
 *   <li>is either PluginState::Enabled or PluginState::New with PluginInfo::loadByDefault() set to true
 * </ul>
 */
bool PluginsManager::shouldActivate(const QString &pluginName) const noexcept
{
	if (!m_pluginStateService)
		return false;

	auto state = m_pluginStateService.data()->pluginState(pluginName);

	if (PluginState::Enabled == state)
		return true;
	if (PluginState::Disabled == state)
		return false;

	if (!m_pluginInfoRepository || !m_pluginInfoRepository.data()->hasPluginInfo(pluginName))
		return false;

	return m_pluginInfoRepository.data()->pluginInfo(pluginName).loadByDefault();
}

QString PluginsManager::findReplacementPlugin(const QString &pluginToReplace) const noexcept
{
	if (!m_pluginInfoRepository)
		return {};

	for (auto const &pluginInfo : m_pluginInfoRepository.data())
		if (m_pluginInfoRepository.data()->pluginInfo(pluginInfo.name()).replaces().contains(pluginToReplace))
			return pluginInfo.name();

	return {};
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activate all plugins that are enabled.
 *
 * This method deactivated all active plugins. First iteration of deactivation check Plugin::usageCounter() value
 * to check if given plugin can be safely removed (no other active plugins depends on it). This procedure is
 * performed for all active plugins until no more plugins can be deactivated. Then second iteration is performed.
 * This time no checks are performed.
 */
void PluginsManager::deactivatePlugins()
{
	if (!m_pluginActivationService)
		return;

	for (auto const &pluginName : m_pluginActivationService.data()->activePlugins())
	{
		kdebugm(KDEBUG_INFO, "plugin: %s\n", qPrintable(pluginName));
		deactivatePluginWithDependents(pluginName, PluginDeactivationReason::Exiting);
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns name of active plugin that provides given feature.
 * @param feature feature to search
 * @return name of active plugins that conflicts provides given feature.
 */
QString PluginsManager::findActiveProviding(const QString &feature) const
{
	if (feature.isEmpty() || !m_pluginActivationService || !m_pluginInfoRepository)
		return {};

	for (auto const &activePluginName : m_pluginActivationService.data()->activePlugins())
		if (m_pluginInfoRepository.data()->hasPluginInfo(activePluginName))
			if (m_pluginInfoRepository.data()->pluginInfo(activePluginName).provides() == feature)
				return activePluginName;

	return {};
}

QVector<QString> PluginsManager::allDependencies(const QString &pluginName) noexcept
{
	return m_pluginDependencyDAG ? m_pluginDependencyDAG.get()->findDependencies(pluginName) : QVector<QString>{};
}

QVector<QString> PluginsManager::allDependents(const QString &pluginName) noexcept
{
	return m_pluginDependencyDAG ? m_pluginDependencyDAG.get()->findDependents(pluginName) : QVector<QString>{};
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activates given plugin and all its dependencies.
 * @param plugin plugin to activate
 * @param reason plugin activation reason
 * @return true, if plugin was successfully activated
 *
 * This method activates given plugin and all its dependencies. Plugin can be activated only when no conflict
 * is found and all dependencies can be activated. In other case false is returned and plugin will not be activated.
 * Please note that no dependency plugin activated in this method will be automatically deactivated if
 * this method fails, so list of active plugins can be changed even if plugin could not be activated.
 *
 * \p reason will be passed to Plugin::activate() method.
 *
 * After successfull activation all dependencies are locked using incDependenciesUsageCount() and cannot be
 * deactivated without deactivating plugin. Plugin::usageCounter() of dependencies is increased.
 */
bool PluginsManager::activatePluginWithDependencies(const QString &pluginName, PluginActivationReason reason)
{
	if (!m_pluginActivationService || !m_pluginInfoRepository)
		return false;

	if (m_pluginActivationService.data()->isActive(pluginName))
		return true;

	if (m_pluginInfoRepository.data()->hasPluginInfo(pluginName))
	{
		auto conflict = findActiveProviding(m_pluginInfoRepository.data()->pluginInfo(pluginName).provides());
		if (!conflict.isEmpty())
		{
			if (m_pluginActivationErrorHandler)
				m_pluginActivationErrorHandler.data()->handleActivationError(pluginName, tr("Plugin %1 conflicts with: %2").arg(pluginName, conflict), reason);
			return false;
		}
	}

	try
	{
		auto dependencies = allDependencies(pluginName);
		auto actions = QVector<PluginActivationAction>{};
		for (auto dependency : dependencies)
		{
			auto state = m_pluginStateService
					? m_pluginStateService.data()->pluginState(dependency)
					: PluginState::Disabled;

			auto activationReason = (state == PluginState::Enabled)
					? PluginActivationReason::KnownDefault
					: PluginActivationReason::Other;

			actions.append({dependency, activationReason, PluginState::New == state});
		}

		try
		{
			for (auto const &action : actions)
			{
				m_pluginActivationService.data()->performActivationAction(action);
				/* This is perfectly intentional. We have to set state to either enabled or disabled, as new
				 * means that it was never loaded. If the only reason to load the plugin was because some other
				 * plugin depended upon it, set state to disabled as we don't want that plugin to be loaded
				 * next time when its reverse dependency will not be loaded. Otherwise set state to enabled.
				 */
				if (m_pluginStateService)
					m_pluginStateService.data()->setPluginState(action.pluginName(), PluginState::Disabled);
			}
		}
		catch (PluginActivationErrorException &e)
		{
			if (m_pluginActivationErrorHandler)
				m_pluginActivationErrorHandler.data()->handleActivationError(e.pluginName(), e.errorMessage(), PluginActivationReason::Other);
			return false;
		}

		try
		{
			if (m_pluginStateService)
			{
				auto state = m_pluginStateService.data()->pluginState(pluginName);
				m_pluginActivationService.data()->performActivationAction({pluginName, reason, PluginState::New == state});
				m_pluginStateService.data()->setPluginState(pluginName, PluginState::Enabled);
			}
		}
		catch (PluginActivationErrorException &e)
		{
			if (m_pluginActivationErrorHandler)
				m_pluginActivationErrorHandler.data()->handleActivationError(e.pluginName(), e.errorMessage(), reason);
			return false;
		}
	}
	catch (PluginDependencyCycleException &e)
	{
		Q_UNUSED(e); // TODO: log? rethrow and use in GUI?

		return false;
	}

	return true;
}

void PluginsManager::deactivatePluginWithDependents(const QString &pluginName, PluginDeactivationReason reason)
{
	if (!m_pluginActivationService)
		return;

	if (!m_pluginActivationService.data()->isActive(pluginName))
		return;

	auto dependents = allDependents(pluginName);
	auto actions = QVector<PluginActivationAction>{};
	for (auto dependent : dependents)
		actions.append({dependent, reason});
	actions.append({pluginName, reason});

	for (auto const &action : actions)
	{
		m_pluginActivationService.data()->performActivationAction(action);
		if (PluginDeactivationReason::UserRequest == reason && m_pluginStateService)
			m_pluginStateService.data()->setPluginState(action.pluginName(), PluginState::Disabled);
	}
}

#include "moc_plugins-manager.cpp"
