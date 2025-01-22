#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS

#include <MockLibraries.hpp>
#include <doctest.h>
#include <string>
#include <optional>
#include <utility>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>
#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>
#include <MainLib/Inc/settings.hpp>

namespace sensorLib = VehicleControlUnit::SensorInterfaceLib;
namespace mcuLib = VehicleControlUnit::MCUInterfaceLib;
namespace r2dLib = VehicleControlUnit::ReadyToDriveLib;
namespace utilsLib = VehicleControlUnit::UtilsLib;
namespace dataLib = VehicleControlUnit::DataStoreLib;
namespace settings = VehicleControlUnit::MainLib::Settings;

TEST_CASE("SensorInterface ReadThrottleSignal") {
    utilsLib::ADCManager adcManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    settings::SensorInterfaceParameters sensorInterfaceParams;
    sensorInterfaceParams.ThrottleMinPin0 = 500;
    sensorInterfaceParams.ThrottleMaxPin0 = 1000;
    sensorInterfaceParams.ThrottleMinPin1 = 1500;
    sensorInterfaceParams.ThrottleMaxPin1 = 2000;
    sensorInterfaceParams.MaxTorque = 500;
    sensorInterfaceParams.ThrottleSignalOutOfRangeThreshold = 20;
    sensorInterfaceParams.ThrottleSignalDeviationThreshold = 50;
    sensorInterfaceParams.ThrottleSignalADCIndex1 = 0;
    sensorInterfaceParams.ThrottleSignalADCIndex2 = 1;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, sensorInterfaceParams);

    SUBCASE("WHEN there exists a throttle sensor that gives value that is out of the threshold THEN error is set in datastore correctly")
    {
        SUBCASE("Pin 0 bottom out of range")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 475;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1500;

            uint16_t checkADC1 = 0;
            uint16_t checkADC2 = 0;
            adcManager.GetBufferByIndex(sensorInterfaceParams.ThrottleSignalADCIndex1, checkADC1);
            adcManager.GetBufferByIndex(sensorInterfaceParams.ThrottleSignalADCIndex2, checkADC2);
            REQUIRE(checkADC1 == 475);
            REQUIRE(checkADC2 == 1500);

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Pin 1 bottom out of range")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 500;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1475;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Pin 0 top out of range")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 1025;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 2000;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Pin 1 top out of range")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 1000;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 2025;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("Both out of range")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 475;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1475;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }
    }

    SUBCASE("WHEN throttle is slight out of range but inside the out of range threshold THEN the correct torque is set in datastore")
    {
        SUBCASE("zero percent")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 485;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1485;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }

        SUBCASE("100 percent")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 1015;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 2015;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == sensorInterfaceParams.MaxTorque);
        }
    }

    SUBCASE("WHEN the out of range threshold is higher than ThrottleMin THEN data is stored to datastore correctly")
    {
        settings::SensorInterfaceParameters sensorInterfaceParamsSpecial;
        sensorInterfaceParamsSpecial.ThrottleMinPin0 = 10;
        sensorInterfaceParamsSpecial.ThrottleMaxPin0 = 510;
        sensorInterfaceParamsSpecial.ThrottleMinPin1 = 1500;
        sensorInterfaceParamsSpecial.ThrottleMaxPin1 = 2000;
        sensorInterfaceParamsSpecial.MaxTorque = 500;
        sensorInterfaceParamsSpecial.ThrottleSignalOutOfRangeThreshold = 20;
        sensorInterfaceParamsSpecial.ThrottleSignalDeviationThreshold = 50;
        sensorLib::SensorInterface sensorInterfaceSpecial(logger, dataStore, adcManager, sensorInterfaceParams);

        SUBCASE("WHEN only pin 1 out of bottom range THEN error is set")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 0;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1475;

            sensorInterfaceSpecial.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 0);
        }
    }

    SUBCASE("WHEN both throttle sensors are at a certain percentage THEN correct torque is set in datastore")
    {
        SUBCASE("50 percent")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 750;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1750;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == sensorInterfaceParams.MaxTorque/2);
        }

        SUBCASE("25 percent")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 625;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1625;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 125);
        }
        
        SUBCASE("75 percent")
        {
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 875;
            adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1875;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
            CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 375);
        }
    }

    SUBCASE("WHEN the throttle sensors has less than 10 percent deviation THEN the one with lower value is set as torque in datastore")
    {
        adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 740; // Torque = 240
        adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1760; // Torque = 260

        sensorInterface.ReadADC();
        CHECK(!dataStore.mDrivingInputDataStore.GetThrottleError());
        CHECK(dataStore.mDrivingInputDataStore.GetTorque() == 240);
    }

    SUBCASE("WHEN the throttle sensors has more than 10 percent deviation THEN error is set in datastore")
    {
        adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex1] = 700; // Torque = 200
        adcManager.buffer[sensorInterfaceParams.ThrottleSignalADCIndex2] = 1755; // Torque = 255

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

    settings::SensorInterfaceParameters sensorInterfaceParams;
    sensorInterfaceParams.BrakeMinPin = 500;
    sensorInterfaceParams.BrakeMaxPin = 1000;
    sensorInterfaceParams.BrakeSignalOutOfRangeThreshold = 20;
    sensorInterfaceParams.MaxBrake = 500;
    sensorInterfaceParams.BrakeSignalADCIndex = 0;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, sensorInterfaceParams);

    SUBCASE("WHEN there exists a brake sensor that gives value within range THEN value is set in datastore correctly")
    {
        adcManager.buffer[sensorInterfaceParams.BrakeSignalADCIndex] = 600;

        uint16_t checkADC = 0;
        adcManager.GetBufferByIndex(sensorInterfaceParams.BrakeSignalADCIndex, checkADC);
        REQUIRE(checkADC == 600);

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
        CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 100);
    }

    SUBCASE("WHEN there exists a brake sensor that gives value out of threshold THEN value is set in datastore correctly")
    {
        SUBCASE("bottom threshold")
        {
            adcManager.buffer[sensorInterfaceParams.BrakeSignalADCIndex] = 470;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetBrake().has_value());
        }

        SUBCASE("bottom threshold")
        {
            adcManager.buffer[sensorInterfaceParams.BrakeSignalADCIndex] = 1025;

            sensorInterface.ReadADC();
            CHECK(!dataStore.mDrivingInputDataStore.GetBrake().has_value());
        }
    }

    SUBCASE("WHEN there exists a brake sensor that gives value out of range but within threshold THEN value is set in datastore correctly")
    {
        SUBCASE("bottom threshold")
        {
            adcManager.buffer[sensorInterfaceParams.BrakeSignalADCIndex] = 485;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 0);
        }

        SUBCASE("bottom threshold")
        {
            adcManager.buffer[sensorInterfaceParams.BrakeSignalADCIndex] = 1015;

            sensorInterface.ReadADC();
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().has_value());
            CHECK(dataStore.mDrivingInputDataStore.GetBrake().value_or(69420) == 500);
        }
    }

    SUBCASE("GIVEN BrakeMinPin <= threshold WHEN there exists a brake sensor that gives value within range THEN value is set in datastore correctly")
    {
        settings::SensorInterfaceParameters sensorInterfaceParams;
        sensorInterfaceParams.BrakeMinPin = 10;
        sensorInterfaceParams.BrakeMaxPin = 510;
        sensorInterfaceParams.BrakeSignalOutOfRangeThreshold = 20;
        sensorInterfaceParams.MaxBrake = 500;
        sensorInterfaceParams.BrakeSignalADCIndex = 0;

        sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, sensorInterfaceParams);
        adcManager.buffer[sensorInterfaceParams.BrakeSignalADCIndex] = 15;

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

    settings::SensorInterfaceParameters sensorInterfaceParams;
    sensorInterfaceParams.RegenMinPin = 500;
    sensorInterfaceParams.RegenMaxPin = 1000;
    sensorInterfaceParams.MaxRegen = 200;
    sensorInterfaceParams.RegenSignalADCIndex = 0;

    sensorLib::SensorInterface sensorInterface(logger, dataStore, adcManager, sensorInterfaceParams);

    SUBCASE("WHEN sensor gives value within range THEN value is set in datastore correctly")
    {
        adcManager.buffer[sensorInterfaceParams.RegenSignalADCIndex] = 750;

        uint16_t checkADC = 0;
        adcManager.GetBufferByIndex(sensorInterfaceParams.RegenSignalADCIndex, checkADC);
        REQUIRE(checkADC == 750);

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetRegen() == 100);
    }

    SUBCASE("WHEN sensor gives value below range THEN value is set in datastore correctly")
    {
        adcManager.buffer[sensorInterfaceParams.RegenSignalADCIndex] = 450;

        sensorInterface.ReadADC();
        CHECK(dataStore.mDrivingInputDataStore.GetRegen() == 0);
    }
    
    SUBCASE("WHEN sensor gives value above range THEN value is set in datastore correctly")
    {
        adcManager.buffer[sensorInterfaceParams.RegenSignalADCIndex] = 1200;

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

    SUBCASE("GIVEN first MCU broadcast message is received since more than 1 second ago WHEN command message frequency is less than 20 THEN error is set in DataStore")
    {
        dataStore.mMCUDataStore.SetLastMCUBroadcastMessageReceiveTs(10); // First broadcast message
        mcuErrorManager.CheckCommandMessageFrequency();

        dataStore.mMCUDataStore.SetCommandMessageFrequency(15);
        MockCurrentTick = 1100;
        mcuErrorManager.CheckCommandMessageFrequency();

        CHECK(dataStore.GetCommandMessageFrequencyError());
    }

    SUBCASE("GIVEN first MCU broadcast message is received since more than 1 second ago WHEN command message frequency is more than 20 THEN no error is set in DataStore")
    {
        dataStore.mMCUDataStore.SetLastMCUBroadcastMessageReceiveTs(10); // First broadcast message
        mcuErrorManager.CheckCommandMessageFrequency();

        dataStore.mMCUDataStore.SetCommandMessageFrequency(25);
        MockCurrentTick = 1100;
        mcuErrorManager.CheckCommandMessageFrequency();
        
        CHECK(!dataStore.GetCommandMessageFrequencyError());
    }

    SUBCASE("WHEN last MCU broadcast message is received more than 500 ms ago THEN error is set in DataStore")
    {
        dataStore.mMCUDataStore.SetLastMCUBroadcastMessageReceiveTs(10);
        MockCurrentTick = 600;

        mcuErrorManager.CheckMCUTimeout();
        CHECK(dataStore.GetBroadcastMessageReceiveTimeoutError());
    }

    SUBCASE("WHEN last MCU broadcast message is received less than 500 ms ago THEN no error is set in DataStore")
    {
        dataStore.mMCUDataStore.SetLastMCUBroadcastMessageReceiveTs(10);
        MockCurrentTick = 450;

        mcuErrorManager.CheckMCUTimeout();
        CHECK(!dataStore.GetBroadcastMessageReceiveTimeoutError());
    }

    SUBCASE("WHEN first broadcast message is not received AND frequency is less than 20 THEN error is not set in DataStore")
    {
        dataStore.mMCUDataStore.SetCommandMessageFrequency(15);
        mcuErrorManager.CheckCommandMessageFrequency();

        CHECK(!dataStore.GetCommandMessageFrequencyError());
    }
}

