/**
 * @file   mfdmgr.cpp
 * @brief  Command-line interface to libmfd.
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

#include <boost/program_options.hpp>
#include <libmfd/libmfd.hpp>

namespace po = boost::program_options;

#define PROGNAME "mfdmgr"

/*** Return values ***/
#define RET_OK                  0 ///< All is good
#define RET_BADARGS             1 ///< Bad arguments (missing/invalid parameters)
#define RET_SHOWSTOPPER         2 ///< Major error (couldn't connect to device, etc.)
#define RET_BE_MORE_SPECIFIC    3 ///< More info needed (-t auto didn't work, specify a type)

/// Split a string in two at a delimiter
/**
 * For example "one=two" becomes "one" and "two" and true is returned.  If there
 * is no delimiter both output strings will be the same as the input string and
 * false will be returned.
 *
 * If delim == '=' then:
 * in        -> ret, out1, out2
 * "one=two" -> true, "one", "two"
 * "one=two=three" -> true, "one=two", "three"
 * "four" -> false, "four", "four"
 * If delim == '@' then:
 * "one@two" -> true, "one", "two"
 * "@1=myfile@@4"
 * "test.txt@here.txt"
 * "@2=test.txt"
 * "e1m1.mid=mysong.mid:@4"
 * "e1m1.mid=mysong.mid:e1m2.mid"
 */
bool split(const std::string& in, char delim, std::string *out1, std::string *out2)
{
	std::string::size_type iEqualPos = in.find_last_of(delim);
	*out1 = in.substr(0, iEqualPos);
	// Does the destination have a different filename?
	bool bAltDest = iEqualPos != std::string::npos;
	*out2 = bAltDest ? in.substr(iEqualPos + 1) : *out1;
	return bAltDest;
}

