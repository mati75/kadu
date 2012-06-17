/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "services/jabber-vcard-service.h"
#include "utils/vcard-factory.h"
#include "jabber-protocol.h"

#include "jabber-personal-info-service.h"

JabberPersonalInfoService::JabberPersonalInfoService(QObject *parent) :
		PersonalInfoService(parent)
{
}

JabberPersonalInfoService::~JabberPersonalInfoService()
{
}

void JabberPersonalInfoService::setVCardService(XMPP::JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

void JabberPersonalInfoService::fetchPersonalInfo(const QString &id)
{
	CurrentBuddy = Buddy::create();
	if (VCardService)
		VCardService.data()->fetch(id, this);
}

void JabberPersonalInfoService::vcardFetched(bool ok, const XMPP::VCard &vcard)
{
	if (!ok)
		return;

	CurrentBuddy.setNickName(vcard.nickName());
	CurrentBuddy.setFirstName(vcard.fullName());
	CurrentBuddy.setFamilyName(vcard.familyName());
	QDate bday = QDate::fromString(vcard.bdayStr(), "yyyy-MM-dd");
	if (bday.isValid() && !bday.isNull())
		CurrentBuddy.setBirthYear(bday.year());

	if (!vcard.addressList().isEmpty())
		CurrentBuddy.setCity(vcard.addressList().at(0).locality);
	if (!vcard.emailList().isEmpty())
		CurrentBuddy.setEmail(vcard.emailList().at(0).userid);
	CurrentBuddy.setWebsite(vcard.url());

	emit personalInfoAvailable(CurrentBuddy);
}

void JabberPersonalInfoService::updatePersonalInfo(const QString &id, Buddy buddy)
{
	if (!VCardService)
	{
		emit personalInfoUpdated(false);
		return;
	}

	CurrentBuddy = buddy;

	XMPP::Jid jid = XMPP::Jid(id);
	XMPP::VCard vcard;
	vcard.setFullName(CurrentBuddy.firstName());
	vcard.setNickName(CurrentBuddy.nickName());
	vcard.setFamilyName(CurrentBuddy.familyName());
	QDate birthday;
	birthday.setDate(CurrentBuddy.birthYear(), 1, 1);
	vcard.setBdayStr(birthday.toString("yyyy-MM-dd"));

	XMPP::VCard::Address addr;
	XMPP::VCard::AddressList addrList;
	addr.locality = CurrentBuddy.city();
	addrList.append(addr);
	vcard.setAddressList(addrList);

	XMPP::VCard::Email email;
	XMPP::VCard::EmailList emailList;
	email.userid = CurrentBuddy.email();
	emailList.append(email);
	vcard.setEmailList(emailList);

	vcard.setUrl(CurrentBuddy.website());

	VCardService.data()->update(jid, vcard, this);
}

void JabberPersonalInfoService::vcardUpdated(bool ok)
{
	emit personalInfoUpdated(ok);
}
