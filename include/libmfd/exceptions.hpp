/**
 * @file   exceptions.hpp
 * @brief  Shared exceptions.
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

#ifndef _LIBMFD_EXCEPTIONS_HPP_
#define _LIBMFD_EXCEPTIONS_HPP_

#include <iostream>

/// Main namespace
namespace mfd {

/// Interface to a particular device.
class ECommFailure: public std::ios::failure {
	public:
		ECommFailure(const std::string& msg);
};

} // namespace mfd

#endif // _LIBMFD_EXCEPTIONS_HPP_
