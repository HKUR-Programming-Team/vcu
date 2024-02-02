#include <MCUInterfaceLib/Inc/MCUInterface.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

void MCUInterface::MessageReceiveHandler(const uint32_t messageID, const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: unit test cases for MCUInterface messageReceiveHandler. two's complement");
	switch(messageID) {
	    case 0x0A5: { // Motor Speed
	        const int16_t motorSpeed = static_cast<int16_t>(static_cast<uint16_t>(message[3]) << 8 | static_cast<uint16_t>(message[2]));
	        mDataStore.mMCUDataStore.SetMotorSpeed(motorSpeed);
	        break;
	    }
	    case 0x0A6: { // DC Bus Current
	        const int16_t dcBusCurrent = static_cast<int16_t>(static_cast<uint16_t>(message[7]) << 8 | static_cast<uint16_t>(message[6]));
	        mDataStore.mMCUDataStore.SetDCBusCurrent(dcBusCurrent);
	        break;
	    }
	    case 0x0A7: { // DC Bus Voltage
	    	const int16_t dcBusVoltage = static_cast<int16_t>(static_cast<uint16_t>(message[1]) << 8 | static_cast<uint16_t>(message[0]));
	        mDataStore.mMCUDataStore.SetDCBusVoltage(dcBusVoltage);
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
	        mDataStore.mMCUDataStore.SetVSMState(GetVSMStateDescription(vsmState));
	    }
	    // ... Other cases ...
	    default: {
	        mLogger.LogError("Received unknown CAN ID in MessageReceiveHandler of MCUInterface. Please raise to developer");
			return;
	        break;
	    }
	}
	mDataStore.mMCUDataStore.SetUpdateTs(HAL_GetTick());
}

void MCUInterface::SendCommandMessageInErrorState()
{
	mCANManager.SetTransmitHeader(mParameters.CommandMessageHeaderId, mParameters.CommandMessageLength);
	SetCommandMessage(0, false, true, false, 0);
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

	mLogger.LogInfo("TODO: Button to toggle TCS");
	if (mTCSEnabled)
	{
		CheckTractionControlTriggered();

		if (mTCSTriggered)
		{
			ModifyCommandMessageByTractionControl();
		}
	}
	mCANManager.SetTransmitHeader(mParameters.CommandMessageHeaderId, mParameters.CommandMessageLength);
	const auto error = mCANManager.SendMessage(mTransmitBuffer);

	mLogger.LogInfo("TODO: MCUInterface set error (implausible)");

	// If there is error state, store it to the data store

	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
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

	mLogger.LogCustom("Before TCS:" + std::to_string(mTransmitBuffer[0]) + ", " + std::to_string(mTransmitBuffer[1])
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

bool MCUInterface::GetTCSEnabled() const
{
	return mTCSEnabled;
}


void MCUInterface::SetTCSEnabled(const bool tcsEnabled)
{
	mTCSEnabled = tcsEnabled;
}

}
