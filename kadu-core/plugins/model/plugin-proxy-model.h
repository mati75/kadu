/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#ifndef PLUGIN_PROXY_MODEL_H
#define PLUGIN_PROXY_MODEL_H

#include "model/categorized-sort-filter-proxy-model.h"

class PluginListWidget;


class ProxyModel : public CategorizedSortFilterProxyModel
{
        PluginListWidget *pluginSelector_d;

public:
        ProxyModel(PluginListWidget *pluginSelector_d, QObject *parent = 0);
        ~ProxyModel();

protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        virtual bool subSortLessThan(const QModelIndex &left, const QModelIndex &right) const;

};

#endif