/**
 * @file   device-ricoh-aficio.hpp
 * @brief  Device and DeviceType implementation for Ricoh Aficio MFDs.
 *
 * Copyright (C) 2010 Adam Nielsen <adam.nielsen@uq.edu.au>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h> // sleep()
#include "device-ricoh-aficio.hpp"
#include "uDirectory.nsmap"

namespace mfd {

#define SESSION_TIMEOUT    30

stringArray *vectorToStringArray(struct soap* soap, const VC_STRING& v)
{
	stringArray *sa = soap_new_stringArray(soap, -1);//v.size());
	sa->__size = v.size();
	sa->__ptr = const_cast<std::string *>(&v[0]);
	/*
	//std::string *strs = (std::string *)soap_malloc(soap, sizeof(std::string) * sa.__size);
	std::string *strs = new std::string[sa->__size]; // TODO: This never gets freed
	int j = 0;
	//std::string *strs = soap_new_std__string(soap, v.size());
	for (VC_STRING::const_iterator i = v.begin(); i != v.end(); i++) {
		//sa->__ptr[j++] = *i;
		strs[j++] = *i;
	}
	//sa.__ptr = s;
	sa->__ptr = strs;
	*/
	return sa;
}

propertyList *mapToPropertyList(struct soap* soap, MP_PROPERTYLIST& map)
{
	propertyList *pl = soap_new_propertyList(soap, -1);
	pl->__size = map.size();
	//*pl->__ptr = soap_new_itt__property(soap, pl->__size);
	//std::string *strs = (std::string *)soap_malloc(soap, sizeof(std::string) * sa.__size);
	//itt__property **pp = new itt__property*;
	//itt__property *props = soap_new_itt__property(soap, pl->__size);
	itt__property **props = new itt__property*[pl->__size];
	//memcpy(props, props2, sizeof(itt_property) * pl->__size);
	//delete[] props2;
	int j = 0;
	for (MP_PROPERTYLIST::iterator i = map.begin(); i != map.end(); i++) {
		props[j] = new itt__property;
		props[j]->propName = i->first;
		props[j]->propVal = i->second;
		j++;
	}
	//*pp = props;
	pl->__ptr = props; // props will be deallocated on return! (bad)
	return pl;
}


std::string DeviceType_RicohAficio::getDeviceCode() const
	throw ()
{
	return "ricoh-aficio";
}

std::string DeviceType_RicohAficio::getFriendlyName() const
	throw ()
{
	return "Ricoh Aficio-compatible";
}

E_CERTAINTY DeviceType_RicohAficio::isInstance(const std::string& hostname) const
	throw (std::ios::failure)
{
	// Send off a SOAP request to get the protocol version
	uDirectoryProxy ud;
	std::string endpoint = "http://";
	endpoint.append(hostname);
	endpoint.append("/DH/udirectory");
	ud.soap_endpoint = endpoint.c_str();
	int dummy;
	if (ud.getProtocolVersion(dummy) == SOAP_OK) return EC_DEFINITELY_YES;
	// SOAP request failed, not a supported device
	return EC_DEFINITELY_NO;
}

DevicePtr DeviceType_RicohAficio::open(const std::string& hostname,
	const std::string& username, const std::string& password
) const
	throw (ECommFailure)
{
	return DevicePtr(new Device_RicohAficio(hostname, username, password));
}


Device_RicohAficio::Device_RicohAficio(const std::string& hostname,
	const std::string& username, const std::string& password
)
	throw (ECommFailure) :
		hostname(hostname)
{
	// Map the uDirectory field strings to Field variables
	this->fieldMap["id"] = Id;
	this->fieldMap["name"] = Name;
	this->fieldMap["mail:address"] = EmailAddress;

	std::string endpoint = "http://";
	endpoint.append(this->hostname);
	endpoint.append("/DH/udirectory");
	this->ud.soap_endpoint = endpoint.c_str();
	int ver;
	if (ud.getProtocolVersion(ver) == SOAP_OK) {
		if ((ver < 302) || (ver > 304)) {
			std::cerr << "Warning: This device is using an unknown protocol version "
				<< ver << std::endl;
		}
	} else {
		std::cerr << "Unable to contact device via HTTP/SOAP:" << std::endl;
		this->ud.soap_stream_fault(std::cerr);
		throw ECommFailure("Unable to contact device via HTTP/SOAP.");
	}

	ud__getServiceVersionResponse sr;
	if (this->ud.getServiceVersion(sr) == SOAP_OK) {
		propertyList *items = sr.returnValue;
		std::cout << "Service version response: ";
		for (int i = 0; i < items->__size; i++) {
			std::cout
				<< items->__ptr[i]->propName << "="
				<< items->__ptr[i]->propVal << "; ";
		}
		std::cout << std::endl;
	} else {
		this->ud.soap_stream_fault(std::cerr);
	}

	if (!this->udirOpenSession(SharedSession)) {
		throw std::ios::failure("Unable to log in - bad password?");
	}

/*
	std::string p1("id");
	std::string p2("label");
	//std::string *props[1];
	//props[0] = &p;
	selectProps.__ptr = new std::string[2];
	selectProps.__size = 2;
	selectProps.__ptr[0] = p1;
	selectProps.__ptr[1] = p2;

	VC_RESULTS labels;
	this->udirSearch(&selectProps, "tag", "3", 0, 10, labels);

	std::cout << "Tags (type 3):" << std::endl;
	for (VC_RESULTS::iterator i = labels.begin(); i != labels.end(); i++) {
		std::cout << "  ";
		for (MP_PROPERTYLIST::iterator j = i->begin(); j != i->end(); j++) {
			std::cout << j->first << "=" << j->second << "; ";
		}
		std::cout << std::endl;
	}
*/


}

