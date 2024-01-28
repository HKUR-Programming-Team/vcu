#include <MCUInterfaceLib/Inc/MCUInterface.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

void MCUInterface::MessageReceiveHandler(const uint32_t messageID, const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: unit test cases for MCUInterface messageReceiveHandler. two's complement");
	switch(messageID) {
	    case 0x0A5: { // Motor Speed
	        const int16_t motorSpeed = static_cast<uint16_t>(message[3]) << 8 | static_cast<uint16_t>(message[2]);
	        mDataStore.mMCUAndBMSDataStore.SetMotorSpeed(motorSpeed);
	        break;
	    }
	    case 0x0A6: { // DC Bus Current
	        const auto dcBusCurrent = ~(static_cast<uint16_t>(message[7]) << 8 | static_cast<uint16_t>(message[6]));
	        mDataStore.mMCUAndBMSDataStore.SetDCBusCurrent(dcBusCurrent);
	        break;
	    }
	    case 0x0A7: { // DC Bus Voltage
	    	const auto dcBusVoltage = (static_cast<uint16_t>(message[1]) << 8 | static_cast<uint16_t>(message[0]));
	        mDataStore.mMCUAndBMSDataStore.SetDCBusVoltage(dcBusVoltage);
	        break;
	    }
	    case 0x0AA: { // VSM State
	        uint8_t vsmState = message[0];
	        auto GetVSMStateDescription = [](const uint8_t vsmState){
				switch(vsmState) {
						case 0: return DataStoreLib::VSMState::StartUp; break;
						case 1: return DataStoreLib::VSMState::PreChargeInit; break;
						case 2: return DataStoreLib::VSMState::PreChargeActivate; break;
						case 3: return DataStoreLib::VSMState::PreChangeComplete; break;
						case 4: return DataStoreLib::VSMState::Idle; break;
						case 5: return DataStoreLib::VSMState::Ready; break;
						case 6: return DataStoreLib::VSMState::MotorRunning; break;
						case 7: return DataStoreLib::VSMState::BlinkFaultCode; break;
						case 14: return DataStoreLib::VSMState::ShutdownInProgress; break;
						case 15: return DataStoreLib::VSMState::PowerRecycling; break;
						default: return DataStoreLib::VSMState::Unknown; break;
				}
	        };
	        mDataStore.mMCUAndBMSDataStore.SetVSMState(GetVSMStateDescription(vsmState));
	    }
	    // ... Other cases ...
	    default: {
	        mLogger.LogError("Received unknown CAN ID in MessageReceiveHandler of MCUInterface. Please raise to developer");
			return;
	        break;
	    }
	}
	mDataStore.mMCUAndBMSDataStore.SetMCUUpdateTs(HAL_GetTick());
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
	mLogger.LogInfo("TODO: MCUInterface Torque change to two's complement");
	const uint16_t torque = mDataStore.mDriveDataStore.GetTorque();
	const uint16_t angularVelocity = 0;
	const bool directionForward = mDataStore.mDriveDataStore.GetGear() == DataStoreLib::Gear::FORWARD;
	const bool inverter = torque >= InverterEnableTorqueThreshold * MainLib::Settings::sensorInterfaceParameters.MaxTorque;
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
