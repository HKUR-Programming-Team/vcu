#pragma once

#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/ConfigParser.hpp>
#include <MainLib/Inc/json.hpp>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

using json = nlohmann::json;

namespace VehicleControlUnit::MainLib::Config {

class ConfigLoader{
public:

// MASSIVE FOOTGUN:
// The parameter configEndAddress is very critical.
// It is used to stop the search of the config string from passing
// the limit defined by the linker script.
// SEGFAULT may occur if this is not set correctly.
ConfigLoader(const char* configStartAddress, const char* configEndAddress, const std::string& supportedConfigVersion):
    mConfigStartAddress{configStartAddress},
    mConfigEndAddress{configEndAddress},
    mConfigParser{supportedConfigVersion}
{}

Config GetConfig() const;

private:
std::optional<std::string> LoadConfigStringFromFlash() const;

const char* mConfigStartAddress;
const char* mConfigEndAddress;
ConfigParser mConfigParser;

};

}