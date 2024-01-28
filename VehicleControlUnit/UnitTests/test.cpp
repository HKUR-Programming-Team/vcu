#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#include <MockLibraries.hpp>
#include <doctest.h>
#include <string>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>
#include <MainLib/Inc/settings.hpp>

namespace sensorLib = VehicleControlUnit::SensorInterfaceLib;
namespace utilsLib = VehicleControlUnit::UtilsLib;
namespace dataLib = VehicleControlUnit::DataStoreLib;
namespace settings = VehicleControlUnit::MainLib::Settings;

TEST_CASE("SensorInterface Test Cases") {
    utilsLib::ADCManager adcManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    const uint8_t ThrottleSignalADCIndex0 = 0;
    const uint8_t ThrottleSignalADCIndex1 = 1;
    settings::SensorInterfaceParameters sensorInterfaceParams;
    sensorInterfaceParams.ThrottleMinPin0 = 500;
    sensorInterfaceParams.ThrottleMaxPin1 = 3500;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, ThrottleSignalADCIndex0, ThrottleSignalADCIndex1, sensorInterfaceParams);

    SUBCASE("WHEN one throttle sensor is at 0 percent and one throttle sensor is at 100 percent THEN error is set in dataStore correctly")
    {
        adcManager.buffer[ThrottleSignalADCIndex0] = sensorInterfaceParams.ThrottleMinPin0;
        adcManager.buffer[ThrottleSignalADCIndex1] = sensorInterfaceParams.ThrottleMaxPin1;

        uint16_t checkADC1 = 0;
        uint16_t checkADC2 = 0;
        adcManager.GetBufferByIndex(ThrottleSignalADCIndex0, checkADC1);
        adcManager.GetBufferByIndex(ThrottleSignalADCIndex1, checkADC2);
        REQUIRE(checkADC1 == sensorInterfaceParams.ThrottleMinPin0);
        REQUIRE(checkADC2 == sensorInterfaceParams.ThrottleMaxPin1);

        sensorInterface.ReadADC();
        CHECK(dataStore.mDriveDataStore.GetError());
        CHECK(dataStore.mDriveDataStore.GetTorque() == 0);
    }
}