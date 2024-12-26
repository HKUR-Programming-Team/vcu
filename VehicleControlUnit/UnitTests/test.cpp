#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#include <MockLibraries.hpp>
#include <doctest.h>
#include <string>
#include <optional>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>
#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>
#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/ConfigParser.hpp>
#include <MainLib/Inc/ConfigValueParser.hpp>
#include <MainLib/Inc/ConfigGroupParser.hpp>
#include <MainLib/Inc/json.hpp>

namespace sensorLib = VehicleControlUnit::SensorInterfaceLib;
namespace mcuLib = VehicleControlUnit::MCUInterfaceLib;
namespace r2dLib = VehicleControlUnit::ReadyToDriveLib;
namespace utilsLib = VehicleControlUnit::UtilsLib;
namespace dataLib = VehicleControlUnit::DataStoreLib;
namespace config = VehicleControlUnit::MainLib::Config;
using json = nlohmann::json;

TEST_CASE("SensorInterface ReadThrottleSignal") {
    utilsLib::ADCManager adcManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    config::SensorInterfaceThrottleConfig throttleConfig;
    throttleConfig.ThrottleMinPin0 = 500;
    throttleConfig.ThrottleMaxPin0 = 1000;
    throttleConfig.ThrottleMinPin1 = 1500;
    throttleConfig.ThrottleMaxPin1 = 2000;
    throttleConfig.MaxTorque = 500;
    throttleConfig.ThrottleSignalOutOfRangeThreshold = 20;
    throttleConfig.ThrottleSignalDeviationThreshold = 50;
    throttleConfig.ThrottleSignalADCIndex1 = 0;
    throttleConfig.ThrottleSignalADCIndex2 = 1;

    config::Config config;
    config.mSensorInterfaceThrottleConfig = throttleConfig;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, config);

    SUBCASE("WHEN there exists a throttle sensor that gives value that is out of the threshold THEN error is set in datastore correctly")
    {
        SUBCASE("Pin 0 bottom out of range")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 475;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1500;

            uint16_t checkADC1 = 0;
            uint16_t checkADC2 = 0;
            adcManager.GetBufferByIndex(throttleConfig.ThrottleSignalADCIndex1, checkADC1);
            adcManager.GetBufferByIndex(throttleConfig.ThrottleSignalADCIndex2, checkADC2);
            REQUIRE(checkADC1 == 475);
            REQUIRE(checkADC2 == 1500);

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Pin 1 bottom out of range")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 500;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1475;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Pin 0 top out of range")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 1025;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 2000;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Pin 1 top out of range")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 1000;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 2025;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Both out of range")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 475;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1475;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }
    }

    SUBCASE("WHEN throttle is slight out of range but inside the out of range threshold THEN the correct torque is set in datastore")
    {
        SUBCASE("zero percent")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 485;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1485;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("100 percent")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 1015;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 2015;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == throttleConfig.MaxTorque);
        }
    }

    SUBCASE("WHEN the out of range threshold is higher than ThrottleMin THEN data is stored to datastore correctly")
    {

        config::SensorInterfaceThrottleConfig throttleConfig;
        throttleConfig.ThrottleMinPin0 = 10;
        throttleConfig.ThrottleMaxPin0 = 510;
        throttleConfig.ThrottleMinPin1 = 1500;
        throttleConfig.ThrottleMaxPin1 = 2000;
        throttleConfig.MaxTorque = 500;
        throttleConfig.ThrottleSignalOutOfRangeThreshold = 20;
        throttleConfig.ThrottleSignalDeviationThreshold = 50;

        config::Config config;
        config.mSensorInterfaceThrottleConfig = throttleConfig;

        sensorLib::SensorInterface sensorInterfaceSpecial(logger, dataStore, adcManager, config);

        SUBCASE("WHEN only pin 1 out of bottom range THEN error is set")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 0;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1475;

            sensorInterfaceSpecial.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }
    }

    SUBCASE("WHEN both throttle sensors are at a certain percentage THEN correct torque is set in datastore")
    {
        SUBCASE("50 percent")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 750;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1750;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == throttleConfig.MaxTorque/2);
        }

        SUBCASE("25 percent")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 625;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1625;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 125);
        }
        
        SUBCASE("75 percent")
        {
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 875;
            adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1875;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 375);
        }
    }

    SUBCASE("WHEN the throttle sensors has less than 10 percent deviation THEN the one with lower value is set as torque in datastore")
    {
        adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 740; // Torque = 240
        adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1760; // Torque = 260

        sensorInterface.ReadADC();
        CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
        CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 240);
    }

    SUBCASE("WHEN the throttle sensors has more than 10 percent deviation THEN error is set in datastore")
    {
        adcManager.buffer[throttleConfig.ThrottleSignalADCIndex1] = 700; // Torque = 200
        adcManager.buffer[throttleConfig.ThrottleSignalADCIndex2] = 1755; // Torque = 255

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
        CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0); 
    }
}

TEST_CASE("SensorInterface ReadBrakeSignal")
{
    utilsLib::ADCManager adcManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    config::SensorInterfaceBrakeConfig brakeConfig;
    brakeConfig.BrakeMinPin = 500;
    brakeConfig.BrakeMaxPin = 1000;
    brakeConfig.BrakeSignalOutOfRangeThreshold = 20;
    brakeConfig.MaxBrake = 500;
    brakeConfig.BrakeSignalADCIndex = 0;

    config::Config config;
    config.mSensorInterfaceBrakeConfig = brakeConfig;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, config);

    SUBCASE("WHEN there exists a brake sensor that gives value within range THEN value is set in datastore correctly")
    {
        adcManager.buffer[brakeConfig.BrakeSignalADCIndex] = 600;

        uint16_t checkADC = 0;
        adcManager.GetBufferByIndex(brakeConfig.BrakeSignalADCIndex, checkADC);
        REQUIRE(checkADC == 600);

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
        CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 100);
    }

    SUBCASE("WHEN there exists a brake sensor that gives value out of threshold THEN value is set in datastore correctly")
    {
        SUBCASE("bottom threshold")
        {
            adcManager.buffer[brakeConfig.BrakeSignalADCIndex] = 470;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetBrake().has_value());
        }

        SUBCASE("bottom threshold")
        {
            adcManager.buffer[brakeConfig.BrakeSignalADCIndex] = 1025;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetBrake().has_value());
        }
    }

    SUBCASE("WHEN there exists a brake sensor that gives value out of range but within threshold THEN value is set in datastore correctly")
    {
        SUBCASE("bottom threshold")
        {
            adcManager.buffer[brakeConfig.BrakeSignalADCIndex] = 485;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 0);
        }

        SUBCASE("bottom threshold")
        {
            adcManager.buffer[brakeConfig.BrakeSignalADCIndex] = 1015;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 500);
        }
    }

    SUBCASE("GIVEN BrakeMinPin <= threshold WHEN there exists a brake sensor that gives value within range THEN value is set in datastore correctly")
    {
        config::SensorInterfaceBrakeConfig brakeConfig;
        brakeConfig.BrakeMinPin = 10;
        brakeConfig.BrakeMaxPin = 510;
        brakeConfig.BrakeSignalOutOfRangeThreshold = 20;
        brakeConfig.MaxBrake = 500;
        brakeConfig.BrakeSignalADCIndex = 0;

        config::Config config;
        config.mSensorInterfaceBrakeConfig = brakeConfig;

        sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, config);
        adcManager.buffer[brakeConfig.BrakeSignalADCIndex] = 15;

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
        CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 5);
    }
}

