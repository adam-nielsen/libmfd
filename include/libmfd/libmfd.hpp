/**
 * @file   libmfd.hpp
 * @brief  Main header for libmfd (includes everything)
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

#ifndef _LIBMFD_HPP_
#define _LIBMFD_HPP_

/// Main namespace
namespace mfd {

/**

\mainpage libmfd

libmfd provides a standard interface to access different multi-function devices
(MFDs - networked photocopiers/printers/scanners.)  It (eventually) will
support multiple devices, providing a single, consistent interface regardless
of the device manufacturer.

\section structure Structure

The main interface to the library is the getManager() function, which returns
an instance of the Manager class.  The Manager is used to query supported
devices, and for each supported device it returns an instance of the DeviceType
class.

The DeviceType class can be used to connect to a device.  Successfully
connecting produces an instance of the Device class.

The Device class is used to directly manipulate the MFD, such as by editing the
internal address book.

\section example Examples

The libmfd distribution comes with example code in the form of the
<a href="http://github.com/adam-nielsen/libmfd/blob/master/examples/mfdmgr.cpp">mfdmgr
utility</a>, which provides a simple command-line interface to the
full functionality of the library.

**/
}

// These are all in the mfd namespace
#include <libmfd/device.hpp>
#include <libmfd/devicetype.hpp>
#include <libmfd/manager.hpp>

#endif // _LIBMFD_HPP_
