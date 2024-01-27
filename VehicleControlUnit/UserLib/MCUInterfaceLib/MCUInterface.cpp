#include <MCUInterfaceLib/Inc/MCUInterface.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

UtilsLib::ErrorState MCUInterface::MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Handle the CAN message received from MCU, store the data in the DataStore");
	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

void MCUInterface::SendCommandMessageInErrorState()
{
	mCANManager.SetTransmitHeader(CommandMessageHeaderId, CommandMessageLength);
	SetCommandMessage(0, 0, true, false, true, false, 0);
	mCANManager.SendMessage(mTransmitBuffer);
}

UtilsLib::ErrorState MCUInterface::SendCommandMessage()
{
	mLogger.LogInfo("TODO: Send message to motor controller");

	// Check if the engine should be stopped
	if (mDataStore.GetPersistedImplausibleStatus())
	{
		SendCommandMessageInErrorState();
		return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_FAIL; // placeholder
	}

	SetCommandMessageInNonErrorState();

	if (TractionControlShouldBeTriggered())
	{
		ModifyCommandMessageByTractionControl();
	}

	mCANManager.SetTransmitHeader(CommandMessageHeaderId, CommandMessageLength);
	const auto error = mCANManager.SendMessage(mTransmitBuffer);

	mLogger.LogInfo("TODO: MCUInterface set error");

	// If there is error state, store it to the data store

	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

void MCUInterface::SetCommandMessageInNonErrorState()
{
	mLogger.LogInfo("TODO: MCUInterface Torque");
	const uint16_t torque = mDataStore.mDriveDataStore.GetTorque();
	const uint16_t angularVelocity = 0;
	const bool directionForward = mDataStore.mDriveDataStore.GetGear() == DataStoreLib::Gear::FORWARD;
	const bool inverter = torque >= InverterEnableTorqueThreshold * SensorInterfaceLib::SensorInterface::MaxTorque;
	const bool inverterDischarge = !inverter;
	const auto speedMode = false;
	const uint32_t torqueLimit = 0;

	SetCommandMessage(torque, angularVelocity, directionForward, inverter, inverterDischarge, speedMode, torqueLimit);

	mLogger.LogCustom("Before TCS:" + std::to_string(mTransmitBuffer[0]) + ", " + std::to_string(mTransmitBuffer[1])
				+ ", " + std::to_string(mTransmitBuffer[2]) + ", " + std::to_string(mTransmitBuffer[3])
				+ ", " + std::to_string(mTransmitBuffer[4]) + ", " + std::to_string(mTransmitBuffer[5])
				+ ", " + std::to_string(mTransmitBuffer[6]) + ", " + std::to_string(mTransmitBuffer[7]));
}

void MCUInterface::SetCommandMessage(uint16_t torque,
		uint16_t angularVelocity,
		bool directionForward,
		bool inverter,
		bool inverterDischarge,
		bool speedMode,
		uint16_t torqueLimit)
{
	mTransmitBuffer[0] = torque % 256;
	mTransmitBuffer[1] = torque / 256;
	mTransmitBuffer[2] = angularVelocity % 256;
	mTransmitBuffer[3] = angularVelocity / 256;
	mTransmitBuffer[4] = (directionForward ? 1 : 0);
	mTransmitBuffer[5] = (inverter ? 1 : 0) + 2 * (inverterDischarge ? 1 : 0) + 4 * (speedMode ? 1 : 0);
	mTransmitBuffer[6] = torqueLimit % 256;
	mTransmitBuffer[7] = torqueLimit / 256;
}

bool MCUInterface::TractionControlShouldBeTriggered()
{
	mLogger.LogInfo("TODO: MCUInterface decide TCS on or off");

	const bool decision = false;
	mLogger.LogCustom("TCS Decision:" + std::to_string(decision));
	return false;
}

void MCUInterface::ModifyCommandMessageByTractionControl()
{


	mLogger.LogCustom("After TCS:" + std::to_string(mTransmitBuffer[0]) + ", " + std::to_string(mTransmitBuffer[1])
			+ ", " + std::to_string(mTransmitBuffer[2]) + ", " + std::to_string(mTransmitBuffer[3])
			+ ", " + std::to_string(mTransmitBuffer[4]) + ", " + std::to_string(mTransmitBuffer[5])
			+ ", " + std::to_string(mTransmitBuffer[6]) + ", " + std::to_string(mTransmitBuffer[7]));
}

}