TEST_CASE("MCUInterface driving input")
{
    utilsLib::CANManager canManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    settings::MCUInterfaceParameters mcuInterfaceParams;
    mcuInterfaceParams.InverterEnableTorqueThreshold = 10;
    mcuInterfaceParams.RegenEnableTorqueThreshold = 5;
    mcuInterfaceParams.CommandMessageTransmitInterval = 5;
    mcuInterfaceParams.commandFrequencyUpdateInterval = 1000; 

    mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, mcuInterfaceParams);
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

        SUBCASE("WHEN the current tick has past the transmit interval THEN new command message is sent")
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

    SUBCASE("Command Delay Guarantee and Frequency Monitoring")
    {
        
        SUBCASE("WHEN commandFrequencyUpdateInterval has passed since last frequency update THEN data store is updated correctly")
        {
            // Before
            CHECK_FALSE(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());

            MockCurrentTick = 1002;
            mcuInterface.StoreCommandMessageFrequency();

            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 0);
        }

        SUBCASE("WHEN only one message is sent but two complete callbacks are received THEN only one message is counted")
        {
            MockCurrentTick = 1002;
            mcuInterface.SendCommandMessage(); //Mailbox with index 1 is used.

            mcuInterface.CANMailboxCompletedCallbackHandler(1);
            mcuInterface.CANMailboxCompletedCallbackHandler(1);
            mcuInterface.StoreCommandMessageFrequency();
            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 1); // one message in the last 1000 ms => frequency is 1
        }

        SUBCASE("WHEN commandFrequencyUpdateInterval has not passed since last frequency update THEN data store is not updated")
        {
            MockCurrentTick = 1002;
            mcuInterface.SendCommandMessage(); //Mailbox with index 1 is used.
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            MockCurrentTick = 1007;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            mcuInterface.StoreCommandMessageFrequency();
            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 2);

            MockCurrentTick = 1012;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);
            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 2);
        }

        SUBCASE("WHEN commandFrequencyUpdateInterval has passed since last frequency update THEN the count is reset")
        {
            MockCurrentTick = 1002;
            mcuInterface.SendCommandMessage(); //Mailbox with index 1 is used.
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            MockCurrentTick = 1007;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            mcuInterface.StoreCommandMessageFrequency();
            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 2);

            MockCurrentTick = 2008;
            mcuInterface.StoreCommandMessageFrequency(); // reset
            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 0);
        }

        SUBCASE("WHEN transmit message complete callback is invoked on another mailbox THEN the count is not updated")
        {
            MockCurrentTick = 1002;
            mcuInterface.SendCommandMessage(); //Mailbox with index 1 is used.
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            MockCurrentTick = 1007;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            MockCurrentTick = 1012;
            mcuInterface.SendCommandMessage(); //Mailbox with index 1 is used.
            mcuInterface.CANMailboxCompletedCallbackHandler(2); // callback for index 2.

            mcuInterface.StoreCommandMessageFrequency();
            CHECK(dataStore.mMCUDataStore.GetCommandMessageFrequency().has_value());
            CHECK_EQ(dataStore.mMCUDataStore.GetCommandMessageFrequency().value_or(69420), 2);
        }

        SUBCASE("WHEN the current tick has past the transmit interval THEN abort is called")
        {
            REQUIRE_EQ(canManager.mAbortCount, 0);
            MockCurrentTick = 6;
            mcuInterface.SendCommandMessage(); // First send function will always abort everything
            mcuInterface.CANMailboxCompletedCallbackHandler(1);

            MockCurrentTick = 11;
            mcuInterface.SendCommandMessage(); // Should NOT abort as the ACK has been received.
            CHECK_EQ(canManager.mAbortCount, 2);
        }
    }
}

