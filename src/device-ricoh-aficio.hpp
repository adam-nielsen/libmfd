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

#ifndef _LIBMFD_DEVICE_RICOH_AFICIO_HPP_
#define _LIBMFD_DEVICE_RICOH_AFICIO_HPP_

#include <boost/enable_shared_from_this.hpp>

#include <libmfd/addressbook.hpp>
#include <libmfd/device.hpp>
#include <libmfd/devicetype.hpp>

#include "soapuDirectoryProxy.h"

namespace mfd {

typedef std::vector<std::string> VC_STRING;
typedef std::map<std::string, std::string> MP_PROPERTYLIST;
typedef std::vector< MP_PROPERTYLIST > VC_RESULTS;

/// Different uDirectory session types.
enum SessionType {
	NoSession,          // not logged in yet, or got logged out/timed out
	SharedSession,      // read only
	ExclusiveSession,   // allow updates
};

class DeviceType_RicohAficio: virtual public DeviceType {

	public:

		virtual std::string getDeviceCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual E_CERTAINTY isInstance(const std::string& hostname) const
			throw (std::ios::failure);

		virtual DevicePtr open(const std::string& hostname,
			const std::string& username, const std::string& password) const
			throw (ECommFailure);

};

class Device_RicohAficio: virtual public Device, virtual public AddressBook,
	public boost::enable_shared_from_this<Device_RicohAficio>
{
	protected:
		std::string hostname;
		uDirectoryProxy ud;
		SessionType sessionType;
		std::string idSession;
		std::map<std::string, AddressBook::Field> fieldMap;

		// AddressBook
		VC_ENTRYID entryIds;

	public:
		/**
		 * @throws std::ios::failure if the device couldn't be contacted via HTTP
		 */
		Device_RicohAficio(const std::string& hostname, const std::string& username,
			const std::string& password)
			throw (ECommFailure);

		~Device_RicohAficio()
			throw ();

		AddressBookPtr getAddressBook()
			throw ();

		// AddressBook functions

		virtual const VC_ENTRYID& getEntryIds()
			throw (ECommFailure);

		/// Get details for a given entry ID.
		virtual FieldList getEntry(const EntryId& id)
			throw (ECommFailure);

		/// Get details for multiple entry IDs in one operation.
		virtual void getEntries(const VC_ENTRYID& ids, VC_FIELDLIST& results)
			throw (ECommFailure);

		/// Set details for an entry ID.
		virtual void setEntry(const EntryId& id, const FieldList& update)
			throw (ECommFailure);

		/// Add a new entry ID.
		virtual EntryId createEntry()
			throw (ECommFailure);

	protected:
		/// Open a uDirectory session.
		/**
		 * @return true on success, false on bad password.
		 * @throws ECommFailure on SOAP error.
		 */
		bool udirOpenSession(SessionType sessionType)
			throw (ECommFailure);

		/// Change the session type (read only, read/write)
		/**
		 * @param  timeout  Keep retrying for this many sections
		 * @return true on success, false on session lost
		 */
		bool udirReopenSession(SessionType sessionType, int timeout)
			throw (ECommFailure);

		void udirCloseSession()
			throw (ECommFailure);

		void udirSearch(stringArray *fields, const std::string& fromClass,
			const std::string& parentObjectId, int start, int count,
			VC_RESULTS& resultMap)
			throw (ECommFailure);

		void getAddressBookEntries(const VC_STRING& ids, VC_RESULTS& results)
			throw (ECommFailure);

		void setAddressBookEntry(const std::string& id, MP_PROPERTYLIST update)
			throw (ECommFailure);

};

} // namespace mfd

#endif // _LIBMFD_DEVICE_RICOH_AFICIO_HPP_
