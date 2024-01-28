#pragma once

#ifndef MOCK_TEST
	#include <UtilsLib/Inc/CANManager.hpp>
	#include <UtilsLib/Inc/Logger.hpp>
#else
	#include <MockLibraries.hpp>
#endif

#include <DataStoreLib/Inc/DataStore.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

// Forward declaration to make circular dependency between CANManager and MCUInterface work
namespace VehicleControlUnit::UtilsLib {
	class CANManager;
};

namespace VehicleControlUnit::MCUInterfaceLib {

class MCUInterface
{
public:
	MCUInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, UtilsLib::CANManager& CANManager):
			mLogger{logger},
			mDataStore{dataStore},
			mCANManager{CANManager}
	{}

	UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

	void SendCommandMessageInErrorState();
	UtilsLib::ErrorState SendCommandMessage();

private:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::CANManager& mCANManager;

	uint8_t mTransmitBuffer[8];

	const uint32_t CommandMessageHeaderId = 0x0C0;
	const uint32_t CommandMessageLength = 8;

	const uint32_t CommandMessageTransmitInterval = 2;

	const double InverterEnableTorqueThreshold = 0.005;

	// Traction Control System


	void SetCommandMessageInNonErrorState();
	void SetCommandMessage(uint16_t torque, uint16_t angularVelocity, bool directionForward, bool inverter, bool inverterDischarge, bool speedMode, uint16_t torqueLimit);
	bool TractionControlShouldBeTriggered();
	void ModifyCommandMessageByTractionControl();

};

};
