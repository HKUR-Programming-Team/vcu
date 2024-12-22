#include <MainLib/Inc/ConfigValueParser.hpp>

namespace VehicleControlUnit::MainLib::Config {

std::optional<bool> ConfigValueParser::GetBool(const json& config, const std::string& key)
{
	if (!config.contains(key))
	{
		printf("%s %s does not exist\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto& value = config[key];
	if (!value.is_boolean())
	{
		printf("%s %s is not a boolean\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto unpackedValue = value.template get<bool>();
	printf("%s %s successfully loaded as bool with value %d\n", logHeader.c_str(), key.c_str(), unpackedValue);
	return unpackedValue;
}

std::optional<long long> ConfigValueParser::GetInteger(const json& config, const std::string& key)
{
	if (!config.contains(key))
	{
		printf("%s %s does not exist\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto& value = config[key];
	if (!value.is_number_integer())
	{
		printf("%s %s is not an integer\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto unpackedValue = value.template get<long long>();
	printf("%s %s successfully loaded as long long with value %lld", logHeader.c_str(), key.c_str(), unpackedValue);
	return unpackedValue;
}

std::optional<unsigned long long> ConfigValueParser::GetUnsignedInteger(const json& config, const std::string& key)
{
	if (!config.contains(key))
	{
		printf("%s %s does not exist\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto& value = config[key];
	if (!value.is_number_unsigned())
	{
		printf("%s %s is not an integer\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto unpackedValue = value.template get<unsigned long long>();
	printf("%s %s successfully loaded as unsigned long long with value %llu\n", logHeader.c_str(), key.c_str(), unpackedValue);
	return unpackedValue;
}

std::optional<uint32_t> ConfigValueParser::GetU32(const json& config, const std::string& key)
{
	const auto valueOpt = GetUnsignedInteger(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value > UINT32_MAX)
	{
		printf("%s %s is not a uint32_t with value %llu\n", logHeader.c_str(), key.c_str(), value);
		return std::nullopt;
	}

	return static_cast<uint32_t>(value);
}

std::optional<uint16_t> ConfigValueParser::GetU16(const json& config, const std::string& key)
{
	const auto valueOpt = GetUnsignedInteger(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value > UINT16_MAX)
	{
		printf("%s %s is not a uint16_t with value %llu\n", logHeader.c_str(), key.c_str(), value);
		return std::nullopt;
	}

	return static_cast<uint16_t>(value);
}

std::optional<uint8_t> ConfigValueParser::GetU8(const json& config, const std::string& key)
{
	const auto valueOpt = GetUnsignedInteger(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value > UINT8_MAX)
	{
		printf("%s %s is not a uint8_t with value %llu\n", logHeader.c_str(), key.c_str(), value);
		return std::nullopt;
	}

	return static_cast<uint8_t>(value);
}

std::optional<int16_t> ConfigValueParser::GetI16(const json& config, const std::string& key)
{
	const auto valueOpt = GetInteger(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value > INT16_MAX || value < INT16_MIN)
	{
		printf("%s %s is not a int16_t with value %lld\n", logHeader.c_str(), key.c_str(), value);
		return std::nullopt;
	}

	return static_cast<int16_t>(value);
}

std::optional<std::string> ConfigValueParser::GetString(const json& config, const std::string& key)
{
	if (!config.contains(key))
	{
		printf("%s %s does not exist\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto& value = config[key];
	if (!value.is_string())
	{
		printf("%s %s is not a string\n", logHeader.c_str(), key.c_str());
		return std::nullopt;
	}

	const auto unpackedValue = value.template get<std::string>();
	printf("%s %s successfully loaded as unsigned long with value %s\n", logHeader.c_str(), key.c_str(), unpackedValue.c_str());
	return unpackedValue;
}

std::optional<char> ConfigValueParser::GetChar(const json& config, const std::string& key)
{
	const auto valueOpt = GetString(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value.size() != 1)
	{
		printf("%s %s is not a character with value %s\n", logHeader.c_str(), key.c_str(), value.c_str());
		return std::nullopt;
	}

	return value.at(0);
}

std::optional<UtilsLib::GPIOPort> ConfigValueParser::GetGPIOPort(const json& config, const std::string& key)
{
	const auto valueOpt = GetChar(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value < 'A' || value > 'E')
	{
		printf("%s %s is not a GPIOPort with value %c\n", logHeader.c_str(), key.c_str(), value);
		return std::nullopt;
	}

	switch (value) {
		case 'A': return UtilsLib::GPIOPort::A;
		case 'B': return UtilsLib::GPIOPort::B;
		case 'C': return UtilsLib::GPIOPort::C;
		case 'D': return UtilsLib::GPIOPort::D;
		case 'E': return UtilsLib::GPIOPort::E;
		default: {
			printf("%s Unreachable code in GPIOPort. %c\n", logHeader.c_str(), value);
			return std::nullopt;
		}
	}
}

std::optional<UtilsLib::GPIOPinNum> ConfigValueParser::GetGPIOPinNum(const json& config, const std::string& key)
{
	const auto valueOpt = GetUnsignedInteger(config, key);
	if (!valueOpt.has_value())
	{
		return std::nullopt;
	}

	const auto value = valueOpt.value();
	if (value < 0 || value > 15)
	{
		printf("%s %s is not a GPIOPinNum with value %llu\n", logHeader.c_str(), key.c_str(), value);
		return std::nullopt;
	}

	switch (value) {
		case 0: return UtilsLib::GPIOPinNum::Pin0;
		case 1: return UtilsLib::GPIOPinNum::Pin1;
		case 2: return UtilsLib::GPIOPinNum::Pin2;
		case 3: return UtilsLib::GPIOPinNum::Pin3;
		case 4: return UtilsLib::GPIOPinNum::Pin4;
		case 5: return UtilsLib::GPIOPinNum::Pin5;
		case 6: return UtilsLib::GPIOPinNum::Pin6;
		case 7: return UtilsLib::GPIOPinNum::Pin7;
		case 8: return UtilsLib::GPIOPinNum::Pin8;
		case 9: return UtilsLib::GPIOPinNum::Pin9;
		case 10: return UtilsLib::GPIOPinNum::Pin10;
		case 11: return UtilsLib::GPIOPinNum::Pin11;
		case 12: return UtilsLib::GPIOPinNum::Pin12;
		case 13: return UtilsLib::GPIOPinNum::Pin13;
		case 14: return UtilsLib::GPIOPinNum::Pin14;
		case 15: return UtilsLib::GPIOPinNum::Pin15;
		default: {
			printf("%s Unreachable code in GPIOPort. %llu\n", logHeader.c_str(), value);
			return std::nullopt;
		}
	}
}

}