int main(int iArgC, char *cArgV[])
{
	// Set a better exception handler
	std::set_terminate( __gnu_cxx::__verbose_terminate_handler );

	// Disable stdin/printf/etc. sync for a speed boost
	std::ios_base::sync_with_stdio(false);

	// Declare the supported options.
	po::options_description poActions("Actions");
	poActions.add_options()
		("list,l",
			"list contents of the address book")

		("update,u", po::value<std::string>(),
			"select an address book entry to change by ID")

		("add,a", po::value<std::string>(),
			"add a new address book entry and select it")

		("set,s", po::value<std::string>(),
			"field:value parameter updates entry selected by earlier -u or -a")
	;

	po::options_description poOptions("Options");
	poOptions.add_options()
		("type,t", po::value<std::string>(),
			"specify the MFD type (default is autodetect)")
		("user,u", po::value<std::string>(),
			"username to log in as")
		("pass,p", po::value<std::string>(),
			"password")
	;

	po::options_description poHidden("Hidden parameters");
	poHidden.add_options()
		("host", "MFD hostname")
		("help", "produce help message")
	;

	po::options_description poVisible("");
	poVisible.add(poActions).add(poOptions);

	po::options_description poComplete("Parameters");
	poComplete.add(poActions).add(poOptions).add(poHidden);
	po::variables_map mpArgs;

	std::string host, user, pass, type;

	try {
		po::parsed_options pa = po::parse_command_line(iArgC, cArgV, poComplete);

		// Parse the global command line options
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.empty()) {
				// If we've already got an archive filename, complain that a second one
				// was given (probably a typo.)
				if (!host.empty()) {
					std::cerr << "Error: unexpected extra parameter (multiple MFDs "
						"given?!)" << std::endl;
					return 1;
				}
				assert(i->value.size() > 0);  // can't have no values with no name!
				host = i->value[0];
			} else if (i->string_key.compare("help") == 0) {
				std::cout <<
					"Copyright (C) 2010 Adam Nielsen <adam.nielsen@uq.edu.au>\n"
					"This program comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
					"and you are welcome to change and redistribute it under certain conditions;\n"
					"see <http://www.gnu.org/licenses/> for details.\n"
					"\n"
					"Utility to remotely configure multi-function devices (networked photocopiers.)\n"
					"Build date " __DATE__ " " __TIME__ << "\n"
					"\n"
					"Usage: mfdmgr <hostname> <action> [action...]\n" << poVisible << "\n"
					<< std::endl;
				return RET_OK;
			} else if (
				(i->string_key.compare("t") == 0) ||
				(i->string_key.compare("type") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --type (-t) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				type = i->value[0];
			} else if (
				(i->string_key.compare("u") == 0) ||
				(i->string_key.compare("user") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --user (-u) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				user = i->value[0];
			} else if (
				(i->string_key.compare("p") == 0) ||
				(i->string_key.compare("pass") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --pass (-p) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				pass = i->value[0];
			}
		}

		if (host.empty()) {
			std::cerr << "Error: no hostname given" << std::endl;
			return RET_BADARGS;
		}
		std::cout << "Opening " << host << " as type "
			<< (type.empty() ? "<autodetect>" : type) << std::endl;

		// Get the format handler for this file format
		boost::shared_ptr<mfd::Manager> pManager(mfd::getManager());

		mfd::DeviceTypePtr pDeviceType;
		if (type.empty()) {
			// Need to autodetect the file format.
			mfd::DeviceTypePtr pTestType;
			int i = 0;
			while ((pTestType = pManager->getDeviceType(i++))) {
				mfd::E_CERTAINTY cert = pTestType->isInstance(host);
				switch (cert) {
					case mfd::EC_DEFINITELY_NO:
						// Don't print anything (TODO: Maybe unless verbose?)
						break;
					case mfd::EC_DEFINITELY_YES:
						std::cout << "Device is definitely a " << pTestType->getFriendlyName()
							<< " [" << pTestType->getDeviceCode() << "]" << std::endl;
						pDeviceType = pTestType;
						// Don't bother checking any other formats if we got a 100% match
						goto finishTesting;
				}
			}
finishTesting:
			if (!pDeviceType) {
				std::cerr << "Unable to automatically determine the file type.  Use "
					"the --type option to manually specify the file format." << std::endl;
				return RET_BE_MORE_SPECIFIC;
			}
		} else {
			mfd::DeviceTypePtr pTestType(pManager->getDeviceTypeByCode(type));
			if (!pTestType) {
				std::cerr << "Unknown device type given to -t/--type: " << type
					<< std::endl;
				return RET_BADARGS;
			}
			pDeviceType = pTestType;
		}

		assert(pDeviceType != NULL);

		// Connect to the device
		boost::shared_ptr<mfd::Device> pDevice(pDeviceType->open(host, user, pass));
		assert(pDevice);

		int iRet = RET_OK;

		// ID of previously selected address book item
		std::string idABSelected;
		std::map<std::string, std::string> fieldSet; // TODO: enum for field type

		// Run through the actions on the command line
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.compare("list") == 0) {
				boost::shared_ptr<mfd::AddressBook> ab = pDevice->getAddressBook();
				if (!ab) {
					std::cerr << "This device type does not have an address book." << std::endl;
					iRet = RET_BADARGS;
					continue;
				}
				const mfd::AddressBook::VC_ENTRYID& entryIds = ab->getEntryIds();
				for (mfd::AddressBook::VC_ENTRYID::const_iterator i = entryIds.begin();
					i != entryIds.end(); i++
				) {
					std::cout << "id is " << *i << std::endl;
				}
				mfd::AddressBook::VC_FIELDLIST allEntries;
				ab->getEntries(entryIds, allEntries);
				for (mfd::AddressBook::VC_FIELDLIST::iterator i = allEntries.begin();
					i != allEntries.end(); i++
				) {
					mfd::AddressBook::FieldList& fl = *i;
					for (mfd::AddressBook::FieldList::iterator j = fl.begin();
						j != fl.end(); j++
					) {
						std::cout << j->first << "=" << j->second << "; ";
					}
					std::cout << std::endl;
				}

			} else if (i->string_key.compare("update") == 0) {
				idABSelected = i->value[0];
				std::cout << "Selected ID " << idABSelected << " for update" << std::endl;
				fieldSet.clear(); // empty any fields previously set

			} else if (i->string_key.compare("set") == 0) {
				if (idABSelected.empty()) {
					std::cerr << "ERROR: You must use -u or -a before --set/-s" << std::endl;
					iRet = RET_BADARGS;
					break;
				}
				idABSelected = i->value[0];
				std::string fieldName, fieldVal;
				bool bAltDest = split(i->value[0], ':', &fieldName, &fieldVal);
				if (!bAltDest) {
					std::cerr << "ERROR: --set/-s requires a parameter of the form "
						"field:value" << std::endl;
					iRet = RET_BADARGS;
					break;
				}
				std::cout << "Set field \"" << fieldName << "\" to \"" << fieldVal
					<< "\"" << std::endl;

			// Ignore --type/-t
			} else if (i->string_key.compare("type") == 0) {
			} else if (i->string_key.compare("t") == 0) {

			}
		} // for (all command line elements)

	} catch (po::unknown_option& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;
	} catch (po::invalid_command_line_syntax& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;
	}

	return RET_OK;
}