TEST_CASE("SensorInterface ReadRegenSignal")
{
    utilsLib::ADCManager adcManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    config::SensorInterfaceRegenConfig regenConfig;
    regenConfig.RegenMinPin = 500;
    regenConfig.RegenMaxPin = 1000;
    regenConfig.MaxRegen = 200;
    regenConfig.RegenSignalADCIndex = 0;

    config::Config config;
    config.mSensorInterfaceRegenConfig = regenConfig;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, config);

    SUBCASE("WHEN sensor gives value within range THEN value is set in datastore correctly")
    {
        adcManager.buffer[regenConfig.RegenSignalADCIndex] = 750;

        uint16_t checkADC = 0;
        adcManager.GetBufferByIndex(regenConfig.RegenSignalADCIndex, checkADC);
        REQUIRE(checkADC == 750);

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetRegen() == 100);
    }

    SUBCASE("WHEN sensor gives value below range THEN value is set in datastore correctly")
    {
        adcManager.buffer[regenConfig.RegenSignalADCIndex] = 450;

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetRegen() == 0);
    }
    
    SUBCASE("WHEN sensor gives value above range THEN value is set in datastore correctly")
    {
        adcManager.buffer[regenConfig.RegenSignalADCIndex] = 1200;

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetRegen() == 200);
    }
}

TEST_CASE("MCUErrorManager")
{
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    const uint32_t implausibleThresholdInterval = 1000;

    mcuLib::MCUErrorManager mcuErrorManager(logger, dataStore, implausibleThresholdInterval);
    MockCurrentTick = 15000;

    SUBCASE("WHEN error is present for more than the threshold interval THEN persisted implausible status is set in data store correctly")
    {
        SUBCASE("Only ThrottleError is present")
        {
            dataStore.mDrivingInputDataStore.SetThrottleError(true);
            dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);

            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 15600;
            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 16100;
            mcuErrorManager.CheckImplausibility();
            CHECK(dataStore.GetPersistedImplausibleStatus());
        }

        SUBCASE("Only TimeoutError is present")
        {
            dataStore.mDrivingInputDataStore.SetThrottleError(false);
            dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(true);

            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 15600;
            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 16100;
            mcuErrorManager.CheckImplausibility();
            CHECK(dataStore.GetPersistedImplausibleStatus());
        }

        SUBCASE("Both ThrottleError and TimeoutError is present")
        {
            dataStore.mDrivingInputDataStore.SetThrottleError(true);
            dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(true);

            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 15600;
            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 16100;
            mcuErrorManager.CheckImplausibility();
            CHECK(dataStore.GetPersistedImplausibleStatus());
        }

        SUBCASE("Two errors present in different times, but errors exist continuously")
        {
            dataStore.mDrivingInputDataStore.SetThrottleError(true);
            dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);

            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 15600;
            dataStore.mDrivingInputDataStore.SetThrottleError(true);
            dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(true);
            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 15750;
            dataStore.mDrivingInputDataStore.SetThrottleError(false);
            dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(true);
            mcuErrorManager.CheckImplausibility();
            CHECK(!dataStore.GetPersistedImplausibleStatus());

            MockCurrentTick = 16001;
            mcuErrorManager.CheckImplausibility();
            CHECK(dataStore.GetPersistedImplausibleStatus());
        }
    }

    SUBCASE("WHEN error is present for NOT more than the threshold interval THEN no persisted implausible status is set in data store correctly")
    {
        dataStore.mDrivingInputDataStore.SetThrottleError(true);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);

        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 15600;
        dataStore.mDrivingInputDataStore.SetThrottleError(false);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);
        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 15750;
        dataStore.mDrivingInputDataStore.SetThrottleError(false);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(true);
        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 16001;
        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());
    }

    SUBCASE("WHEN error is present for NOT more than the threshold interval THEN persisted implausibility is reconsidered when error presents again")
    {
        dataStore.mDrivingInputDataStore.SetThrottleError(true);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);

        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 15600;
        dataStore.mDrivingInputDataStore.SetThrottleError(false);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);
        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        // Error presents again. Start counting for 1000 miliseconds here (15750th tick)
        MockCurrentTick = 15750;
        dataStore.mDrivingInputDataStore.SetThrottleError(false);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(true);
        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 16001;
        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        // Over 1000 miliseconds from 15750. Hence, persisted implausibility should be set here.
        MockCurrentTick = 16751; 
        mcuErrorManager.CheckImplausibility();
        CHECK(dataStore.GetPersistedImplausibleStatus());
    }

    SUBCASE("WHEN persisted implausibility is set to true in datastore THEN it will not be reset to false unless ResetErrorState is called")
    {
        dataStore.mDrivingInputDataStore.SetThrottleError(true);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);

        mcuErrorManager.CheckImplausibility();
        CHECK(!dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 16001;
        mcuErrorManager.CheckImplausibility();
        CHECK(dataStore.GetPersistedImplausibleStatus());

        dataStore.mDrivingInputDataStore.SetThrottleError(false);
        dataStore.mMCUDataStore.SetMessageReceiveTimeoutError(false);

        MockCurrentTick = 16150;
        mcuErrorManager.CheckImplausibility();
        CHECK(dataStore.GetPersistedImplausibleStatus());

        MockCurrentTick = 18150;
        mcuErrorManager.CheckImplausibility();
        CHECK(dataStore.GetPersistedImplausibleStatus());

        mcuErrorManager.ResetErrorState();
        CHECK(!dataStore.GetPersistedImplausibleStatus());
    }
}

