#pragma once

#include <BMSInterfaceLib/Inc/BMSInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

#include <stm32f1xx.h>

// Forward declaration to make circular dependency between CANManager and MCUInterface work
namespace VehicleControlUnit { namespace MCUInterfaceLib {
	class MCUInterface;
}};

namespace VehicleControlUnit { namespace UtilsLib {

// TODO :: Write test cases for all these fuckers

class CANManager
{
public:
	CANManager(UtilsLib::Logger& logger, CAN_HandleTypeDef& canHandler, const std::string & canPortName):
		mLogger{logger},
		mCanHandler{canHandler},
		mCanPortName(canPortName)
	{}

	ErrorState init();

	void SetTransmitHeader(
			const uint32_t messageIdentifier,
			const uint32_t messageLength,
			const bool identifierExtension = false,
			const bool isRemoteTransmissionRequest = false,
			const bool transmitGlobalTime = false);

	UtilsLib::ErrorState SendMessage(const uint8_t message[8]);

	// MessageHandler of different CAN transceiver should have different implementations
	virtual UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]) = 0;

protected:
	const Logger& mLogger;
	CAN_HandleTypeDef& mCanHandler;
	const std::string mCanPortName;

	// Transmission
	CAN_TxHeaderTypeDef mTransmitHeader;
	uint8_t mTransmitBuffer[8];
	uint32_t mLastMailboxUsed;

	// Receive
	CAN_RxHeaderTypeDef mReceiveHeader;
	uint8_t mReceiveBuffer[8];
};


class CANManangerForBMSAndMCU : public CANManager
{

public:
	CANManangerForBMSAndMCU(UtilsLib::Logger& logger, CAN_HandleTypeDef& canHandler, const std::string & canPortName, BMSInterfaceLib::BMSInterface& BMSInterface, MCUInterfaceLib::MCUInterface& MCUInterface):
			CANManager(logger, canHandler, canPortName),
			mBMSInterface(BMSInterface),
			mMCUInterface(MCUInterface)
			// TODO :: Initialize dependency injection for MCU here
	{}

	virtual UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

private:
	// TODO :: Inject dependency here: MCU interface library.
	BMSInterfaceLib::BMSInterface &mBMSInterface;
	MCUInterfaceLib::MCUInterface &mMCUInterface;
};

class CANManangerForSensors : public CANManager
{

public:
	CANManangerForSensors(UtilsLib::Logger& logger, CAN_HandleTypeDef& canHandler, const std::string & canPortName):
			CANManager(logger, canHandler, canPortName)
			// TODO :: Initialize dependency injection here
	{}

	virtual UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

private:
	// TODO :: Inject dependency here: Sensor interface libraries.
};

}}
