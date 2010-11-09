/**
 * @file   manager.hpp
 * @brief  Manager class, used for accessing the various device handlers.
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

#ifndef _LIBMFD_MANAGER_HPP_
#define _LIBMFD_MANAGER_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <libmfd/devicetype.hpp>

namespace mfd {

class Manager;

/// Shared pointer to a Manager.
typedef boost::shared_ptr<Manager> ManagerPtr;

/// Library entry point.
/**
 * All further functionality is provided by calling functions in the Manager
 * class.
 *
 * @return A shared pointer to a Manager instance.
 */
ManagerPtr getManager(void)
	throw ();

/// Top-level class to manage archive types.
/**
 * This class provides access to the different devices supported by the library.
 *
 * In order to open a device, this class must be used to access an instance
 * of the device type.  This DeviceType instance is then used to create an
 * Device instance around a particular hostname.  It is this Device instance
 * that is then used to manipulate the MFD itself.
 *
 * @note This class shouldn't be created manually, use the global function
 *       getManager() to obtain a pointer to it.
 */
class Manager {
	private:
		/// List of available archive types.
		VC_DEVICETYPE vcTypes;

		Manager()
			throw ();

		friend ManagerPtr getManager(void)
			throw ();

	public:

		~Manager()
			throw ();

		/// Get an DeviceType instance for a supported file format.
		/**
		 * This can be used to enumerate all available file formats.
		 *
		 * @param  iIndex Index of the format, starting from 0.
		 * @return A shared pointer to an DeviceType for the given index, or
		 *         an empty pointer once iIndex goes out of range.
		 * @todo Remove this and replace it with a function that just returns the vector.
		 */
		DeviceTypePtr getDeviceType(int iIndex)
			throw ();

		/// Get an DeviceType instance by its code.
		/**
		 * @param  strCode %Device code (e.g. "grp-duke3d")
		 * @return A shared pointer to an DeviceType for the given code, or
		 *         an empty pointer on an invalid code.
		 */
		DeviceTypePtr getDeviceTypeByCode(const std::string& strCode)
			throw ();
};

} // namespace mfd

#endif // _LIBMFD_MANAGER_HPP_