Device_RicohAficio::~Device_RicohAficio()
	throw ()
{
	this->udirCloseSession();
}

// Change the value of a metadata element.
AddressBookPtr Device_RicohAficio::getAddressBook()
	throw ()
{
	return boost::static_pointer_cast<AddressBook>(shared_from_this());
}

const AddressBook::VC_ENTRYID& Device_RicohAficio::getEntryIds()
	throw (ECommFailure)
{
	if (this->entryIds.empty()) {
		VC_STRING fields;
		fields.push_back(std::string("id"));
		stringArray *addrObjectFields = vectorToStringArray(&this->ud, fields);
		// TODO: get more than 50!
		VC_RESULTS results;
		this->udirSearch(addrObjectFields, "entry", "", 0, 50, results);

		for (VC_RESULTS::iterator i = results.begin(); i != results.end(); i++) {
			MP_PROPERTYLIST& object = *i;
			//std::cout << "Addr book entry: " << object["id"] << std::endl;
			unsigned long v = strtoul(object["id"].c_str(), NULL, 0);
			if (v < 1<<30) {
				std::string val("entry:");
				val.append(object["id"]);
				this->entryIds.push_back(val);
				std::cout << "added " << val << std::endl;
			}// else std::cout << "out of range: " << object["id"] <<std::endl;
		}
	}
	return this->entryIds;
}

AddressBook::FieldList Device_RicohAficio::getEntry(const EntryId& id)
	throw (ECommFailure)
{
	throw ECommFailure("not implemented");
}

void Device_RicohAficio::getEntries(const AddressBook::VC_ENTRYID& ids,
	AddressBook::VC_FIELDLIST& results
)
	throw (ECommFailure)
{
	stringArray *objectIdList = vectorToStringArray(&this->ud, ids);

	stringArray selectProps;
	std::string fields2[] = {"entryType", "id", "name", "longName",
		/*"phoneticName", */"index",/* "passwordEncoding", "isDestination", "isSender",
		"auth:", "auth:name", "auth:password", "password:", "password:password",
		"password:usedForMailSender", "password:usedForRemoteFolder",
		"password:passwordEncoding", "mail:", */"mail:address",/* "mail:parameter",
		"mail:isDirectSMTP", "fax:", "fax:number", "fax:lineType", "fax:isAbroad",
		"fax:parameter", "faxAux:", "faxAux:ttiNo", "faxAux:label1",
		"faxAux:label2String", "faxAux:messageNo", "remoteFolder:",
		"remoteFolder:type", "remoteFolder:serverName", "remoteFolder:path",
		"remoteFolder:accountName", "remoteFolder:password", "remoteFolder:port",
		"remoteFolder:characterEncoding", "remoteFolder:passwordEncoding",
		"remoteFolder:select", "remoteFolder:logonMode",
		"ldap:", "ldap:accountName", "ldap:password", "ldap:passwordEncoding",
		"ldap:select",
		"smtp:", "smtp:accountName", "smtp:password", "smtp:passwordEncoding",
		"smtp:select",
		"ifax:", "ifax:address", "ifax:parameter", "ifax:isDirectSMTP",*/
		"tagId"
	};
	selectProps.__ptr = fields2;
	selectProps.__size = sizeof(fields2) / sizeof(fields2[0]);

	ud__getObjectsPropsResponse getObjectsPropsRes;
	if (this->ud.getObjectsProps(
		idSession,
		objectIdList,
		&selectProps,
		NULL,//propertyList,
		getObjectsPropsRes
	) != SOAP_OK) {
		std::cerr << "[udir] getObjectsProps() failed:" << std::endl;
		this->ud.soap_stream_fault(std::cerr);
		throw ECommFailure("SOAP error in getObjectsProps()");
	}

	propertyListArray *rows = getObjectsPropsRes.returnValue;
	for (int i = 0; i < rows->__size; i++) {
		propertyList *row = rows->__ptr[i];
		AddressBook::FieldList fl;
		for (int j = 0; j < row->__size; j++) {
			std::string& propName = row->__ptr[j]->propName;
			std::string& propVal = row->__ptr[j]->propVal;
			std::map<std::string, AddressBook::Field>::iterator fi = this->fieldMap.find(propName);
			if (fi != this->fieldMap.end()) {
				fl[fi->second] = propVal;
			} // else field isn't in the map, ignore it and keep going
		}
		results.push_back(fl);
	}
	return;
}

