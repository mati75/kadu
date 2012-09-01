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

#ifndef IGNORE_LINKS_DOM_VISITOR
#define IGNORE_LINKS_DOM_VISITOR

#include <QtCore/QScopedPointer>

#include "dom/dom-visitor.h"
#include "exports.h"

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class IgnoreLinksDomVisitor
 * @short Proxy visitor that hides all content inside A elements.
 * @author Rafał 'Vogel' Malinowski
 *
 * This visitor acts like proxy on top of other visitor. It hides content of A elements before proxied visitor.
 * It also takes care of deleting proxied visitor.
 */
class KADUAPI IgnoreLinksDomVisitor : public DomVisitor
{
	QScopedPointer<DomVisitor> Visitor;
	int LinksDepth;

public:
	/**
	 * @short Create new IgnoreLinksDomVisitor over visitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitor this visitor will get all data about processed DOM document with exception of content of A elements
	 *
	 * IgnoreLinksDomVisitor takes care of deleting visitor instance when not needed anymore. Do not delete it manually.
	 */
	explicit IgnoreLinksDomVisitor(DomVisitor *visitor);
	virtual ~IgnoreLinksDomVisitor();

	virtual void visit(QDomText textNode);
	virtual void beginVisit(QDomElement elementNode);
	virtual void endVisit(QDomElement elementNode);

};

/**
 * @}
 */

#endif // IGNORE_LINKS_DOM_VISITOR