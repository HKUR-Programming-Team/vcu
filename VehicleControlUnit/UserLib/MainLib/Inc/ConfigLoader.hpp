#pragma once

#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/ConfigGroupParser.hpp>
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
Config GetConfig()
{
	printf("Config Load started.\n");
    return Config();
}

private:
std::optional<std::string> LoadConfigFromMemory();

std::optional<Config> ParseConfig(const json config);
};

}
