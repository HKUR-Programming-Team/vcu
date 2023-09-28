#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

#include <stm32f1xx.h>

namespace VehicleControlUnit { namespace UtilsLib {

// TODO :: Write test cases for all these fuckers

class CANManager
{
public:
	CANManager(UtilsLib::Logger& logger, CAN_HandleTypeDef& canHandler, const std::string & canPortName):
		logger{logger},
		canHandler{canHandler},
		canPortName(canPortName)
	{}

	ErrorState init();

	void SetTransmitHeader(
			const uint32_t messageIdentifier,
			const uint32_t messageLength,
			const bool identifierExtension = false,
			const bool isRemoteTransmissionRequest = false,
			const bool transmitGlobalTime = false);

	UtilsLib::ErrorState SendMessage(const uint8_t message[8]);

private:
	const Logger& logger;
	CAN_HandleTypeDef& canHandler;
	const std::string canPortName;

	// Transmission
	CAN_TxHeaderTypeDef transmitHeader;
	uint8_t transmitBuffer[8];
	uint32_t lastMailboxUsed;

	// Receive
	CAN_RxHeaderTypeDef receiveHeader;
	uint8_t receiveBuffer[8];
};

}}