void Device_RicohAficio::setEntry(const EntryId& id, const FieldList& update)
	throw (ECommFailure)
{
	throw ECommFailure("not implemented");
}

AddressBook::EntryId Device_RicohAficio::createEntry()
	throw (ECommFailure)
{
	throw ECommFailure("not implemented");
}

bool Device_RicohAficio::udirOpenSession(SessionType sessionType)
	throw (ECommFailure)
{
	std::string sessionInfo =
		"SCHEME=QkFTSUM=;" // BASIC
		"UID:UserName=YWRtaW4=;" // admin
		"PWD:Password=PL6+vibgqmlnv38/7u8/Lv+vK/6//6omLz/+ZL6n/icsrqH/bikmL3x//ql"
		"8/uBnKb7pvukpv34/YClsPj4//z986GD/fGlhfP++/2lroTx+vyiv//7qb66+Kz6vvuKp7ny"
		"n/uw=;" // ??? (blank password)
		"PES:Encoding=gwpwes003";
	ud__startSessionResponse ssres;

	std::string sessionTypeString;
	switch (sessionType) {
		case SharedSession:    sessionTypeString = "S"; break;
		case ExclusiveSession: sessionTypeString = "X"; break;
	}
	if (this->ud.startSession(
		sessionInfo, SESSION_TIMEOUT, sessionTypeString, ssres
	) != SOAP_OK) {
		this->sessionType = NoSession;
		throw ECommFailure("SOAP protocol error when logging in");
	}
	std::cout << "[udir] Open session: " << ssres.returnValue << std::endl;
	if (ssres.returnValue.compare("OK") != 0) {
		this->sessionType = NoSession;
		return false;
	}

	this->idSession = ssres.stringOut;
	this->sessionType = sessionType;
	std::cout << "[udir] Session ID is " << this->idSession << std::endl;
	return true;
}

bool Device_RicohAficio::udirReopenSession(SessionType sessionType, int timeout)
	throw (ECommFailure)
{
	// Do nothing if we've already got a session of that type
	if (this->sessionType == sessionType) return true;

	this->udirCloseSession();
	while (!this->udirOpenSession(sessionType) && timeout) {
		std::cout << "[udir] Device busy, unable to open session.  Retrying for "
			<< timeout << " more second(s)." << std::endl;
		sleep(1);
		timeout--;
	}
	return this->sessionType != NoSession;
}

void Device_RicohAficio::udirCloseSession()
	throw (ECommFailure)
{
	std::string status;
	if (this->ud.terminateSession(this->idSession, status) != SOAP_OK) {
		std::cout << "[udir] Error closing session: " << std::endl;
		this->ud.soap_stream_fault(std::cerr);
		throw ECommFailure("SOAP protocol error when attempting to close the session");
	}
	std::cout << "[udir] Close session: " << status << std::endl;
	return;
}

void Device_RicohAficio::udirSearch(stringArray *fields,
	const std::string& fromClass, const std::string& parentObjectId, int start,
	int count, VC_RESULTS& resultMap
)
	throw (ECommFailure)
{
	ud__searchObjectsResponse searchRes;
	if (this->ud.searchObjects(
		this->idSession,
		fields,
		fromClass,
		parentObjectId,
		std::string(),
		NULL,//whereAnd,
		NULL,//whereOr,
		NULL,//orderBy,
		start,
		count,
		std::string(),
		NULL,
		searchRes
	) != SOAP_OK) {
		std::cerr << "[udir] searchObjects() failed:" << std::endl;
		this->ud.soap_stream_fault(std::cerr);
		throw ECommFailure("SOAP error in searchObjects()");
	}

	propertyListArray *rows = searchRes.rowList;
	for (int i = 0; i < rows->__size; i++) {
		propertyList *row = rows->__ptr[i];
		MP_PROPERTYLIST pl;
		for (int j = 0; j < row->__size; j++) {
			pl[row->__ptr[j]->propName] = row->__ptr[j]->propVal;
		}
		resultMap.push_back(pl);
	}
	return;
}

void Device_RicohAficio::setAddressBookEntry(const std::string& id,
	MP_PROPERTYLIST update
)
	throw (ECommFailure)
{
	//MP_PROPERTYLIST update;
	//update["mail:address"] = "itadmin@research.uq.edu.au";
	MP_PROPERTYLIST options;
	options["replaceAll"] = "false";

	propertyList *updateList = mapToPropertyList(&this->ud, update);
	propertyList *optionsList = mapToPropertyList(&this->ud, options);
	std::string resPut;
	if (this->ud.putObjectProps(
		idSession,
		std::string("entry:107"),
		updateList,
		optionsList,
		resPut
	) != SOAP_OK) {
		std::cerr << "[udir] putObjectProps() failed:" << std::endl;
		this->ud.soap_stream_fault(std::cerr);
		// This can happen when attempting an update and udir has been opened
		// in shared/readonly mode.
		throw ECommFailure("SOAP protocol error when attempting an update");
	}
	std::cout << "[udir] Update result: " << resPut << std::endl;

	return;
}

} // namespace mfd