TEST_CASE("MCUInterface driving input")
{
    utilsLib::CANManager canManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    config::MCUInterfaceConfig mcuInterfaceParams;
    mcuInterfaceParams.InverterEnableTorqueThreshold = 10;
    mcuInterfaceParams.RegenEnableTorqueThreshold = 5;
    mcuInterfaceParams.CommandMessageTransmitInterval = 5;

    config::Config config;
    config.mMcuInterfaceConfig = mcuInterfaceParams;

    mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, config);
    MockCurrentTick = 6;

    SUBCASE("WHEN persisted implausibility is set in data store THEN error command message is sent via CAN Manager")
    {
        dataStore.SetPersistedImplausibleStatus(true);
        dataStore.mDrivingInputDataStore.SetTCSEnabled(false);

        dataStore.mDrivingInputDataStore.SetTorque(300);
        dataStore.mDrivingInputDataStore.SetRegen(0);
        dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

        mcuInterface.SendCommandMessage();

        // Check the header
        CHECK(canManager.mMessageId == 0x0C0);
        CHECK(canManager.mMessageLength == 8);

        // Check the payload
        CHECK(canManager.buffer[0] == 0);
        CHECK(canManager.buffer[1] == 0);
        CHECK(canManager.buffer[2] == 0);
        CHECK(canManager.buffer[3] == 0);
        CHECK(canManager.buffer[4] == 0b00000001);
        CHECK(canManager.buffer[5] == 0b00000010);
        CHECK(canManager.buffer[6] == 0);
        CHECK(canManager.buffer[7] == 0);
    }

    SUBCASE("WHEN there is no persisted implausibility and traction control is disabled")
    {
        dataStore.SetPersistedImplausibleStatus(false);
        dataStore.mDrivingInputDataStore.SetTCSEnabled(false);
        
        SUBCASE("WHEN torque is positive and gear forward THEN correct command message is sent via CAN Manager")
        {
            dataStore.mDrivingInputDataStore.SetTorque(300);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 44);
            CHECK(canManager.buffer[1] == 1);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }

        SUBCASE("WHEN torque is above RegenEnableTorqueThreshold and regen is positive and gear forward THEN correct command message is sent via CAN Manager")
        {
            dataStore.mDrivingInputDataStore.SetTorque(10);
            dataStore.mDrivingInputDataStore.SetRegen(200);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 10);
            CHECK(canManager.buffer[1] == 0);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }

        SUBCASE("WHEN torque is below RegenEnableTorqueThreshold the and regen is positive THEN correct command message is sent via CAN Manager")
        {
            dataStore.mDrivingInputDataStore.SetTorque(3);
            dataStore.mDrivingInputDataStore.SetRegen(320);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 0b11000000);
            CHECK(canManager.buffer[1] == 0b11111110);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }

        SUBCASE("WHEN torque is between the InverterEnableTorqueThreshold THEN correct command message is sent via CAN Manager")
        {
            SUBCASE("Forward Torque > Regen")
            {
                dataStore.mDrivingInputDataStore.SetTorque(6);
                dataStore.mDrivingInputDataStore.SetRegen(3);
                dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

                mcuInterface.SendCommandMessage();

                // Check the header
                CHECK(canManager.mMessageId == 0x0C0);
                CHECK(canManager.mMessageLength == 8);

                // Check the payload
                CHECK(canManager.buffer[0] == 0);
                CHECK(canManager.buffer[1] == 0);
                CHECK(canManager.buffer[2] == 0);
                CHECK(canManager.buffer[3] == 0);
                CHECK(canManager.buffer[4] == 0b00000001);
                CHECK(canManager.buffer[5] == 0b00000010);
                CHECK(canManager.buffer[6] == 0);
                CHECK(canManager.buffer[7] == 0);
            }

            SUBCASE("Forward Torque < Regen")
            {
                dataStore.mDrivingInputDataStore.SetTorque(3);
                dataStore.mDrivingInputDataStore.SetRegen(6);
                dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

                mcuInterface.SendCommandMessage();

                // Check the header
                CHECK(canManager.mMessageId == 0x0C0);
                CHECK(canManager.mMessageLength == 8);

                // Check the payload
                CHECK(canManager.buffer[0] == 0);
                CHECK(canManager.buffer[1] == 0);
                CHECK(canManager.buffer[2] == 0);
                CHECK(canManager.buffer[3] == 0);
                CHECK(canManager.buffer[4] == 0b00000001);
                CHECK(canManager.buffer[5] == 0b00000010);
                CHECK(canManager.buffer[6] == 0);
                CHECK(canManager.buffer[7] == 0);
            }

            SUBCASE("Forward Torque == Regen == 0")
            {
                dataStore.mDrivingInputDataStore.SetTorque(0);
                dataStore.mDrivingInputDataStore.SetRegen(0);
                dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

                mcuInterface.SendCommandMessage();

                // Check the header
                CHECK(canManager.mMessageId == 0x0C0);
                CHECK(canManager.mMessageLength == 8);

                // Check the payload
                CHECK(canManager.buffer[0] == 0);
                CHECK(canManager.buffer[1] == 0);
                CHECK(canManager.buffer[2] == 0);
                CHECK(canManager.buffer[3] == 0);
                CHECK(canManager.buffer[4] == 0b00000001);
                CHECK(canManager.buffer[5] == 0b00000010);
                CHECK(canManager.buffer[6] == 0);
                CHECK(canManager.buffer[7] == 0);
            }
        }

        SUBCASE("WHEN torque is positive and gear neutral THEN correct command message is sent via CAN Manager")
        {
            dataStore.mDrivingInputDataStore.SetTorque(300);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::NEUTRAL);

            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 0);
            CHECK(canManager.buffer[1] == 0);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000010);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }
    }

    SUBCASE("Transmit Interval")
    {
        dataStore.SetPersistedImplausibleStatus(false);
        dataStore.mDrivingInputDataStore.SetTCSEnabled(false);
        
        SUBCASE("WHEN the current tick has not yet past the transmit interval THEN no new command message is sent")
        {
            dataStore.mDrivingInputDataStore.SetTorque(300);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 6;
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 44);
            CHECK(canManager.buffer[1] == 1);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);

            dataStore.mDrivingInputDataStore.SetTorque(150); // Torque changed
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 8; // Not yet past the CommandMessageTransmitInterval of 5 ms
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload: torque has not changed
            CHECK(canManager.buffer[0] == 44);
            CHECK(canManager.buffer[1] == 1);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }

        SUBCASE("WHEN the current tick has past the transmit interval THEN no new command message is sent")
        {
            dataStore.mDrivingInputDataStore.SetTorque(300);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 6;
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 44);
            CHECK(canManager.buffer[1] == 1);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);

            dataStore.mDrivingInputDataStore.SetTorque(150); // Torque changed
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 12; // Past the CommandMessageTransmitInterval of 5 ms
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload: torque has changed
            CHECK(canManager.buffer[0] == 150);
            CHECK(canManager.buffer[1] == 0);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }

        SUBCASE("WHEN command message is sent THEN the last transmit time is updated")
        {
            dataStore.mDrivingInputDataStore.SetTorque(300);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 6;
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload
            CHECK(canManager.buffer[0] == 44);
            CHECK(canManager.buffer[1] == 1);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);

            dataStore.mDrivingInputDataStore.SetTorque(150); // Torque changed
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 12; // Past the CommandMessageTransmitInterval of 5 ms. The next transmit time should be 15 ms
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload: torque has changed
            CHECK(canManager.buffer[0] == 150);
            CHECK(canManager.buffer[1] == 0);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);

            dataStore.mDrivingInputDataStore.SetTorque(220);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 14; // Not yet past the CommandMessageTransmitInterval of 5 ms
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload: torque has changed
            CHECK(canManager.buffer[0] == 150);
            CHECK(canManager.buffer[1] == 0);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);

            dataStore.mDrivingInputDataStore.SetTorque(220);
            dataStore.mDrivingInputDataStore.SetRegen(0);
            dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);

            MockCurrentTick = 15; // past the CommandMessageTransmitInterval of 5 ms
            mcuInterface.SendCommandMessage();

            // Check the header
            CHECK(canManager.mMessageId == 0x0C0);
            CHECK(canManager.mMessageLength == 8);

            // Check the payload: torque has changed
            CHECK(canManager.buffer[0] == 220);
            CHECK(canManager.buffer[1] == 0);
            CHECK(canManager.buffer[2] == 0);
            CHECK(canManager.buffer[3] == 0);
            CHECK(canManager.buffer[4] == 0b00000001);
            CHECK(canManager.buffer[5] == 0b00000001);
            CHECK(canManager.buffer[6] == 0);
            CHECK(canManager.buffer[7] == 0);
        }
    }
}

