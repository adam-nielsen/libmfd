/**
 * @file   addressbook.hpp
 * @brief  Interface to an MFD's address book.
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

#ifndef _LIBMFD_ADDRESSBOOK_HPP_
#define _LIBMFD_ADDRESSBOOK_HPP_

//#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

#include <libmfd/exceptions.hpp>

/// Main namespace
namespace mfd {

/// Access to the address book in an MFD.
/**
 * This class represents the list of addresses in an MFD, often used for
 * scanning to e-mail.
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class AddressBook {

	public:

		enum Field {
			Id,
			Name,
			EmailAddress
		};
		typedef std::string EntryId;
		typedef std::vector<EntryId> VC_ENTRYID;
		typedef std::map<Field, std::string> FieldList;
		typedef std::vector<FieldList> VC_FIELDLIST;
/*
		AddressBook()
			throw ();

		virtual ~AddressBook()
			throw ();
*/
		/// Get a list of entry IDs.
		/**
		 * @return Reference to internal list of IDs.  This will remain valid as
		 *   long as this object is.
		 */
		virtual const VC_ENTRYID& getEntryIds()
			throw (ECommFailure) = 0;

		/// Get details for a given entry ID.
		virtual FieldList getEntry(const EntryId& id)
			throw (ECommFailure) = 0;

		/// Get details for multiple entry IDs in one operation.
		virtual void getEntries(const VC_ENTRYID& ids, VC_FIELDLIST& results)
			throw (ECommFailure) = 0;

		/// Set details for an entry ID.
		virtual void setEntry(const EntryId& id, const FieldList& update)
			throw (ECommFailure) = 0;

		/// Add a new entry ID.
		virtual EntryId createEntry()
			throw (ECommFailure) = 0;

};

/// Shared pointer to an AddressBook.
typedef boost::shared_ptr<AddressBook> AddressBookPtr;

/// Vector of AddressBook shared pointers.
typedef std::vector<AddressBookPtr> VC_ADDRESSBOOK;

} // namespace mfd

#endif // _LIBMFD_ADDRESSBOOK_HPP_
