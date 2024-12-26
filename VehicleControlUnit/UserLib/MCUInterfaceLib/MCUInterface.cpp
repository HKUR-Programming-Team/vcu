#include <MCUInterfaceLib/Inc/MCUInterface.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

void MCUInterface::MessageReceiveHandler(const uint32_t messageID, const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	switch(messageID) {
	    case 0x0A5: { // Motor Speed
	        const int16_t motorSpeed = static_cast<int16_t>(static_cast<uint16_t>(message[3]) << 8 | static_cast<uint16_t>(message[2]));
	        mDataStore.mMCUDataStore.SetMotorSpeed(motorSpeed);
			mDataStore.mMCUDataStore.SetMotorSpeedUpdateTs(HAL_GetTick());
	        break;
	    }
	    case 0x0AB: { // Run Fault
	        const uint8_t byte5 = message[5];
	        const uint8_t commandMessageLostBit = (byte5 >> 3) & 1;
			const bool commandMessageLost = commandMessageLostBit == 1;
			mDataStore.mMCUDataStore.SetMessageReceiveTimeoutError(commandMessageLost);
			break;
	    }
	    // ... Other cases ...
	    default: {
	        mLogger.LogError("Received unknown CAN ID in MessageReceiveHandler of MCUInterface. Please raise to developer");
			return;
	        break;
	    }
	}
}

void MCUInterface::SendCommandMessageInErrorState()
{
	mCANManager.SetTransmitHeader(mParameters.CommandMessageHeaderId, mParameters.CommandMessageLength);
	SetCommandMessage(0, false, true, false, 0);
	mCANManager.SendMessage(mTransmitBuffer);
}

void MCUInterface::SendCommandMessage()
{
	// Send one command message every mParameters.CommandMessageTransmitInterval
	if (HAL_GetTick() < mLastCommandMessageSendTs + mParameters.CommandMessageTransmitInterval)
	{
		return;
	}

	// Check if the engine should be stopped
	if (mDataStore.GetPersistedImplausibleStatus())
	{
		SendCommandMessageInErrorState();
		return;
	}

	SetCommandMessageInNonErrorState();

	// mLogger.LogInfo("TODO: Button to toggle TCS");
	if (mDataStore.mDrivingInputDataStore.GetTCSEnabled())
	{
		CheckTractionControlTriggered();

		if (mTCSTriggered)
		{
			ModifyCommandMessageByTractionControl();
		}
	}
	mCANManager.SetTransmitHeader(mParameters.CommandMessageHeaderId, mParameters.CommandMessageLength);
	const auto error = mCANManager.SendMessage(mTransmitBuffer);

	if (error == UtilsLib::ErrorState::CAN_MSG_TRANSMIT_SUCCESS)
	{
		auto GetQuotient = [&](const uint32_t dividend, const uint32_t divisor) -> uint32_t
		{
			return dividend / divisor;
		};

		mLastCommandMessageSendTs = GetQuotient(HAL_GetTick(), mParameters.CommandMessageTransmitInterval) * mParameters.CommandMessageTransmitInterval;
	}
	else
	{
		mCANManager.AbortAllSendRequests(); // Clear transmit box. Try again in the next loop.
	}

	return;
}

void MCUInterface::SetCommandMessageInNonErrorState()
{
	const int16_t torqueFromPedals = mDataStore.mDrivingInputDataStore.GetTorque();
	const int16_t regen = mDataStore.mDrivingInputDataStore.GetRegen();
	int16_t torqueCommand = torqueFromPedals;
	if (torqueCommand <= mParameters.RegenEnableTorqueThreshold)
	{
		torqueCommand = -regen;
	}

	const bool torqueEnablesInverter = torqueCommand >= mParameters.InverterEnableTorqueThreshold || torqueCommand <= -mParameters.InverterEnableTorqueThreshold;
	const bool inverter = mDataStore.mDrivingInputDataStore.GetGear() == DataStoreLib::Gear::FORWARD && torqueEnablesInverter;

	const bool inverterDischarge = !inverter;

	const auto speedMode = false;
	const uint16_t torqueLimit = 0;

	SetCommandMessage(torqueCommand, inverter, inverterDischarge, speedMode, torqueLimit);

	mLogger.LogSpam("Before TCS:" + std::to_string(mTransmitBuffer[0]) + ", " + std::to_string(mTransmitBuffer[1])
				+ ", " + std::to_string(mTransmitBuffer[2]) + ", " + std::to_string(mTransmitBuffer[3])
				+ ", " + std::to_string(mTransmitBuffer[4]) + ", " + std::to_string(mTransmitBuffer[5])
				+ ", " + std::to_string(mTransmitBuffer[6]) + ", " + std::to_string(mTransmitBuffer[7]));
}

