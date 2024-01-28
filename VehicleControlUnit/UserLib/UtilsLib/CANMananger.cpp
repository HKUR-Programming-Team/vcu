#include <UtilsLib/Inc/CANManager.hpp>

namespace VehicleControlUnit::UtilsLib {

UtilsLib::ErrorState CANManager::init()
{
	mLogger.LogInfo("Initialization of CAN Manager Starts");
	// Initialize CAN
	auto error = HAL_CAN_Start(&mCanHandler);
	if (error == HAL_ERROR)
	{
		mLogger.LogError("Initialization of HAL_CAN failed. Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	mLogger.LogInfo("Initialization of CAN Manager Success");
	return ErrorState::INIT_SUCCESS;
}

void CANManager::SetTransmitHeader(
			const uint32_t messageIdentifier,
			const uint32_t messageLength,
			const bool identifierExtension,
			const bool isRemoteTransmissionRequest,
			const bool transmitGlobalTime)
{
	// StdId, ExtId, IDE
	if (identifierExtension)
	{
		mTransmitHeader.ExtId = messageIdentifier;
		mTransmitHeader.StdId = 0;
		mTransmitHeader.IDE = CAN_ID_EXT;
	}
	else
	{
		mTransmitHeader.ExtId = 0;
		mTransmitHeader.StdId = messageIdentifier;
		mTransmitHeader.IDE = CAN_ID_STD;
	}

	// DLC
	mTransmitHeader.DLC = messageLength;

	// RTR
	if (isRemoteTransmissionRequest)
	{
		mTransmitHeader.RTR = 1;
	}
	else
	{
		mTransmitHeader.RTR = 0;
	}

	// TransmitGlobalTime
	if (transmitGlobalTime)
	{
		mTransmitHeader.TransmitGlobalTime = FunctionalState::ENABLE;
	}
	else
	{
		mTransmitHeader.TransmitGlobalTime = FunctionalState::DISABLE;
	}

	mLogger.LogSpam("TransmitHeader is set for " + mCanPortName +
			". transmitHeader: {StdId: " + std::to_string(mTransmitHeader.StdId) +
			". ExtId: " + std::to_string(mTransmitHeader.ExtId) +
			". IDE: " + ((mTransmitHeader.ExtId == CAN_ID_STD) ? "CAN_ID_STD" : "CAN_ID_EXT") +
			". DLC: " + std::to_string(mTransmitHeader.DLC) +
			". RTR: " + std::to_string(mTransmitHeader.RTR) +
			". transmitGlobalTime: " + std::to_string(transmitGlobalTime) + "}");
}

UtilsLib::ErrorState CANManager::SendMessage(const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Send CAN message");

	// TODO :: use HAL_CAN_GetTxMailboxesFreeLevel() to check whether a mailbox is free
	// What to do if none is free? Implausible? abortPendingMessage?

	// Store the data to the buffer
	for (uint32_t i = 0; i < mTransmitHeader.DLC; i++)
	{
		mTransmitBuffer[i] = message[i];
	}

	// Then request transmission of a message using HAL_CAN_AddTxMessage()
	const auto returnStatus = HAL_CAN_AddTxMessage(&mCanHandler, &mTransmitHeader, mTransmitBuffer, &mLastMailboxUsed);

	if (returnStatus == HAL_OK)
	{
		mLogger.LogInfo("TODO: Send CAN message HAL OK");
		return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_SUCCESS;
	}
	else if (returnStatus == HAL_ERROR)
	{
		mLogger.LogInfo("TODO: Send CAN message HAL ERROR");
		return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_FAIL;
	}

	// TODO :: Do appropriate action according to the return state: implausible? send again?
	return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_FAIL;
}

UtilsLib::ErrorState CANManager::CheckReceiveFIFO()
{
	uint32_t fillLevel = HAL_CAN_GetRxFifoFillLevel(&mCanHandler, CAN_RX_FIFO0);
	mLogger.LogSpam(mCanPortName + " fillLevel = " + std::to_string(fillLevel));

	while (fillLevel != 0)
	{
		HAL_CAN_GetRxMessage(&mCanHandler, CAN_RX_FIFO0, &mReceiveHeader, mReceiveBuffer);
		MessageReceiveHandler();
		fillLevel = HAL_CAN_GetRxFifoFillLevel(&mCanHandler, CAN_RX_FIFO0);
	}

	mLogger.LogInfo("TODO: CheckReceiveFIFO error handling");

	return UtilsLib::ErrorState::CAN_MSG_RECEIVE_SUCCESS; // Placeholder
}

UtilsLib::ErrorState CANManager::MessageReceiveHandler()
{
	// Check the CAN message header
	uint32_t messageID;

	if (mReceiveHeader.IDE == CAN_ID_STD)
	{
		messageID = mReceiveHeader.StdId;
	}
	else if (mReceiveHeader.IDE == CAN_ID_EXT)
	{
		messageID = mReceiveHeader.ExtId;
	}
	else
	{
		mLogger.LogError("CAN Message with unknown IDE is received. canPortName: " + mCanPortName);
		return UtilsLib::ErrorState::CAN_MSG_RECEIVE_FAIL;
	}
	mLogger.LogCustom("CANMsg:" + std::to_string(messageID) + ", " + std::to_string(mReceiveBuffer[0]) + ", " + std::to_string(mReceiveBuffer[1])
			+ ", " + std::to_string(mReceiveBuffer[2]) + ", " + std::to_string(mReceiveBuffer[3])
			+ ", " + std::to_string(mReceiveBuffer[4]) + ", " + std::to_string(mReceiveBuffer[5])
			+ ", " + std::to_string(mReceiveBuffer[6]) + ", " + std::to_string(mReceiveBuffer[7]));

	// TODO :: Map CAN message ID to action (call a function from another library)
	// You can call mBMSInterface.MessageReceiveHandler(header, message) to pass the message to BMS interface library
	if (messageID == 0x0A5 || messageID == 0x0A6 || messageID == 0x0A7 || messageID == 0x0AA)
	{
		mMCUInterface.MessageReceiveHandler(messageID, mReceiveHeader, mReceiveBuffer);
		return UtilsLib::ErrorState::CAN_MSG_RECEIVE_SUCCESS;
	}

	mLogger.LogInfo("TODO: Map the received CAN Message's ID to an action for CANMananger");
	return UtilsLib::ErrorState::CAN_MSG_RECEIVE_SUCCESS;
}

}