/// TCS IS DISABLED
// TEST_CASE("MCUInterface TCS")
// {
//     utilsLib::CANManager canManager;
//     dataLib::DataStore dataStore;
//     utilsLib::Logger logger;
//     settings::MCUInterfaceParameters mcuInterfaceParams;
//     mcuInterfaceParams.InverterEnableTorqueThreshold = 10;
//     mcuInterfaceParams.RegenEnableTorqueThreshold = 5;
//     mcuInterfaceParams.CommandMessageTransmitInterval = 3;
//     mcuInterfaceParams.TCSTriggeringSlipRatioThreshold = 20;
//     mcuInterfaceParams.TCSHaltSlipRatioThreshold = 12;
//     mcuInterfaceParams.WheelRadius = 1;

//     mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, mcuInterfaceParams);
//     MockCurrentTick = 4;

//     dataStore.SetPersistedImplausibleStatus(false);
//     dataStore.mDrivingInputDataStore.SetTCSEnabled(true);

//     SUBCASE("Originally tcs not triggered, WHEN slipRatio > TCSTriggeringSlipRatioThreshold, tcs is triggered, THEN WHEN slipRatio >= TCSHaltSlipRatioThreshold, torque in command message is restricted")
//     {
//         dataStore.mDrivingInputDataStore.SetTorque(300);
//         dataStore.mDrivingInputDataStore.SetRegen(0);
//         dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);
//         dataStore.mVehicleSensorDataStore.SetAngularWheelSpeedRearLeft(44.0f);
//         dataStore.mVehicleSensorDataStore.SetAngularWheelSpeedRearRight(44.0f);
//         dataStore.mVehicleSensorDataStore.SetLinearVelocity(2.0f);

//         // maxWheelSpeed = leftWheelSpeed > rightWheelSpeed ? leftWheelSpeed : rightWheelSpeed;
//         // slipRatio = (mParameters.WheelRadius * maxWheelSpeed - linearSpeed)/linearSpeed;
        
//         mcuInterface.SendCommandMessage();

//         // Check the header
//         CHECK(canManager.mMessageId == 0x0C0);
//         CHECK(canManager.mMessageLength == 8);

//         // Check the payload
//         CHECK(canManager.buffer[0] == 44);
//         CHECK(canManager.buffer[1] == 1);
//         CHECK(canManager.buffer[2] == 0);
//         CHECK(canManager.buffer[3] == 0);
//         CHECK(canManager.buffer[4] == 0b00000001);
//         CHECK(canManager.buffer[5] == 0b00000001);
//         CHECK(canManager.buffer[6] == 0);
//         CHECK(canManager.buffer[7] == 0);

//         dataStore.mDrivingInputDataStore.SetTorque(350);

//         MockCurrentTick = 7;
//         mcuInterface.SendCommandMessage();

//         // Check the header
//         CHECK(canManager.mMessageId == 0x0C0);
//         CHECK(canManager.mMessageLength == 8);

//         // Check the payload
//         CHECK(canManager.buffer[0] == 44);
//         CHECK(canManager.buffer[1] == 1);
//         CHECK(canManager.buffer[2] == 0);
//         CHECK(canManager.buffer[3] == 0);
//         CHECK(canManager.buffer[4] == 0b00000001);
//         CHECK(canManager.buffer[5] == 0b00000001);
//         CHECK(canManager.buffer[6] == 0);
//         CHECK(canManager.buffer[7] == 0);
//     }
// }