TEST_CASE("MCUInterface TCS")
{
    utilsLib::CANManager canManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    settings::MCUInterfaceParameters mcuInterfaceParams;
    mcuInterfaceParams.InverterEnableTorqueThreshold = 10;
    mcuInterfaceParams.RegenEnableTorqueThreshold = 5;
    mcuInterfaceParams.CommandMessageTransmitInterval = 3;
    mcuInterfaceParams.TCSTriggeringSlipRatioThreshold = 20;
    mcuInterfaceParams.TCSHaltSlipRatioThreshold = 12;
    mcuInterfaceParams.WheelRadius = 1;

    mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, mcuInterfaceParams);
    MockCurrentTick = 4;

    dataStore.SetPersistedImplausibleStatus(false);
    dataStore.mDrivingInputDataStore.SetTCSEnabled(true);

    SUBCASE("Originally tcs not triggered, WHEN slipRatio > TCSTriggeringSlipRatioThreshold, tcs is triggered, THEN WHEN slipRatio >= TCSHaltSlipRatioThreshold, torque in command message is restricted")
    {
        dataStore.mDrivingInputDataStore.SetTorque(300);
        dataStore.mDrivingInputDataStore.SetRegen(0);
        dataStore.mDrivingInputDataStore.SetGear(dataLib::Gear::FORWARD);
        dataStore.mVehicleSensorDataStore.SetAngularWheelSpeedRearLeft(44.0f);
        dataStore.mVehicleSensorDataStore.SetAngularWheelSpeedRearRight(44.0f);
        dataStore.mVehicleSensorDataStore.SetLinearVelocity(2.0f);

        // maxWheelSpeed = leftWheelSpeed > rightWheelSpeed ? leftWheelSpeed : rightWheelSpeed;
        // slipRatio = (mParameters.WheelRadius * maxWheelSpeed - linearSpeed)/linearSpeed;
        
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

        dataStore.mDrivingInputDataStore.SetTorque(350);

        MockCurrentTick = 7;
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
}

