#pragma once

#include <stdio.h>

#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/ConfigGroupParser.hpp>
#include <MainLib/Inc/ConfigValueParser.hpp>
#include <MainLib/Inc/json.hpp>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

using json = nlohmann::json;

namespace VehicleControlUnit::MainLib::Config {

class ConfigParser{
public:
ConfigParser(const std::string& supportedVersion):
	mSupportedVersion{supportedVersion}
{}

std::optional<Config> Parse(const json& config) const;

private:
const std::string mSupportedVersion;
};

}