void MCUInterface::SetCommandMessage(int16_t torque,
		bool inverter,
		bool inverterDischarge,
		bool speedMode,
		uint16_t torqueLimit)
{
	if (inverter)
	{
		SetCommandMessageTorque(torque);
	}
	else
	{
		SetCommandMessageTorque(0);
	}
	mTransmitBuffer[2] = 0;
	mTransmitBuffer[3] = 0;
	mTransmitBuffer[4] = 1; // FSUK2024: car going backward is illegal
	mTransmitBuffer[5] = (inverter ? 1 : 0) + 2 * (inverterDischarge ? 1 : 0) + 4 * (speedMode ? 1 : 0);
	mTransmitBuffer[6] = torqueLimit % 256;
	mTransmitBuffer[7] = torqueLimit / 256;
}

int16_t MCUInterface::GetCommandMessageTorque() const
{
	const uint16_t torqueTwosComplement = static_cast<uint16_t>(mTransmitBuffer[1]) << 8 | static_cast<uint16_t>(mTransmitBuffer[0]);
	return static_cast<int16_t>(torqueTwosComplement);
}

void MCUInterface::SetCommandMessageTorque(int16_t torque)
{
	const uint16_t torqueTwosComplement = static_cast<uint16_t>(torque);
	mTransmitBuffer[0] = torqueTwosComplement % 256;
	mTransmitBuffer[1] = torqueTwosComplement / 256;
}

void MCUInterface::CheckTractionControlTriggered()
{
	mLogger.LogInfo("TODO: MCUInterface decide TCS on or off");

	const auto optLeftWheelSpeed = mDataStore.mVehicleSensorDataStore.GetAngularWheelSpeedRearLeft();
	const auto optRightWheelSpeed = mDataStore.mVehicleSensorDataStore.GetAngularWheelSpeedRearRight();
	const auto optLinearSpeed = mDataStore.mVehicleSensorDataStore.GetLinearVelocity();
	if(!optLeftWheelSpeed.has_value() || !optRightWheelSpeed.has_value() || !optLinearSpeed.has_value())
	{
		mTCSTriggered = false;
		return;
	}

	const auto linearSpeed = optLinearSpeed.value();
	if (linearSpeed < mParameters.MinimumTCSTriggeringLinearSpeed)
	{
		mTCSTriggered = false;
		return;
	}

	const auto leftWheelSpeed = optLeftWheelSpeed.value();
	const auto rightWheelSpeed = optRightWheelSpeed.value();
	const auto maxWheelSpeed = leftWheelSpeed > rightWheelSpeed ? leftWheelSpeed : rightWheelSpeed;

	const auto slipRatio = (mParameters.WheelRadius * maxWheelSpeed - linearSpeed)/linearSpeed;

	if (!mTCSTriggered)
	{
		if (slipRatio > mParameters.TCSTriggeringSlipRatioThreshold)
		{
			mTCSTriggered = true;
			mTCSTriggeredStartTorque = mDataStore.mDrivingInputDataStore.GetTorque();
			return;
		}
	}
	else if (slipRatio < mParameters.TCSHaltSlipRatioThreshold)
	{
		mTCSTriggered = false;
	}
}

void MCUInterface::ModifyCommandMessageByTractionControl()
{
	const int16_t torqueFromPedalSensor = GetCommandMessageTorque();
	const int16_t minTorque = mTCSTriggeredStartTorque < torqueFromPedalSensor ? mTCSTriggeredStartTorque : torqueFromPedalSensor;
	SetCommandMessageTorque(minTorque);
	mLogger.LogCustom("After TCS:" + std::to_string(mTransmitBuffer[0]) + ", " + std::to_string(mTransmitBuffer[1]) + ", torque: " );
}

}
