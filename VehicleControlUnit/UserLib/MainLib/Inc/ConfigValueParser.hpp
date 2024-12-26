#pragma once

#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/json.hpp>

#include <optional>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
	#include <UtilsLib/Inc/GPIOManager.hpp>
#else
	#include <MockLibraries.hpp>
#endif

using json = nlohmann::json;

namespace VehicleControlUnit::MainLib::Config {

class ConfigValueParser{

public:
static std::optional<bool> GetBool(const json& config, const std::string& key);
static std::optional<uint32_t> GetU32(const json& config, const std::string& key);
static std::optional<uint16_t> GetU16(const json& config, const std::string& key);
static std::optional<uint8_t> GetU8(const json& config, const std::string& key);
static std::optional<int16_t> GetI16(const json& config, const std::string& key);
static std::optional<UtilsLib::GPIOPort> GetGPIOPort(const json& config, const std::string& key);
static std::optional<UtilsLib::GPIOPinNum> GetGPIOPinNum(const json& config, const std::string& key);
static std::optional<std::string> GetString(const json& config, const std::string& key);

private:
static std::optional<long long> GetInteger(const json& config, const std::string& key);
static std::optional<unsigned long long> GetUnsignedInteger(const json& config, const std::string& key);
static std::optional<char> GetChar(const json& config, const std::string& key);

// Logger class in UtilsLib is not used as the Logger is not initialized at this point.
static const inline std::string logHeader{"[ConfigValueParser]"};
};

}
