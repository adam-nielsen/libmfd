/**
 * @file   devicetype.hpp
 * @brief  DeviceType class, used to identify and open an instance of a
 *         particular device format.
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

#ifndef _LIBMFD_DEVICETYPE_HPP_
#define _LIBMFD_DEVICETYPE_HPP_

#include <vector>
#include <map>

#include <libmfd/device.hpp>
#include <libmfd/exceptions.hpp>

/// Main namespace
namespace mfd {

/// Confidence level when guessing an device format.
enum E_CERTAINTY {
	/// This device isn't the given type.
	EC_DEFINITELY_NO,
	/// This device is the given type.
	EC_DEFINITELY_YES,
};

/// Interface to a particular device.
class DeviceType {

	public:

		/// Get a short code to identify this file format, e.g. "ricoh-aficio"
		/**
		 * This can be useful for command-line arguments.
		 *
		 * @return The device short name/ID.
		 */
		virtual std::string getDeviceCode() const
			throw () = 0;

		/// Get the device name, e.g. "Ricoh Aficio compatible"
		/**
		 * @return The device name.
		 */
		virtual std::string getFriendlyName() const
			throw () = 0;

		/// Check a host to see if it's in a supported device.
		/**
		 * @param  hostname  The hostname of the device to check.
		 * @param  username  Username with sufficient access.
		 * @param  password  Plain-text password.
		 * @return A single confidence value from \ref E_CERTAINTY.
		 */
		virtual E_CERTAINTY isInstance(const std::string& hostname) const
			throw (std::ios::failure) = 0;

		/// Open a device.
		/**
		 * @pre    Recommended that isInstance() has returned > EC_DEFINITELY_NO.
		 * @param  hostname The device hostname or IP address.
		 * @return A pointer to an instance of the Device class.  Will throw an
		 *         exception if the data is invalid (i.e. if isInstance() returned
		 *         EC_DEFINITELY_NO) however it will try its best to read the data
		 *         anyway, to make it possible to "force" a file to be opened by a
		 *         particular format handler.
		 */
		virtual DevicePtr open(const std::string& hostname,
			const std::string& username, const std::string& password) const
			throw (ECommFailure) = 0;

};

/// Shared pointer to an DeviceType.
typedef boost::shared_ptr<DeviceType> DeviceTypePtr;

/// Vector of DeviceType shared pointers.
typedef std::vector<DeviceTypePtr> VC_DEVICETYPE;

} // namespace mfd

#endif // _LIBMFD_DEVICETYPE_HPP_