TEST_CASE("MCUInterface MessageReceiveHandler")
{
    utilsLib::CANManager canManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    settings::MCUInterfaceParameters mcuInterfaceParams;

    mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, mcuInterfaceParams);
    uint8_t payload[8] = {0,0,0,0,0,0,0,0};
    CAN_RxHeaderTypeDef header;
    MockCurrentTick = 10;

    SUBCASE("WHEN no meesage from MCUInterface has yet been received THEN member variables in MCUDataStore gives empty value")
    {
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
        CHECK(!dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
        CHECK(!dataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs().has_value());
    }

    SUBCASE("WHEN a motor speed message with message id 0x0A5 has been received THEN data is stored correctly")
    {
        const uint32_t messageId = 0x0A5;
        header.StdId = 0x0A5;
        MockCurrentTick = 569;

        payload[2] = 44;
        payload[3] = 1; 

        mcuInterface.MessageReceiveHandler(messageId, header, payload);

        REQUIRE(dataStore.mMCUDataStore.GetMotorSpeed().has_value());
        REQUIRE(dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
        CHECK(dataStore.mMCUDataStore.GetMotorSpeed().value() == 300);
        CHECK(dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().value() == 569);

        REQUIRE(!dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
        CHECK_EQ(dataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs().value_or(6969), 569);
    }

    SUBCASE("WHEN a RUN FAULT message with message id 0x0AB has been received THEN data is stored correctly")
    {
        const uint32_t messageId = 0x0AB;
        header.StdId = 0x0A5;
        MockCurrentTick = 569;

        SUBCASE("No error")
        {
            payload[5] = 0b00000000;
            mcuInterface.MessageReceiveHandler(messageId, header, payload);

            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
            CHECK(!dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
            CHECK_EQ(dataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs().value_or(6969), 569);
            
        }

        SUBCASE("Has error")
        {
            payload[5] = 0b00001000;
            mcuInterface.MessageReceiveHandler(messageId, header, payload);

            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
            REQUIRE(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
            CHECK(dataStore.mMCUDataStore.GetMessageReceiveTimeoutError());
            CHECK_EQ(dataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs().value_or(6969), 569);
        }
    }

    SUBCASE("WHEN a message with unknown message id has been received THEN member variables in MCUDataStore gives empty value")
    {
        const uint32_t messageId = 0x0C0;
        header.StdId = 0x0C0;

        mcuInterface.MessageReceiveHandler(messageId, header, payload);
        
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeed().has_value());
        CHECK(!dataStore.mMCUDataStore.GetMotorSpeedUpdateTs().has_value());
        CHECK(!dataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs().has_value());
    }
}

TEST_CASE("Ready to drive")
{
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;
    settings::ReadyToDriveParameters r2dparams;

    r2dparams.readyToDriveSoundDuration = 1000;
	r2dparams.readyToDriveTriggeringBrakeThreshold = 100;
	r2dparams.readyToDriveButtonPort = utilsLib::GPIOPort::A;
	r2dparams.readyToDriveButtonPinNum = utilsLib::GPIOPinNum::Pin0;
	r2dparams.readyToDriveSoundPort = utilsLib::GPIOPort::B;
	r2dparams.readyToDriveSoundPinNum = utilsLib::GPIOPinNum::Pin1;

    r2dLib::ReadyToDrive r2d(logger, dataStore, r2dparams);

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

TEST_CASE("Command Frequency Check Integration test")
{
    utilsLib::CANManager canManager;
    dataLib::DataStore dataStore;
    utilsLib::Logger logger;

    const uint32_t implausibleThresholdInterval = 1000;
    mcuLib::MCUErrorManager mcuErrorManager(logger, dataStore, implausibleThresholdInterval);

    settings::MCUInterfaceParameters mcuInterfaceParams;
    mcuLib::MCUInterface mcuInterface(logger, dataStore, canManager, mcuInterfaceParams);

    CAN_RxHeaderTypeDef header;
    uint8_t payload[8] = {0,0,0,0,0,0,0,0};

    SUBCASE("GIVEN first MCU broadcast message is received AND more than one second has passed since then WHEN there are not enough CAN messages completed callbacks to satisfy minimum frequency requirements THEN error is set in DataStore")
    {
        // Set first message received at time 100.
        MockCurrentTick = 100;
        mcuInterface.MessageReceiveHandler(0x0AB, header, payload);

        // Record the first MCU broadcast message receive timestamp
        mcuErrorManager.CheckCommandMessageFrequency();
        
        // Addition to command message frequency: Only 5 messages in the last 0.5 seconds => frequency of 10 hz
        for (int i = 0; i < 5; i++)
        {
            MockCurrentTick += 3;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);
        }

        // Time to store the command message frequency
        MockCurrentTick = 600;
        mcuInterface.StoreCommandMessageFrequency();

        // Already one second after the first message, can give error.
        MockCurrentTick = 1200;
        mcuErrorManager.CheckCommandMessageFrequency();
        CHECK(dataStore.GetCommandMessageFrequencyError());
    }

    SUBCASE("GIVEN first MCU broadcast message is received AND less than one second has passed since then WHEN there are not enough CAN messages completed callbacks to satisfy minimum frequency requirements THEN error NOT is set in DataStore")
    {
        // Set first message received at time 100.
        MockCurrentTick = 100;
        mcuInterface.MessageReceiveHandler(0x0AB, header, payload);

        // Record the first MCU broadcast message receive timestamp
        mcuErrorManager.CheckCommandMessageFrequency();
        
        // Addition to command message frequency: Only 5 messages in the last 0.5 seconds => frequency of 10 hz
        for (int i = 0; i < 5; i++)
        {
            MockCurrentTick += 3;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);
        }

        // Time to store the command message frequency
        MockCurrentTick = 850;
        mcuInterface.StoreCommandMessageFrequency();

        mcuErrorManager.CheckCommandMessageFrequency();
        CHECK(!dataStore.GetCommandMessageFrequencyError());
    }

    SUBCASE("GIVEN first MCU broadcast message is received AND more than one second has passed since then WHEN there are enough CAN messages completed callbacks to satisfy minimum frequency requirements THEN error is NOT set in DataStore")
    {
        // Set first message received at time 500.
        MockCurrentTick = 100;
        mcuInterface.MessageReceiveHandler(0x0AB, header, payload);

        // Record the first MCU broadcast message receive timestamp
        mcuErrorManager.CheckCommandMessageFrequency();
        
        // Addition to command message frequency: 15 messages in the last 0.5 seconds => frequency of 30 hz
        for (int i = 0; i < 15; i++)
        {
            MockCurrentTick += 3;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);
        }

        // Time to store the command message frequency
        MockCurrentTick = 550;
        mcuInterface.StoreCommandMessageFrequency();

        // Already one second after the first message, can give error.
        MockCurrentTick = 1200;
        
        mcuErrorManager.CheckCommandMessageFrequency();
        CHECK(!dataStore.GetCommandMessageFrequencyError());
    }
    
    SUBCASE("GIVEN first MCU broadcast message is NOT received WHEN there are not enough CAN messages completed callbacks to satisfy minimum frequency requirements THEN error is NOT set in DataStore")
    {
        // Addition to command message frequency: Only 5 messages in the last 0.5 seconds => frequency of 10 hz
        for (int i = 0; i < 5; i++)
        {
            MockCurrentTick += 3;
            mcuInterface.SendCommandMessage();
            mcuInterface.CANMailboxCompletedCallbackHandler(1);
        }

        // Time to store the command message frequency
        MockCurrentTick = 550;
        mcuInterface.StoreCommandMessageFrequency();

        mcuErrorManager.CheckCommandMessageFrequency();
        CHECK(!dataStore.GetCommandMessageFrequencyError()); // No error since first broadcast message is not received
    }

    SUBCASE("GIVEN first MCU broadcast message is received WHEN no broadcast message is received for more than 0.5 seconds THEN error is set in DataStore")
    {
        // Set first message received at time 500.
        MockCurrentTick = 100;
        mcuInterface.MessageReceiveHandler(0x0AB, header, payload);

        MockCurrentTick = 650;
        mcuErrorManager.CheckMCUTimeout();
        CHECK(dataStore.GetBroadcastMessageReceiveTimeoutError());
    }
}