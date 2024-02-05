#pragma once

#include <BMSInterfaceLib/Inc/BMSInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

#include <stm32f1xx.h>

// Forward declaration to make circular dependency between CANManager and MCUInterface work
namespace VehicleControlUnit::MCUInterfaceLib {
	class MCUInterface;
};

namespace VehicleControlUnit::UtilsLib {

// TODO :: Write test cases for all these fuckers

class CANManager
{
public:
	CANManager(UtilsLib::Logger& logger,
			CAN_HandleTypeDef& canHandler,
			const std::string & canPortName,
			BMSInterfaceLib::BMSInterface& BMSInterface,
			MCUInterfaceLib::MCUInterface& MCUInterface,
			SensorInterfaceLib::SensorInterface& sensorInterface):
		mLogger{logger},
		mCanHandler{canHandler},
		mCanPortName(canPortName),
		mBMSInterface{BMSInterface},
		mMCUInterface{MCUInterface},
		mSensorInterface{sensorInterface}
	{}

	ErrorState init();

	void SetTransmitHeader(
			const uint32_t messageIdentifier,
			const uint32_t messageLength,
			const bool identifierExtension = false,
			const bool isRemoteTransmissionRequest = false,
			const bool transmitGlobalTime = false);

	UtilsLib::ErrorState SendMessage(const uint8_t message[8]);
	void AbortAllSendRequests();
	void CheckReceiveFIFO();

private:
	void MessageReceiveHandler();

	const Logger& mLogger;
	CAN_HandleTypeDef& mCanHandler;
	const std::string mCanPortName;

	// Transmission
	CAN_TxHeaderTypeDef mTransmitHeader;
	uint32_t mLastMailboxUsed;

	// Receive
	CAN_RxHeaderTypeDef mReceiveHeader;
	uint8_t mReceiveBuffer[8];

	BMSInterfaceLib::BMSInterface &mBMSInterface;
	MCUInterfaceLib::MCUInterface &mMCUInterface;
	SensorInterfaceLib::SensorInterface &mSensorInterface;
};

}
