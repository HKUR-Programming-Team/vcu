#pragma once

#ifndef MOCK_TEST
	#include <UtilsLib/Inc/CANManager.hpp>
	#include <UtilsLib/Inc/Logger.hpp>
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

#include <DataStoreLib/Inc/DataStore.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <MainLib/Inc/Config.hpp>

// Forward declaration to make circular dependency between CANManager and MCUInterface work
namespace VehicleControlUnit::UtilsLib {
	class CANManager;
};

namespace VehicleControlUnit::MCUInterfaceLib {

class MCUInterface
{
public:
	MCUInterface(UtilsLib::Logger& logger,
			DataStoreLib::DataStore& dataStore,
			UtilsLib::CANManager& CANManager,
			const MainLib::Config::Config& config):
		mLogger{logger},
		mDataStore{dataStore},
		mCANManager{CANManager},
		mLastCommandMessageSendTs{0},
		mParameters{config.mMcuInterfaceConfig},
		mTCSTriggered{false},
		mTCSTriggeredStartTorque{config.mSensorInterfaceThrottleConfig.MaxTorque}
	{}

	void MessageReceiveHandler(const uint32_t messageID, const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

	void SendCommandMessageInErrorState();
	void SendCommandMessage();

private:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::CANManager& mCANManager;

	uint8_t mTransmitBuffer[8];
	uint32_t mLastCommandMessageSendTs;

	const MainLib::Config::MCUInterfaceConfig mParameters;

	bool mTCSTriggered;
	int16_t mTCSTriggeredStartTorque;

	// Traction Control System

	void SetCommandMessageTorque(int16_t torque);
	int16_t GetCommandMessageTorque() const;
	void SetCommandMessageInNonErrorState();
	void SetCommandMessage(int16_t torque, bool inverter, bool inverterDischarge, bool speedMode, uint16_t torqueLimit);
	void CheckTractionControlTriggered();
	void ModifyCommandMessageByTractionControl();

};

};
