/**
 * @file   device.hpp
 * @brief  Interface to generic MFD devices.
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

#ifndef _LIBMFD_DEVICE_HPP_
#define _LIBMFD_DEVICE_HPP_

#include <boost/shared_ptr.hpp>
#include <exception>
#include <iostream>
#include <sstream>
#include <vector>

#include <libmfd/addressbook.hpp>

/// Main namespace
namespace mfd {

/// Primary interface to a device.
/**
 * This class represents a multi-function device (e.g. a photocopier.)  Its
 * functions are used to change settings on the device.
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class Device {

	public:
/*
		Device()
			throw ();

		virtual ~Device()
			throw ();
*/
		/// Get an AddressBook interface to this device.
		virtual AddressBookPtr getAddressBook()
			throw () = 0;

};

/// Shared pointer to an Device.
typedef boost::shared_ptr<Device> DevicePtr;

/// Vector of Device shared pointers.
typedef std::vector<DevicePtr> VC_DEVICE;

} // namespace mfd

#endif // _LIBMFD_DEVICE_HPP_