TEST_CASE("MCUInterface MessageReceiveHandler")
{
    utilsLib::CANManager canManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    config::Config config;

    mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, config);
    uint8_t payload[8] = {0,0,0,0,0,0,0,0};
    CAN_RxHeaderTypeDef header;
    MockCurrentTick = 10;

    SUBCASE("WHEN no meesage from MCUInterface has yet been received THEN member variables in MCUDataStore gives empty value")
    {
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
        CHECK(!dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
    }

    SUBCASE("WHEN a motor speed message with message id 0x0A5 has been received THEN data is stored correctly")
    {
        const uint32_t messageId = 0x0A5;
        header.StdId = 0x0A5;

        payload[2] = 44;
        payload[3] = 1; 

        mcuInterface.MessageReceiveHandler(messageId, header, payload);

        REQUIRE(dataStore.mMCUDataStore.GetMotorSpeed().has_value());
        REQUIRE(dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
        CHECK(dataStore.mMCUDataStore.GetMotorSpeed().value() == 300);
        CHECK(dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().value() == 10);

        REQUIRE(!dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
    }

    SUBCASE("WHEN a RUN FAULT message with message id 0x0AB has been received THEN data is stored correctly")
    {
        const uint32_t messageId = 0x0AB;
        header.StdId = 0x0A5;

        SUBCASE("No error")
        {
            payload[5] = 0b00000000;
            mcuInterface.MessageReceiveHandler(messageId, header, payload);

            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
            CHECK(!dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
        }

        SUBCASE("Has error")
        {
            payload[5] = 0b00001000;
            mcuInterface.MessageReceiveHandler(messageId, header, payload);

            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
            CHECK(dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
        }
    }

    SUBCASE("WHEN a message with unknown message id has been received THEN member variables in MCUDataStore gives empty value")
    {
        const uint32_t messageId = 0x0C0;
        header.StdId = 0x0C0;

        mcuInterface.MessageReceiveHandler(messageId, header, payload);
        
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
    }
}

TEST_CASE("Ready to drive")
{
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    config::ReadyToDriveConfig r2dparams;

    r2dparams.readyToDriveSoundDuration = 1000;
	r2dparams.readyToDriveTriggeringBrakeThreshold = 100;
	r2dparams.readyToDriveButtonPort = utilsLib::GPIOPort::A;
	r2dparams.readyToDriveButtonPinNum = utilsLib::GPIOPinNum::Pin0;
	r2dparams.readyToDriveSoundPort = utilsLib::GPIOPort::B;
	r2dparams.readyToDriveSoundPinNum = utilsLib::GPIOPinNum::Pin1;

    config::Config config;
    config.mReadyToDriveConfig = r2dparams;

    r2dLib::ReadyToDrive r2d(logger, dataStore, config);

    SUBCASE("WHEN it is initially not in ready to drive mode and Gear is set to neutral in data store")
    {
        MockCurrentTick = 2000;
        dataStore.mDrivingInputDataStore.SetBrake(0);
        utilsLib::GPIOManager::A0 = false;
        r2d.Check();
        REQUIRE(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::NEUTRAL);

        // Button returns to HIGH
        utilsLib::GPIOManager::A0 = true;
        r2d.Check();
        REQUIRE(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::NEUTRAL);

        SUBCASE("WHEN the brake is higher than triggering threshold AND a voltage fall from the button is detected THEN ready to drive mode is enabled AND gear is set in data store correctly AND ready to drive sound is enabled")
        {
            dataStore.mDrivingInputDataStore.SetBrake(200);
            utilsLib::GPIOManager::A0 = false;
            r2d.Check();
            CHECK(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::FORWARD);
            CHECK(utilsLib::GPIOManager::B1);
        }

        SUBCASE("WHEN the brake is lower than triggering threshold AND a voltage fall from the button is detected THEN ready to drive mode is not enabled and gear is set in data store correctly AND ready to drive sound is not enabled")
        {
            dataStore.mDrivingInputDataStore.SetBrake(90);
            utilsLib::GPIOManager::A0 = false;
            r2d.Check();
            CHECK(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::NEUTRAL);
            CHECK(!utilsLib::GPIOManager::B1);
        }

        SUBCASE("WHEN the brake is undefined THEN ready to drive mode is not enabled and gear is set in data store correctly AND ready to drive sound is not enabled")
        {
            dataStore.mDrivingInputDataStore.SetBrake(std::nullopt);
            utilsLib::GPIOManager::A0 = false;
            r2d.Check();
            CHECK(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::NEUTRAL);
            CHECK(!utilsLib::GPIOManager::B1);
        }
    }

    SUBCASE("WHEN it is in ready to drive mode and Gear is set to FORWARD in data store")
    {
        MockCurrentTick = 2000;
        dataStore.mDrivingInputDataStore.SetBrake(200);
        utilsLib::GPIOManager::A0 = false;
        r2d.Check();
        REQUIRE(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::FORWARD);

        // Button returns to HIGH
        utilsLib::GPIOManager::A0 = true;
        r2d.Check();
        REQUIRE(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::FORWARD);

        SUBCASE("WHEN the brake is higher than triggering threshold AND a voltage fall from the button is detected THEN ready to drive mode is enabled and gear is set in data store correctly AND ready to drive sound is enabled")
        {
            MockCurrentTick = 2500;
            dataStore.mDrivingInputDataStore.SetBrake(200);
            utilsLib::GPIOManager::A0 = false;
            r2d.Check();
            CHECK(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::FORWARD);
            CHECK(utilsLib::GPIOManager::B1);
        }

        SUBCASE("WHEN the brake is lower than triggering threshold AND a voltage fall from the button is detected THEN ready to drive mode is not enabled and gear is set in data store correctly AND ready to drive sound is not enabled")
        {
            dataStore.mDrivingInputDataStore.SetBrake(90);
            utilsLib::GPIOManager::A0 = false;
            r2d.Check();
            CHECK(dataStore.mDrivingInputDataStore.GetGear() == dataLib::Gear::NEUTRAL);
            CHECK(!utilsLib::GPIOManager::B1);
        }

        SUBCASE("WHEN voltage has not fall AND ready to drive sound duration has not yet passed THEN ready to drive sound is enabled")
        {
            MockCurrentTick = 2500;
            utilsLib::GPIOManager::A0 = true;
            r2d.Check();
            CHECK(utilsLib::GPIOManager::B1);
        }

        SUBCASE("WHEN voltage has not fall AND ready to drive sound duration has passed THEN ready to drive sound is disabled")
        {
            MockCurrentTick = 4000;
            utilsLib::GPIOManager::A0 = true;
            r2d.Check();
            CHECK(!utilsLib::GPIOManager::B1);
        }
    }

}

TEST_CASE("Config Value Parser")
{
    SUBCASE("GetBool") 
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": true, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetBool(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is not a boolean THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 1, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetBool(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is boolean THEN optional bool is returned")
        {
            std::string testJson = "{\"haha\": true, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetBool(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), true);
        }
    }

    SUBCASE("GETU32")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 32, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU32(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is not an integer THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 1.0, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU32(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is negative THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": -10, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU32(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 4294967400, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU32(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<u32> is returned")
        {
            std::string testJson = "{\"haha\": 132, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetU32(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), 132);
        }
    }

    SUBCASE("GETU16")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 32, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU16(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is not an integer THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 1.0, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU16(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is negative THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": -10, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU16(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 65536, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU16(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<u16> is returned")
        {
            std::string testJson = "{\"haha\": 132, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetU16(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), 132);
        }
    }

    SUBCASE("GETU8")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 32, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU8(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is not an integer THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 1.0, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU8(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is negative THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": -10, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU8(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 256, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetU8(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<u8> is returned")
        {
            std::string testJson = "{\"haha\": 132, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetU8(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), 132);
        }
    }

    SUBCASE("GETI16")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 32, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetI16(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is not an integer THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 1.0, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetI16(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is positive out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 32768, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetI16(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is negative out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": -32769, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetI16(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<i16> is returned")
        {
            std::string testJson = "{\"haha\": 132, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetI16(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), 132);
        }
    }

    SUBCASE("GetString")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": \"Crap\", \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetString(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<string> is returned")
        {
            std::string testJson = "{\"haha\": \"nuts\", \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetString(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), std::string("nuts"));
        }
    }

    SUBCASE("GETGPIOPort")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": \"C\", \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetGPIOPort(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is multiple characters THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": \"AB\", \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetGPIOPort(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": \"F\", \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetGPIOPort(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<GPIOPort> is returned")
        {
            std::string testJson = "{\"haha\": \"B\", \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetGPIOPort(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), utilsLib::GPIOPort::B);
        }
    }

    SUBCASE("GETGPIOPinNum")
    {
        SUBCASE("WHEN key does not exist THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 13, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetGPIOPinNum(parsedTestJson, "not_funny").has_value());
        }

        SUBCASE("WHEN value is out of range THEN nullopt is returned")
        {
            std::string testJson = "{\"haha\": 20, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            CHECK_FALSE(config::ConfigValueParser::GetGPIOPinNum(parsedTestJson, "haha").has_value());
        }

        SUBCASE("WHEN value is valid THEN optional<GPIOPinNum> is returned")
        {
            std::string testJson = "{\"haha\": 8, \"funny\": false}";
            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception

            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto parsedValue = config::ConfigValueParser::GetGPIOPinNum(parsedTestJson, "haha");
            CHECK(parsedValue.has_value());
            CHECK_EQ(parsedValue.value(), utilsLib::GPIOPinNum::Pin8);
        }
    }
}

