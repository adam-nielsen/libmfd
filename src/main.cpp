/**
 * @file   main.cpp
 * @brief  Entry points for libmfd
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

#include <string>
#include <libmfd/manager.hpp>
#include <libmfd/devicetype.hpp>

// Include all the device types for the Manager to load
#include "device-ricoh-aficio.hpp"
//#include "device-toshiba-estudio.hpp"

namespace mfd {

ManagerPtr getManager()
	throw ()
{
	return ManagerPtr(new Manager());
}

Manager::Manager()
	throw ()
{
	this->vcTypes.push_back(DeviceTypePtr(new DeviceType_RicohAficio()));
//	this->vcTypes.push_back(DeviceTypePtr(new DeviceType_ToshibaEStudio()));
}

Manager::~Manager()
	throw ()
{
}

DeviceTypePtr Manager::getDeviceType(int iIndex)
	throw ()
{
	if (iIndex >= this->vcTypes.size()) return DeviceTypePtr();
	return this->vcTypes[iIndex];
}

DeviceTypePtr Manager::getDeviceTypeByCode(const std::string& strCode)
	throw ()
{
	for (VC_DEVICETYPE::const_iterator i = this->vcTypes.begin(); i != this->vcTypes.end(); i++) {
		if ((*i)->getDeviceCode().compare(strCode) == 0) return *i;
	}
	return DeviceTypePtr();
}

} // namespace mfd
