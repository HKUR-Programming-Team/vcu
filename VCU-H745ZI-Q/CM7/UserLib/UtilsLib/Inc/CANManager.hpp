#pragma once

#include <BMSInterfaceLib/Inc/BMSInterface.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

#include <stm32h7xx.h>

// Forward declaration to make circular dependency between CANManager and MCUInterface work
namespace VehicleControlUnit { namespace MCUInterfaceLib {
	class MCUInterface;
}};

namespace VehicleControlUnit { namespace UtilsLib {

enum class CANFrameType
{
	DATA,
	REMOTE
};

// You probably wonder why the fuck would I do this instead of using an unsigned integer
// The reason is that I don't want invalid data length values to be used. I want this to be enforced in compile time instead of runtime.
enum class CANDataLength
{
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT
};

enum class CANReceiveFIFONum
{
	ZERO,
	ONE
};

class CANManager
{
public:
	CANManager(UtilsLib::Logger& logger, FDCAN_HandleTypeDef& canHandler, const std::string & canPortName, const CANReceiveFIFONum receiveFIFONum):
		mLogger{logger},
		mCanHandler{canHandler},
		mCanPortName(canPortName),
		mReceiveFIFONum{receiveFIFONum}
	{}

	ErrorState init();

	void SetTransmitHeader(
			const uint32_t messageIdentifier,
			const bool identifierExtension,
			const CANFrameType frameType = CANFrameType::DATA,
			const CANDataLength dataLength = CANDataLength::EIGHT);

	UtilsLib::ErrorState SendMessage(const uint8_t message[8]);
	void CheckReceiveFIFO();

	// MessageHandler of different CAN transceiver should have different implementations
	virtual UtilsLib::ErrorState MessageReceiveHandler() = 0;

protected:
	const Logger& mLogger;
	FDCAN_HandleTypeDef& mCanHandler;
	const std::string mCanPortName;

	// Transmission
	FDCAN_TxHeaderTypeDef mTransmitHeader;
	uint8_t mTransmitBuffer[8];

	// Receive
	FDCAN_RxHeaderTypeDef mReceiveHeader;
	uint8_t mReceiveBuffer[8];
	CANReceiveFIFONum mReceiveFIFONum;
};


class CANManangerForBMSAndMCU : public CANManager
{

public:
	CANManangerForBMSAndMCU(UtilsLib::Logger& logger,
			FDCAN_HandleTypeDef& canHandler,
			const std::string & canPortName,
			BMSInterfaceLib::BMSInterface& BMSInterface,
			MCUInterfaceLib::MCUInterface& MCUInterface,
			const CANReceiveFIFONum receiveFIFONum):
		CANManager(logger, canHandler, canPortName, receiveFIFONum),
		mBMSInterface(BMSInterface),
		mMCUInterface(MCUInterface)
	{}

	virtual UtilsLib::ErrorState MessageReceiveHandler();

private:
	BMSInterfaceLib::BMSInterface &mBMSInterface;
	MCUInterfaceLib::MCUInterface &mMCUInterface;
};

class CANManangerForSensors : public CANManager
{

public:
	CANManangerForSensors(UtilsLib::Logger& logger,
			FDCAN_HandleTypeDef& canHandler,
			const std::string & canPortName,
			SensorInterfaceLib::SensorInterface &sensorInterface,
			const CANReceiveFIFONum receiveFIFONum):
		CANManager(logger, canHandler, canPortName, receiveFIFONum),
		mSensorInterface(sensorInterface)
	{}

	virtual UtilsLib::ErrorState MessageReceiveHandler();

private:
	SensorInterfaceLib::SensorInterface &mSensorInterface;
};

}}