TEST_CASE("Config Group Parser")
{
    SUBCASE("LoggerConfig")
    {
        SUBCASE("WHEN everything is right THEN correct LoggerConfig is returned")
        {
            std::string testJson = R"({
                "spamLoggingEnabled": false,
                "infoLoggingEnabled": true,
                "errorLoggingEnabled": false,
                "customLoggingEnabled": true
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseLoggerParameteres(parsedTestJson);
            CHECK(loggerParametersOpt.has_value());
            const auto loggerParameters = loggerParametersOpt.value();

            CHECK_EQ(loggerParameters.spamLoggingEnabled, false);
            CHECK_EQ(loggerParameters.infoLoggingEnabled, true);
            CHECK_EQ(loggerParameters.errorLoggingEnabled, false);
            CHECK_EQ(loggerParameters.customLoggingEnabled, true);
        }

        SUBCASE("WHEN a field is invalid is right THEN nullopt is returned")
        {
            std::string testJson = R"({
                "spamLoggingEnabled": false,
                "infoLoggingEnabled": true,
                "errorLoggingEnabled": false,
                "customLoggingEnabled": 1
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseLoggerParameteres(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }

        SUBCASE("WHEN a field is missing THEN nullopt is returned")
        {
            std::string testJson = R"({
                "spamLoggingEnabled": false,
                "infoLoggingEnabled": true,
                "errorLoggingEnabled": false
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseLoggerParameteres(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }
    }

    SUBCASE("ErrorConfig")
    {
        SUBCASE("WHEN everything is right THEN correct ErrorConfig is returned")
        {
            std::string testJson = R"({
                "implausibleThresholdInterval": 100
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto errorConfigOpt = config::ConfigGroupParser::ParseErrorConfig(parsedTestJson);
            CHECK(errorConfigOpt.has_value());
            const auto errorConfig = errorConfigOpt.value();

            CHECK_EQ(errorConfig.implausibleThresholdInterval, 100);
        }

        SUBCASE("WHEN a field is invalid is right THEN nullopt is returned")
        {
            std::string testJson = R"({
                "implausibleThresholdInterval": -5,
                "infoLoggingEnabled": true,
                "errorLoggingEnabled": false,
                "customLoggingEnabled": 1
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseErrorConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }

        SUBCASE("WHEN a field is missing THEN nullopt is returned")
        {
            std::string testJson = R"({
                "spamLoggingEnabled": false
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseErrorConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }
    }

    SUBCASE("ReadyToDriveConfig")
    {
        SUBCASE("WHEN everything is right THEN correct ReadyToDriveConfig is returned")
        {
            std::string testJson = R"({
                "readyToDriveSoundDuration": 1000,
                "readyToDriveTriggeringBrakeThreshold": 40,
                "readyToDriveButtonPort": "A",
                "readyToDriveButtonPinNum": 5,
                "readyToDriveSoundPort": "D",
                "readyToDriveSoundPinNum": 2
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto readToDriveOpt = config::ConfigGroupParser::ParseReadyToDriveConfig(parsedTestJson);
            CHECK(readToDriveOpt.has_value());
            const auto readyToDrive = readToDriveOpt.value();

            CHECK_EQ(readyToDrive.readyToDriveSoundDuration, 1000);
            CHECK_EQ(readyToDrive.readyToDriveTriggeringBrakeThreshold, 40);
            CHECK_EQ(readyToDrive.readyToDriveButtonPort, utilsLib::GPIOPort::A);
            CHECK_EQ(readyToDrive.readyToDriveButtonPinNum, utilsLib::GPIOPinNum::Pin5);
            CHECK_EQ(readyToDrive.readyToDriveSoundPort, utilsLib::GPIOPort::D);
            CHECK_EQ(readyToDrive.readyToDriveSoundPinNum, utilsLib::GPIOPinNum::Pin2);
        }

        SUBCASE("WHEN a field is invalid is right THEN nullopt is returned")
        {
            std::string testJson = R"({
                "readyToDriveSoundDuration": -1000,
                "readyToDriveTriggeringBrakeThreshold": 40,
                "readyToDriveButtonPort": "A",
                "readyToDriveButtonPinNum": 5,
                "readyToDriveSoundPort": "G",
                "readyToDriveSoundPinNum": 2
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseReadyToDriveConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }

        SUBCASE("WHEN a field is missing THEN nullopt is returned")
        {
            std::string testJson = R"({
                "readyToDriveSoundDuration": 1000,
                "readyToDriveTriggeringBrakeThreshold": 40,
                "readyToDriveButtonPort": "A",
                "readyToDriveSoundPort": "D",
                "readyToDriveSoundPinNum": 2
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseReadyToDriveConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }
    }

    SUBCASE("SensorInterfaceThrottleConfig")
    {
        SUBCASE("WHEN everything is right THEN correct ReadyToDriveConfig is returned")
        {
            std::string testJson = R"({
                "ThrottleMinPin0": 600,
                "ThrottleMaxPin0": 2000,
                "ThrottleMinPin1": 700,
                "ThrottleMaxPin1": 3000,
                "MaxTorque": 1500,
                "ThrottleSignalOutOfRangeThreshold": 25,
                "ThrottleSignalDeviationThreshold": 25,
                "SignalDeadzone": 10
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto sensorInterfaceOpt = config::ConfigGroupParser::ParseSensorInterfaceThrottleConfig(parsedTestJson);
            CHECK(sensorInterfaceOpt.has_value());
            const auto sensorInterface = sensorInterfaceOpt.value();

            CHECK_EQ(sensorInterface.ThrottleMinPin0, 600);
            CHECK_EQ(sensorInterface.ThrottleMaxPin0, 2000);
            CHECK_EQ(sensorInterface.ThrottleMinPin1, 700);
            CHECK_EQ(sensorInterface.ThrottleMaxPin1, 3000);
            CHECK_EQ(sensorInterface.MaxTorque, 1500);
            CHECK_EQ(sensorInterface.ThrottleSignalOutOfRangeThreshold, 25);
            CHECK_EQ(sensorInterface.ThrottleSignalDeviationThreshold, 25);
            CHECK_EQ(sensorInterface.SignalDeadzone, 10);
        }

        SUBCASE("WHEN a field is invalid is right THEN nullopt is returned")
        {
            std::string testJson = R"({
                "ThrottleMinPin0": 600,
                "ThrottleMaxPin0": 2000,
                "ThrottleMinPin1": 700,
                "ThrottleMaxPin1": 3000,
                "MaxTorque": 1500,
                "ThrottleSignalOutOfRangeThreshold": 25,
                "ThrottleSignalDeviationThreshold": 25,
                "SignalDeadzone": "fuck"
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseSensorInterfaceThrottleConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }

        SUBCASE("WHEN a field is missing THEN nullopt is returned")
        {
            std::string testJson = R"({
                "ThrottleMinPin0": 600,
                "ThrottleMaxPin0": 2000,
                "ThrottleMinPin1": 700,
                "ThrottleMaxPin1": 3000,
                "ThrottleSignalOutOfRangeThreshold": 25,
                "ThrottleSignalDeviationThreshold": 25
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseSensorInterfaceThrottleConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }
    }

    SUBCASE("SensorInterfaceBrakeConfig")
    {
        SUBCASE("WHEN everything is right THEN correct ReadyToDriveConfig is returned")
        {
            std::string testJson = R"({
                "MaxBrake": 500,
                "BrakeMinPin": 10,
                "BrakeMaxPin": 200,
                "BrakeSignalOutOfRangeThreshold": 11
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto sensorInterfaceOpt = config::ConfigGroupParser::ParseSensorInterfaceBrakeConfig(parsedTestJson);
            CHECK(sensorInterfaceOpt.has_value());
            const auto sensorInterface = sensorInterfaceOpt.value();

            CHECK_EQ(sensorInterface.MaxBrake, 500);
            CHECK_EQ(sensorInterface.BrakeMinPin, 10);
            CHECK_EQ(sensorInterface.BrakeMaxPin, 200);
            CHECK_EQ(sensorInterface.BrakeSignalOutOfRangeThreshold, 11);
        }

        SUBCASE("WHEN a field is invalid is right THEN nullopt is returned")
        {
            std::string testJson = R"({
                "MaxBrake": 500,
                "BrakeMinPin": -10,
                "BrakeMaxPin": 200,
                "BrakeSignalOutOfRangeThreshold": 11
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseSensorInterfaceBrakeConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }

        SUBCASE("WHEN a field is missing THEN nullopt is returned")
        {
            std::string testJson = R"({
                "MaxBrake": 500,
                "BrakeSignalOutOfRangeThreshold": 11
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseSensorInterfaceBrakeConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }
    }

    SUBCASE("SensorInterfaceRegenConfig")
    {
        SUBCASE("WHEN everything is right THEN correct ReadyToDriveConfig is returned")
        {
            std::string testJson = R"({
                "RegenMinPin": 20,
                "RegenMaxPin": 220,
                "MaxRegen": 50,
                "haha": false
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto sensorInterfaceOpt = config::ConfigGroupParser::ParseSensorInterfaceRegenConfig(parsedTestJson);
            CHECK(sensorInterfaceOpt.has_value());
            const auto sensorInterface = sensorInterfaceOpt.value();

            CHECK_EQ(sensorInterface.RegenMinPin, 20);
            CHECK_EQ(sensorInterface.RegenMaxPin, 220);
            CHECK_EQ(sensorInterface.MaxRegen, 50);
        }

        SUBCASE("WHEN a field is invalid is right THEN nullopt is returned")
        {
            std::string testJson = R"({
                "RegenMinPin": 20,
                "RegenMaxPin": "C",
                "MaxRegen": 50,
                "haha": false
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseSensorInterfaceRegenConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }

        SUBCASE("WHEN a field is missing THEN nullopt is returned")
        {
            std::string testJson = R"({
                "RegenMinPin": 20,
                "RegenMaxPin": 220,
                "haha": false
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto loggerParametersOpt = config::ConfigGroupParser::ParseSensorInterfaceRegenConfig(parsedTestJson);
            CHECK_FALSE(loggerParametersOpt.has_value());
        }
    }
}

TEST_CASE("Config Parser")
{
    SUBCASE("WHEN everything is right THEN correct Config is returned")
    {
            std::string testJson = R"({
                "version": "1.0.0", 
                "spamLoggingEnabled": true, 
                "infoLoggingEnabled": true, 
                "errorLoggingEnabled": false, 
                "customLoggingEnabled": true, 
                "implausibleThresholdInterval": 69, 
                "readyToDriveSoundDuration": 1500, 
                "readyToDriveTriggeringBrakeThreshold": 12, 
                "readyToDriveButtonPort": "C", 
                "readyToDriveButtonPinNum": 7, 
                "readyToDriveSoundPort": "C", 
                "readyToDriveSoundPinNum": 1, 
                "ThrottleMinPin0": 610, 
                "ThrottleMaxPin0": 3725, 
                "ThrottleMinPin1": 1350, 
                "ThrottleMaxPin1": 3900, 
                "MaxTorque": 1510, 
                "ThrottleSignalOutOfRangeThreshold": 45, 
                "ThrottleSignalDeviationThreshold": 31,
                "SignalDeadzone": 18, 
                "BrakeMinPin": 350, 
                "BrakeMaxPin": 1550, 
                "BrakeSignalOutOfRangeThreshold": 200, 
                "MaxBrake": 120,
                "RegenMinPin": 500, 
                "RegenMaxPin": 1700, 
                "MaxRegen": 600
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto configParser = config::ConfigParser("1.0.0");
            const auto parsedOpt = configParser.Parse(parsedTestJson);
            CHECK(parsedOpt.has_value());
            const auto parsed = parsedOpt.value();

            // Logger
            CHECK_EQ(parsed.mLoggerConfig.spamLoggingEnabled, true);
            CHECK_EQ(parsed.mLoggerConfig.infoLoggingEnabled, true);
            CHECK_EQ(parsed.mLoggerConfig.errorLoggingEnabled, false);
            CHECK_EQ(parsed.mLoggerConfig.customLoggingEnabled, true);

            // Error
            CHECK_EQ(parsed.mErrorConfig.implausibleThresholdInterval, 69);

            // Ready To Drive
            CHECK_EQ(parsed.mReadyToDriveConfig.readyToDriveSoundDuration, 1500);
            CHECK_EQ(parsed.mReadyToDriveConfig.readyToDriveTriggeringBrakeThreshold, 12);
            CHECK_EQ(parsed.mReadyToDriveConfig.readyToDriveButtonPort, utilsLib::GPIOPort::C);
            CHECK_EQ(parsed.mReadyToDriveConfig.readyToDriveButtonPinNum, utilsLib::GPIOPinNum::Pin7);
            CHECK_EQ(parsed.mReadyToDriveConfig.readyToDriveSoundPort, utilsLib::GPIOPort::C);
            CHECK_EQ(parsed.mReadyToDriveConfig.readyToDriveSoundPinNum, utilsLib::GPIOPinNum::Pin1);

            // Sensor Interface Throttle
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.ThrottleMinPin0, 610);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.ThrottleMaxPin0, 3725);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.ThrottleMinPin1, 1350);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.ThrottleMaxPin1, 3900);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.MaxTorque, 1510);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.ThrottleSignalOutOfRangeThreshold, 45);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.ThrottleSignalDeviationThreshold, 31);
            CHECK_EQ(parsed.mSensorInterfaceThrottleConfig.SignalDeadzone, 18);

            // Sensor Interface Brake
            CHECK_EQ(parsed.mSensorInterfaceBrakeConfig.MaxBrake, 120);
            CHECK_EQ(parsed.mSensorInterfaceBrakeConfig.BrakeMinPin, 350);
            CHECK_EQ(parsed.mSensorInterfaceBrakeConfig.BrakeMaxPin, 1550);
            CHECK_EQ(parsed.mSensorInterfaceBrakeConfig.BrakeSignalOutOfRangeThreshold, 200);

            // Sensor Interface Regen
            CHECK_EQ(parsed.mSensorInterfaceRegenConfig.RegenMinPin, 500);
            CHECK_EQ(parsed.mSensorInterfaceRegenConfig.RegenMaxPin, 1700);
            CHECK_EQ(parsed.mSensorInterfaceRegenConfig.MaxRegen, 600);
    }

    SUBCASE("WHEN config version is not supported THEN nullopt is returned")
    {
            std::string testJson = R"({
                "version": "0.9.0", 
                "spamLoggingEnabled": true, 
                "infoLoggingEnabled": true, 
                "errorLoggingEnabled": false, 
                "customLoggingEnabled": true, 
                "implausibleThresholdInterval": 69, 
                "readyToDriveSoundDuration": 1500, 
                "readyToDriveTriggeringBrakeThreshold": 12, 
                "readyToDriveButtonPort": "C", 
                "readyToDriveButtonPinNum": 7, 
                "readyToDriveSoundPort": "C", 
                "readyToDriveSoundPinNum": 1, 
                "ThrottleMinPin0": 610, 
                "ThrottleMaxPin0": 3725, 
                "ThrottleMinPin1": 1350, 
                "ThrottleMaxPin1": 3900, 
                "MaxTorque": 1510, 
                "ThrottleSignalOutOfRangeThreshold": 45, 
                "ThrottleSignalDeviationThreshold": 31,
                "SignalDeadzone": 18, 
                "BrakeMinPin": 350, 
                "BrakeMaxPin": 1550, 
                "BrakeSignalOutOfRangeThreshold": 200, 
                "MaxBrake": 120,
                "RegenMinPin": 500, 
                "RegenMaxPin": 1700, 
                "MaxRegen": 600
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto configParser = config::ConfigParser("1.0.0");
            const auto parsedOpt = configParser.Parse(parsedTestJson);
            CHECK_FALSE(parsedOpt.has_value());
    }

    SUBCASE("WHEN some fields are missing THEN nullopt is returned")
    {
            std::string testJson = R"({
                "version": "0.9.0", 
                "spamLoggingEnabled": true, 
                "infoLoggingEnabled": true, 
                "errorLoggingEnabled": false, 
                "customLoggingEnabled": true, 
                "implausibleThresholdInterval": 69, 
                "readyToDriveSoundDuration": 1500, 
                "readyToDriveTriggeringBrakeThreshold": 12, 
                "readyToDriveButtonPort": "C", 
                "readyToDriveButtonPinNum": 7, 
                "readyToDriveSoundPort": "C", 
                "readyToDriveSoundPinNum": 1, 
                "ThrottleMinPin0": 610, 
                "ThrottleMaxPin0": 3725, 
                "ThrottleMinPin1": 1350, 
                "ThrottleMaxPin1": 3900, 
                "MaxTorque": 1510, 
                "ThrottleSignalOutOfRangeThreshold": 45, 
                "ThrottleSignalDeviationThreshold": 31,
                "SignalDeadzone": 18, 
                "BrakeSignalOutOfRangeThreshold": 200, 
                "MaxBrake": 120,
                "RegenMinPin": 500, 
                "RegenMaxPin": 1700, 
                "MaxRegen": 600
            })";

            const json parsedTestJson = json::parse(testJson, nullptr, false); // Parse without exception
            REQUIRE_FALSE(parsedTestJson.is_discarded());

            const auto configParser = config::ConfigParser("1.0.0");
            const auto parsedOpt = configParser.Parse(parsedTestJson);
            CHECK_FALSE(parsedOpt.has_value());
    }
}