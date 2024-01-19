#include <UtilsLib/Inc/CANManager.hpp>

namespace VehicleControlUnit { namespace UtilsLib {

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

	// Activate interrupts
	error = HAL_CAN_ActivateNotification(&mCanHandler, CAN_IT_RX_FIFO0_MSG_PENDING);
	if (error == HAL_ERROR)
	{
		mLogger.LogError("Activation of CAN_IT_RX_FIFO0_MSG_PENDING failed. Please raise to developers");
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

UtilsLib::ErrorState CANManangerForBMSAndMCU::MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// Check the CAN message header
	uint32_t messageID;

	if (header.IDE == CAN_ID_STD)
	{
		messageID = header.StdId;
	}
	else if (header.IDE == CAN_ID_EXT)
	{
		messageID = header.ExtId;
	}
	else
	{
		mLogger.LogError("CAN Message with unknown IDE is received. canPortName: " + mCanPortName);
		return UtilsLib::ErrorState::CAN_MSG_RECEIVE_FAIL;
	}


	// TODO :: Map CAN message ID to action (call a function from another library)
	// You can call mBMSInterface.MessageReceiveHandler(header, message) to pass the message to BMS interface library
	mLogger.LogInfo("TODO: Map the received CAN Message's ID to an action for CANManangerForBMSAndMCU");
	return UtilsLib::ErrorState::CAN_MSG_RECEIVE_SUCCESS;
}

UtilsLib::ErrorState CANManangerForSensors::MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// Check the CAN message header
	uint32_t messageID;

	if (header.IDE == CAN_ID_STD)
	{
		messageID = header.StdId;
	}
	else if (header.IDE == CAN_ID_EXT)
	{
		messageID = header.ExtId;
	}
	else
	{
		mLogger.LogError("CAN Message with unknown IDE is received. canPortName: " + mCanPortName);
	}

	// TODO :: Map CAN message ID to action (call a function from another library)
	mLogger.LogInfo("TODO: Map the received CAN Message's ID to an action for CANManangerForBMSAndMCU");
	return UtilsLib::ErrorState::INIT_SUCCESS;
}

}}
