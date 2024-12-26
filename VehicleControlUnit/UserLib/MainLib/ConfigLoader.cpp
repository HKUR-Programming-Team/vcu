#include <MainLib/Inc/ConfigLoader.hpp>

namespace VehicleControlUnit::MainLib::Config {

Config ConfigLoader::GetConfig() const
{
    const auto configStringOpt = LoadConfigStringFromFlash();
    if (!configStringOpt.has_value())
    {
        printf("[Config Loader] using default config.\n");
        return Config();
    }

    const auto configString = configStringOpt.value();

    const auto configJson = json::parse(configString, nullptr, false); // Disable exception
    if (configJson.is_discarded())
    {
        printf("[Config Loader] failed to load config. using default config.\n");
        return Config();
    }

    const auto configOpt = mConfigParser.Parse(configJson);
    if (!configOpt.has_value())
    {
        printf("[Config Loader] using default config.\n");
        return Config();
    }

    return configOpt.value();
}

std::optional<std::string> ConfigLoader::LoadConfigStringFromFlash() const
{
    printf("[Config Loader] Starting finding the config string. If this does not end, you are likely running into memory problems...\n");

    const size_t maxLengthOfString = (mConfigEndAddress - mConfigStartAddress) * sizeof(char);

    const void* addressOfTerminatingCharacter = memchr(mConfigStartAddress, '\0', maxLengthOfString); // Search for the terminating character
    if (addressOfTerminatingCharacter == NULL)
    {
        printf("[Config Loader] Config String not found. Could not find the terminating character.\n");
        return std::nullopt;
    }

    printf("[Config Loader] Config String found.\n -- Start of string --\n%s\n -- End of string --\n", mConfigStartAddress);
    const std::string configString(mConfigStartAddress);
    return configString;
}

}
