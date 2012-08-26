/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_PREFIX_TREE_BUILDER_H
#define EMOTICON_PREFIX_TREE_BUILDER_H

#include <QtCore/QScopedPointer>

#include "emoticons/emoticon-prefix-tree.h"

class QChar;

class EmoticonPrefixTreeBuilder
{
	QScopedPointer<EmoticonPrefixTree> Root;

	QChar extractLetter(QChar c);

public:
	EmoticonPrefixTreeBuilder();

	void addEmoticon(const Emoticon &emoticon);

	EmoticonPrefixTree * tree();

};

#endif // EMOTICON_PREFIX_TREE_BUILDER_H
